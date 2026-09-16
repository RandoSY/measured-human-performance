Observed technique issue: ______________________________________________________________________

DONE reason: ______________________________________________________________________

Recovery notes: ______________________________________________________________________

Validity decision: ______________________________________________________________________

Export filenames: ______________________________________________________________________

# Appendix F. Workload equations and example

How target tempo, estimated power, energy, and compliance are derived.

## Equations

## Default example

# Appendix G. R2.1 validation status and open verification

What was tested in software and what still requires the operator workstation and hardware.

## Completed package validation

43 Python/unit/regression tests for protocol, trial engine, and new R2.1 behavior.

JavaScript runtime/parser test including guided workflow, HR canonicalization, STOP/ABORT, and state transitions.

Warning-strict C++11/C++17 syntax builds against API-shaped Arduino/NimBLE/matrix stubs, matrix on and off.

Executable Heart Rate Service parser and safety-engine test.

Package file validation and SHA-256 manifest generation.

Dashboard visual QA for initial, ready, armed, prescription, safety, calibration, and running states.

## Still required on physical hardware

Real PlatformIO ARM cross-build with the pinned n-able platform on the Windows workstation.

Flash and boot verification on micro:bit V2.21.

End-to-end R2.1 browser connection and guided preflight.

Real speaker, matrix, buttons, accelerometer calibration, and controlled motion qualification.

Real dual-role connection: browser NUS plus standard HRS monitor/FFT-HRM-SIM.

No-participant validation of all stop, stale, contact, disconnect, pause/resume, export, and recovery paths.

Protocol-specific human factors review before any real participant event.

FFT+ OPERATING PRINCIPLE

Measure what matters.
Keep timing and protection local.
Make readiness visible.
Preserve the record.
Never let the display replace the operator.


## Table 1


SAFETY STATUS / Experimental educational/research instrument. Not a medical device and not a substitute for medical screening, clinical supervision, or emergency response. Use only under an approved protocol with an attentive operator. Symptoms, participant request, and operator judgment override every numeric reading.


## Table 2


Item | Value

System | Fair Fitness Tester FFT+

Firmware | FairFitFFTPlus-R2.1.0

Dashboard | R2.1.0-G1

Protocol | FAIRFIT-NUS-TEXT/2.0

Tester hardware | BBC micro:bit V2.21 / nRF52833

HR input | Standard BLE Heart Rate Service 0x180D / Measurement 0x2A37

Guide scope | Configuration, preflight, calibration, test control, live supervision, data export, troubleshooting, and qualification

Validation status | Host-side tests passed; target cross-build and physical R2.1 qualification must be completed on the operator workstation


## Table 3


USE THE PAIRED RELEASE / Do not mix an older R2.0 dashboard with R2.1 firmware. Confirm both the firmware boot line and the dashboard header before operating a real session.


## Table 4


Convention | Meaning

Button labels | Bold labels such as Send setup, Arm test, STOP, and ABORT refer to dashboard controls.

Monospace text | Commands or expected protocol records sent through NUS/serial.

Green checklist item | Condition currently accepted by the browser/firmware preflight.

Amber/red item | Action required or condition blocks ARM/START.

STOP | Controlled operator termination; recorded as a stop reason.

ABORT | Immediate operator termination; recorded separately from routine STOP.


## Table 5


Component | Role | Must continue locally if browser is busy/disconnected?

BLE HRM or FFT-HRM-SIM | Advertises standard HRS and sends 0x2A37 notifications. | Yes - it is independent of the browser.

FFT+ tester micro:bit | HRS central, NUS peripheral, metronome, accelerometer, state machine, safety enforcement, telemetry. | Yes - timing and configured protection remain local.

Chromium dashboard | Configuration, preflight guidance, live display, command administration, CSV/JSON/transcript export. | No - it is the console, not the timing engine.


## Table 6


Phenomenon -> Sensor -> Controller -> Link -> Dashboard -> Record -> Context -> Action


## Table 7


AUTHORITATIVE TIMESTAMPS / The micro:bit timestamps are authoritative for trial time, cycles, HR events, pause time, and DONE reasons. Browser receipt time is useful for logging but does not replace device time.


## Table 8


Optimization | Operational effect

Firmware PREFLIGHT command | Returns a single authoritative readiness record with explicit blocker tokens.

Explicit state fields | ARM, START, COUNTDOWN, RUNNING, PAUSED, RESUMED, CAL, and DONE now carry state= values.

Guided run-control panel | Organizes setup, calibration, readiness, ARM, and START into a visible numbered sequence.

State-aware controls | Buttons are enabled only when the current firmware state permits the operation.

Audio interlock | ARM, START, and RESUME reject disabled metronome audio.

Configuration interlock | Invalid or infeasible prescriptions reject ARM/START.

Separate STOP and ABORT | Routine termination and immediate abort remain distinct in the transcript and DONE reason.

Canonical HR fields | Live HR messages correctly update the browser rather than leaving stale STATUS values.

Expanded exports | CSV and JSON include cycles, misses, raw events, target values, compliance, HR validity, contact, age, and motion envelope.


## Table 9


WHY THIS MATTERS / R2.0.2 had the core exercise engine, but operators could encounter fragmented setup and stale browser state. R2.1 makes readiness visible and keeps the firmware, not the browser, as the final authority.


## Table 10


Software | Purpose | Recommended status

