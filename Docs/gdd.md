# Crazy Car — GDD v0.2

## 1. Concept

**Crazy Car** è una tech demo physics-based in Unreal Engine.
Il player controlla una macchina cartoon low poly in una piccola area urbana e interagisce con l’ambiente tramite fisica, collisioni e distruzione arcade.

Il focus è:

```text
Arcade car feel
Physical interaction
Small destructible environment
```

Non è un gioco completo con missioni, punteggio o progressione.

## 2. Camera

Camera fissa inclinata dall’alto.

```text
CameraType = Fixed Angled Top-Down
FollowsPlayerPosition = True
FollowsPlayerRotation = False
```

La camera può seguire la posizione della macchina, ma non ruota con essa.

## 3. Controls

La macchina supporta due sistemi di input.

```text
Controller Left Stick = Movement Direction
Left Mouse Button = Move toward mouse direction
```

Nel caso del mouse, la direzione di movimento viene calcolata dalla posizione della macchina verso il cursore nel mondo.

## 4. Car Feel

La macchina deve avere un comportamento arcade, non simulativo.

Obiettivi:

```text
Responsive movement
Limited max speed
Readable turning
Controlled drift
High stability
```

La macchina non deve ribaltarsi oltre una soglia massima.

```text
MaxTiltAngle = 80 degrees
```

Se supera la soglia, viene corretta automaticamente.

## 5. Drift

Il drift si attiva quando l’angolo tra la direzione della velocity attuale e la direzione di input supera una certa soglia.

```text
DriftAngleThreshold
DriftGripMultiplier
DriftRotationSpeed
LateralSlideForce
```

Durante il drift:

```text
Grip laterale ridotto
Rotazione più accentuata
Movimento laterale leggero
```

Il drift deve servire al feeling arcade, non al realismo.

## 6. Environment

La mappa è una micro-area urbana low poly.

```text
BuildingsCount = 6-7
VisualStyle = Low Poly / Cartoon
AssetDirection = POLYGON-style pack
```

La mappa deve avere abbastanza spazio per:

```text
Guidare
Curvare
Driftare
Colpire props
Testare distruzione
```

## 7. Physics Interaction

Gli oggetti ambientali sono regolati principalmente da proprietà fisiche.

Variabili utili:

```text
Mass
LinearDamping
AngularDamping
ImpactThreshold
PhysicsEnabledOnHit
CanDetach
CanCollapse
```

Non serve una classificazione rigida dei props.
Ogni oggetto viene bilanciato tramite massa, collisione e soglie di impatto.

## 8. Destruction

La distruzione deve essere arcade e controllata.

Principi:

```text
Pochi pezzi grandi
Niente micro-frammenti inutili
Collisioni leggibili
Performance prima del realismo
```

Gli elementi distruttibili possono essere composti da macro-moduli:

```text
DoorPanel
WallStrip
FenceSegment
WindowSection
SupportPiece
FloorBlock
```

Ogni modulo può rompersi, staccarsi o diventare fisico in base all’impatto ricevuto.

## 9. Buildings

Gli edifici sono principalmente scenario, ma alcuni elementi possono essere interattivi o distruttibili.

MVP:

```text
Porte distruttibili
Fence distruttibili
Sezioni di muro interagibili
Piccole strutture fisiche
```

Stretch goal:

```text
Semi-Physical Building Destruction
```

In questo sistema, gli edifici sono composti da macro-cluster: piani, pareti, supporti e sezioni distruttibili. I pezzi restano uniti finché collisioni o instabilità non li separano.

Se questa feature diventa troppo complessa, il progetto resta valido anche senza palazzi collassabili.

## 10. Game Mode

Per la prima versione:

```text
Free Physics Sandbox
```

Il player guida e sperimenta liberamente con fisica e distruzione.

Feature future possibili:

```text
Score System
Timed Challenge
Combo System
Destruction Targets
```

## 11. MVP

Il prototipo minimo deve includere:

```text
Arcade car controller
Camera fissa inclinata
Controller input
Mouse input
Drift system
Anti-flip stabilization
Piccola mappa urbana low poly
Props fisici
Almeno una struttura semplice distruttibile
```

## 12. Stretch Goals

```text
Semi-Physical Building Destruction
Score System
Timed Challenge
Impact VFX
Skid Marks
Slow Motion Impact
Advanced Audio Feedback
```

## 13. Main Risks

```text
Car feel poco divertente
Fisica instabile
Distruzione troppo complessa
Troppi rigidbody attivi
```

Mitigazione principale:

```text
Prototipare prima la macchina
Tenere la mappa piccola
Usare pochi pezzi grandi
Considerare i palazzi collassabili come stretch goal
```

## 14. Current Priority

```text
Build the Arcade Car Controller
```

Prima di lavorare sulla distruzione, la macchina deve essere divertente da guidare.