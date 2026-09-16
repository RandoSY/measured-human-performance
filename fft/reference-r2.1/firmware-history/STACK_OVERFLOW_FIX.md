# R2.0.2 connection-time stack-overflow repair

## Observed failure

The micro:bit booted, advertised, accepted a BLE connection, emitted `CONNECTED`, began its automatic long `STATUS` response, and then reset with:

```text
Task: mlt, stack overflow
```

## Root cause

`mlt` is the n-able Arduino task that executes `setup()` and `loop()`. The pinned n-able core defaults this task to 512 stack units unless `CONFIG_MAIN_TASK_STACK_SIZE` is defined. R2.0.1's `emitf()` allocated a `TX_LINE_MAX + 1` local array: 897 bytes. That array remained live while `queueLine()` called serial and BLE transport code. The long STATUS path therefore exhausted the main-loop task stack.

## Repair

R2.0.2 applies two independent protections:

1. `g_emitBuffer[897]` is static/global, so it no longer consumes loop-task stack.
2. PlatformIO defines `CONFIG_MAIN_TASK_STACK_SIZE=1024`, doubling the core default.

All `emitf()` calls execute in Arduino loop context. BLE callbacks only publish flags, bytes, or mailbox values, preserving safe single-context use of the static formatting workspace.

## Expected proof after flashing

```text
BLE_ADV service_uuid=1 name=1 start=1 active=1 device=FairFitFFTPlus
STACK_CONFIG main_words=1024 emit_buffer=static bytes=897
BOOT seq=1 fw=FairFitFFTPlus-R2.0.2 ...
CONNECTED seq=2 ...
STATUS seq=3 ...
```

The STATUS line must complete and the board must remain connected without rebooting. Send `STATUS` repeatedly from the BLE terminal as an additional stress check.
