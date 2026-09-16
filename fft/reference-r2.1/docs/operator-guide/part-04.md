target_cpm = target_W * 60 / (mass_kg * 9.80665 * step_height_m) / beat_bpm   = target_cpm * beats_per_cycle / beat_ms    = 60000 / beat_bpm


## Table 26


FEASIBLE TIMING GATE / The beat period must be between 120 and 5000 ms. The dashboard marks the calculation Feasible, and the firmware independently rejects ARM/START with INFEASIBLE_TIMING when outside this range.


## Table 27


Field | Allowed | Default | Meaning

Fitness mode | FFT_PLUS / FFT | FFT_PLUS | FFT_PLUS enables required HR enforcement when HR required = Yes.

Require HRM | Yes / No | Yes | When required, ARM/START/RESUME depend on valid live HR.

Limit source | Named source; not UNSET | UNSET | Records who/what authorized the limits. UNSET blocks ARM.

Warning BPM | 60-220 | 150 | Warning boundary; must be below Stop.

Stop BPM | 70-230 | 165 | Upper-bound stop after configured consecutive samples.

Emergency BPM | 80-240 | 180 | Immediate configured emergency stop boundary.

Warning hold (ms) | 0-60000 | 5000 | Duration required by safety evaluator for sustained warning behavior.

Stop count | 1-10 | 3 | Consecutive at/above-stop samples required for STOP_LIMIT.

Warning action | ALERT / PAUSE | ALERT | Alert only or local pause when warning decision is issued.

Stale warning (ms) | 500-30000 | 3000 | Age that produces stale warning/pause behavior.

Stale stop (ms) | 1000-60000 | 7000 | Age that produces HRM_STALE stop; must exceed warning age.

Contact required | Yes / No | No | If required, absent contact blocks start and can pause/stop.

Rise warning (BPM) | 5-100 | 25 | Rapid-rise magnitude used by the safety evaluator.

Rise window (s) | 5-120 | 15 | Window associated with rise warning.


## Table 28


POLICY RELATIONSHIPS / Warning must be lower than Stop, Stop lower than Emergency, and stale warning lower than stale stop. Limits are configuration values, not medical clearance and not software-selected recommendations.


## Table 29


OK ... command=CALIBRATE duration_ms=3000 instruction=stand_still / CAL ... result=PASS state=READY calibrated=1 baseline_mg=... noise_std_mg=... threshold_mg=...


## Table 30


effective_threshold_mg = max(configured_threshold_mg, 4 * noise_std_mg + 70)


## Table 31


Control | Expected use

Read accelerometer | Request current x/y/z/magnitude and sensor identity.

Scan internal I2C | Verify the internal motion-sensor bus; FFT+ uses the internal Wire1 path.

Full self-test | Runs capabilities, STATUS, PREFLIGHT, I2C, accelerometer, buttons, and HRM status.

Speaker test | Produces a short tone; required before relying on metronome cues.

Matrix test | Cycles key icons on the 5x5 display and browser mirror.

Enable metronome audio | Sends AUDIO ON and refreshes PREFLIGHT.

Reset trial state | Sends RESET, STATUS, and PREFLIGHT when configuration is not locked by an active state.


## Table 32


CALIBRATION DOES NOT CLASSIFY TECHNIQUE / A passed calibration establishes a motion baseline. It does not prove platform contact, exact step height, full range of motion, foot identity, or ordered technique.


## Table 33


Step | Operator action | Successful result

1 Send setup | Sends workload configuration and HR policy after validating browser-side ranges. | Configuration = Sent and accepted; new PREFLIGHT requested.

2 Calibrate still | Starts three-second local calibration. | CAL result=PASS; Calibration green.

3 Check readiness | Requests firmware PREFLIGHT. | blockers=NONE and arm_ready=1.

4 Arm test | Waits for authoritative ARM OK. | Firmware state becomes ARMED; start_ready can become 1.

5 Start countdown | Rechecks state/preflight and issues START. | State COUNTDOWN, then RUNNING.


