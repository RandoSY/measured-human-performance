# Validation report

## Completed in the creation environment

- Python project-structure and regression tests: passed.
- Pure C++11 Heart Rate Measurement encoder tests: passed with `-Wall -Wextra -Werror`.
- Full `src/main.cpp` C++11 syntax check against API-shaped Arduino, NimBLE-Arduino, and Adafruit Microbit stubs: passed with `-Wall -Wextra -Werror`.
- Verified source uses standard HRS UUID `0x180D`, Heart Rate Measurement `0x2A37`, and Body Sensor Location `0x2A38`.
- Verified scan response is enabled before the advertised name is assigned.
- Verified the main-loop stack override is 1024 units.
- Verified buttons A/B and A+B reset paths are present.
- Verified no large local formatting buffer is placed on the Arduino main-loop stack.

## PlatformIO ARM build attempt

A real PlatformIO build was attempted. PlatformIO itself installed and started correctly, but the environment could not resolve `github.com` while cloning the pinned `platform-n-able` platform. Consequently, no ARM object, ELF, HEX, upload, or physical radio test was produced here.

See `PLATFORMIO_BUILD_ATTEMPT.txt` for the complete output.

## Required physical confirmation

1. Build on the user's Windows/nRF development workstation.
2. Flash a micro:bit V2.21.
3. Confirm startup reports `active=1` for advertising.
4. Confirm the FFT+ tester discovers `FFT-HRM-SIM` using `HRM_SCAN`.
5. Confirm the tester subscribes and receives changing BPM values.
6. Confirm Button A, Button B, hold-repeat, and A+B reset.
7. Confirm `CONTACT OFF` causes the tester to report contact not detected.

This is a laboratory simulator, not a medical measurement device.
