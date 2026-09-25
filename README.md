# ButterBot controller firmware – NUIT OVERKLOKING mod

Fork of the [CircuitMess ButterBot controller](https://github.com/CircuitMess/ButterBotCtrl-Firmware-Public) firmware by **NUIT d.o.o.** ([nuit.hr](https://nuit.hr)).
⚠️ Use together with the [robot mod](https://github.com/nuit-dev/ButterBot-Firmware-Public) – flash both.

## What's new
- **Menu navigation fix:** joystick up/down now reliably moves through the action menu (the dominant stick axis wins, and left/right also move in the list).
- **Action menu:** OVERKLOKING, BENDER and ULTRON right after SETTINGS – the robot says a random quote and the controller shows it. Long quotes follow the robot sentence by sentence; Shut Up or Poke stops them.
- **Hold Poke 1 s:** a fill bar, then the robot drives ~10 cm forward and says "nju aj ti OVERKLOKING is the best!" A short press is still the normal poke.
- **Settings → SENSOR:** ALL ON / FRONT OFF / FLOOR OFF / ALL OFF, for surfaces where the robot's proximity sensors misfire. Saved on the controller and sent to the robot on every connect. With the floor sensor off, the robot can drive off a table edge.

## About nju aj ti OVERKLOKING

**OVERKLOKING** is a Croatian satirical comic by [Dubravko Mataković](https://nuit.hr/overkloking/dubravko-matakovic/), focused on computers, technology, everyday life and the increasingly absurd relationship between people and the digital world.

The comic began more than two decades ago, when the web looked very different and computers were still mysterious enough to be funny on their own. Over time, it grew far beyond IT: into social satire, black comedy, current events and the ongoing disasters of one thoroughly dysfunctional family.

After more than **1,000 published pages**, the original run ended at Christmas 2025.

At Easter 2026 it returned at NUIT under a new name:

**nju aj ti OVERKLOKING**

The format remains simple: **one new strip every Thursday**, freely available online and without advertising.

Expect computers, bureaucracy, artificial intelligence, family catastrophes, current events and technology that supposedly exists to make life easier – including robots that can *"do nothing instead of me, and do it better."*

**Read the comic:** [nuit.hr/overkloking](https://nuit.hr/overkloking/)

## Build & flash
ESP-IDF 5.5.3: `idf.py build`, then `idf.py -p <port> flash`.
Copy `components/CMF/lib/glm` from the robot repo before building – it's missing here (git-ignored upstream).
The controller has 4 MB flash, the robot 16 MB – don't mix up the firmwares.
Full change list: [NUIT-CHANGES.md](NUIT-CHANGES.md).

## Credits
Original firmware © CircuitMess (MIT licence). Mod by Cyberlord / NUIT d.o.o.

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
