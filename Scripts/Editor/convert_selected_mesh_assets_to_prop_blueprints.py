"""
Convert placed StaticMeshActors into generated BP_PhysicsProp child Blueprints.

Usage:
    1. Select one or more Static Mesh assets in the Content Browser.
    2. Run this script from the Unreal Editor Python console or script runner.
    3. Review the dry-run output.
    4. Set DRY_RUN = False when the report looks correct.

This script is intentionally editor-only. It does not touch gameplay C++ or
existing Blueprint assets except for creating/reusing generated child Blueprints.
"""

import re

import unreal


DRY_RUN = False
DELETE_ORIGINALS = True
PARENT_BLUEPRINT_PATH = "/Game/Blueprints/BP_PhysicsProp.BP_PhysicsProp"
GENERATED_BLUEPRINT_FOLDER = "/Game/Blueprints/Props/Generated"

PROP_MESH_COMPONENT_NAME = "PropMesh"
LOG_PREFIX = "[ConvertSelectedMeshesToProps]"
PROP_STATIC_MESH_VARIABLE_NAME = "PropStaticMesh"


def log(message):
    unreal.log(f"{LOG_PREFIX} {message}")


def warn(message):
    unreal.log_warning(f"{LOG_PREFIX} {message}")


def error(message):
    unreal.log_error(f"{LOG_PREFIX} {message}")


def get_asset_path(asset):
    """Return a stable object path for comparisons and logs."""
    if not asset:
        return ""
    try:
        return asset.get_path_name()
    except Exception:
        return str(asset)


def get_selected_static_mesh_assets():
    """Read Content Browser selection and keep only valid StaticMesh assets."""
    selected_assets = unreal.EditorUtilityLibrary.get_selected_assets()
    static_meshes = []

    for asset in selected_assets:
        if not asset:
            continue
        if isinstance(asset, unreal.StaticMesh):
            static_meshes.append(asset)
        else:
            warn(f"Skipping non-StaticMesh selected asset: {get_asset_path(asset)}")

    # De-duplicate by asset path while preserving selection order.
    unique_meshes = []
    seen_paths = set()
    for mesh in static_meshes:
        mesh_path = get_asset_path(mesh)
        if mesh_path not in seen_paths:
            unique_meshes.append(mesh)
            seen_paths.add(mesh_path)

    return unique_meshes


def load_parent_blueprint():
    """Load and validate the BP_PhysicsProp parent Blueprint."""
    parent_blueprint = unreal.EditorAssetLibrary.load_asset(PARENT_BLUEPRINT_PATH)
    if not parent_blueprint:
        error(f"Parent Blueprint was not found: {PARENT_BLUEPRINT_PATH}")
        return None

    if not isinstance(parent_blueprint, unreal.Blueprint):
        error(f"Parent asset is not a Blueprint: {PARENT_BLUEPRINT_PATH}")
        return None

    if not get_blueprint_generated_class(parent_blueprint):
        error(f"Parent Blueprint has no generated class: {PARENT_BLUEPRINT_PATH}")
        return None

    return parent_blueprint


def get_blueprint_generated_class(blueprint):
    """Safely fetch the actual generated UClass from a Blueprint."""
    if not blueprint:
        return None

    # Best path for UE Python: read the actual reflected property.
    try:
        generated_class = blueprint.get_editor_property("generated_class")
        if generated_class:
            return generated_class
    except Exception:
        pass

    # Fallback for API variants where generated_class exists as callable.
    try:
        generated_class_attr = getattr(blueprint, "generated_class", None)

        if callable(generated_class_attr):
            generated_class_attr = generated_class_attr()

        if generated_class_attr:
            return generated_class_attr
    except Exception:
        pass

    return None


def sanitize_object_name(name):
    """Keep generated asset names valid for Unreal object/package names."""
    sanitized = re.sub(r"[^A-Za-z0-9_]", "_", name)
    sanitized = sanitized.strip("_")
    return sanitized or "StaticMesh"


def get_generated_blueprint_name(static_mesh):
    return f"BP_Prop_{sanitize_object_name(static_mesh.get_name())}"


def get_generated_blueprint_asset_path(static_mesh):
    blueprint_name = get_generated_blueprint_name(static_mesh)
    return f"{GENERATED_BLUEPRINT_FOLDER}/{blueprint_name}"


