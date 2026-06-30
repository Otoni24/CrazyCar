# Crazy Car — Roadmap v0.1

## Phase 1: Arcade Car Controller

## Goal

Completare la prima versione giocabile della macchina arcade:

```text
Controller input
Mouse input
Arcade movement
Drift
Anti-flip stability
Camera follow
Debug tools
Tuning data
```

Questa roadmap non include props fisici, distruzione, edifici o scoring.

---

# Milestone 0 — Project Baseline

## Obiettivo

Preparare il progetto per lavorare in modo ordinato prima di scrivere logica gameplay.

## Tasks

```text
0.1 Verificare Git, .gitignore e Git LFS
0.2 Creare struttura cartelle Source/CrazyCar
0.3 Creare struttura cartelle Content base
0.4 Creare una test map minimale L_Test_Car
0.5 Impostare GameMode e PlayerController di default
```

## Output atteso

```text
Progetto compilabile
Mappa test avviabile
Cartelle ordinate
Nessuna logica gameplay complessa ancora implementata
```

## Review checklist

```text
Il progetto compila senza errori
Git non traccia file temporanei Unreal
La test map si apre correttamente
Il GameMode custom è assegnato
```

---

# Milestone 1 — Core Gameplay Classes

## Obiettivo

Creare le classi base del sistema macchina senza implementare ancora il movimento completo.

## Tasks

```text
1.1 Creare ACrazyCarGameMode
1.2 Creare ACrazyCarPlayerController
1.3 Creare ACrazyCarPawn
1.4 Creare UArcadeCarMovementComponent
1.5 Creare UCarStabilityComponent
1.6 Creare UCarTuningData
1.7 Creare UCarDebugComponent
```

## Output atteso

```text
Tutte le classi principali esistono
ACrazyCarPawn possiede i componenti previsti
Il progetto compila
Nessuna logica complessa ancora necessaria
```

## Review checklist

```text
I nomi delle classi sono coerenti
Le responsabilità sono separate
Il Pawn non contiene formule di movimento complesse
I componenti sono creati e referenziati correttamente
Il codice non contiene classi speculative fuori scope
```

---

# Milestone 2 — Car Pawn Physical Setup

## Obiettivo

Avere una macchina fisica presente nel mondo, controllabile come Pawn ma non ancora guidabile.

## Tasks

```text
2.1 Aggiungere CarBody / PhysicsBody al Pawn
2.2 Configurare collisione base
2.3 Abilitare Simulate Physics sul body principale
2.4 Impostare massa e damping iniziali
2.5 Creare BP_CrazyCarPawn per assegnare mesh e asset
2.6 Piazzare la macchina nella test map
```

## Output atteso

```text
La macchina appare nella mappa
La macchina è un Pawn posseduto dal player
Il corpo fisico reagisce alla gravità/collisione
```

## Review checklist

```text
Il Pawn viene posseduto correttamente
La mesh non è scollegata dal body fisico
La collisione è semplice e stabile
La macchina non usa Chaos Vehicle
La macchina non si muove ancora tramite input
```

---

# Milestone 3 — Camera Follow Base

## Obiettivo

Implementare la camera fissa inclinata con follow position e senza follow rotation.

## Tasks

```text
3.1 Aggiungere SpringArmComponent al Pawn
3.2 Aggiungere CameraComponent al Pawn
3.3 Impostare rotazione fissa della camera
3.4 Disabilitare inheritance della rotazione dal Pawn
3.5 Abilitare Camera Lag dello SpringArm
3.6 Esporre CameraDistance, CameraAngle e CameraLagSpeed
```

## Output atteso

```text
La camera segue la macchina
La camera non ruota con la macchina
Il follow è morbido
```

## Review checklist

```text
La camera resta leggibile durante rotazioni rapide
La camera non eredita roll/pitch/yaw del body fisico
I valori principali sono editabili
Il sistema non richiede ancora un LazyFollow custom component
```

---

# Milestone 4 — Enhanced Input Setup

## Obiettivo

Preparare input controller, mouse e reset car.

## Tasks

```text
4.1 Creare IMC_Player
4.2 Creare IA_Move
4.3 Creare IA_MouseMove
4.4 Creare IA_ResetCar
4.5 Collegare Enhanced Input nel PlayerController
4.6 Inviare input direzionale al Pawn
4.7 Implementare API SetMoveInput sul Pawn
```

