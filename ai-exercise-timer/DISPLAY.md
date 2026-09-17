# 5x5 LED display language

The micro:bit's 5x5 matrix is treated as a **status channel**, not decoration.

The person providing the movement should be able to look at the device and know
what it is doing without watching the host computer.

The vocabulary deliberately follows conventions already used in earlier
micro:bit work in this project family: a check mark means active/ready, an X
means stopped, and short non-blocking bitmap overrides communicate state without
interrupting BLE service.

## Core icons

### Advertising / waiting for BLE

The antenna/beacon alternates with a center dot.

```text
..#..
.#.#.
#.#.#
..#..
..#..
```

Meaning: **powered, healthy, waiting for a host connection**.

### Ready

```text
.....
....#
...#.
#.#..
.#...
```

Meaning: **connected and ready for the next instruction**.

### Streaming

```text
..#..
...#.
#####
...#.
..#..
```

Meaning: **raw accelerometer data is streaming to the host**.

### Recording a labeled example

```text
.###.
#...#
#...#
#...#
.###.
```

During recording, this circle alternates every 300 ms with the first
alphanumeric character of the active label.

For example, an `exercise` sample alternates:

```text
.###.      .####
#...#      #....
#...#  <-> ###..
#...#      #....
.###.      #####
 RECORD       E
```

That gives the participant two pieces of information at once:

1. **data is being captured now**;
2. **which action label is being captured**.

### Recognized class

The first alphanumeric character of the recognized class is displayed.

Examples:

```text
E = exercise
R = rest
W = walking
S = sitting
J = jumping
```

All digits and A-Z are available in the firmware's compact 5x5 font.

### Unknown

```text
.###.
#...#
...#.
.....
..#..
```

Meaning: **no class met the recognition threshold**.

This is an important machine-learning state. The device is explicitly allowed
to say "I do not know."

### Stopped

```text
#...#
.#.#.
..#..
.#.#.
#...#
```

Meaning: **stream/capture stopped**.

### Error

```text
..#..
..#..
..#..
.....
..#..
```

Meaning: **sensor or acquisition error requiring operator attention**.

## BLE display commands

The host can explicitly control the matrix through the same Nordic UART Service:

```text
D,ADVERTISING
D,READY
D,STREAM
D,STOP
D,RECORD,E
D,CLASS,E
D,CHAR,7
D,UNKNOWN
D,OK
D,ERROR
```

The firmware replies:

```text
D,OK
```

Display messages never replace the measurement stream; they are a parallel
human-interface channel.

## State sequence during data collection

Typical one-second sample:

```text
ADVERTISING -> READY -> label character -> RECORD/label blink -> check -> label character
```

Typical live recognition:

```text
ADVERTISING -> READY -> STREAM -> E/R/W/... or ? -> STOP
```

## Design rule

A display state must answer one of three questions immediately:

1. **Is the device ready?**
2. **Is it collecting data right now, and for what label?**
3. **What did it recognize?**

If an icon does not help answer one of those questions, it probably does not
belong in the core visual language.