## Table 34


Check | What must be true

Browser link | NUS BLE connected, not Offline or Demo.

Configuration | Current form values were sent and accepted after the last edit.

Motion sensor | LSM303AGR or supported sensor is ready.

Calibration | Firmware calibrated flag is set and threshold reported.

Metronome audio | Audio is enabled; speaker should be audibly qualified.

HR policy | Threshold ordering valid and limit source not UNSET when FFT+ requires HR.

HRM subscription | HRS client is SUBSCRIBED or approved simulator is active for bench testing.

Fresh HR sample | Sample age remains below the configured stale-warning boundary.

Starting HR | Current BPM is below the warning boundary.

Firmware state | State permits ARM or, after arming, permits START.


## Table 35


BROWSER GUIDANCE VS FIRMWARE AUTHORITY / The browser checklist is an operator aid. The firmware PREFLIGHT record and command response are authoritative. Do not infer success from a clicked button or changed color alone.


## Table 36


PREFLIGHT ... arm_ready=1 start_ready=0 state=READY blockers=NONE / OK ... command=ARM state=ARMED target_cpm=... beat_bpm=... beat_ms=... / PREFLIGHT ... arm_ready=0 start_ready=1 state=ARMED blockers=NONE


## Table 37


OK ... command=START state=COUNTDOWN countdown_s=3 ... / COUNTDOWN ... state=COUNTDOWN value=3 / COUNTDOWN ... state=COUNTDOWN value=2 / COUNTDOWN ... state=COUNTDOWN value=1 / RUNNING ... state=RUNNING t_ms=0 target_cpm=... beat_bpm=...


## Table 38


NEVER FORCE START / When START is locked or rejected, resolve the blocker. Do not use terminal commands to bypass missing calibration, stale HR, high starting HR, disabled audio, invalid timing, or an inappropriate state.


## Table 39


Beat | Tone | Intended cue | Firmware treatment

1 | 880 Hz, 70 ms | First UP | Opens beat window 1.

2 | 660 Hz, 45 ms | Second UP | Opens beat window 2.

3 | 440 Hz, 45 ms | First DOWN | Opens beat window 3.

4 | 440 Hz, 45 ms | Second DOWN | Opens beat window 4; cycle is evaluated after this window closes.


## Table 40


if active_beats >= min_active_beats: /     count one completed cycle / else: /     count one missed cycle


## Table 41


INTERPRETATION BOUNDARY / A counted cycle means that enough metronome windows contained distinct accepted motion onsets. It does not directly verify foot contact, platform height, foot order, full up-up-down-down technique, or safe biomechanics. The operator remains the technique observer.


## Table 42


Metric | Source and interpretation

Cadence | Instantaneous completed cycles/minute from time between accepted cycles.

Power | Estimated external mechanical power from entered mass/height and average counted cadence.

Heart rate | Most recent live valid HRS BPM.

HR safety | Current local safety-evaluator state such as NORMAL, WARNING, STOP_LIMIT, or stale/contact conditions.

Cycles | Accepted cycles meeting minimum active-beat requirement.

In zone | Percentage of active time whose cadence lies within configured Zone %.

Energy | Estimated mass * gravity * height * completed cycles, shown in kJ.

Elapsed | Active elapsed time; pauses are excluded.


## Table 43


HUMAN OBSERVATION REMAINS PRIMARY / A technically normal dashboard does not prove that the participant should continue. Stop for participant request, concerning observation, equipment instability, or any condition specified by the approved protocol.


## Table 44


Control | Allowed state | Effect | Record meaning

Pause | RUNNING | Closes/discards the partial cycle, stops active timing, retains configuration and calibration. | PAUSED reason=<cause>.

Resume | PAUSED | Rechecks audio and HR safety, reopens a metronome window, resumes active time. | RESUMED state=RUNNING.

STOP | COUNTDOWN, RUNNING, PAUSED | Controlled termination and DONE summary. | reason=REMOTE_STOP or button-specific stop.

