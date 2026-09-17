# Measured Human Performance

This repository is the canonical public home for the human-performance measurement family: **FFT, SQM+, Tone Trainer, PlateLab lineage, and shared measurement infrastructure**.

The emphasis is transparent measurement rather than opaque scoring. Raw observations should remain inspectable, derived metrics should be explainable, and a system should state what it does and does not claim.

## Project family

### FFT — Fair Fitness Testing

FFT is the reference application for guided walking/fitness testing. Its architecture combines a requested work level, confirmation that the activity actually occurred, and live monitoring intended to keep the session within defined limits.

### SQM+

SQM+ explores movement quality from inexpensive inertial sensing. Representative dimensions include cadence stability, forward smoothness, lateral stability, yaw stability, jerk, and symmetry. Composite scores should remain traceable to the underlying measurements.

### Tone Trainer

Tone Trainer uses inexpensive motion sensing with pacing feedback for repeatable machine-based exercise. The goal is to make rep timing and consistency visible without requiring specialized gym equipment.

### AI Exercise Timer — Arduino/NimBLE + transparent ML

**[Open the complete AI Exercise Timer project](ai-exercise-timer/)**

This is a complete BBC micro:bit V2 physical-AI example inspired by the micro:bit Foundation CreateAI exercise-timer workflow, rebuilt with **Arduino IDE + NimBLE-Arduino**. The micro:bit streams raw x/y/z acceleration over BLE; a Python host collects labeled one-second examples, trains an inspectable classifier, applies a confidence threshold, and reports a known action or `unknown`.

The directory includes firmware, protocol documentation, sample collection, model training, live recognition, an exercise/rest timer, teaching notes, licensing, third-party references, and a hardware-validation checklist.

### PlateLab

PlateLab is related work in low-cost measurement and behavior feedback around food mass and daily budgets. It remains part of the broader APPS lineage even where its technical components overlap Measured World Lab.

## Shared infrastructure

Where practical, these applications should reuse:

- BLE/USB transport patterns;
- simple human-readable measurement messages;
- sensor simulation for testing;
- common device/session setup patterns;
- explicit communication-loss behavior;
- reproducible synthetic test paths.

## Repository rules

- Keep measurement and interpretation distinguishable.
- Document units and transformations.
- Preserve test simulators where they make reproduction possible.
- Do not present experimental movement metrics as clinical diagnosis.
- Keep application-specific logic separate from shared transport and sensor infrastructure.

## Planned repository structure

- `fft/`
- `sqm-plus/`
- `tone-trainer/`
- `platelab/`
- `shared/`
- `simulators/`
- `docs/`
- `validation/`

## Current state

**Lifecycle:** `active`

FFT is the P1 reference application for this family. Existing app source, embedded sensor code, protocols, simulators, and documentation still need to be migrated and verified before this repository can be treated as a complete reproducible release.