## Output atteso

```text
Il PlayerController riceve input
Il Pawn riceve DesiredMoveDirection
Il movimento fisico non è ancora necessario
```

## Review checklist

```text
L’input non viene letto direttamente dal MovementComponent
Il PlayerController non applica forze fisiche
Il Pawn espone una API semplice per ricevere input
Controller e mouse restano separati a livello di lettura input
```

---

# Milestone 5 — Basic Arcade Movement

## Obiettivo

Implementare il primo movimento fisico arcade: accelerazione, velocità massima e sterzata base.

## Tasks

```text
5.1 UArcadeCarMovementComponent legge DesiredMoveDirection
5.2 Applicare forza di accelerazione al PhysicsBody
5.3 Limitare MaxSpeed
5.4 Applicare rotazione/torque verso la direzione desiderata
5.5 Aggiungere damping laterale base
5.6 Esporre valori base in UCarTuningData
```

## Output atteso

```text
La macchina si muove con input controller
La macchina ha velocità massima
La macchina ruota verso la direzione di input
Il feeling è arcade, non simulativo
```

## Review checklist

```text
Il movimento è nel MovementComponent
Il Pawn resta leggero
I valori non sono hardcoded in modo definitivo
La macchina non accelera all’infinito
La macchina resta controllabile a bassa velocità
```

---

# Milestone 6 — Mouse Movement

## Obiettivo

Aggiungere movimento tramite mouse verso la posizione del cursore nel mondo.

## Tasks

```text
6.1 Calcolare mouse world position dal PlayerController
6.2 Calcolare direzione CarPosition → MouseWorldPosition
6.3 Normalizzare la direzione
6.4 Inviare la direzione al Pawn quando Left Mouse Button è attivo
6.5 Gestire fallback quando il mouse non colpisce il piano di gioco
```

## Output atteso

```text
Tenendo/cliccando Left Mouse Button la macchina si muove verso il cursore
Il sistema funziona con camera inclinata fissa
```

## Review checklist

```text
Il mouse input non rompe il controller input
La direzione è calcolata in world space
La componente Z viene ignorata o gestita correttamente
Il comportamento resta leggibile con camera inclinata
```

---

# Milestone 7 — Drift Detection

## Obiettivo

Rilevare correttamente quando la macchina deve entrare in drift.

## Tasks

```text
7.1 Calcolare CurrentVelocity
7.2 Calcolare VelocityDirection
7.3 Calcolare InputDirection
7.4 Calcolare InputVelocityAngle
7.5 Confrontare con DriftAngleThreshold
7.6 Esporre IsDrifting come debug value
```

## Output atteso

```text
Il sistema riconosce lo stato di drift
Il drift non modifica ancora necessariamente il movimento
```

## Review checklist

```text
Il drift non si attiva da fermo in modo indesiderato
L’angolo è stabile e leggibile
Le soglie sono modificabili
Il debug mostra chiaramente InputVelocityAngle e IsDrifting
```

---

# Milestone 8 — Drift Response

## Obiettivo

Rendere il drift percepibile nel movimento della macchina.

## Tasks

```text
8.1 Ridurre grip laterale durante drift
8.2 Aumentare turn response durante drift
8.3 Applicare lateral slide controllato
8.4 Aggiungere transizione morbida tra grip normale e drift grip
8.5 Esporre valori drift in UCarTuningData
```

## Output atteso

```text
La macchina può driftare in modo controllabile
Il drift è arcade e leggibile
Il player mantiene controllo durante la scivolata
```

## Review checklist

```text
Il drift non sembra casuale
La macchina non diventa incontrollabile
Il drift non genera velocità laterale infinita
Le transizioni non sono troppo brusche
Il comportamento è tunabile senza ricompilare
```

---

# Milestone 9 — Stability / Anti-Flip

## Obiettivo

Impedire alla macchina di ribaltarsi in modo incontrollato.

## Tasks

```text
9.1 UCarStabilityComponent legge rotazione del PhysicsBody
9.2 Calcolare TiltAngle rispetto all’asse up del mondo
9.3 Applicare correzione se TiltAngle supera soglia
9.4 Esporre MaxTiltAngle
9.5 Esporre UprightCorrectionStrength
9.6 Configurare eventuale CenterOfMassOffset
```

## Output atteso

