Command | Purpose

BEEP 880 100 | Short speaker test; blocked during active timing.

AUDIO ON / OFF | Enable/disable metronome audio; OFF blocks ARM/START/RESUME in R2.1.

DISPLAY_ICON HEART | Display a named icon.

DISPLAY_DIGIT 5 | Display a digit 0-9.

DISPLAY_MASK <value> | Apply a raw 5x5 mask.

DISPLAY_AUTO ON / OFF | Enable/disable automatic state/beat display behavior.

I2CSCAN INT | Scan internal sensor bus.

ACCEL? | Report sensor and current sample.

BUTTONS? | Report physical A/B button state.


## Table 57


Command | What it tests | What it does not prove

HRM_SIMULATE_ON / BPM / OFF | HR parser downstream state, safety thresholds, display, warnings/stops. | BLE scan, HRS discovery, subscription, radio continuity.

SIMULATE_ON / OFF | Synthetic motion path and trial engine. | Physical accelerometer, attachment, real step movement.

Demo Stream | Browser rendering and export behavior. | Any hardware or BLE path.


## Table 58


SIMULATION LABELING / Every simulated run should be marked as simulation in the participant/session token and kept separate from real participant data.


## Table 59


Blocker | Cause | First response

CONFIG | One or more trial values invalid or not sent. | Correct ranges; press Send setup; request PREFLIGHT.

TIMING | Calculated beat period outside 120-5000 ms. | Adjust target power, mass/height entry, or beats/cycle.

SENSOR | Motion sensor not ready. | Run I2CSCAN INT and ACCEL?; reset/reflash if needed.

CALIBRATION | Calibration absent or invalid. | Calibrate still in actual position.

AUDIO | Metronome audio disabled. | Press Enable metronome audio and Speaker test.

HR_CONFIG | Threshold/stale relationships invalid. | Ensure warning < stop < emergency and stale warning < stale stop.

HR_SOURCE | Limit source remains UNSET. | Select and send approved source.

HRM | Required HRM not subscribed. | Wake HRM; scan; remove competing connections.

HR_STALE | No sufficiently fresh valid sample. | Restore notifications; inspect age, battery, range.

HR_CONTACT | Required contact absent/unsupported. | Correct sensor placement or follow approved policy.

HR_HIGH | Starting HR at/above warning. | Do not start; follow the responsible human protocol.

STATE | Current state does not permit operation. | STOP/ABORT active run or RESET completed/error state as appropriate.


## Table 60


Symptom | Trace and response

Browser cannot find FairFitFFTPlus | Confirm R2.1 firmware boot, BLE_ADV start=1 active=1, no other central connected, Chrome/Edge localhost context.

HRM scan finds nothing | Wake/wear simulator/monitor, close other centrals, verify 0x180D advertising, bring close, rescan.

HRM connected but BPM freezes | Inspect age_ms, received, parse_errors, contact, range, battery, and competing connections.

ARM returns NEED_CALIBRATION | Run calibration and wait for CAL PASS.

ARM returns AUDIO_DISABLED | Send AUDIO ON and verify speaker.

START returns BAD_STATE | ARM first and confirm state=ARMED.

Dashboard value differs from terminal | Confirm R2.1.0-G1 dashboard, hard refresh Ctrl+F5, avoid mixed cached versions.

OpenOCD upload fails | Close serial/debug sessions; reconnect USB; use MICROBIT drag-and-drop HEX path.

No cycles despite movement | Review placement, calibration, envelope, threshold, refractory, active-beat requirement, and actual timing.

Too many false cycles | Recalibrate, increase threshold/refractory cautiously, improve attachment stability, and requalify.


## Table 61


NO PARTICIPANT DURING FORCED SAFETY TESTS / Use FFT-HRM-SIM, firmware simulation, or controlled device motion. Never force a person above a configured stop threshold to prove the stop path.


## Table 62


Control item | Practice

Release archive | Keep the original ZIP and SHA-256 alongside deployed firmware/dashboard.

Version pairing | Record firmware boot version and dashboard header for every qualification/session.

