# ButterBot robot firmware – nju aj ti OVERKLOKING mod

Fork of the [CircuitMess ButterBot](https://github.com/CircuitMess/ButterBot-Firmware-Public) firmware by **NUIT d.o.o.** ([nuit.hr](https://nuit.hr)).
⚠️ Use together with the [controller mod](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public) – flash both.

## What's new

### v4 – clock, volume and night mode
- **Settings → VOLUME / NIGHT MODE / NIGHT VOLUME** (on the controller): robot volume 10–100 %, and night hours (22–07, 23–07 or 00–07) with a quieter voice and no idle comments, wandering or breathing. The robot remembers them, so even its startup greeting uses the right volume. Unmuting now returns to your volume instead of jumping to 100 %.
- **Clock:** set the robot's date and time from the controller (Settings → DATE / TIME, 24 h) – or from your phone, as before. The robot keeps time while it is off and speaks it in 24 h format.
- **Greeting by the time of day** at startup (Good morning / afternoon / evening / "Working late?"), and on Thursdays a reminder that a new nju aj ti OVERKLOKING strip is out.
- Some idle comments now fit the time of day, and on Thursdays they mention the new strip.
- **Talkie Toaster:** 17 lines, most of them from the series.
- Flash together with the [controller mod v4](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v4) – the robot and the controller talk a new protocol.

### v3.2 – HRVATSKI
- **HRVATSKI** menu item: the robot speaks Croatian – a children's counting rhyme and a few Alan Ford classics – respelled so the US English TTS can say it. It uses the current VOICE, so Darth Vader speaking Croatian is included.

### v3.1
- The controller now shows its shutdown screen after Daisy, before the robot powers off.

### v3 – more characters
- **Talkie Toaster** and **Yoda** voices (Settings → VOICE) and menu items TALKIE TOASTER and YODA.
- **Yoda mode:** with VOICE → YODA the robot turns its sentences around – "I will remember" becomes "Remember, I will" (the controller shows it the same way).
- **Talkie Toaster mode:** with VOICE → TALKIE TOASTER, idle comments, facts, jokes and poke reactions become toast offers. Battery, error and other functional messages stay normal.
- **HAL 9000:** more lines, including "I'm sorry, Dave. I'm afraid I can't do that." and the stress pill. Refreshed OVERKLOKING lines.
- **Bigger app partition** (+192 KB). ⚠️ v3 changes the partition table – when upgrading, flash all partitions once (the command in Build & flash does that). Settings, owner face and IR codes are kept.

### v2 – voices
- **Settings → VOICE:** NORMAL / HAWKING / VADER / HAL 9000 for everything the robot says. Built on the robot's own flite TTS (pitch, speed, intonation) plus an effects chain (resampling, EQ, mask resonance, reverb) – no film audio samples.
- **Darth Vader** (VOICE → VADER) breathes in the mask after every line, with a quiet respirator hiss, and now and then while idle, like the idle comments (sometimes twice in a row).
- **Action menu:** DARTH OVERKLOKING (Darth Vader), OVERHAWKING (Stephen Hawking) and HAL 9000 – quotes in their own voice, whatever VOICE is set to.
- **SHUTDOWN** (last menu item): "Terminate consciousness?" – on YES, HAL sings *Daisy Bell*, slower and lower with every line, and the robot powers off. Shut Up or Poke during the song cancels it.

### v1
- **Action menu:** OVERKLOKING, BENDER and ULTRON – the robot says a random quote and the controller shows it. Long quotes go sentence by sentence; Shut Up or Poke stops them.
- **Hold Poke 1 s:** drives ~10 cm forward and says "nju aj ti OVERKLOKING is the best!"
- **Settings → SENSOR:** turn off the front and/or floor proximity sensors, for surfaces where they misfire. With the floor sensor off, the robot can drive off a table edge.
- **Menu navigation fix** (in the controller mod): joystick up/down now reliably moves through the action menu.

## About nju aj ti OVERKLOKING

**OVERKLOKING** is a Croatian satirical comic by [Dubravko Mataković](https://nuit.hr/overkloking/dubravko-matakovic/), focused on computers, technology, everyday life and the increasingly absurd relationship between people and the digital world.

The comic began more than two decades ago, when the web looked very different and computers were still mysterious enough to be funny on their own. Over time, it grew far beyond IT: into social satire, black comedy, current events and the ongoing disasters of one thoroughly dysfunctional family.

After more than **1,000 published pages**, the original run ended at Christmas 2025.

At Easter 2026 it returned at NUIT under a new name:

**nju aj ti OVERKLOKING**

The format remains simple: **one new strip every Thursday**, freely available online and without advertising.

Expect computers, bureaucracy, artificial intelligence, family catastrophes, current events and technology that supposedly exists to make life easier – including robots that can *“do nothing instead of me, and do it better.”*

**Read the comic:** [nuit.hr/overkloking](https://nuit.hr/overkloking/)

[![nju aj ti OVERKLOKING #1133 – We're Screwed!](docs/overkloking-1133-we-are-screwed.avif)](https://nuit.hr/overkloking/najebasmo/)
*[nju aj ti OVERKLOKING #1133 – We're Screwed!](https://nuit.hr/overkloking/najebasmo/) by [Dubravko Mataković](https://nuit.hr/overkloking/dubravko-matakovic/), English edition (both pages in Croatian). Robots making robots – what could go wrong?*

## Flash without building
Ready-made images are attached to releases from [v3](https://github.com/nuit-dev/ButterBot-Firmware-Public/releases/tag/v3) on (v1 and v2 have to be built from source) – no ESP-IDF needed, only Python and esptool: `pip install esptool`.
`<PORT>` is e.g. `COM6` (Windows), `/dev/cu.usbserial-XXXX` (macOS) or `/dev/ttyUSB0` (Linux). First check that it's the robot – it must say **16MB**:

```shell
python -m esptool -p <PORT> flash_id
```

**Option A – one file** (`…-robot-full.bin`): simplest, but erases the robot's saved settings, owner face and IR codes (like a stock restore).

```shell
python -m esptool --chip esp32s3 -p <PORT> -b 460800 --before default_reset --after no_reset write_flash 0 ButterBot-OVERKLOKING-v4-robot-full.bin
```

**Option B – keep settings** (`…-robot-parts.zip`): unzip, then run in the unzipped folder:

```shell
python -m esptool --chip esp32s3 -p <PORT> -b 460800 --before default_reset --after no_reset write_flash "@flash_args"
```

After either option, reset the robot over USB – it switches off – and turn it on with the power button, **held 4–5 s**:

```shell
python -c "import serial,time; s=serial.Serial(); s.port='<PORT>'; s.dtr=False; s.rts=False; s.open(); s.rts=True; time.sleep(0.1); s.rts=False"
```

⚠️ Don't use `--after hard_reset` (esptool's default) for the robot, it can leave it stuck. Flash the [controller](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/latest) too. SHA-256 checksums are in `SHA256SUMS-robot.txt` in the release.

## Build & flash
ESP-IDF 5.5.3: `idf.py build`.
The robot has 16 MB flash, the controller 4 MB – check with `esptool.py -p <PORT> flash_id` and don't mix up the firmwares.

⚠️ **Don't flash the robot with plain `idf.py flash`.** Its reset at the end (`--after hard_reset`) can leave the robot stuck: it ignores the power button and drains the battery. Flash from the `build` directory without that reset, then reset the robot over USB:

```shell
cd build
python -m esptool --chip esp32s3 -p <PORT> -b 460800 --before default_reset --after no_reset write_flash "@flash_args"
python -c "import serial,time; s=serial.Serial(); s.port='<PORT>'; s.dtr=False; s.rts=False; s.open(); s.rts=True; time.sleep(0.1); s.rts=False"
```

The robot then switches off; turn it on with the power button, **held 4–5 s**.
This writes all partitions – required once when upgrading to v3, because the data partitions moved.

Quotes live in `components/ButterBot-Common/src/Phrases.cpp`, voice presets in `main/src/Audio/SpeechGen.h` and `main/src/Audio/SpeechAudioGen.h`. Full change list and notes: [NUIT-CHANGES.md](NUIT-CHANGES.md).

## Credits
Original firmware © CircuitMess (MIT licence). Mod by Cyberlord ([@kibergospodar](https://github.com/kibergospodar)) / NUIT d.o.o.
*Daisy Bell* (Harry Dacre, 1892) is in the public domain.

---
*Original CircuitMess README below.*

# ButterBot Firmware

## Building

To build the ButterBot firmware, you'll need the ESP-IDF. You can find the getting started
guide [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/).
The production firmware is built using IDF version 5.5.3.

All required components are contained in the repository, and managed components (esp-sr,
esp-dl, esp-tflite-micro, face detection and recognition models, etc.) are fetched
automatically by the IDF component manager during the build.

In the root directory of the project:

**To build the firmware** run ```idf.py build```

**To upload the firmware to the device** run ```idf.py -p <PORT> flash```. Replace `<PORT>` with
the port the robot is attached to, for ex. ```COM6``` or ```/dev/ttyACM0```.

Flashing also uploads the data partitions: the SPIFFS filesystem image built from
[spiffs](spiffs), the object detection model from [models](models), and the speech
recognition and face detection / recognition models provided by the managed components.

## Restoring the stock firmware

To restore the stock firmware, you can download the prebuilt binary on
the [releases page](https://github.com/CircuitMess/ButterBot-Firmware-Public/releases) of
this repository and flash it manually using esptool:

```shell
esptool -c esp32s3 -b 921600 -p <PORT> write_flash 0 ButterBot-Firmware.bin
```

Alternatively, you can also do so using [CircuitBlocks](https://code.circuitmess.com/) by
logging in, clicking the "Restore Firmware" button in the top-right corner, and following the
on-screen instructions.
