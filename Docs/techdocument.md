# Crazy Car — Technical Design Document v0.1

## 1. Purpose

Questo documento definisce la prima architettura tecnica di **Crazy Car**, concentrandosi sulla fase iniziale del progetto:

```text
Arcade Car Controller
Camera Follow
Input
Stability
Debug Tools
```

I sistemi relativi a props fisici, distruzione, edifici e cluster verranno definiti in documenti successivi, dopo aver validato il comportamento della macchina.

## 2. Technical Direction

Il progetto userà Unreal Engine con una preferenza per implementazione in **C++**.

Blueprint verrà usato principalmente per:

```text
Asset setup
Mesh/material assignment
Input assets
Level setup
Tuning rapido in editor
```

La logica core del veicolo deve restare in C++ per mantenere il progetto ordinato, leggibile e facilmente modificabile.

## 3. Phase 1 Scope

La prima fase tecnica include solo:

```text
Player controller
Car pawn
Arcade movement
Drift behavior
Anti-flip / stability
Camera follow
Car debug tools
Tuning data
```

Fuori scope per questa fase:

```text
Physics props architecture
Destructible objects
Building clusters
Score system
Game objectives
Advanced VFX/audio
```

## 4. Class Overview

Classi principali previste:

```text
ACrazyCarGameMode
ACrazyCarPlayerController
ACrazyCarPawn
UArcadeCarMovementComponent
UCarStabilityComponent
UCarTuningData
UCarDebugComponent
```

## 5. ACrazyCarGameMode

Responsabilità:

```text
Set default pawn class
Set player controller class
Define basic game setup
```

Per ora il GameMode non deve contenere logica di gameplay complessa.

## 6. ACrazyCarPlayerController

Responsabilità:

```text
Read player input
Handle controller movement input
Handle mouse movement input
Calculate desired movement direction
Send movement input to ACrazyCarPawn
Handle reset/debug input
```

Il PlayerController non applica direttamente forze fisiche alla macchina.

### Input Flow

```text
Player Input
→ ACrazyCarPlayerController
→ DesiredMoveDirection
→ ACrazyCarPawn
→ UArcadeCarMovementComponent
```

## 7. ACrazyCarPawn

`ACrazyCarPawn` rappresenta la macchina controllata dal player.

Componenti previsti:

```text
CarBody / PhysicsBody
CameraBoom
Camera
UArcadeCarMovementComponent
UCarStabilityComponent
UCarDebugComponent
```

Responsabilità:

```text
Own physical car body
Expose movement input API
Own car-related components
Setup camera components
Forward tick/update logic to components
```

Il Pawn non deve contenere direttamente formule complesse di movimento.
La logica di guida deve stare in `UArcadeCarMovementComponent`.

## 8. UArcadeCarMovementComponent

Componente principale per il movimento arcade della macchina.

Responsabilità:

```text
Store current movement input
Calculate velocity direction
Calculate input/velocity angle
Detect drift state
Apply acceleration force
Apply turning force/torque
Apply lateral grip/damping
Limit max speed
Expose debug values
```

Variabili principali:

```text
Acceleration
MaxSpeed
TurnStrength
GripStrength
LateralDamping
DriftAngleThreshold
DriftGripMultiplier
DriftTurnMultiplier
LateralSlideForce
```

### Movement Model

La macchina non usa una simulazione veicolo realistica.

Il movimento è basato su:

```text
DesiredMoveDirection
CurrentVelocity
ForwardDirection
LateralVelocity
AppliedForce
AppliedTorque
```

L’obiettivo è ottenere un feeling arcade, leggibile e stabile.

## 9. Drift Behavior

Il drift si attiva quando l’angolo tra la direzione della velocity attuale e la direzione di input supera una soglia.

Variabili:

```text
DriftAngleThreshold
DriftGripMultiplier
DriftTurnMultiplier
LateralSlideForce
```

Durante il drift:

```text
Grip laterale ridotto
Rotazione più accentuata
Scivolamento laterale controllato
```

Il drift deve essere uno strumento di game feel, non una simulazione realistica.

## 10. UCarStabilityComponent

Componente dedicato alla stabilità della macchina.

Responsabilità:

```text
Track current tilt angle
Prevent full rollover
Apply upright correction
Optionally configure center of mass
Expose stability debug values
```

Variabili principali:

```text
MaxTiltAngle
UprightCorrectionStrength
UprightCorrectionSpeed
CenterOfMassOffset
```

Valore iniziale di riferimento:

