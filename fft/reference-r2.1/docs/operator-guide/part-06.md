HRM_CONTACT | Required contact absent. | Correct placement/policy.

HR_ALREADY_HIGH | Starting BPM at/above warning. | Do not start.

BAD_STATE | Command not allowed in current state. | Follow state sequence.

BUSY | Configuration/diagnostic locked by active state. | Stop/reset as appropriate.

RANGE | Value outside explicit bounds. | Use reported min/max.

ENUM | Unsupported named value. | Use allowed list.

UNKNOWN_COMMAND | Command not defined. | Use HELP; e.g., HRM_SCAN, not SCAN.


## Table 74


Item | Recorded value

Date / session | ____________________________________________

Operator | ____________________________________________

Tester ID | ____________________________________________

Firmware / dashboard | ____________________________________________

Participant token | ____________________________________________

Protocol / mode | ____________________________________________

Mass / step height | ____________ kg / ____________ m

Target power / duration | ____________ W / ____________ s

Beats/cycle / active beats | ____________ / ____________

HRM ID | ____________________________________________

Warning / Stop / Emergency | ________ / ________ / ________ BPM

Limit source | ____________________________________________

Contact required | Yes / No

Stale warning / stop | ____________ / ____________ ms


## Table 75


Sign-off | Name / initials / date

Operator | ____________________________________________

Reviewer, when required | ____________________________________________


## Table 76


work_per_cycle_J = mass_kg * 9.80665 * step_height_m / target_cpm      = target_W * 60 / work_per_cycle_J / beat_bpm        = target_cpm * beats_per_cycle / beat_period_ms  = 60000 / beat_bpm / avg_cpm         = completed_cycles * 60000 / active_elapsed_ms / power_W         = mass_kg * 9.80665 * step_height_m * avg_cpm / 60 / energy_J        = mass_kg * 9.80665 * step_height_m * completed_cycles / compliance_pct  = 100 * min(actual_cpm,target_cpm) / max(actual_cpm,target_cpm)


## Table 77


Input / result | Value

Mass | 75 kg

Step height | 0.10 m

Target power | 30 W

Work per cycle | 73.55 J

Target cadence | 24.47 cycles/min

Beats per cycle | 4

Metronome | 97.89 beats/min

Beat period | approximately 613 ms

60-second target cycles | approximately 24.47

Target external energy | approximately 1.80 kJ


## Table 78


ENTERED-VALUE DEPENDENCE / The estimate is only as accurate as mass, step height, accepted cycles, and the simplified external-work model. It is not metabolic energy expenditure.


## Table 79


RELEASE READINESS CRITERION / Do not describe the R2.1 system as physically qualified until the complete bench checklist in Section 20 has been run and archived on the actual tester hardware.


## Table 80


FINAL REMINDER / A successful test is not merely one that reaches DONE. It is one that was authorized, configured, calibrated, supervised, honestly interpreted, and completely documented.
