# MicroBlocks companion — AI Exercise Timer

This directory is a **MicroBlocks implementation of the wireless measurement node** used by the Arduino/NimBLE AI Exercise Timer.

Its purpose is educational: keep the same measurement and BLE ideas while making the device-side logic visible at a lower programming floor.

## Architecture

```text
micro:bit V2 accelerometer
        |
        v
MicroBlocks
        |
        v
BLE Serial / Nordic UART Service
        |
        v
same Python collection/training/recognition tools
```

The measurement message is deliberately the same:

```text
A,x_mg,y_mg,z_mg
```

MicroBlocks reports its accelerometer axes in units where approximately **100 = 1 g**. The project multiplies each axis by 10 so the host sees **mg**, matching the Arduino/NimBLE implementation.

## File

Open:

`ai_exercise_timer_microblocks.ubp`

in a current MicroBlocks IDE with a **BBC micro:bit V2**.

## BLE identity

MicroBlocks provides BLE Serial using the standard Nordic UART Service. Unlike the Arduino firmware, it normally advertises with a MicroBlocks-generated device name rather than `MB2-AI-TIMER`.

The host scripts therefore support device-name prefix matching. A typical command is:

```bash
cd ../host
python collect_samples.py --name MicroBlocks --rate 25 --out createai_samples.csv
```

Then train normally:

```bash
python train_samples.py createai_samples.csv --out movement_model.joblib
```

And recognize:

```bash
python recognize_samples.py --name MicroBlocks --rate 25 --model movement_model.joblib --threshold 0.70
```

## Important MicroBlocks BLE/IDE behavior

MicroBlocks' BLE Serial service and the MicroBlocks IDE share the board's BLE resources. The upstream BLE Serial documentation notes that BLE supports one client at a time and that connecting the IDE by serial can disconnect a BLE client.

For the first hardware test:

1. Connect the micro:bit V2 to MicroBlocks.
2. Open this project and verify that its scripts are running.
3. Release/disconnect the IDE connection to the board before starting the Python BLE host.
4. Scan for the board using `--name MicroBlocks`.
5. Reconnect the IDE only when you need to edit the program.

The exact best workflow should be recorded after physical testing on the target computer/OS.

## Rate

The project starts at **25 Hz**. That is intentionally conservative for an interpreted visual environment plus BLE transport.

The protocol accepts `R,<hz>` from 10 through 100 for compatibility, but **do not describe 50 or 100 Hz as validated MicroBlocks rates until measured on actual hardware**.

For a 25 Hz model, use `--rate 25` consistently during collection and recognition.

## 5x5 state language

The companion preserves the same participant-facing states:

- advertising/waiting — beacon animation
- ready — check
- streaming — arrow
- recording — circle alternating with label initial
- recognized class — class initial
- unknown — `?`
- stopped — `X`
- error — `!`

The Python host sends the same `D,...` commands used by the Arduino firmware.

## Supported commands

```text
I
P
S,1
S,0
R,<hz>
D,ADVERTISING
D,READY
D,STREAM
D,STOP
D,RECORD,E
D,CLASS,E
D,CHAR,E
D,UNKNOWN
D,OK
D,ERROR
```

## Implementation note

The `.ubp` uses MicroBlocks built-in primitives for:

- x/y/z accelerometer readings;
- BLE UART connection/read/write;
- 5x5 shape drawing and character shapes.

A small BLE Serial write helper is included in the project source so outgoing strings are converted to byte arrays and sent in the same manner as MicroBlocks' BLE Serial library.

## Validation status

This companion source has been constructed against current MicroBlocks `.ubp` syntax and current BLE/sensor/display primitives. It has **not yet been imported, flashed, and end-to-end tested on a physical micro:bit V2 here**.

That boundary is intentional. See the parent [`VALIDATION.md`](../VALIDATION.md) for the hardware checklist.

## Why keep both implementations?

The two versions serve different educational purposes:

- **MicroBlocks:** see the flow and modify it quickly.
- **Arduino/NimBLE:** inspect the lower-level embedded implementation and timing.

Both should produce measurements that the same host can understand.
