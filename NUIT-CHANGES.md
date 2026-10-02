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
  `ToasterQuote`, `YodaQuote`, `CroatianQuote`,
  routines in `Routines/QuoteRoutine.h`. They always use their own voice (`Voice::setOverride`, cleared in the
  QuoteRoutine destructor, so the VOICE setting is back afterwards).
- `DaisySong` = controller menu item SHUTDOWN: HAL sings the Daisy chorus from 2001, every line slower and lower
  (`Voice::dying`), then the robot powers off via `ShutdownService::Shutdown(Command, /*speak*/ false)` (no
  "turning off" line; it waits 3 s instead so the controller can show its shutdown screen before the BLE link
  drops). Shut Up / Poke during the song cancels the shutdown.
  Texts in `Phrases.cpp`: `DarthPhrases`, `HawkingPhrases`, `HalPhrases`, `DaisyPhrases` (Daisy Bell, 1892, public domain),
  `ToasterPhrases`, `YodaPhrases`, `CroatianPhrases`.
- HRVATSKI (`CroatianQuote`): Croatian lines respelled for the US English TTS ("braat", "zhivvy yeh tee"...), spoken in
  the VOICE setting's voice; 'shown' is written without diacritics (the controller font has none). flite spells out
  words it doesn't think are English ("braht", "nahsh"), so check new respellings with a host build of
  `components/flite` (print the Word / Segment relations of `flite_synth_text`) before flashing.
- Voice modes in ButterBot-Common (`Phrases::toasterMode` / `Phrases::yodaMode`, set from the VOICE setting on robot
  and controller, so both pick and show the same line):
  - TALKIE TOASTER: Ramble, Fact, Joke, Poke and Profanity lines come from `ToasterPhrases`; functional messages
    (battery, errors, time, dice, modules...) stay as they are.
  - YODA: `map()` / `mapShown()` turn sentences around ("I will remember." -> "Remember, I will."), using the first
    auxiliary verb in the first four words; sentences without one stay as they are.
  - Character quotes (Darth, Hawking, HAL, Daisy, Toaster, Yoda, Croatian) are never changed.
- `components/ButterBot-Common/CMakeLists.txt` builds `Phrases.cpp` with `-Os` (rarely called, saves ~6 KB).
- Partition table (v3): `factory` app partition 8432k -> 8624k, all data partitions after it moved by 0x30000
  (they are found by name, not by address). ~140 KB of the app partition is free (v5), ~44 KB of flash left at the end.
  NVS stays at 0x9000, so settings, owner face and IR codes survive the upgrade.
