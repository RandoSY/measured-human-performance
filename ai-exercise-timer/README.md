# micro:bit V2 Arduino/NimBLE AI Exercise Timer

A small, inspectable physical-AI project for the **BBC micro:bit V2**.

This project takes the educational idea behind the micro:bit Foundation's **Simple AI exercise timer / CreateAI** workflow and rebuilds the instrument around **Arduino IDE + NimBLE-Arduino**:

```text
micro:bit V2 accelerometer
        |
        v
Arduino firmware + NimBLE
        |
        v
Nordic UART Service BLE
        |
        v
Python host
  |          |            |
  v          v            v
collect   train ML     recognize/timer
```

The central design decision is simple: the micro:bit is a **general wireless motion instrument**, not a one-purpose AI appliance. Raw x/y/z motion remains available while training and classification can be inspected, replaced, compared, or improved without reflashing the sensor node.

## Included

- Arduino IDE firmware for BBC micro:bit V2 / nRF52833
- NimBLE-Arduino peripheral using Nordic UART Service (NUS)
- automatic detection of either micro:bit V2 motion sensor: ST LSM303AGR or NXP FXOS8700CQ
- human-readable ASCII acceleration packets
- adjustable 10–100 Hz BLE sampling; 50 Hz default
- CreateAI-style labeled **1-second samples**
- 10-second continuous capture split into ten 1-second samples
- transparent statistical features
- Random Forest classification with per-class probabilities
- recognition threshold with explicit `unknown`
- alternate exercise/rest timer workflow
- protocol, teaching notes, third-party attribution, and validation checklist

## Why this exists

The official CreateAI workflow is excellent for teaching the machine-learning loop:

```text
measure -> label -> train -> test -> inspect failures -> retrain -> recognize
```

This implementation preserves that loop while exposing the measurement and transport layer. The same BLE stream can later feed gait analysis, rep analysis, visualization, threshold algorithms, or a completely different model.

It also encourages a crucial engineering question:

> Does this problem really need machine learning, or can a conventional algorithm solve it adequately?

## Status

**Reference/experimental release.**

The Python scripts have passed syntax checks, and the feature-extraction/training path has passed a synthetic-data smoke test. The firmware and end-to-end BLE path still require real micro:bit V2 hardware qualification before this release should be described as hardware-validated. See `VALIDATION.md`.

This is an educational/research movement-classification project. It is **not a medical device**, diagnostic system, or exercise-prescription system.

## Arduino setup

Add this Boards Manager URL:

```text
https://h2zero.github.io/n-able-Arduino/package_n-able_boards_index.json
```

Install:

1. **Arm BLE Boards** / h2zero n-able Arduino core
2. **NimBLE-Arduino** 2.x

Select:

- Board: **BBC micro:bit V2**
- BLE role: **Peripheral**
- Bootloader: **No bootloader** for the direct CMSIS-DAP/OpenOCD path used here

Open and upload:

```text
firmware/microbit_v2_nimble_streamer/microbit_v2_nimble_streamer.ino
```

The device advertises as `MB2-AI-TIMER`.

## BLE measurement format

The micro:bit sends newline-terminated ASCII:

```text
A,x_mg,y_mg,z_mg
```

Example: `A,-32,101,987`

Units are **mg**. The packet is intentionally compact enough to fit the normal 20-byte ATT notification payload at the configured ±2 g range.

Commands:

```text
I       information
P       ping
S,1     start streaming
S,0     stop streaming
R,50    set 50 Hz
```

See `PROTOCOL.md` for UUIDs and details.

## Python setup

Python 3.10+ recommended.

```bash
cd host
python -m venv .venv
```

Windows:

```powershell
.venv\Scripts\activate
pip install -r requirements.txt
```

macOS/Linux:

```bash
source .venv/bin/activate
pip install -r requirements.txt
```

## Primary workflow: 1-second labeled samples

Collect:

```bash
python collect_samples.py --out createai_samples.csv
```

Example session:

```text
exercise> add rest
rest> ten
rest> add exercise
exercise> ten
exercise> quit
```

Commands: `add NAME`, `one`, `ten`, `cont10`, `list`, `info`, `quit`.

Start with at least three samples per action; use more for a useful model. Record real variation rather than many nearly identical examples.

Train:

```bash
python train_samples.py createai_samples.csv --out movement_model.joblib
```

Each one-second sample becomes transparent numerical features for x, y, z, and vector magnitude: mean, standard deviation, minimum, maximum, range, RMS, and sample-to-sample difference RMS.

Recognize:

```bash
python recognize_samples.py --model movement_model.joblib --threshold 0.70
```

If no class exceeds the recognition threshold the result is `unknown`. A classifier should be allowed to say “I do not know.”

## Alternate exercise/rest timer

```bash
python collect.py --out exercise_data.csv
python train.py exercise_data.csv --out exercise_model.joblib
python live_timer.py --model exercise_model.joblib
```

## File map

```text
ai-exercise-timer/
├── README.md
├── LICENSE
├── PROTOCOL.md
├── TEACHING.md
├── THIRD_PARTY.md
├── VALIDATION.md
├── .gitignore
├── firmware/microbit_v2_nimble_streamer/microbit_v2_nimble_streamer.ino
└── host/
    ├── collect_samples.py
    ├── train_samples.py
    ├── recognize_samples.py
    ├── collect.py
    ├── train.py
    ├── live_timer.py
    └── requirements.txt
```

## Relationship to CreateAI

This project is **inspired by the public educational workflow**, not a port of CreateAI source code and not a claim of algorithmic equivalence.

References:

- https://microbit.org/projects/make-it-code-it/simple-ai-exercise-timer/
- https://createai.microbit.org/data-samples
- https://github.com/microbit-foundation/ml-trainer

The preprocessing and classifier here are intentionally explicit so learners can see and modify what happens between measurement and decision.

## Upstream BLE references

- https://github.com/h2zero/n-able-Arduino
- https://github.com/h2zero/NimBLE-Arduino

## License

Original code and documentation in this project are released under the **MIT License**. Third-party projects and referenced educational material remain under their own licenses. See `THIRD_PARTY.md`.
