FAIR FITNESS TESTER

FFT+

R2.1.0-G1 OPERATOR'S GUIDE

Browser-guided setup, metronome-controlled stepping, accelerometer verification,
BLE heart-rate supervision, data administration, and troubleshooting

Release: FairFitFFTPlus-R2.1.0 firmware / R2.1.0-G1 dashboard
Target: BBC micro:bit V2.21 (nRF52833)
Document date: July 11, 2026

# Document control and operating authority

## Operator authority

The operator must: verify equipment, configuration, approved HR policy, participant readiness, safe environment, and continuous supervision.

The micro:bit must: own local timing, metronome cues, motion event extraction, state transitions, HR freshness checks, warnings, and configured stop decisions.

The browser must: send and document configuration, display authoritative records, guide preflight, and preserve exports.

The participant may stop at any time. No software state or prescribed duration overrides a participant request to stop.

## Guide conventions

# Contents

1. System purpose and architecture

2. R2.1 software optimization summary

3. Equipment and software checklist

4. Build, flash, and version verification

5. Launching and connecting the dashboard

6. Establishing the heart-rate link

7. Dashboard orientation and control hierarchy

8. Workload prescription page

9. Heart Safety policy page

10. Accelerometer calibration and diagnostics

11. Guided preflight workflow

12. Arming and starting the real event

13. Metronome control and motion verification

14. Supervising the live trial

15. Pause, resume, STOP, ABORT, and disconnect behavior

16. Completion, recovery, and trial validity

17. Data administration and exports

18. Terminal and hardware diagnostics

19. Troubleshooting and blocker resolution

20. No-participant bench qualification

21. Real-test standard operating procedure

22. Maintenance, release control, and requalification

23. Interpretation boundaries and known limitations

Appendix A. Dashboard field reference

Appendix B. Command reference

Appendix C. Telemetry and event records

Appendix D. Error and blocker reference

Appendix E. Printable operator worksheets

Appendix F. Workload equations and example

Appendix G. R2.1 validation status and open verification

# 1. System purpose and architecture

What FFT+ measures, what it controls, and where authority resides.

FFT+ is a locally timed stepping-assessment instrument. The operator enters mass, step height, target external power, duration, cycle structure, motion-detection settings, and an approved heart-rate policy. The tester micro:bit converts the workload prescription into a metronome cadence, observes motion with its onboard accelerometer, and receives standard BLE heart-rate notifications while the browser supervises and records the session.

Figure 1. Dual-role BLE architecture and division of responsibilities.

## Three connected components

## Canonical pathway

# 2. R2.1 software optimization summary

Changes made so an operator can reliably reach, start, supervise, and close a real test.

Figure 2. R2.1 five-step guided workflow.

# 3. Equipment and software checklist

Prepare the complete test environment before connecting a participant.

## Required hardware

One BBC micro:bit V2.21 programmed with FFT+ tester firmware.

USB cable for programming/serial qualification; a suitable battery pack for portable operation.

Standard BLE Heart Rate Service monitor, or the companion FFT-HRM-SIM micro:bit for bench testing.

Stable step platform of known height for STEP_UP, or an approved open-floor area for HIGH_KNEE.

Consistent tester-micro:bit attachment position selected by the protocol. Calibrate in that exact position.

A Windows computer with Bluetooth LE and a compatible Chromium browser for the dashboard.

## Required software

## Environment and administration

☐  The step/platform area is clear, stable, dry, and adequately lit.

☐  The operator can remain close enough to assist and to press STOP/ABORT immediately.

☐  The approved protocol and HR limit source are available in writing.

☐  Participant identifier and configuration record are ready; no unnecessary personal data will be exported.

☐  An emergency response plan exists outside the FFT+ software.

# 4. Build, flash, and version verification

Install the paired R2.1 tester firmware before using the R2.1 dashboard.

## Build from PlatformIO

## Program the micro:bit

If CMSIS-DAP/OpenOCD fails but the build succeeded, copy the generated HEX file to the MICROBIT drive:

## Verify through USB serial

Press reset and confirm that the boot report contains all of the following:

## Package validation

The validator checks package files and host-side tests. It does not replace a physical flash, speaker, accelerometer, HRM, and end-to-end bench qualification.

# 5. Launching and connecting the dashboard

Serve the single-file console locally, grant Web Bluetooth access, and confirm the NUS link.

Start the local server. Double-click scripts\serve_console.bat or run the Python server specified in the project tools.

Open the localhost address. Use Chrome or Edge. A file:// URL may not provide the secure context required by Web Bluetooth.

Press Connect micro:bit. Select FairFitFFTPlus from the browser chooser.

Wait for initialization. The browser sends HELLO, CAPS?, STATUS, and PREFLIGHT after the NUS connection is established.

Confirm the top-right status. The device indicator should show BLE rather than Offline or Demo.