def ensure_generated_blueprint_folder_exists():
    if unreal.EditorAssetLibrary.does_directory_exist(GENERATED_BLUEPRINT_FOLDER):
        return True

    if DRY_RUN:
        return True

    if unreal.EditorAssetLibrary.make_directory(GENERATED_BLUEPRINT_FOLDER):
        return True

    error(f"Could not create generated Blueprint folder: {GENERATED_BLUEPRINT_FOLDER}")
    return False


def create_or_reuse_child_blueprint(parent_blueprint, static_mesh):
    """
    Create or load BP_Prop_<StaticMeshName>, set its inherited PropMesh default,
    compile, and save.
    """
    blueprint_name = get_generated_blueprint_name(static_mesh)
    blueprint_asset_path = get_generated_blueprint_asset_path(static_mesh)

    existing_asset = unreal.EditorAssetLibrary.load_asset(blueprint_asset_path)
    if existing_asset:
        if not isinstance(existing_asset, unreal.Blueprint):
            error(f"Generated asset path exists but is not a Blueprint: {blueprint_asset_path}")
            return None, False
        child_blueprint = existing_asset
        was_created = False
        log(f"Reusing child Blueprint: {blueprint_asset_path}")
    else:
        parent_class = get_blueprint_generated_class(parent_blueprint)
        if not parent_class:
            error("Cannot create child Blueprint because the parent generated class is missing.")
            return None, False

        factory = unreal.BlueprintFactory()
        factory.set_editor_property("parent_class", parent_class)

        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        child_blueprint = asset_tools.create_asset(
            asset_name=blueprint_name,
            package_path=GENERATED_BLUEPRINT_FOLDER,
            asset_class=unreal.Blueprint,
            factory=factory,
        )

        if not child_blueprint:
            error(f"Failed to create child Blueprint: {blueprint_asset_path}")
            return None, False

        was_created = True
        log(f"Created child Blueprint: {blueprint_asset_path}")

    if not set_blueprint_prop_mesh_default(child_blueprint, static_mesh):
        return None, was_created

    compile_blueprint(child_blueprint)
    unreal.EditorAssetLibrary.save_loaded_asset(child_blueprint)
    return child_blueprint, was_created


def compile_blueprint(blueprint):
    try:
        if hasattr(unreal, "BlueprintEditorLibrary"):
            unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
            return
    except Exception as exception:
        warn(f"Could not compile Blueprint {get_asset_path(blueprint)}: {exception}")

    # Compilation is not critical for this script if save succeeds.
    return


def set_blueprint_prop_mesh_default(blueprint, static_mesh):
    """
    Set the child Blueprint default PropStaticMesh variable.
    BP_PhysicsProp Construction Script will assign it to PropMesh.
    """
    blueprint_class = get_blueprint_generated_class(blueprint)
    if not blueprint_class:
        error(f"Blueprint has no generated class: {get_asset_path(blueprint)}")
        return False

    class_default_object = unreal.get_default_object(blueprint_class)
    if not class_default_object:
        error(f"Could not get CDO for Blueprint: {get_asset_path(blueprint)}")
        return False

    try:
        blueprint.modify()
    except Exception:
        pass

    try:
        class_default_object.modify()
    except Exception:
        pass

    try:
        class_default_object.set_editor_property(PROP_STATIC_MESH_VARIABLE_NAME, static_mesh)
        log(
            f"Set {get_asset_path(blueprint)}.{PROP_STATIC_MESH_VARIABLE_NAME} "
            f"to {get_asset_path(static_mesh)}"
        )
        unreal.EditorAssetLibrary.save_loaded_asset(blueprint)
        return True

    except Exception as exception:
        error(
            f"Failed to set variable '{PROP_STATIC_MESH_VARIABLE_NAME}' "
            f"on {get_asset_path(blueprint)}: {exception}"
        )
        return False


