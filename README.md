# Crazy Car

**A physics-based arcade driving tech demo developed in Unreal Engine 5.**

`Crazy Car` is a small low-poly driving sandbox focused on arcade car control, drifting, and physical interaction with the environment.

The project was developed as a hands-on Unreal Engine C++ project, with the main goal of building a custom physics-based car controller instead of relying on Unreal's built-in vehicle system.

## Objective

**Drive, drift, and crash through a compact urban sandbox.**

The project is not a full game with missions or progression. It is a playable tech demo focused on car feel, physics interaction, and clean gameplay architecture.

## Features

* **Custom Arcade Car Controller:** The car uses a custom C++ movement component built around physics forces, lateral grip, steering behavior, and velocity control.
* **Manual Drift System:** Drift is activated by player input. While drifting, the movement behavior switches to a dedicated arcade drift mode instead of using the normal driving simulation.
* **Physics Interaction:** The car can collide with and push physics props placed around the level.
* **Camera Stabilization:** The follow camera is designed to remain readable while ignoring unwanted physics jitter from the car body.
* **Blueprint Asset Setup:** Blueprints are mainly used for mesh assignment, VFX, tuning, and level setup, while the core gameplay logic remains in C++.

## External Assets Notice

For licensing reasons, the commercial art assets used in this project are **not included** in the repository.

The project was built using the **POLYGON City Pack**. To make the project fully buildable and restore the expected environment assets, download the pack separately and place its folder inside the Unreal `Content` directory:

```text
Content/
  PolygonCity/
```

Without this folder, the source code remains available, but most asset references may be missing or broken.

## Build Instructions

This project requires **Unreal Engine 5** and a C++ compiler environment.

### Prerequisites

* **Unreal Engine 5.7**
* **C++ IDE:** Visual Studio 2022 with the "Game development with C++" workload, or JetBrains Rider.
* **POLYGON City Pack**, placed inside `Content/PolygonCity/`.

## License

This project is licensed under the **MIT License**. See the `LICENSE` file for more details.
