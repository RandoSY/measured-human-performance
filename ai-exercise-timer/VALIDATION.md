# Validation status

## Checked

- Host Python scripts passed `py_compile` during package construction.
- Feature extraction and training were exercised with synthetic rest/exercise accelerometer data.
- The public repository contains the Arduino/NimBLE firmware, host tools, 5x5 state vocabulary, and MicroBlocks companion source.

## Not yet claimed

This publication does **not** claim completed physical qualification of either full path:

```text
micro:bit sensor -> Arduino/NimBLE -> BLE -> host -> model -> live recognition
```

or

```text
micro:bit sensor -> MicroBlocks -> BLE -> host -> model -> live recognition
```

The MicroBlocks `.ubp` source was constructed against current documented MicroBlocks project syntax and primitives, but has not yet been imported and exercised on a physical micro:bit V2 in this publication workflow.

## Arduino/NimBLE minimum hardware qualification

- [ ] Arduino sketch builds with the selected n-able core and required libraries.
- [ ] Upload succeeds on BBC micro:bit V2.
- [ ] Device advertises as `MB2-AI-TIMER`.
- [ ] 5x5 display shows advertising beacon before connection and ready/check after connection.
- [ ] Host connects through Nordic UART Service.
- [ ] `I` reports `LSM303AGR` or `FXOS8700`.
- [ ] Stationary vector magnitude is approximately 1 g.
- [ ] Rotation produces plausible axis changes.
- [ ] 50 Hz streaming runs for 10 minutes without malformed lines/disconnect.
- [ ] `S,0` and `S,1` work.
- [ ] `R,10`, `R,50`, and `R,100` are exercised.
- [ ] `D,READY`, `D,STREAM`, `D,STOP`, `D,RECORD,E`, `D,CLASS,E`, `D,UNKNOWN`, and `D,ERROR` render correctly.
- [ ] Recording display alternates between record circle and class character without disrupting BLE streaming.
- [ ] `collect_samples.py` records valid one-second samples.
- [ ] A two-class model trains.
- [ ] Live recognition changes state during obvious movement/rest transitions.
- [ ] Increasing threshold produces more `unknown` results.
- [ ] Disconnect/reconnect is tested.

Complete sensor coverage should test both micro:bit V2 accelerometer variants when available: ST LSM303AGR and NXP FXOS8700CQ.

## MicroBlocks minimum hardware qualification

- [ ] `ai_exercise_timer_microblocks.ubp` imports into a current MicroBlocks IDE.
- [ ] Project runs on BBC micro:bit V2.
- [ ] BLE Serial/NUS is visible to the host after the IDE connection is released.
- [ ] `--name MicroBlocks` finds the advertised board using host prefix matching.
- [ ] Stationary x/y/z values correspond to approximately 1 g total after conversion to mg.
- [ ] `S,1`, `S,0`, `R,25`, and display commands work.
- [ ] 25 Hz stream remains stable for 10 minutes.
- [ ] Higher requested rates are measured before being described as supported.
- [ ] One-second labeled samples can be collected and trained with the same host scripts.
- [ ] Live class/unknown results return to the 5x5 display.
- [ ] IDE/USB/BLE connection behavior is documented from actual hardware use.

## Generalization boundary

A model working for its training subject does not establish generalization. Test different people, placements, orientations, speeds, and untrained motions.

This project is for education/research and does not diagnose, prescribe exercise, or establish safety for a particular person.