ABORT | COUNTDOWN, RUNNING, PAUSED | Immediate operator termination through same safe stop path but distinct reason. | reason=REMOTE_ABORT.

A+B hold 1.2 s | Active timing states | Local immediate abort without browser dependence. | reason=LOCAL_ABORT.


## Table 45


Policy | Behavior during RUNNING | Use consideration

PAUSE | The tester pauses when NUS browser link disconnects. | Preferred when loss of operator console should interrupt the event.

CONTINUE | The local metronome, accelerometer, and HR safety continue without browser display. | Use only when the approved protocol explicitly allows local continuation and the operator remains able to stop locally.


## Table 46


Input | State | Action

Button A | READY or DONE | ARM

Button A | ARMED | START

Button A | PAUSED | RESUME

Button B | RUNNING | PAUSE

Button B | PAUSED or COUNTDOWN | STOP

A+B held 1.2 s | COUNTDOWN, RUNNING, or PAUSED | LOCAL_ABORT


## Table 47


STOP VS ABORT / Use STOP for controlled termination. Use ABORT when the operator wants an immediate, unmistakably separate termination reason. Both paths end in DONE and preserve a reason in the transcript.


## Table 48


DONE ... state=DONE reason=<reason> duration_ms=... cycles=... missed=... avg_cpm=... / avg_power_W=... energy_J=... zone_pct=... peak_hr_bpm=... hr_at_stop_bpm=... / hr_valid_pct=... hr_stop_reason=...


## Table 49


Reason | Meaning

DURATION | Configured active duration completed.

REMOTE_STOP | Operator pressed dashboard STOP.

REMOTE_ABORT | Operator pressed dashboard ABORT.

LOCAL_ABORT | A+B local abort was held.

HR_UPPER_BOUND | Configured stop threshold/consecutive-sample logic triggered.

HR_EMERGENCY | Configured emergency threshold triggered.

HRM_STALE | Fresh HR data exceeded configured stale-stop limit.

HRM_DISCONNECT | Required HRM link disconnected during an active state.


## Table 50


RECOVERY ... offset_s=30 bpm=... drop_bpm=... / RECOVERY ... offset_s=60 bpm=... drop_bpm=... / RECOVERY ... offset_s=120 bpm=... drop_bpm=... complete=1


## Table 51


Export | Contents | Best use

CSV | Time-series trial rows with motion, cadence, power, HR, safety, and state fields. | Spreadsheet analysis and plotting.

JSON bundle | Schema, export time, configuration, calculated prescription, last state, rows, HR records, safety events, sequence gaps, transcript. | Complete machine-readable session archive.

Transcript | Timestamped command and device lines as plain text. | Audit trail and troubleshooting.


## Table 52


Group | Fields

Identity/time | seq, t_ms, state, metric, source

Movement | cycle, missed, raw, beat, env_mg, threshold_mg

Cadence/work | cadence, avg_cpm, target_cpm, power_W, target_W, energy_J, zone_pct, compliance

Heart rate | hr_bpm, hr_peak, hr_age_ms, hr_contact, hr_safety, hr_valid_pct

Display | led, ledmask


## Table 53


YYYY-MM-DD_<participant-token>_<protocol>_<session-number>_FFTPlus_R2.1.csv / YYYY-MM-DD_<participant-token>_<protocol>_<session-number>_FFTPlus_R2.1.json / YYYY-MM-DD_<participant-token>_<protocol>_<session-number>_FFTPlus_R2.1_transcript.txt


## Table 54


CLEAR IS DESTRUCTIVE FOR THE BROWSER SESSION / The Data Clear control removes currently recorded browser rows. Export and verify the files first. The micro:bit does not provide a cloud or permanent session database.


## Table 55


HELLO / VERSION / CAPS? / STATUS / PREFLIGHT / HRM STATUS / ACCEL? / I2CSCAN INT / BUTTONS?


## Table 56
