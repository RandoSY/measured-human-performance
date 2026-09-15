# FFT Heart-Rate Monitor Simulator — micro:bit V2

**Status:** `validated / active lineage`

**Current verified source:** [RandoSY/FFT-HRM-Simulator-Microbit-V2](https://github.com/RandoSY/FFT-HRM-Simulator-Microbit-V2)

**License:** MIT, copyright 2026 Randall Young.

This component turns a BBC micro:bit V2 into a controllable Bluetooth Heart Rate Service simulator for testing FFT and other compatible BLE clients. It generates laboratory test data; it does not measure a person.

## Why it belongs here

This is shared FFT test infrastructure. It provides a reproducible synthetic input path so scanning, connection, subscription, BPM display, threshold behavior, and contact-loss behavior can be tested without requiring a person to exercise.

## Verified legacy-repository evidence

The existing repository documents:

- BBC micro:bit V2 / V2.21 target hardware;
- standard Bluetooth Heart Rate Service (`0x180D`);
- adjustable simulated rate from 30–220 BPM;
- button and serial control;
- contact-loss simulation;
- host-side packet/regression tests;
- a successful build, physical flash, and live hardware test;
- MIT licensing.

## Migration disposition

For now, **do not duplicate or delete the verified legacy source**. Treat the existing repository as the known-good source package and this directory as its canonical estate registration.

A later consolidation may copy the complete source tree here in one integrity-preserving operation. That migration should preserve history or record the source commit, retain the MIT license, and rerun the documented tests before this path is declared the new verified source.

## Next action

Use this simulator as part of the FFT reference-release acceptance test, then record the exact simulator release/commit used by the app test path.