```text
MaxTiltAngle = 80 degrees
```

Se la macchina supera la soglia, il sistema deve correggere la rotazione o applicare una forza/torque di stabilizzazione.

## 11. Camera System

La camera è fissa, inclinata dall’alto, e segue la posizione della macchina senza ruotare con essa.

Setup iniziale:

```text
CameraBoom = SpringArmComponent
Camera = CameraComponent
CameraRotation = Fixed
FollowPlayerPosition = True
FollowPlayerRotation = False
```

La prima implementazione userà il camera lag dello SpringArm.

Possibile estensione futura:

```text
ULazyFollowCameraComponent
```

Questa estensione verrà valutata solo se il camera lag standard non sarà sufficiente.

## 12. Lazy Follow Direction

Il comportamento desiderato della camera è:

```text
Camera follows car position smoothly
Camera does not react instantly to every small movement
Camera does not rotate with car
```

Prima soluzione:

```text
SpringArm camera lag
```

Soluzione futura, se necessaria:

```text
Dead zone / lazy follow custom component
```

## 13. UCarTuningData

Data asset usato per raccogliere i valori principali di tuning della macchina.

Contenuto previsto:

```text
Acceleration
MaxSpeed
TurnStrength
GripStrength
LateralDamping
DriftAngleThreshold
DriftGripMultiplier
DriftTurnMultiplier
LateralSlideForce
MaxTiltAngle
UprightCorrectionStrength
CameraLagSpeed
CameraDistance
CameraAngle
```

Obiettivo:

```text
Avoid hardcoded tuning values
Allow fast iteration in editor
Keep car behavior configurable
```

## 14. UCarDebugComponent

Componente di debug per aiutare il tuning della macchina.

Responsabilità:

```text
Display car speed
Display input direction
Display velocity direction
Display input/velocity angle
Display drift state
Display current grip
Display tilt angle
Draw debug vectors in world
```

Debug values utili:

```text
Speed
InputDirection
VelocityDirection
InputVelocityAngle
IsDrifting
CurrentGrip
TiltAngle
AngularVelocity
```

Debug draw utile:

```text
Forward direction arrow
Velocity direction arrow
Input direction arrow
```

Il debug tool è parte della fase MVP perché serve a validare il comportamento fisico e il feeling della macchina.

## 15. Suggested Folder Structure

```text
Source/CrazyCar/
  Core/
    CrazyCarGameMode
    CrazyCarPlayerController

  Vehicle/
    CrazyCarPawn
    ArcadeCarMovementComponent
    CarStabilityComponent
    CarTuningData
    CarDebugComponent
```

Content structure indicativa:

```text
Content/
  Blueprints/
    BP_CrazyCarPawn
    BP_CrazyCarGameMode
    BP_CrazyCarPlayerController

  Input/
    IA_Move
    IA_MouseMove
    IA_ResetCar
    IMC_Player

  Data/
    DA_CarTuning_Default

  Maps/
    L_Test_Car
    L_Prototype_Blockout
```

## 16. Implementation Order

Ordine consigliato:

```text
1. Create ACrazyCarPawn
2. Add physical car body
3. Add camera boom and camera
4. Create ACrazyCarPlayerController
5. Setup Enhanced Input assets
6. Implement controller movement input
7. Implement mouse direction input
8. Create UArcadeCarMovementComponent
9. Add acceleration and max speed
10. Add turning behavior
11. Add drift detection
12. Add drift response
13. Create UCarStabilityComponent
14. Add anti-flip correction
15. Create UCarTuningData
16. Move tuning values into data asset
17. Create UCarDebugComponent
18. Add debug values and debug vectors
```

## 17. Validation Goals

La fase car controller è considerata valida quando:

```text
The car can be controlled with controller input
The car can be controlled with mouse input
The camera follows smoothly without rotating
The car feels arcade and responsive
The car can drift in a controllable way
The car does not flip uncontrollably
Main tuning values are editable through data asset
Debug information is available in editor/play mode
```

## 18. Future Systems Placeholder

Dopo la validazione del car controller verranno progettati sistemi separati per:

```text
Physics props
Object interaction
Small destructible structures
Semi-physical building destruction
Game mode / scoring, if needed
```

Questi sistemi non vengono definiti in questo documento per evitare architettura prematura.

## 19. Current Technical Priority

```text
Implement and validate the arcade car controller.
```

Tutto il resto del progetto dipende dal fatto che la macchina sia piacevole da guidare.