Python 3.13 + PlatformIO | Build/flash/monitor the pinned firmware project. | Installed and proven with micro:bit V2.21.

Chrome or Edge | Web Bluetooth dashboard. | Current version with BLE permission enabled.

Local HTTP server | Provides a secure browser context. | Use scripts\serve_console.bat.

FFT+ R2.1 dashboard | Single-file operator console. | dashboard\fair_fitness_console.html.

Optional nRF Connect | Independent BLE inspection only. | Not required for normal operation.


## Table 11


cd <project>\firmware / Remove-Item -Recurse -Force .pio -ErrorAction SilentlyContinue / py -3.13 -m platformio run


## Table 12


py -3.13 -m platformio run -t upload


## Table 13


.pio\build\fairfit_microbit_v2\firmware.hex


## Table 14


py -3.13 -m platformio device monitor -b 115200


## Table 15


Fair Fitness Tester FFT+ dual-role boot / BLE_ADV service_uuid=1 name=1 start=1 active=1 device=FairFitFFTPlus / STACK_CONFIG main_words=1024 emit_buffer=static bytes=897 / BOOT ... fw=FairFitFFTPlus-R2.1.0 ... board_target=microbit_v2.21 ... sensor=LSM303AGR ... matrix=1


## Table 16


DO NOT PROCEED ON MIXED VERSIONS / The browser header must show R2.1.0-G1 and the firmware must report FairFitFFTPlus-R2.1.0. A mixed release can display incorrect state or omit the new preflight protocol.


## Table 17


scripts\validate_package.bat


## Table 18


Toolbar control | Use

Connect micro:bit | Opens the Web Bluetooth chooser and establishes NUS.

Disconnect | Closes the browser-to-tester connection. During RUNNING, firmware disconnect policy determines pause/continue behavior.

Demo Stream | Browser-only demonstration. Never use demo data as participant data.

Arm / Start | Global mirrors of the guided controls; the guide recommends the numbered workflow.

Pause / Resume | Temporary interruption and continuation.

STOP | Controlled termination.

ABORT | Immediate operator termination.


## Table 19


CONNECTION IS NOT READINESS / A blue BLE indicator only proves the NUS console link. It does not prove that configuration, calibration, audio, HR policy, HRM subscription, sample freshness, starting HR, or firmware state are acceptable.


## Table 20


Display | Interpretation

Heart rate | Most recent valid BPM from live HR records.

Warning / Stop / Emergency | Configured policy boundaries; they are not automatically selected by the system.

HRM link | SUBSCRIBED means the micro:bit has connected and enabled measurement notifications.

Sensor contact | Detected when the HRM reports contact support and contact present.

Sample age | Milliseconds since the most recent valid measurement. Rising age indicates loss of fresh data.


## Table 21


OK ... command=HRM_SCAN service=0x180D duration_ms=10000 / HRM_FOUND ... name=<monitor> ... / HRM_CONNECTED ... service=0x180D characteristic=0x2A37 mode=NOTIFY / HR ... bpm=95 peak=95 age_ms=0 contact_supported=1 contact=1 ...


## Table 22


BENCH SIMULATOR / FFT-HRM-SIM is ideal for qualification. It exercises real BLE scan, connection, service discovery, subscription, notifications, parsing, and safety logic. Its generated BPM must never be recorded as participant physiology.


## Table 23


Tab | Primary purpose

Live Test | Guided workflow, HR status, live metrics, charts, progress, recent protocol traffic.

Prescription | Participant/test configuration and calculated cadence feasibility.

Heart Safety | FFT+ mode, HR policy, approved source, HRM tools, and safety events.

Calibration | Accelerometer, I2C, speaker, matrix, audio, reset, and self-test tools.

Data | Recorded rows and export controls.

Terminal | Direct command access and command palette.

Guide | Embedded concise operating sequence.


## Table 24


Field | Allowed | Default | Operational meaning

Participant | Safe token, 1-24 characters | P001 | Session identifier used in records; avoid unnecessary personal information.

Protocol | STEP_UP or HIGH_KNEE | STEP_UP | STEP_UP defaults to 4 beats/cycle; HIGH_KNEE defaults to 2.

Mode | LAB or PE | LAB | Administrative context token.

Mass (kg) | 20-300 | 75 | Entered mass used in external power estimate.

Step height (m) | 0.02-0.50 | 0.10 | Measured platform height; entered value is not verified by accelerometer.

Target power (W) | 1-500 | 30 | Used to calculate target cycles/minute.

Duration (s) | 5-3600 | 60 | Active elapsed time; paused time is excluded.

Telemetry (Hz) | 0.25-4 | 2 | Browser-facing DATA rate, not accelerometer sampling rate.

Zone (%) | 1-50 | 10 | Allowed cadence error for time-in-zone calculation.

Beats/cycle | 1-8 | 4 | Metronome windows in one counted cycle.

Active beats needed | 1 to beats/cycle | 2 | Minimum beat windows containing accepted motion onset.

Countdown (s) | 0-10 | 3 | Local pre-start countdown.

Motion threshold (mg) | 30-2000 | 150 | Minimum configured dynamic envelope; calibration may raise effective value.

Refractory (ms) | 80-1500 | 220 | Minimum time between accepted raw event onsets.

Browser disconnect | PAUSE or CONTINUE | PAUSE | Local action if NUS browser link drops during RUNNING.


## Table 25