def find_prop_mesh_component(actor_or_cdo):
    """Find PropMesh by property, default subobject name, or component list."""
    if not actor_or_cdo:
        return None

    # 1. Try Blueprint variable/property name.
    try:
        prop_mesh = actor_or_cdo.get_editor_property(PROP_MESH_COMPONENT_NAME)
        if isinstance(prop_mesh, unreal.StaticMeshComponent):
            return prop_mesh
    except Exception:
        pass

    # 2. Try default subobject lookup.
    possible_names = [
        PROP_MESH_COMPONENT_NAME,
        f"{PROP_MESH_COMPONENT_NAME}_GEN_VARIABLE",
    ]

    for name in possible_names:
        try:
            subobject = actor_or_cdo.get_default_subobject_by_name(name)
            if isinstance(subobject, unreal.StaticMeshComponent):
                return subobject
        except Exception:
            pass

    # 3. Try component list.
    try:
        components = actor_or_cdo.get_components_by_class(unreal.StaticMeshComponent)
    except Exception:
        components = []

    for component in components:
        try:
            if component.get_name() == PROP_MESH_COMPONENT_NAME:
                return component
        except Exception:
            continue

    # 4. Fallback: if there is exactly one StaticMeshComponent, use it.
    if len(components) == 1:
        return components[0]

    if components:
        names = []
        for component in components:
            try:
                names.append(component.get_name())
            except Exception:
                pass
        warn(f"Found StaticMeshComponents, but none named '{PROP_MESH_COMPONENT_NAME}': {names}")
    else:
        warn(f"No StaticMeshComponents found on object: {actor_or_cdo}")

    return None


def get_static_mesh_actor_component(actor):
    """Return the StaticMeshComponent for a placed StaticMeshActor, if valid."""
    if not isinstance(actor, unreal.StaticMeshActor):
        return None

    try:
        component = actor.static_mesh_component
        if component:
            return component
    except Exception:
        pass

    try:
        return actor.get_component_by_class(unreal.StaticMeshComponent)
    except Exception:
        return None


def get_component_static_mesh(static_mesh_component):
    if not static_mesh_component:
        return None

    try:
        return static_mesh_component.get_editor_property("static_mesh")
    except Exception:
        try:
            return static_mesh_component.static_mesh
        except Exception:
            return None


def scan_matching_static_mesh_actors(static_meshes):
    """Find placed StaticMeshActors whose component mesh is in the selection."""
    mesh_paths = {get_asset_path(mesh) for mesh in static_meshes}
    matches_by_mesh_path = {get_asset_path(mesh): [] for mesh in static_meshes}

    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        component = get_static_mesh_actor_component(actor)
        if not component:
            continue

        component_mesh = get_component_static_mesh(component)
        component_mesh_path = get_asset_path(component_mesh)
        if component_mesh_path in mesh_paths:
            matches_by_mesh_path[component_mesh_path].append(actor)

    return matches_by_mesh_path


def copy_actor_transform(source_actor, target_actor):
    """Copy location, rotation, and scale as exactly as the editor API allows."""
    transform = source_actor.get_actor_transform()

    try:
        target_actor.set_actor_transform(transform, False, True)
        return
    except Exception:
        pass

    target_actor.set_actor_location(source_actor.get_actor_location(), False, False)
    target_actor.set_actor_rotation(source_actor.get_actor_rotation(), False)
    target_actor.set_actor_scale3d(source_actor.get_actor_scale3d())


def copy_actor_label(source_actor, target_actor):
    try:
        source_label = source_actor.get_actor_label()
    except Exception:
        return

    if not source_label:
        return

    desired_label = source_label if DELETE_ORIGINALS else f"{source_label}_Prop"
    try:
        target_actor.set_actor_label(desired_label)
    except Exception as exception:
        warn(f"Could not set actor label for {target_actor.get_name()}: {exception}")


def copy_actor_folder(source_actor, target_actor):
    try:
        folder_path = source_actor.get_folder_path()
        target_actor.set_folder_path(folder_path)
    except Exception as exception:
        warn(f"Could not copy folder path from {source_actor.get_name()}: {exception}")


def copy_material_overrides(source_component, target_component):
    """Copy material overrides from the original StaticMeshActor to PropMesh."""
    if not source_component or not target_component:
        return

    try:
        override_materials = source_component.get_editor_property("override_materials")
    except Exception as exception:
        warn(f"Could not read material overrides: {exception}")
        return

    for material_index, material in enumerate(override_materials):
        if not material:
            continue
        try:
            target_component.set_material(material_index, material)
        except Exception as exception:
            warn(f"Could not copy material override slot {material_index}: {exception}")