Configuration templates | Use browser Save locally for convenience, but verify and resend every field after load.

Battery/USB | Check battery state and USB/DAPLink reliability before use; do not assume prior charge.

Attachment | Use a repeatable placement and recalibrate whenever it changes.

Speaker | Perform a speaker test after hardware/firmware changes and whenever cues sound abnormal.

HRM | Track battery, firmware/app conflicts, strap/contact condition, and connection history.

Data retention | Preserve raw exports and avoid editing originals; analyze copies.

Change control | Any code, library, pin, protocol, or dashboard parser change requires documented regression and bench qualification.


## Table 63


Metadata | Example

Tester ID | FFT-MB-01

Firmware | FairFitFFTPlus-R2.1.0

Dashboard | R2.1.0-G1

HRM ID | FFT-HRM-SIM-02 or real monitor serial/token

Operator | Initials/token

Protocol source | RESEARCH_PROTOCOL / controlled document reference

Qualification date | YYYY-MM-DD

Session notes | Pause/stop/deviation summary


## Table 64


Metric | Boundary

Power W | External mechanical estimate from entered mass/height and counted cycles; incorrect entries or false/missed cycles propagate directly.

Compliance % | Cadence agreement ratio, not a full technique score.

Time in zone | Based on cadence error and active timing; it does not represent physiologic training zone.

Cycle confidence | Heuristic from active-beat count, not validated probability of correct technique.

HR safety | Configured guardrail dependent on correct limits and continuous valid HR data; never sole authority to continue.


## Table 65


RESPONSIBLE USE / The software should reduce ambiguity and improve documentation, not create false certainty. Human supervision, protocol authority, and honest interpretation remain essential.


## Table 66


Field | Command | Range / values | Notes

Participant | SET PARTICIPANT | 1-24 safe token chars | Letters, digits, underscore, dash, dot.

Protocol | SET PROTOCOL | STEP_UP, HIGH_KNEE | Changing protocol resets default beats/active beats.

Mode | SET MODE | LAB, PE | Administrative context.

Mass | SET MASS_KG | 20-300 kg | Used in target cadence and power.

Step height | SET STEP_HEIGHT_M | 0.02-0.50 m | Entered measurement.

Target power | SET TARGET_W | 1-500 W | External power target.

Duration | SET DURATION_S | 5-3600 s | Active time.

Telemetry | SET TELEMETRY_HZ | 0.25-4 Hz | NUS DATA rate.

Zone | SET ZONE_PCT | 1-50 % | Cadence error band.

Threshold | SET THRESHOLD_MG | 30-2000 mg | Calibration may raise effective threshold.

Refractory | SET REFRACTORY_MS | 80-1500 ms | Event-onset spacing.

Beats/cycle | SET BEATS_PER_CYCLE | 1-8 | Default STEP_UP=4.

Active beats | SET MIN_ACTIVE_BEATS | 1 to beats/cycle | Cycle acceptance rule.

Countdown | SET COUNTDOWN_S | 0-10 s | Local countdown.

Disconnect policy | SET DISCONNECT_POLICY | PAUSE, CONTINUE | Action on NUS loss during RUNNING.


## Table 67


Field | Command | Range / values | Notes

Fitness mode | SET FFT_MODE | FFT, FFT_PLUS | FFT_PLUS plus HR_REQUIRED=1 activates enforcement.

HR required | SET HR_REQUIRED | 0, 1 | Required link/data interlocks.

Contact required | SET HR_CONTACT_REQUIRED | 0, 1 | Requires supported/detected contact.

Warning | SET HR_WARNING_BPM | 60-220 | Must be below Stop.

Stop | SET HR_STOP_BPM | 70-230 | Must be below Emergency.

Emergency | SET HR_EMERGENCY_BPM | 80-240 | Highest configured boundary.

Warning hold | SET HR_WARNING_HOLD_MS | 0-60000 | Sustained-warning timing.

Stop count | SET HR_STOP_COUNT | 1-10 | Consecutive stop samples.

