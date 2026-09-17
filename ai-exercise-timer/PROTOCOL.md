# YMP BLE protocol

The Arduino/NimBLE and MicroBlocks implementations use the same **measurement message** and the standard Nordic UART Service (NUS).

Arduino device name: `MB2-AI-TIMER`

MicroBlocks device names normally begin with `MicroBlocks`; use the actual advertised name or the host tools' prefix matching.

## Transport

- Service: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- RX, host -> micro:bit: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E`
- TX, micro:bit -> host notify: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E`

## Measurement stream

```text
A,x_mg,y_mg,z_mg\n
```

Example:

```text
A,-32,101,987\n
```

Host arrival time is used as the stream timestamp.

## Core commands

- `I\n` -> implementation/sensor information
- `P\n` -> `P,OK\n`
- `S,1\n` -> start stream
- `S,0\n` -> stop stream
- `R,50\n` -> request 50 Hz

Arduino response to `I`:

```text
I,<sensor>,<rate>\n
```

MicroBlocks response to `I`:

```text
I,MICROBLOCKS,<rate>\n
```

## Display status commands

The 5x5 matrix is independently controllable over the same NUS channel:

- `D,ADVERTISING\n` - BLE waiting/beacon animation
- `D,READY\n` - ready/check mark
- `D,STREAM\n` - streaming arrow
- `D,STOP\n` - stopped/X
- `D,RECORD,E\n` - recording icon alternating with `E`
- `D,CLASS,E\n` - show recognized class initial
- `D,CHAR,7\n` - show a supported character
- `D,UNKNOWN\n` - question mark
- `D,OK\n` - check mark
- `D,ERROR\n` - exclamation mark

Display messages are a parallel human-interface channel; they do not replace the measurement stream.

See [`DISPLAY.md`](DISPLAY.md) for the visual vocabulary.

## Rate note

The Arduino/NimBLE implementation is designed for 10-100 Hz operation and defaults to 50 Hz. The MicroBlocks companion accepts the same rate command for protocol compatibility, but **25 Hz is the conservative starting point until sustained hardware testing establishes higher reliable rates**.