def spawn_replacement_actor(source_actor, source_component, child_blueprint):
    blueprint_class = get_blueprint_generated_class(child_blueprint)
    if not blueprint_class:
        error(f"Cannot spawn Blueprint with missing generated class: {get_asset_path(child_blueprint)}")
        return None

    spawned_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        blueprint_class,
        source_actor.get_actor_location(),
        source_actor.get_actor_rotation(),
    )

    if not spawned_actor:
        error(f"Failed to spawn replacement for actor: {source_actor.get_name()}")
        return None

    copy_actor_transform(source_actor, spawned_actor)
    copy_actor_folder(source_actor, spawned_actor)

    target_component = find_prop_mesh_component(spawned_actor)
    if target_component:
        copy_material_overrides(source_component, target_component)
    else:
        warn(f"Spawned actor has no {PROP_MESH_COMPONENT_NAME} component: {spawned_actor.get_name()}")

    copy_actor_label(source_actor, spawned_actor)
    return spawned_actor


def print_dry_run_report(static_meshes, matches_by_mesh_path):
    log("DRY_RUN is True. No Blueprints, actors, or level objects will be modified.")
    log(f"Selected StaticMesh assets: {len(static_meshes)}")

    for static_mesh in static_meshes:
        mesh_path = get_asset_path(static_mesh)
        blueprint_name = get_generated_blueprint_name(static_mesh)
        blueprint_path = get_generated_blueprint_asset_path(static_mesh)
        action = "reuse" if unreal.EditorAssetLibrary.does_asset_exist(blueprint_path) else "create"
        matching_actors = matches_by_mesh_path.get(mesh_path, [])

        log(f"Selected mesh: {mesh_path}")
        log(f"  Matching actors in current level: {len(matching_actors)}")
        log(f"  Would {action} Blueprint: {GENERATED_BLUEPRINT_FOLDER}/{blueprint_name}")


def convert_static_mesh_actors(static_meshes, parent_blueprint, matches_by_mesh_path):
    generated_blueprints_by_mesh_path = {}
    created_or_reused_count = 0
    converted_actor_count = 0
    deleted_actor_count = 0

    if not ensure_generated_blueprint_folder_exists():
        return

    for static_mesh in static_meshes:
        child_blueprint, _was_created = create_or_reuse_child_blueprint(parent_blueprint, static_mesh)
        if child_blueprint:
            generated_blueprints_by_mesh_path[get_asset_path(static_mesh)] = child_blueprint
            created_or_reused_count += 1

    with unreal.ScopedEditorTransaction("Convert StaticMeshActors to BP_PhysicsProp children"):
        for static_mesh in static_meshes:
            mesh_path = get_asset_path(static_mesh)
            child_blueprint = generated_blueprints_by_mesh_path.get(mesh_path)
            if not child_blueprint:
                warn(f"Skipping actors for mesh because Blueprint generation failed: {mesh_path}")
                continue

            for source_actor in matches_by_mesh_path.get(mesh_path, []):
                source_component = get_static_mesh_actor_component(source_actor)
                spawned_actor = spawn_replacement_actor(source_actor, source_component, child_blueprint)
                if not spawned_actor:
                    continue

                converted_actor_count += 1

                if DELETE_ORIGINALS:
                    if unreal.EditorLevelLibrary.destroy_actor(source_actor):
                        deleted_actor_count += 1
                    else:
                        warn(f"Failed to delete original actor: {source_actor.get_name()}")

    log("Conversion summary:")
    log(f"  Selected meshes: {len(static_meshes)}")
    log(f"  Generated/reused Blueprints: {created_or_reused_count}")
    log(f"  Converted actors: {converted_actor_count}")
    log(f"  Deleted original actors: {deleted_actor_count}")

    if not DELETE_ORIGINALS:
        warn("DELETE_ORIGINALS is False. Replacement actors were spawned, but original StaticMeshActors remain.")


def main():
    static_meshes = get_selected_static_mesh_assets()
    if not static_meshes:
        warn("No StaticMesh assets selected in the Content Browser. Select one or more StaticMesh assets and rerun.")
        return

    parent_blueprint = load_parent_blueprint()
    if not parent_blueprint:
        return

    if not ensure_generated_blueprint_folder_exists():
        return

    matches_by_mesh_path = scan_matching_static_mesh_actors(static_meshes)

    if DRY_RUN:
        print_dry_run_report(static_meshes, matches_by_mesh_path)
        return

    convert_static_mesh_actors(static_meshes, parent_blueprint, matches_by_mesh_path)


if __name__ == "__main__":
    main()