Stale warning | SET HR_STALE_WARNING_MS | 500-30000 | Freshness warning/pause.

Stale stop | SET HR_STALE_STOP_MS | 1000-60000 | Must exceed stale warning.

Rise warning | SET HR_RISE_WARNING_BPM | 5-100 | Rapid rise magnitude.

Rise window | SET HR_RISE_WINDOW_S | 5-120 s | Rapid rise time window.

Warning action | SET HR_WARNING_ACTION | ALERT, PAUSE | Configured response to warning.

Limit source | SET HR_LIMIT_SOURCE | safe token, max 24 | UNSET blocks FFT+ ARM.


## Table 68


Group | Commands | Use

Discovery | HELLO, VERSION, CAPS?, STATUS, PREFLIGHT, HELP | Read-only identity, state, and readiness.

Configuration | SET <FIELD> <VALUE> or SET_FIELD <VALUE> | Rejected while configuration is locked by active states.

HRM | HRM SCAN 10000, HRM STATUS, HRM DISCONNECT | Real standard HRS client.

Trial | CALIBRATE, ARM, START, PAUSE, RESUME, STOP, ABORT, RESET | State-changing operations.

Hardware | I2CSCAN INT, ACCEL?, BUTTONS?, BEEP, AUDIO, DISPLAY_* | Diagnostics and local I/O.

Simulation | HRM_SIMULATE_*, SIMULATE_ON/OFF | Developer bench use only.


## Table 69


HELLO / VERSION / CAPS? / STATUS / PREFLIGHT / HRM STATUS


## Table 70


CALIBRATE / PREFLIGHT / ARM / PREFLIGHT / START / PAUSE / RESUME / STOP


## Table 71


USE THE DASHBOARD FOR REAL ADMINISTRATION / Direct commands remain available for engineering control, but the guided dashboard better preserves configuration-dirty state, preflight detail, exports, and operator context.


## Table 72


Record | Key content | Operator use

BOOT | Firmware, board, sensor, matrix, dual-role capabilities. | Version/hardware proof.

BLE_ADV | Advertising configuration and active state. | BLE startup proof.

STATUS | Complete snapshot of configuration, state, motion, HR, and metrics. | Baseline/browser refresh.

PREFLIGHT | Readiness booleans, state, BPM/age, blockers. | ARM/START authority.

CAL | Calibration pass/fail and baseline/noise/threshold. | Calibration checklist.

HRM_CONNECTED | Peer, RSSI, HRS/measurement, notify mode. | Subscription proof.

HR | Live BPM, peak, age, contact, RR, safety, source. | Heart card/chart/events.

EVENT type=CYCLE | Accepted cycle, cadence, power, active beats, confidence. | Cycle metrics.

EVENT type=MISSED_CYCLE | Cycle window did not meet active-beat requirement. | Missed count.

DATA | Periodic state, movement, cadence, power, energy, HR, display. | Charts/table/CSV.

PAUSED / RESUMED | State transition and reason/time. | Trial state/audit.

DONE | Authoritative terminal summary and reason. | Session closure.

RECOVERY | Post-stop BPM/drop at 30/60/120 s. | Recovery record.

ERR | Rejected command and code. | Corrective action.


## Table 73


Code | Meaning | Response

CONFIG_INVALID | Trial configuration outside accepted values. | Correct and resend.

INFEASIBLE_TIMING | Beat period outside 120-5000 ms. | Change workload/cycle parameters.

AUDIO_DISABLED | Metronome disabled. | AUDIO ON; speaker test.

NO_SENSOR | Motion sensor unavailable. | I2C/ACCEL diagnostics.

NEED_CALIBRATION | No valid calibration. | Calibrate still.

HR_CONFIG_INVALID | HR thresholds/stale values inconsistent. | Correct relationships.

HR_LIMIT_SOURCE_UNSET | No approved source token. | Select/send source.

HRM_NOT_READY | No subscribed/approved HR source. | Scan/connect.

HRM_STALE | Last valid HR too old. | Restore data; do not hide with arbitrary timeout.
