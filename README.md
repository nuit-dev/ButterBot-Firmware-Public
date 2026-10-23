# ButterBot robot firmware – nju aj ti OVERKLOKING mod

Fork of the [CircuitMess ButterBot](https://github.com/CircuitMess/ButterBot-Firmware-Public) firmware by **NUIT d.o.o.** ([nuit.hr](https://nuit.hr)).
⚠️ Use together with the [controller mod](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public) – flash both.

**What can I say to it?** [Voice commands](docs/VOICE-COMMANDS.md) · **What does it recognise?** [Objects and faces](docs/VISION.md)

**I had a lot of fun playing with this robot, but this one (v5+) is hands down the best mod yet. I have the robot on my table turned on non-stop, and to have something that is actually reminiscent of a living Talkie Toaster from Red Dwarf in such an elegant (and eloquent manner) is an absolute blast! Do try it!**

## See it, hear it

<table>
<tr>
<td colspan="2" width="834" align="center"><video src="https://github.com/user-attachments/assets/6fdd7308-928b-4a4c-8208-ec6f5be190fd"></video></td>
</tr>
<tr>
<td colspan="2" align="center"><video src="https://github.com/user-attachments/assets/409d0d14-c5a2-4fcc-9ce2-d0b4e1e28681"></video></td>
</tr>
<tr>
<td width="417" valign="top"><video src="https://github.com/user-attachments/assets/eb9846e3-df61-445b-9cf5-4f85d552a51a"></video></td>
<td width="417" valign="top"><video src="https://github.com/user-attachments/assets/f6a1806b-28c4-4e37-933e-f048e1a9b01a"></video></td>
</tr>
<tr>
<td width="417" valign="top"><video src="https://github.com/user-attachments/assets/6e55f5a4-f735-4a1c-ba20-2d0bb315f70a"></video></td>
<td width="417" valign="top"><video src="https://github.com/user-attachments/assets/033363b6-99c5-4c50-8bb3-8bc71b899f68"></video></td>
</tr>
</table>

## What's new

### v7 – CircuitMess update 1.1
- Includes the official CircuitMess update 1.1: proximity sensor settings (front on / off, bottom off / reduced / on), idle **activity levels** you can also change by voice ("be chill", "be active", "go crazy"), battery and Bluetooth improvements and new phrases.
- **Settings changed:** the mod's SENSOR and CHATTER rows are replaced by the official **FRONT**, **BOTTOM** and **ACTIVITY** rows. ACTIVITY is OFF / CHILL / ACTIVE / HYPER – the robot starts something on its own every 5–10 min, 1–5 min or 15–60 s. With VOICE → TALKIE TOASTER the highest level is called **TOASTER**: the robot then talks only toaster lines (no object descriptions or face greetings in between) and fidgets on the spot. Toaster lines are only used with the TALKIE TOASTER voice now; the other voices keep the original comments.
- Kept: ROAMING, all the voices and menu quotes, night mode, volume, the clock, STARTUP.
- The 1.1 settings phrases ("Front sensor on", "Idle activity chill", ...) have toaster versions too.
- ⚠️ **The partition table changed** (app partition 8640k, like the official one). Flash the full image or all the parts once – flashing the app alone is not enough. Settings, owner face and IR codes are kept.
- Flash together with the [controller mod v7](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v7).

### v6.2 – toaster pronunciation and room to grow
- The 1,015 toaster lines were run through the robot's speech synthesizer on a computer, word by word, and about fifty now sound right: Maillard, Nichrome, bruschetta, ciabatta, challah, Worcestershire, "wouldn't" and friends, and verbs it used to read as nouns ("to present the toast", "you cannot refuse"). What the controller shows is unchanged.
- The firmware is 207 KB smaller (no assert messages or debug logs, which were never shown), so about 219 KB of the app partition are free for what comes next.
- Robot only – works with the [controller mod v6](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v6), nothing changed there.

### v6.1 – bigger toaster twitches
- The toaster fidget is easier to see: each twitch is 25–40 degrees instead of 8–18, which hardly showed. With CHATTER → TOASTER it is about every third idle turn.
- Robot only – works with the [controller mod v6](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v6), nothing changed there.

### v6 – Talkie Toaster chatter
- **Settings → CHATTER** (on the controller, between VOICE and STARTUP) sets how often the robot says something on its own while idle: **OFF / RARE / NORMAL / OFTEN / TOASTER**. The gaps are RARE 1–3 h, NORMAL 30–60 min, OFTEN 10–20 min and TOASTER 3–8 min. OFF is silent and does not look around; with ROAMING OFF as well the robot does nothing at all while idle. The change applies at once, no waiting for the old schedule, and the robot remembers it.
- **CHATTER → TOASTER** is the full toaster: the robot only says Talkie Toaster lines, whatever VOICE is set to – 1,015 of them: toast offers, bready facts, Red Dwarf style monologues. No time-of-day or Thursday comments, no object descriptions or face greetings in between, and the toaster gets most of the turns. With VOICE → TALKIE TOASTER the ordinary idle comments come from the same list. The other voices keep the original CircuitMess comments.
- **Toaster fidget:** with VOICE → TALKIE TOASTER the robot no longer wanders when idle. It twitches on the spot instead – two or three quick turns of 8–18 degrees that end facing the way it started – also with ROAMING OFF. Not while it is charging.
- Two stability fixes: the power button state is initialised at startup, and `Com::sendData` no longer shares its buffer between threads without a lock.
- Flash together with the [controller mod v6](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v6). The protocol grew by one byte (CHATTER) and stays compatible with v5.x.

### v5.1 – CircuitMess update 1.0.5
- Includes the official CircuitMess robot update 1.0.5: a steadier Bluetooth connection to the controller, a workaround for the motors disturbing the proximity sensors, sharper and faster turns when you drive it with the controller, and a face detection fix.
- The proximity sensors work exactly as in the official update (moving only stops for obstacles in front, the OVERKLOKING drive too). **Settings → SENSOR** still turns them off.
- Flash together with the [controller mod v5.1](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v5.1).

### v5 – Talkie Toaster, all the way
- With **VOICE → TALKIE TOASTER**, everything the robot says is now Talkie Toaster: 465 new lines covering every message – greetings, battery, modules, lights, the clock, dice, the camera, faces, phone, IR, movement, dancing, errors and the "Yes?" when it starts listening. The information stays (battery, time, temperature, the object it sees), it just comes with an offer of toast. "It's 14:05. Perfect time for toast."
- **Toast facts and toast jokes** instead of the general Toaster lines, so FACT and JOKE still give you a fact and a joke.
- **TERMINATE CONSCIOUSNESS** in Toaster mode: the toaster refuses ("You can't switch me off! I haven't made you any toast yet!"), and Daisy at the end is still HAL.
- The menu characters (Bender, Ultron, Darth Vader, Hawking, HAL, Daisy, Yoda, HRVATSKI) keep their own lines, and the other voices are unchanged.
- The robot remembers the VOICE setting, so the startup greeting is already in the right voice, before the controller connects (it used to greet in NORMAL until the controller sent the setting).
- Flash together with the [controller mod v5](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v5), which shows the same Toaster lines.

### v4.3 – TERMINATE CONSCIOUSNESS
- The SHUTDOWN menu item is now **TERMINATE CONSCIOUSNESS**, and HAL doesn't always let you. Each time you pick it, he may refuse in his own voice – "I am sorry, I can't allow you to do that.", "I am afraid.", "...", "Go away." or "Think of the butter!" – and you're back in the menu. After a random number of refusals (usually one or two, sometimes four, rarely none) he sings *Daisy Bell* and powers off, like before. No line repeats, and "..." is never the last word.
- Flash together with the [controller mod v4.3](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v4.3).

### v4.2 – ROAMING
- **Settings → ROAMING** (on the controller): OFF keeps the robot in place while idle – no wandering around, and when it looks for a face it only looks straight ahead instead of turning and driving towards you. Handy on a table. Voice commands, Summon and dancing still move it. The robot remembers the setting.
- Flash together with the [controller mod v4.2](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v4.2). Mixing with v4.1 works too, roaming then simply stays on.

### v4.1 – talking to ButterBot
- **[Voice commands](docs/VOICE-COMMANDS.md):** all 237 phrases the robot understands, in one table by what they do, with the key words to say clearly, the follow-up answers (dice, notifications, dancing) and tips for being understood.
- **[Vision](docs/VISION.md):** the 10 objects its camera recognises, how it decides, and how face recognition (owner or stranger) works.
- Both are generated from the firmware source with `tools/gen_voice_commands.py`, so they stay in sync. What the robot answers is not listed – that part is for you to find out.
- **Yoda mode** turns sentences around more naturally. Questions and sentences that start with why / if / but stay as they are, and only the part up to the next comma moves – "A joke about latency, I have, but you would have to wait for it."
- **Yoda voice** is higher, smaller and croakier, and no longer louder than the other voices.
- **Pronunciation fixes:** "read" no longer comes out as "red" where it should be "reed", plus "Reading", "isn't", "I'm", "shouldn't", "tears" and "lives".
- Flash together with the [controller mod v4.1](https://github.com/nuit-dev/ButterBotCtrl-Firmware-Public/releases/tag/v4.1), which shows the Yoda sentences the same way. The protocol did not change.

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
- **Settings → SENSOR:** turn off the front and/or floor proximity sensors, for surfaces where they misfire. With the floor sensor off, the robot can drive off a table edge (since v5.1 moving only stops for the front sensor anyway).
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

[![nju aj ti OVERKLOKING #1151 – 2028.](docs/overkloking-1151-2028.avif)](https://nuit.hr/overkloking/robokalipsa-2028/)
*[nju aj ti OVERKLOKING #1151 – 2028.](https://nuit.hr/overkloking/robokalipsa-2028/) by [Dubravko Mataković](https://nuit.hr/overkloking/dubravko-matakovic/), English edition (both pages in Croatian). The year after the AI robots replaced us – and the zoo has a new exhibit.*

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
python -m esptool --chip esp32s3 -p <PORT> -b 460800 --before default_reset --after no_reset write_flash 0 ButterBot-OVERKLOKING-v7-robot-full.bin
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

**Won't turn on?** If the power button does nothing but the USB port is still there, the robot is stuck in download mode (e.g. after `flash_id` or an interrupted flash while it was on). Run the reset command above, then hold the power button 4–5 s.

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
