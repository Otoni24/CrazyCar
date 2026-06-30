# AGENTS.md

## Project

This repository contains **Crazy Car**, an Unreal Engine C++ project for a small arcade physics-based car tech demo.

## Current Scope

Work only on the current milestone requested by the user.

Do not add future systems unless explicitly requested.

Out of scope for the current car-controller phase:

* physics props architecture
* destruction systems
* building clusters
* scoring systems
* objectives
* advanced VFX/audio systems

## Unreal C++ Style

Use Unreal naming conventions:

* `A` prefix for Actor-derived classes
* `U` prefix for UObject / component / data asset classes
* `F` prefix for structs
* `E` prefix for enums
* `I` prefix for interfaces

Use UE5-style `TObjectPtr` for UObject/component references stored as `UPROPERTY`.

Place files according to the existing project structure:

* core gameplay classes in `Source/CrazyCar/Public/Core/` and `Source/CrazyCar/Private/Core/`
* vehicle classes in `Source/CrazyCar/Public/Vehicle/` and `Source/CrazyCar/Private/Vehicle/`

## Component Setup Rules

Do not duplicate native component properties as wrapper variables unless explicitly requested.

Examples of properties that should usually stay on their component:

* `UBoxComponent` size, collision, mass, damping, physics simulation
* `USpringArmComponent` target arm length, camera lag, camera lag speed, rotation settings
* `UStaticMeshComponent` mesh, material, visibility, collision settings

Set sensible component defaults in C++ constructors when needed, but let Blueprint instances edit native component properties directly.

Create custom variables only for gameplay concepts that are not already represented by native component properties.

Good custom variables:

* acceleration
* max speed
* grip strength
* drift angle threshold
* drift grip multiplier
* upright correction strength

Avoid wrapper variables such as:

* `CameraDistance` if it only mirrors `SpringArm.TargetArmLength`
* `CameraLagSpeed` if it only mirrors `SpringArm.CameraLagSpeed`
* `Mass` if it only mirrors the physics body mass
* `LinearDamping` if it only mirrors the physics body linear damping
* `AngularDamping` if it only mirrors the physics body angular damping

## Car Pawn Structure

The car pawn should use a dedicated collision body and a separate visual mesh:

```text
ACrazyCarPawn
└── CarBodyCollision : UBoxComponent
    └── CarBodyMesh : UStaticMeshComponent
```

`CarBodyCollision` is the physical root.

`CarBodyMesh` is visual-only and should not drive gameplay collision.

## Architecture Rules

Keep responsibilities separated:

* `ACrazyCarPlayerController` handles input reading and input interpretation.
* `ACrazyCarPawn` owns components and exposes a simple movement-input API.
* `UArcadeCarMovementComponent` owns movement logic.
* `UCarStabilityComponent` owns anti-flip / stability logic.
* `UCarDebugComponent` owns debug display and debug drawing.
* `UCarTuningData` stores gameplay tuning values when introduced.

Do not put movement formulas directly in `ACrazyCarPawn`.

Do not let the PlayerController apply physics forces directly.

## Milestone Discipline

Implement only the requested milestone or sub-task.

Avoid broad refactors unless explicitly requested.

Prefer small compile-safe changes.

Do not introduce placeholder systems for future phases.

When uncertain between a simple Unreal-native setup and a custom abstraction, prefer the Unreal-native setup unless the user explicitly asks for a custom abstraction.

## Editor / Blueprint Boundary

C++ should define structure and core logic.

Blueprint should be used for:

* assigning meshes
* assigning materials
* editing component-native properties
* simple asset setup
* level setup
* tuning values exposed by C++ where appropriate

Do not create Blueprint assets unless explicitly requested.