Figure 3. Global connection and trial-control toolbar.

# 6. Establishing the heart-rate link

Connect a standard BLE HRS monitor or the companion FFT-HRM-SIM.

## Normal procedure

Wake and fit the HR monitor according to its normal operating instructions.

Close vendor applications, watches, exercise machines, or other centrals that may already hold the HRM connection.

On Live Test, press Scan & connect HRM.

Wait for HRM link = SUBSCRIBED, HRM ready, a changing BPM value, and a fresh sample age.

Press HRM status when detailed peer, RSSI, received, parse_errors, and disconnect counts are needed.

Figure 4. Heart monitor card after a successful subscription.

## Expected protocol progression

# 7. Dashboard orientation and control hierarchy

Understand which page configures, which page verifies, and which controls change the trial state.

Figure 5. Complete Live Test page in a ready-to-arm condition.

## Control hierarchy

Preferred: Use the five numbered controls in Guided operator run control.

Secondary: Use top toolbar controls only after you understand and confirm the preflight state.

Diagnostic: Use Terminal and Calibration controls for qualification and troubleshooting, not as substitutes for readiness.

Authoritative: A command is not successful until the firmware returns the corresponding OK/state record.

# 8. Workload prescription page

Enter the intended workload and verify that the calculated metronome timing is feasible.

Figure 6. Prescription page with calculated cadence, beat tempo, period, work, and energy.

## Prescription fields

## Calculated prescription

The dashboard and firmware use the entered mass, gravity, step height, and target power to derive target cadence. For STEP_UP, the default four-beat cycle provides the high-medium-low-low cue sequence described later.

## Saving and sending

Save locally: Stores the form values in browser local storage; it does not send them to the micro:bit.

Load saved: Restores browser values and marks configuration dirty until resent.

Defaults: Restores default form values and marks configuration dirty.

Send complete configuration: Sends workload and safety policy, then STATUS and PREFLIGHT.

Any edit after sending: Invalidates the browser applied marker. Press Send setup again before preflight.

# 9. Heart Safety policy page

Enter only limits authorized by the responsible protocol or authority.

Figure 7. Heart Safety policy, HRM controls, and live safety position.

## Policy fields

## Developer simulation controls

Firmware HR simulator controls on this page are for no-participant bench tests. They bypass the real BLE HRS link and therefore do not qualify scan/discovery/subscription. Use FFT-HRM-SIM when the real BLE path must be tested.

# 10. Accelerometer calibration and diagnostics

Establish the baseline and noise threshold in the actual attachment position.

Figure 8. Calibration and hardware diagnostics page.

## Calibration procedure

Attach or hold the tester micro:bit in the exact position to be used during the event.

Ensure the participant/device remains still. Do not press or reposition the unit during calibration.

Press Calibrate accelerometer or the guided 2 Calibrate still button.

Wait approximately three seconds for CAL result=PASS.

Confirm calibrated=1, a plausible baseline near gravity, noise standard deviation, and effective threshold.

Recalibrate after attachment position, clothing/pocket, hardware, or movement environment changes.

## How the threshold is formed

The firmware samples the accelerometer at 50 Hz. During RUNNING it computes acceleration magnitude, subtracts the calibrated gravity baseline, smooths the dynamic envelope, applies threshold and hysteresis, and accepts a new onset only when the refractory interval permits it.

## Diagnostic controls

# 11. Guided preflight workflow

Use the numbered panel for every real test.

Figure 9. All ten readiness checks green; ARM is permitted but START is still locked.

## Five required steps

## Ten checklist items

Figure 10. Firmware blocker codes and first corrective actions.

# 12. Arming and starting the real event

Transition from READY to ARMED to COUNTDOWN to RUNNING without bypassing interlocks.

## ARM procedure

Confirm all checklist items are green and preflight detail shows blockers=NONE.

Confirm the participant is in the starting position but has not begun stepping.

Press 4 Arm test.

Verify the firmware reports OK command=ARM state=ARMED.

Read the page banner: READY TO START. Reconfirm participant readiness and clear surroundings.

Figure 11. ARMED state: the final Start countdown action is enabled.

## START procedure

Press 5 Start countdown only after the ARMED state is visible.

Listen for local countdown tones and observe the matrix digit countdown.

Do not begin before the first metronome beat/window opens.

Confirm RUNNING appears, trial elapsed time begins, and DATA records start.

# 13. Metronome control and motion verification

Understand exactly what the tester counts and what it cannot prove.

Figure 12. Default four-beat STEP_UP cycle and motion-onset windows.

## Default four-tone cycle

## Motion-event extraction

Accelerometer sampling occurs every 20 ms (50 Hz).

Dynamic acceleration is the absolute difference between magnitude and calibrated baseline.

A smoothed envelope is compared with the effective threshold.

A new event onset is credited once to the current beat window when the refractory interval permits it.
