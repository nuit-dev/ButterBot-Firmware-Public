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
- Voice presets NORMAL / HAWKING / VADER / HAL 9000: `Ctrl::VoiceNormal/VoiceHawking/VoiceVader/VoiceHal`
  (end of `Ctrl::Command`), handled in `main.cpp` -> `Voice::user` (`Audio/VoicePreset.h`). Robot boots with NORMAL,
  the controller sends its setting on every connect. Applied at the start of every utterance:
  - `Audio/SpeechGen` – flite features of cmu_us_kal16 (`int_f0_target_mean`, `int_f0_target_stddev`,
    `duration_stretch`), table `PresetParams` in `SpeechGen.h`
  - `Audio/SpeechAudioGen` – effects per preset, table `FxParams` in `SpeechAudioGen.h`: resample slowdown
    (lowers pitch and formants), high/low-pass, peaking EQ, soft clip, comb "mask" resonance, reverb.
    VADER also breathes after every line (synthesized noise, +-15 % variation) with a quiet respirator hiss
    under the speech. No film samples.
  - VADER while idle: `IdleState` plays one mask breath (`BreathOnlySource`) every 12-22 s when nothing else
    is running. Any new sound interrupts it (`Audio::play` stops the current source).
- New menu scenarios (end of `BB::Action::Scenario`): `DarthQuote`, `HawkingQuote`, `HalQuote`, `DaisySong`,
  routines in `Routines/QuoteRoutine.h`. They always use their own voice (`Voice::setOverride`, cleared in the
  QuoteRoutine destructor, so the VOICE setting is back afterwards).
- `DaisySong` = controller menu item SHUTDOWN: HAL sings the Daisy chorus from 2001, every line slower and lower
  (`Voice::dying`), then the robot powers off via `ShutdownService::Shutdown(Command, /*speak*/ false)` (no
  "turning off" line). Shut Up / Poke during the song cancels the shutdown.
  Texts in `Phrases.cpp`: `DarthPhrases`, `HawkingPhrases`, `HalPhrases`, `DaisyPhrases` (Daisy Bell, 1892, public domain).
- App partition is nearly full: ~9.8 KB free after the voice mod. Check `idf.py build` output after every change.

## Flashing the robot

Do NOT flash the robot with esptool `--after hard_reset` (default of `idf.py flash`): it ends up stuck, ignores
the power button and drains the battery (looks like a dead battery or a crash).
Check the port first (16 MB = robot, 4 MB = controller), flash with `--before no_reset --after no_reset`,
then reset in software over USB (pulse RTS with DTR low, e.g. pyserial). The robot then powers off; turn it on
with the power button, held 4–5 s (boot to `app_main` takes ~2 s). Flashing only the app (0x10000) is enough
when the data partitions did not change.
