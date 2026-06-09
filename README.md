# TinyDrop20

TinyDrop20 is a small ATtiny85-based 5x4 charlieplexed LED matrix board. The
repository contains the PlatformIO firmware project, Eagle design files, and
manufacturing outputs for hardware revision `V1_1`.

## Project background

This project was created in collaboration with [@talofer99](https://github.com/talofer99)
as part of a series of free-time hardware and firmware projects we built years
ago. I decided to move these projects to GitHub in the hope that they may be
useful, educational, or inspiring to others.

## Repository layout

```text
Firmware/
  V1_1/
    platformio.ini      PlatformIO project configuration
    src/main.cpp        ATtiny85 firmware
    include/            Project headers, currently unused
    lib/                Project libraries, currently unused
    test/               PlatformIO test folder, currently unused

Hardware/
  V1_1/
    Eagle/              Eagle schematic and board files
    Manufacturing/
      GERBER/           Gerber package for PCB fabrication
      Schematic/        Exported schematic PDF
```

## Hardware overview

The V1.1 board is built around an `ATTINY85-20SU` microcontroller and a 20 LED
display arranged as a 5 row by 4 column matrix. The LEDs are 0603 green parts
driven through five 100 ohm resistors from ATtiny pins `PB0` through `PB4`.

Other notable hardware:

- 20 green 0603 matrix LEDs
- 1 red 0603 status LED
- ATtiny85 SOIC-8 microcontroller
- Five 100 ohm matrix resistors
- 10 kOhm reset pull-up resistor
- Micro USB connector
- MCP73831 battery charger circuit
- CR2450-size coin-cell holder footprint
- Six round test points for programming or probing
- 3.6 mm mounting pad

Power note: the schematic includes an MCP73831 Li-ion/LiPo charger and a battery
holder labeled `CR2450`. Standard CR2450 cells are not rechargeable. Before
connecting USB power with a cell installed, verify that the assembled board is
intended for a compatible rechargeable lithium coin cell, such as the exact cell
specified by the hardware build.

## Firmware overview

The firmware is an Arduino sketch built with PlatformIO for `attiny85` at
8 MHz. It drives the LEDs directly by changing ATtiny GPIO direction and output
state for charlieplexing.

Display constants in `Firmware/V1_1/src/main.cpp`:

```cpp
#define SPEED          35
#define DISPLAY_WIDTH  4
#define DISPLAY_HEIGHT 5
```

The sketch includes:

- A 4x5 bitmap font for uppercase letters, digits, space, `!`, `#`, `$`, and
  `^`
- `setLed()` and `setPixel()` helpers for charlieplexed LED control
- `renderCharacter()` and `renderString()` helpers for drawing text
- Several visual modes and effects:
  - `matrix_fill_test()`
  - `write_text()`
  - `scrolling_text()`
  - `screen_saver()`
  - `rainEffect()`
  - `rows_effect()`

The current sketch displays `myString`, defined near the top of `main.cpp`:

```cpp
char myString[] = "#$^";
```

In the checked-in firmware, `lastState` is hardcoded to `1`, so `setup()` sets
`mode = 0` and the main loop runs `matrix_fill_test()`. The EEPROM-based mode
toggle code is present but commented out.

## Build firmware

Install PlatformIO, then build from the firmware directory:

```powershell
cd Firmware\V1_1
pio run
```

Build output is generated under:

```text
Firmware/V1_1/.pio/build/attiny85/
```

The main outputs are `firmware.elf` and `firmware.hex`.

## Upload firmware

The PlatformIO environment is configured for a USBtiny-compatible ISP
programmer:

```ini
[env:attiny85]
platform = atmelavr
board = attiny85
framework = arduino
board_build.mcu = attiny85
board_build.f_cpu = 8000000L
upload_protocol = usbtiny
```

Connect the programmer to the ATtiny85 programming pads or test points, then
run:

```powershell
cd Firmware\V1_1
pio run -t upload
```

The Micro USB connector on the hardware is part of the board power and charger
circuit. The firmware configuration expects ISP programming with `usbtiny`, not
USB bootloader programming.

## Hardware files

Open the board in Autodesk Eagle/Fusion Electronics using:

```text
Hardware/V1_1/Eagle/TinyDrop20_V1_1.sch
Hardware/V1_1/Eagle/TinyDrop20_V1_1.brd
```

Manufacturing files are already exported:

```text
Hardware/V1_1/Manufacturing/GERBER/TinyDrop20_V1_1_2020_03_09.zip
Hardware/V1_1/Manufacturing/Schematic/TinyDrop20_V1_1.pdf
```

## Development notes

- The charlieplexing code uses ATtiny pins `0` through `4`, corresponding to
  `PB0` through `PB4` in the schematic.
- Only one LED is actively driven at a time. Persistent images are produced by
  repeatedly scanning pixels fast enough for persistence of vision.
- To show different text, update `myString` in `src/main.cpp`.
- To enable the text display mode, set `mode = 1` or re-enable the EEPROM mode
  selection logic.
- Generated PlatformIO build files under `.pio/` should not be committed.
