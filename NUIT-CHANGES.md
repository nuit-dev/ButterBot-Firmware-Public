# NUIT changes (ButterBot robot firmware)

Custom OVERKLOKING mod by NUIT d.o.o. Must be flashed together with the matching ButterBotCtrl firmware.

- New scenarios (appended at the end of `BB::Action::Scenario` in ButterBot-Common):
  `OverklokingDrive`, `OverklokingQuote`, `BenderQuote`, `UltronQuote`
- `Routines/OverklokingRoutine` – Poke held 1 s: drives cca. 10 cm forward (same safety checks as "Go forward"),
  then says "nju aj ti OVERKLOKING is the best!"
- `Routines/QuoteRoutine` – menu items OVERKLOKING / BENDER / ULTRON: random quote, shown on the controller.
  Quotes over 140 characters are spoken sentence by sentence. Shut Up or Poke stops the quote.
- Quote texts: `components/ButterBot-Common/src/Phrases.cpp` (`OverklokingPhrases`, `OverklokingBestPhrases`,
  `BenderPhrases`, `UltronPhrases`). `pronounced` = spoken (US English TTS spelling), `shown` = controller text.
  Keep ButterBot-Common identical in both repos.
- Proximity sensor filter: `Ctrl::SensorsAllOn/FrontOff/FloorOff/AllOff` commands (end of `Ctrl::Command`),
  handled in `main.cpp`, applied in `Services/BaseBoard` (`setProximityEnabled`). A disabled front sensor reads
  "no obstacle", a disabled bottom sensor reads "on the ground". Robot boots with all sensors on.
- Voice presets NORMAL / HAWKING / VADER / HAL 9000 / TALKIE TOASTER / YODA:
  `Ctrl::VoiceNormal/VoiceHawking/VoiceVader/VoiceHal/VoiceToaster/VoiceYoda`
  (end of `Ctrl::Command`), handled in `main.cpp` -> `Voice::user` (`Audio/VoicePreset.h`). Robot boots with NORMAL,
  the controller sends its setting on every connect. Applied at the start of every utterance:
  - `Audio/SpeechGen` – flite features of cmu_us_kal16 (`int_f0_target_mean`, `int_f0_target_stddev`,
    `duration_stretch`), table `PresetParams` in `SpeechGen.h`
  - `Audio/SpeechAudioGen` – effects per preset, table `FxParams` in `SpeechAudioGen.h`: resample slowdown
    (lowers pitch and formants), high/low-pass, peaking EQ, soft clip, comb "mask" resonance, reverb.
    VADER also breathes after every line (synthesized noise, +-15 % variation) with a quiet respirator hiss
    under the speech. No film samples.
  - VADER while idle: `Routines/BreathRoutine` is one of the idle random routines (like Ramble, every 5-20 min),
    picked only while VOICE is VADER. One mask breath (`BreathOnlySource`), about every 4th time two in a row.
- New menu scenarios (end of `BB::Action::Scenario`): `DarthQuote`, `HawkingQuote`, `HalQuote`, `DaisySong`,
  `ToasterQuote`, `YodaQuote`,
  routines in `Routines/QuoteRoutine.h`. They always use their own voice (`Voice::setOverride`, cleared in the
  QuoteRoutine destructor, so the VOICE setting is back afterwards).
- `DaisySong` = controller menu item SHUTDOWN: HAL sings the Daisy chorus from 2001, every line slower and lower
  (`Voice::dying`), then the robot powers off via `ShutdownService::Shutdown(Command, /*speak*/ false)` (no
  "turning off" line; it waits 3 s instead so the controller can show its shutdown screen before the BLE link
  drops). Shut Up / Poke during the song cancels the shutdown.
  Texts in `Phrases.cpp`: `DarthPhrases`, `HawkingPhrases`, `HalPhrases`, `DaisyPhrases` (Daisy Bell, 1892, public domain),
  `ToasterPhrases`, `YodaPhrases`.
- Voice modes in ButterBot-Common (`Phrases::toasterMode` / `Phrases::yodaMode`, set from the VOICE setting on robot
  and controller, so both pick and show the same line):
  - TALKIE TOASTER: Ramble, Fact, Joke, Poke and Profanity lines come from `ToasterPhrases`; functional messages
    (battery, errors, time, dice, modules...) stay as they are.
  - YODA: `map()` / `mapShown()` turn sentences around ("I will remember." -> "Remember, I will."), using the first
    auxiliary verb in the first four words; sentences without one stay as they are.
  - Character quotes (Darth, Hawking, HAL, Daisy, Toaster, Yoda) are never changed.
- `components/ButterBot-Common/CMakeLists.txt` builds `Phrases.cpp` with `-Os` (rarely called, saves ~6 KB).
- Partition table (v3): `factory` app partition 8432k -> 8624k, all data partitions after it moved by 0x30000
  (they are found by name, not by address). ~198 KB of the app partition is free, ~44 KB of flash left at the end.
  NVS stays at 0x9000, so settings, owner face and IR codes survive the upgrade.

## Flashing the robot

Do NOT flash the robot with esptool `--after hard_reset` (default of `idf.py flash`): it ends up stuck, ignores
the power button and drains the battery (looks like a dead battery or a crash).
Check the port first (16 MB = robot, 4 MB = controller), flash with `--before no_reset --after no_reset`,
then reset in software over USB (pulse RTS with DTR low, e.g. pyserial). The robot then powers off; turn it on
with the power button, held 4–5 s (boot to `app_main` takes ~2 s). Flashing only the app (0x10000) is enough
when the data partitions did not change. Upgrading from v1 / v2 to v3: flash everything once (`@flash_args`),
because the data partitions moved.
