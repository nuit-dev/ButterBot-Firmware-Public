# ButterBot robot firmware – nju aj ti OVERKLOKING mod

Fork of the [CircuitMess ButterBot](https://github.com/CircuitMess/ButterBot-Firmware-Public) firmware by **NUIT d.o.o.** ([nuit.hr](https://nuit.hr)).
⚠️ Use together with the [controller mod](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public) – flash both.

## What's new

### v2 – voices
- **Settings → VOICE:** NORMAL / HAWKING / VADER / HAL 9000 for everything the robot says. Built on the robot's own flite TTS (pitch, speed, intonation) plus an effects chain (resampling, EQ, mask resonance, reverb) – no film audio samples.
- **Darth Vader** (VOICE → VADER) breathes in the mask after every line, with a quiet respirator hiss, and keeps breathing every 12–22 s while idle.
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
