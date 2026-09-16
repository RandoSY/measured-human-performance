# Measured Human Performance — Idea Record

## Problem

Human-performance tests often treat the prescribed task as if it were equally demanding for everyone, then rely on a single outcome score without verifying actual effort, motion quality, or measurement validity.

## Central idea

Human performance should be treated as an instrumentation problem: define the physical task, measure what was actually done, monitor relevant response, expose invalid conditions, and separate interpretable components from opaque composite scores.

## Fair Fitness / DYNAMETICS

A test can prescribe a target **physical work or power** rather than a nominally identical motion. Body mass, step height, cadence, and duration matter because they determine mechanical demand.

The durable three-part structure is:

1. **prescribe** a target output;
2. **confirm** that the required motion was actually executed;
3. **supervise** physiological response and stop/report invalid or unsafe conditions.

This is conceptually different from merely counting steps or recording heart rate.

## Explicit failure rather than plausible numbers

Communication loss, stale measurements, motion failure, impossible cadence, missing sensors, and threshold violations should produce an explicit state rather than silently generating a test result.

## Movement Quality / SQM+

Movement quality is decomposed into understandable physical features such as cadence stability, forward smoothness, lateral motion, yaw instability, jerk, symmetry, and related components. A composite score can summarize them, but the components remain visible.

## Tone Trainer

A small attachable motion sensor can turn ordinary resistance equipment into an instrument for rep timing, pacing, approximate power, and immediate audio/haptic feedback. The key requirement is low friction: attach, move, measure, pace, record.

## PlateLab

A scale can move beyond passive logging into **prescriptive measurement**: compare the current measurement with a rolling target/budget and use evidence to guide the next action. The broader idea is the distinction between tracking and control.

## Why it matters

The shared contribution is to make human-performance measurement more physically interpretable, auditable, and resistant to silent failure.

## Distinctive contribution

The projects do not assume that more sensors automatically mean better measurement. They emphasize a clear physical model, explicit validity rules, immediate feedback, and confirmation that the prescribed activity actually happened.

## Representative evidence

- FFT HRM simulator historical snapshot
- movement-data-logger source and snapshot
- FFT/FFT+ application and operator-guide lineage
- SQM+/Q-Meter/WiseWalking lineage
- Tone Trainer design records
- PlateLab source/design lineage
- motion-lab/IMU materials

## Privacy / publication boundary

Public preservation should focus on algorithms, synthetic examples, protocols, validation procedures, and anonymized/synthetic data. Personal health records and personally identifying measurement histories are not part of the public estate.

## Reconstruction path

Define one physical performance question. Write the mechanical model and validity conditions first. Instrument only the quantities needed to verify task execution and response. Test failure paths deliberately. Preserve component measurements and reasons for invalidation before designing a composite score.
