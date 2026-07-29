# PALE SIGNAL (MVP)

A text-based interactive game for the M5Stack Cardputer-Adv. 
You are an unnamed operator for the FERRYMAN rescue team. The MERIDIAN DEEP station has returned after 34 years. 
Read the logs, hack the docking system, and hear the first voice on the network. 
This is a vertical slice: a prologue and the engine that powers the full game.

Inspired by Classic Marathon / SOMA / SIGNALIS / Zork. Original setting.
## Structure

```
kodzimim/
  src/core/       portable core (engine, parser, pager) — shared between PC and hardware
    platform.h    platform interface (screen/keys/sound/files/save)
    game.h/.cpp   prologue logic, commands, puzzle
  src/desktop/    PC build for testing (emulates a 40x16 screen in the terminal)
  src/esp32/      Build for Cardputer-Adv (M5Unified + M5Cardputer)
  content/prologue/  Game text files (.txt) can be edited without recompilation.
  Makefile        PC version build
  platformio.ini  device assembly/firmware flashing
```

The principle: **the engine is code, the story is data.** 
Text files are stored separately in `content/` and loaded at runtime (from LittleFS on the device).

## Computer-based test (without hardware)

You need `g++` (or clang) and `make`.

```
make            # build
make run        # play in terminal
```

Environment variables for debugging:

- `KD_DELAY=0` — disable the typing effect (instant output; convenient for testing)
- `KD_BEEP=1` — set the audible signal (bell) to "beep"
- `KD_COLS`/`KD_ROWS` — resize the emulated screen

Example of a quick walkthrough: `KD_DELAY=0 ./kodzimim`

## Firmware for Cardputer-Adv

Need [PlatformIO]

```
pio run -t upload      # build and flash the code
pio run -t uploadfs    # Upload text files (content/) to LittleFS.
```

Setting `data_dir = content` in `platformio.ini` places files at the root of LittleFS, 
and the engine reads `/prologue/boot.txt`, etc. 
If PlatformIO doesn't recognize the `m5stack-stamps3` board for the ADV, 
adjust the `board` setting to match your specific core (StampS3/S3A).

## In-game commands

`help  dir  open <name>  mail  logs  status  unlock <code>  clear  shutdown`
(abbreviations: `o`=open, `h`/`?`=help, `l`=logs)

Prologue puzzle: the docking code is a date. It is
cross-referenced between `mail` and `open roster` [REDACTED]. This
is the core loop of the entire game: read logs → find the key → unlock the next step.
