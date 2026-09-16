A latch and lower release threshold prevent one decaying pulse from satisfying multiple windows.

At the end of a cycle, active beat windows are compared with Active beats needed.

## Cycle acceptance

# 14. Supervising the live trial

Use the participant, sound, state, HR, and data displays together.

Figure 13. RUNNING session with local cadence/power and live HRS data.

## Primary live metrics

## Charts and progress

Power chart: blue actual estimate, green target line.

Heart-rate chart: live BPM with warning, stop, and emergency reference lines.

Trial progress: active elapsed time compared with configured duration.

Recent traffic: human-readable authoritative messages, warnings, state changes, and sequence gaps.

## Continuous operator duties

☐  Listen for the intended metronome and verify the participant remains synchronized.

☐  Observe movement quality, stability, fatigue, and participant communication; do not rely on cycle count alone.

☐  Watch live BPM and sample age; investigate a frozen value or increasing sample age immediately.

☐  Watch the state and recent traffic for WARNING, PAUSED, DONE, disconnect, stale, contact, or sequence-gap records.

☐  Keep the browser available, but remember that the micro:bit owns local timing and configured stop logic.

# 15. Pause, resume, STOP, ABORT, and disconnect behavior

Use the correct state transition and preserve the reason in the record.

Figure 14. Trial state machine and principal operator transitions.

## Browser disconnect policy

## Physical tester buttons

# 16. Completion, recovery, and trial validity

Close the event deliberately and preserve the authoritative DONE record.

## DONE record

The firmware emits a DONE line after duration completion, STOP, ABORT, HR stop, HRM stale/disconnect stop, or other terminal causes. Treat this line as the authoritative event closure.

## Common completion reasons

## Recovery records

When valid HR continues after DONE, the firmware may emit RECOVERY records at approximately 30, 60, and 120 seconds, including BPM and drop from HR at stop. Preserve them when the approved protocol uses recovery information.

## Validity review before export

☐  DONE reason is understood and documented.

☐  HR valid percentage and sample continuity are acceptable for the protocol.

☐  Cycle/missed/raw-event behavior is plausible relative to observed movement.

☐  Any pause, warning, stop, abort, disconnect, or technique issue is recorded.

☐  The session is labeled invalid or qualified when equipment or procedure deviated from plan.

# 17. Data administration and exports

Preserve the configuration, records, safety events, and transcript before clearing the browser.

## Three exports

## Expanded CSV fields

## Recommended file naming and storage

Export immediately after the trial and before Clear.

Keep the JSON bundle and transcript with the CSV so configuration and event reasons are not separated from measurements.

Store only the minimum participant-identifying information needed by the approved process.

Record firmware/dashboard versions with the session archive.

Do not merge simulated and participant sessions into the same dataset without an explicit simulation flag.

# 18. Terminal and hardware diagnostics

Use direct commands to prove subsystems and explain rejected operations.

## Terminal page

The Terminal sends the same line-oriented commands used by the dashboard. It is useful for qualification, diagnostics, and capturing exact responses. For normal real operation, use the guided workflow so configuration and state remain synchronized with the browser.

## Core diagnostic sequence

## Hardware tests

## Developer-only simulators

# 19. Troubleshooting and blocker resolution

Read the firmware record first; correct the cause rather than bypassing the interlock.

Figure 15. Preflight blocker map.

## Common non-preflight problems

# 20. No-participant bench qualification

Prove every critical path before a real test and after material software/hardware changes.

## Qualification rule

## Qualification checklist

☐  Firmware and dashboard versions match R2.1.0 / R2.1.0-G1.

☐  BLE advertising and browser NUS connection succeed repeatedly.

☐  Long STATUS/HELP records do not crash or reset the tester.

☐  FFT-HRM-SIM is found as HRS 0x180D, connected, subscribed, and produces changing BPM.

☐  Live HR card follows every BPM change without stale-field lag.

☐  Calibration passes in the intended attachment position.

☐  Speaker and matrix tests pass; audio-off blocks ARM.

☐  PREFLIGHT correctly reports each intentionally introduced blocker.

☐  ARM and START succeed only after blockers=NONE.

☐  Countdown, four-tone metronome, beat visual, and active timing operate locally.

☐  Controlled device movements produce raw events, accepted cycles, misses, cadence, and power records.

☐  Pause and Resume preserve active elapsed timing and reopen the metronome correctly.

☐  STOP produces REMOTE_STOP; ABORT produces REMOTE_ABORT; local A+B produces LOCAL_ABORT.

☐  Browser disconnect policy PAUSE and CONTINUE behave as configured.

☐  HR warning, stop, emergency, stale, contact, and disconnect paths are exercised with simulation.

☐  DONE, 30/60/120-second recovery records, CSV, JSON, and transcript exports are preserved.

☐  Exported files reopen and contain the expected configuration/version metadata.

