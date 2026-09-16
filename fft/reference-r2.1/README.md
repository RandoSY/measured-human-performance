# Fair Fitness Tester FFT+ — recovered R2.1 reference packet

This directory preserves the strongest surviving public-safe FFT+ release material recovered from the Intellectual Estate.

## What is here

- `docs/FFTPlus_R2.1_Operators_Guide.recovered.md.gz` — lossless gzip archive of the recovered text edition of the 43-page FFT+ R2.1 Operator Guide.
- `dashboard/fair_fitness_console_R2.1.0-G1.html.gz` — exact recovered single-file browser dashboard source.
- `firmware-history/FairFitFFTPlus_R2.0.2_main.cpp.gz` — exact recovered predecessor firmware source.
- `firmware-history/platformio_R2.0.2.ini.gz` — recovered PlatformIO configuration for the R2.0.2 predecessor.
- `firmware-history/STACK_OVERFLOW_FIX.md.gz` — recovery evidence for the R2.0.2 connection-time stack fix.
- `START_HERE_R2.txt.gz` — recovered first-session and smoke-test instructions.
- `android-prototype/README_20260823.md.gz` — recovered Android prototype build/safety notes.

## Important version boundary

The surviving browser console and Operator Guide identify the management layer as **R2.1.0-G1** and describe paired **FairFitFFTPlus-R2.1.0** firmware. The strongest standalone firmware source recovered so far identifies itself as **FairFitFFTPlus-R2.0.2**. It is therefore preserved here as historical predecessor source, not silently relabeled as R2.1 firmware.

An exact R2.1 firmware source file remains a forensic target. Until it is recovered, this packet should not be described as a byte-complete paired R2.1 release.

## Recovered operating path

The recovered material documents this path:

1. BBC micro:bit V2.21 / nRF52833 runs the local tester firmware.
2. A standard BLE Heart Rate Service monitor connects to the micro:bit, which acts as BLE central for HRS.
3. A Chromium browser connects to the micro:bit Nordic UART Service, with the micro:bit acting as NUS peripheral.
4. The browser sends configuration and displays status; workload timing, metronome cues, movement verification, and safety-state decisions remain local to the micro:bit.
5. The R2.1 dashboard includes configuration, preflight, calibration, arm/start, stop/abort, HRM status, trial data, CSV/JSON/transcript export, and a browser-only demo stream.

The historical PlatformIO build uses the h2zero n-able Arduino platform and NimBLE-Arduino on the BBC micro:bit V2.21 target. The recovered R2.0.2 configuration also documents the increased main task stack and static transmit formatting buffer used to address a connection-time stack overflow.

## Validation and safety status

This is preservation of an experimental educational/research system. The recovered guide explicitly states that FFT+ is **not a medical device** and that symptoms, participant request, operator judgment, and an approved external protocol override numeric readings.

Recovered package validators and simulator paths are useful engineering evidence, but they do not replace physical bench qualification, real-device testing, protocol review, or supervised human-use validation. No personal participant records are included in this archive.

## Reconstruction

Files ending in `.gz` are gzip-compressed exact recovered text/source bytes. Restore, for example:

```sh
gzip -dc dashboard/fair_fitness_console_R2.1.0-G1.html.gz > fair_fitness_console.html
```

The original Library copies remain preserved separately. This GitHub packet is the public canonical recovery home.