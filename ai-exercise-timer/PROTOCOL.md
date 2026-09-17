# YMP BLE protocol

Device name: `MB2-AI-TIMER`

Transport: Nordic UART Service (NUS)

- Service: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- RX, host -> micro:bit: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E`
- TX, micro:bit -> host notify: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E`

## Stream

`A,x_mg,y_mg,z_mg\n`

Example: `A,-32,101,987\n`

The line is deliberately constrained to fit the default 20-byte ATT notification payload at the +/-2 g sensor range. Host arrival time is the stream timestamp.

## Commands

- `I\n` -> `I,<sensor>,<rate>\n`
- `P\n` -> `P,OK\n`
- `S,1\n` -> start stream
- `S,0\n` -> stop stream
- `R,50\n` -> set rate to 50 Hz