## Acceptance evidence

Save the transcript from each qualification run.

Record firmware ZIP hash and dashboard version.

Document tester micro:bit identity, HRM/simulator identity, date, operator, and deviations.

Repeat qualification after firmware/dashboard changes, micro:bit replacement, sensor/attachment changes, or unexplained behavior.

# 21. Real-test standard operating procedure

A concise but complete operator sequence for an actual supervised Fair Fitness event.

## A. Before the participant begins

☐  Confirm approved protocol, participant authorization/consent process, and HR limit source.

☐  Inspect step/platform, attachment, battery, speaker, computer, and clear area.

☐  Launch the R2.1 dashboard and connect FairFitFFTPlus.

☐  Fit and connect the real HRM; confirm SUBSCRIBED, changing BPM, fresh age, and contact as required.

☐  Enter participant token and verify mass, measured step height, target power, duration, and cycle settings.

☐  Enter only approved HR warning/stop/emergency and associated policy settings.

☐  Press 1 Send setup.

☐  Attach the tester in the intended position; press 2 Calibrate still and remain still.

☐  Press 3 Check readiness; every item must be green and blockers=NONE.

☐  Explain cues and stop procedure to the participant; confirm the participant is ready.

## B. Start and supervise

☐  Press 4 Arm test and confirm state=ARMED.

☐  Press 5 Start countdown only when the participant and environment are ready.

☐  Begin movement with the first metronome cycle, not before countdown completion.

☐  Observe technique, stability, symptoms/communication, synchronization, HR, and sample freshness continuously.

☐  Use Pause only for a temporary interruption that can safely resume.

☐  Use STOP for controlled termination; use ABORT for immediate operator termination.

☐  Never continue solely to complete duration or obtain a target score.

## C. Close and administer

☐  Confirm state=DONE and read the completion reason.

☐  Continue appropriate post-test observation under the approved protocol; capture recovery records when applicable.

☐  Review cycles, misses, HR validity, pauses/warnings/stops, and observed technique deviations.

☐  Export CSV, JSON, and transcript before Clear.

☐  Label the run valid, qualified, or invalid and document the reason.

☐  Reset/clean the equipment and prepare a new configuration for the next session.

# 22. Maintenance, release control, and requalification

Keep a known, repeatable instrument rather than an untracked collection of files.

## Recommended session metadata

# 23. Interpretation boundaries and known limitations

Use the data for what it measures; do not claim what the current sensors cannot establish.

## What FFT+ does establish

A locally generated, calculated metronome tempo based on entered workload parameters.

Timestamped accepted motion onsets within metronome windows.

Completed and missed cycles under the configured active-beat rule.

Cadence, estimated external power, estimated energy, and time-in-zone metrics.

Standard BLE HRS measurements, sample age/contact metadata when available, and configured local safety decisions.

An auditable command/telemetry/event transcript and export bundle.

## What FFT+ does not establish

Medical fitness, medical clearance, diagnosis, or emergency response.

Actual platform contact, measured step height, foot identity, or ordered up-up-down-down technique.

Complete range of motion, joint loading, balance safety, or proper biomechanics.

Metabolic power, oxygen consumption, or total physiologic energy expenditure.

Perfect HR accuracy or immunity from sensor contact, radio, parsing, or latency problems.

Safe unsupervised testing.

## Metric-specific cautions

# Appendix A. Dashboard field reference

Complete field/range reference for routine configuration.

## Workload and motion fields

## Heart safety fields

# Appendix B. Command reference

Direct NUS/serial commands for operators and troubleshooters.

## Recommended status bundle

## Normal trial control

# Appendix C. Telemetry and event records

Principal device-to-browser lines and how the dashboard uses them.

## Sequence numbers

Application records use seq values. The dashboard detects sequence gaps, which can indicate dropped/reordered application lines. A gap does not automatically invalidate a session, but it must be reviewed with the transcript and link behavior.

# Appendix D. Error and blocker reference

Common rejected-command codes and immediate interpretation.

# Appendix E. Printable operator worksheets

Use these pages as a paper companion to the digital record when required.

## Pre-test configuration worksheet

## Readiness sign-off

☐  Browser BLE connected

☐  Configuration sent after last edit

☐  Motion sensor ready

☐  Calibration passed

☐  Metronome audio enabled/audible

☐  HR policy valid/source recorded

☐  HRM subscribed

☐  Fresh HR sample

☐  Starting HR below warning

☐  PREFLIGHT blockers=NONE

☐  Firmware state READY

☐  Participant/operator ready

## Event and disposition notes

Use this page for the narrative record that cannot be reconstructed from numeric telemetry alone.

Start time / state: ______________________________________________________________________

Warnings: ______________________________________________________________________

Pauses: ______________________________________________________________________

STOP/ABORT reason: ______________________________________________________________________