```text
La macchina può inclinarsi ma non ribaltarsi completamente
Se supera la soglia, viene riportata verso una rotazione stabile
```

## Review checklist

```text
La correzione non teletrasporta la macchina in modo evidente
La macchina non vibra continuamente
Il sistema non annulla completamente il feeling fisico
MaxTiltAngle è rispettato
Il comportamento è stabile anche dopo collisioni forti
```

---

# Milestone 10 — Tuning Data Integration

## Obiettivo

Spostare i valori principali in un Data Asset modificabile in editor.

## Tasks

```text
10.1 Finalizzare UCarTuningData
10.2 Creare DA_CarTuning_Default
10.3 Collegare il Data Asset al Pawn o ai componenti
10.4 Rimuovere valori duplicati non necessari
10.5 Verificare che movement, drift, stability e camera leggano i valori corretti
```

## Output atteso

```text
Il car feel è configurabile da editor
I valori principali non sono sparsi in più classi
```

## Review checklist

```text
Esiste una sola fonte principale per il tuning
I componenti gestiscono correttamente un Data Asset mancante
I valori hanno default sensati
Il tuning non richiede modifiche di codice
```

---

# Milestone 11 — Car Debug Tools

## Obiettivo

Aggiungere strumenti di debug per revisionare e bilanciare il comportamento della macchina.

## Tasks

```text
11.1 UCarDebugComponent legge dati dal MovementComponent
11.2 UCarDebugComponent legge dati dallo StabilityComponent
11.3 Mostrare speed
11.4 Mostrare input direction
11.5 Mostrare velocity direction
11.6 Mostrare input/velocity angle
11.7 Mostrare IsDrifting
11.8 Mostrare tilt angle
11.9 Disegnare debug arrows nel mondo
11.10 Aggiungere toggle debug on/off
```

## Output atteso

```text
Durante Play In Editor è possibile capire cosa sta facendo la macchina
Il debug aiuta il tuning del car feel
```

## Review checklist

```text
Il debug è disattivabile
Il debug non contiene logica gameplay
I valori mostrati corrispondono ai dati reali
Le frecce debug sono leggibili
Il codice debug è separato dal movement
```

---

# Milestone 12 — Car Controller Validation

## Obiettivo

Chiudere la prima fase con una macchina guidabile, stabile e revisionabile.

## Tasks

```text
12.1 Test controller input
12.2 Test mouse input
12.3 Test drift a bassa/media/alta velocità
12.4 Test anti-flip
12.5 Test camera follow
12.6 Tuning iniziale di DA_CarTuning_Default
12.7 Pulizia codice
12.8 Rimozione log temporanei inutili
```

## Output atteso

```text
La macchina è piacevole da guidare
La camera è leggibile
Il drift è controllabile
La macchina non si ribalta in modo problematico
Il codice è pronto per la fase successiva
```

## Review checklist finale

```text
Controller input funziona
Mouse input funziona
Camera follows position only
Camera does not follow rotation
Movement logic is inside UArcadeCarMovementComponent
Stability logic is inside UCarStabilityComponent
Debug logic is inside UCarDebugComponent
Main values are inside UCarTuningData
No destruction/props architecture has been introduced yet
```

---

# Codex Task Guidelines

Ogni task dato a Codex dovrebbe rispettare queste regole:

```text
One milestone or sub-milestone at a time
No unrelated systems
No destruction or props classes yet
No large refactors unless requested
Prefer small compile-safe changes
Keep logic in the intended class/component
Expose tuning values where useful
Do not hardcode final car feel values
```

Esempio di prompt valido:

```text
Implement Milestone 5 only. Add basic arcade movement to UArcadeCarMovementComponent using the existing ACrazyCarPawn PhysicsBody. Do not add drift yet. Do not add destruction/props systems. Keep values exposed through UCarTuningData where possible.
```

Esempio di prompt da evitare:

```text
Implement the whole car controller, camera, drift, props, destruction and debug system.
```

---

# Phase 1 Exit Criteria

La Phase 1 è completa quando:

```text
La macchina è guidabile con controller
La macchina è guidabile con mouse
La camera segue correttamente senza ruotare
Il drift funziona ed è controllabile
La macchina non si ribalta in modo problematico
I valori principali sono tunabili da Data Asset
Il debug tool mostra i dati necessari
Il codice è abbastanza pulito da iniziare la fase props/physics interaction
```