# Validation status

## Checked

- Host Python scripts passed `py_compile`.
- Feature extraction and training were exercised with synthetic rest/exercise accelerometer data.

## Not yet claimed

This publication does not claim completed hardware qualification of:

`micro:bit sensor -> Arduino firmware -> NimBLE -> BLE -> host -> model -> live recognition`

## Minimum hardware qualification

- [ ] Arduino sketch builds with the selected n-able core.
- [ ] Upload succeeds on BBC micro:bit V2.
- [ ] Device advertises as `MB2-AI-TIMER`.
- [ ] Host connects through Nordic UART Service.
- [ ] `I` reports `LSM303AGR` or `FXOS8700`.
- [ ] Stationary vector magnitude is approximately 1 g.
- [ ] Rotation produces plausible axis changes.
- [ ] 50 Hz streaming runs for 10 minutes without malformed lines/disconnect.
- [ ] `S,0` and `S,1` work.
- [ ] `R,10`, `R,50`, and `R,100` work.
- [ ] `collect_samples.py` records valid one-second samples.
- [ ] A two-class model trains.
- [ ] Live recognition changes state during obvious movement/rest transitions.
- [ ] Increasing threshold produces more `unknown` results.
- [ ] Disconnect/reconnect is tested.

Complete sensor coverage should test both micro:bit V2 accelerometer variants when available: ST LSM303AGR and NXP FXOS8700CQ.

A model working for its training subject does not establish generalization. Test different people, placements, orientations, speeds, and untrained motions.

This project is for education/research and does not diagnose, prescribe exercise, or establish safety for a particular person.