- v4 - volume, night mode, clock:
  - `Ctrl::RobotConfig` (`RobotConfigData`: volume, `NightMode`, night volume) and `Ctrl::SetTime` (`SetTimeData`,
    local time, 24 h) at the end of `Ctrl::Command`; parsed in `Services/Com` (`OnRobotConfig`, `OnSetTime`).
  - `Util/RobotConfig.h` holds the values; they are also stored in robot NVS (keys "Volume", "NightMode",
    "NightVol", outside the settings blob) so the startup greeting already uses them. `main.cpp applyGain()`:
    muted / night volume / volume (unmute used to jump to 1.0, boot was 0.8). Default volume 80 %, night 40 %.
  - Night: `updateNight()` on every `Time::OnTimeUpdate` (5 s) with `isNightHour()`; `IdleState` starts no random
    routines at night (no Ramble, Wander, Observe, Person, breathing).
  - `Idle::TimeInfo` (`TimeInfoData`) - the RTC time, sent on every connect and after SetTime (controller DATE / TIME).
    The BM8563 RTC keeps running while the robot is off.
  - Startup greeting by `dayPeriod()` (`GreetingMorning/Afternoon/Evening/Night`, "Hello" while the clock isn't set)
    and `Thursday` on Thursdays. `RambleRoutine`: every third comment `RambleMorning/Afternoon/Evening/Night` or,
    on Thursdays, `Thursday`; `RambleData::kind` (`RambleKind`) tells the controller which list `id` is from.
  - `CurrentTimeRoutine` speaks 24 h ("fourteen oh five").
  - Talkie Toaster: 17 lines.
- v4.1 - voice command reference, Yoda and pronunciation fixes:
  - `docs/VOICE-COMMANDS.md` is generated by `tools/gen_voice_commands.py` from `Scenarios.h` (phrases, the fuzzy
    "core" words shown in bold), `Phrases.cpp` (answers, `shown` text), `ObjDet.cpp` (object classes) and
    `DiceRollRoutine.cpp` (follow-up words). Run it after changing any of these.
  - Yoda (`yodaSentence` in `Phrases.cpp`): questions, sentences starting with a question word or conjunction
    (what, why, if, but, so...) and subjects with a second pronoun ("I think it may") stay as they are; "cannot"
    counts as an auxiliary; "be" / "been" stay with the auxiliary ("A mug, it might be"); only the part up to the
    next comma moves ("A joke, I have, but..."); the subject is lower-cased unless it is "I", an acronym or "L.E.Dees".
  - `SpeechAudioGen` `FxParams::gain`: output level per preset, evens out what the soft clip adds (YODA was ~5 dB
    louder than the other voices). All presets 1.0 except YODA.
  - YODA voice: flite 140 Hz / stddev 24 / stretch 1.35, x0.8 resample (smaller head, ~165 Hz), high-pass 250 Hz,
    low-pass 3.8 kHz, +5 dB at 1.1 kHz, drive 1.2, gain 0.5. Chosen by ear from host renders of the real
    `SpeechAudioGen.cpp` (flite built natively), measured to the same loudness as the other voices.
  - Respellings (`pronounced` only, `shown` unchanged): "reed" for present-tense read, "Reeding", "izzent",
    "I am" (flite said "im"), "teers", "Shoodent", "lyves". Check new lines with a host flite build first.
- v4.2 - ROAMING:
  - `RobotConfigData::roaming` (1 = on) appended to the struct; `Com` accepts the 3-byte v4 payload
    (`RobotConfigDataV4Size`), roaming then stays on. NVS key "Roaming", `RobotConfig::roaming`.
  - Off: `IdleState::pickRandomRoutine` skips `WanderRoutine`; `PersonRoutine` looks ahead for
    `StationaryScanWindowMs` (1.5 s) without rotating, and greets a face where it stands (no centering, no driving).
    Commands (voice movement, Summon, Dance, OVERKLOKING drive) are not affected.
- v4.3 - TERMINATE CONSCIOUSNESS (the SHUTDOWN menu item, renamed on the controller):
  - `Scenario::TerminateRefusal` appended at the end of `BB::Action::Scenario`; `ScenarioData::raw` = the line
    (`Phrase::TerminateRefusal`, 5 lines, a character quote so YODA doesn't reorder it).
  - `Routines/TerminateRefusalRoutine` says that line in the HAL voice ("..." is silent) and sends nothing back,
    so the controller's popup stays. One `TerminateRefusalLineRoutine<I>` per line, because scenario mappings match
    the data exactly. The controller decides how many refusals come before `DaisySong`.
  - Daisy window title is now "TERMINATE CONSCIOUSNESS".
- v5 (was v4.4) - TALKIE TOASTER all the way:
  - `Phrases.cpp`: a `Toaster_<Phrase>` array for every phrase category except the character quotes (generated block
    "BEGIN TALKIE TOASTER"), `buildToasterMappings()`, and `PhraseArrays::outputs()`, which `get()` / `map()` /
    `mapShown()` use: the Toaster list while `Phrases::toasterMode`, else the normal one. Same placeholders as the
    originals, so the routines format them unchanged. The old `effective()` remap (fun lines -> `ToasterPhrases`)
    is gone; the TALKIE TOASTER menu item still uses `ToasterPhrases`.
  - Toaster lines were checked with a host flite build like the other respellings (`pronounced` = "izzent", "reed"...).
  - Code strings in Toaster mode: `ListenState` start prompts (durations measured with the Toaster preset), the dice
    roll line, the notification count and "now playing" (through their Toaster templates).
  - `TerminateRefusalRoutine` speaks in the VOICE setting's voice in Toaster mode (HAL otherwise); Daisy stays HAL.
  - `RambleRoutine`: time-of-day and Thursday comments also in Toaster mode (their Toaster versions).
  - VOICE is kept in robot NVS (key "Voice", `Settings::getVoice/setVoice`, written only when it changes) and applied at
    boot right after the robot config, so the startup greeting uses it before the controller connects.

## Flashing the robot

Do NOT flash the robot with esptool `--after hard_reset` (default of `idf.py flash`): it ends up stuck, ignores
the power button and drains the battery (looks like a dead battery or a crash).
Check the port first (16 MB = robot, 4 MB = controller), flash with `--before no_reset --after no_reset`,
then reset in software over USB (pulse RTS with DTR low, e.g. pyserial). The robot then powers off; turn it on
with the power button, held 4–5 s (boot to `app_main` takes ~2 s). Flashing only the app (0x10000) is enough
when the data partitions did not change. Upgrading from v1 / v2 to v3: flash everything once (`@flash_args`),
because the data partitions moved.
