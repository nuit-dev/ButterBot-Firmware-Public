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
- (Removed in v7, replaced by the CircuitMess 1.1 sensor settings) Proximity sensor filter: `Ctrl::SensorsAllOn/FrontOff/FloorOff/AllOff` commands (end of `Ctrl::Command`),
  handled in `main.cpp`, applied in `Services/BaseBoard` (`setProximityEnabled`). A disabled front sensor reads
  "no obstacle", a disabled bottom sensor reads "on the ground". Robot boots with all sensors on.
- Voice presets NORMAL / HAWKING / VADER / HAL 9000 / TALKIE TOASTER / YODA:
  `Ctrl::VoiceNormal/VoiceHawking/VoiceVader/VoiceHal/VoiceToaster/VoiceYoda`
  (end of `Ctrl::Command`), handled in `main.cpp` -> `Voice::user` (`Audio/VoicePreset.h`). The robot boots with
  the last VOICE (robot NVS since v5), and the controller sends its setting on every connect. Applied at the start
  of every utterance:
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
- `DaisySong` = controller menu item TERMINATE CONSCIOUSNESS (SHUTDOWN before v4.3): HAL sings the Daisy chorus from 2001, every line slower and lower
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
  - TALKIE TOASTER: every phrase category except the character quotes has its own Toaster list (since v5, see
    below); before v5 only Ramble, Fact, Joke, Poke and Profanity switched to `ToasterPhrases`.
  - YODA: `map()` / `mapShown()` turn sentences around ("I will remember." -> "Remember, I will."), using the first
    auxiliary verb in the first four words; sentences without one stay as they are.
  - Character quotes (Darth, Hawking, HAL, Daisy, Toaster, Yoda, Croatian) are never changed.
- `components/ButterBot-Common/CMakeLists.txt` builds `Phrases.cpp` with `-Os` (rarely called, saves ~6 KB).
- Partition table (v3): `factory` app partition 8432k -> 8624k, all data partitions after it moved by 0x30000
  (they are found by name, not by address). ~219 KB of the app partition are free (v6.2), ~44 KB of flash left at the end.
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
- v5.1 - merged CircuitMess robot 1.0.5 (tag v1.0.5 = commit 17f440e "Update 1.0.1", branch `merge-upstream-1.0.x`):
  - Upstream: BLE client / ConMan rework, `BaseBoard` pauses proximity scanning around motor writes and rebaselines
    `ProxRebaselineDelayMs` after, `MotionService` rebaselines after `StillBatchCount` still batches while lifted,
    RC turn speeds, FaceDet stop/unload race.
  - Proximity handling is exactly upstream's: the moving routines stop only for the front sensor and no longer on
    motion events, and `OverklokingRoutine` now does the same. Our SENSOR filter (`BaseBoard::setProximityEnabled`)
    sits on top of it, unchanged.
  - `BLE/Bonds.h` / `Bonds.cpp` (`BLE::isBonded`) are missing from the upstream commit, so upstream does not build;
    ours checks the address against `esp_ble_get_bond_device_list`.
- v6 - CHATTER, toaster-only idle chatter, toaster fidget:
  - `RobotConfigData::chatter` (`ChatterMode`: Off / Rare / Normal / Frequent / Toaster, 5th byte, default Normal;
    `RobotConfigDataV5Size`; shorter payloads leave it at the default). NVS key "Chatter", `RobotConfig::chatter`.
  - `IdleState`: `rescheduleRandomRoutine()` takes the delay from CHATTER (Off 15-30 min, or no schedule at all with
    ROAMING OFF; Rare 60-180; Normal 30-60; Frequent 10-20; Toaster 3-8 min). `onRobotConfig` is bound only to wake the
    ticking thread: `maybeStartRandomRoutine()` compares CHATTER / ROAMING with the values the schedule was made for and
    reschedules. `getDynamicTickInterval()` sleeps until woken while nothing is scheduled (UINT64_MAX).
  - `pickRandomRoutine()`: CHATTER Off drops Ramble and Observe; Toaster drops Observe and Person and gives Ramble
    weight 8 and, with the TOASTER voice, the fidget weight 4 (else 2, like Wander). Identical to upstream otherwise (`shouldYieldToEvent`, `endActiveRoutine` untouched).
  - `RambleKind::Toaster` / `Phrase::RambleToaster` (end of the enums): CHATTER Toaster speaks `Toaster_Ramble`
    (1,015 lines) whatever the VOICE, no time-of-day or Thursday kinds. The plain `Ramble` list is the original one
    again; the Toaster table (`Phrases::toasterMode`) uses `Toaster_Ramble` for `Ramble` as before. The 15 hand-picked
    Red Dwarf lines of v5 are in the list. `RambleData::kind` carries `Toaster` so the controller shows the same line.
  - `Routines/ToasterFidgetRoutine`: VOICE TALKIE TOASTER replaces `WanderRoutine` in the idle list (also with ROAMING
    OFF): 2-3 turns on the spot (8-18 degrees, 4000 ms per 180 degrees, right and left add up to the same angle),
    only on the ground and not while charging.
  - `main.cpp`: `pressed` / `lastPress` have initialisers (the application object lives on the heap). `Com::sendData`
    holds `txMutex` around the shared `txBuf` (it is called from several threads; the BLE send only takes the short
    connection lock, no lock cycle).
  - ~16 KB of the app partition are left (8,814,096 of 8,830,976 bytes).
- v6.1 - `ToasterFidgetRoutine`: twitches of 25-40 degrees (`MinTwitchDeg` / `MaxTwitchDeg`, were 8-18: the motors need
  a moment to get going, so a few degrees hardly moved the robot), pauses 150-350 ms. `pickRandomRoutine()`: with
  CHATTER TOASTER and the TOASTER voice the fidget weighs 4 (was 2, same as Wander). The fidget still does nothing while
  the robot is charging (also on USB).
- v6.2 - the 1,015 toaster lines were checked with a host build of `components/flite` (the same sources as the robot;
  print the phones of every word of every `pronounced` string): 48 lines respelled (`pronounced` only, `shown` is
  untouched) - "my yar" for Maillard, "nigh crome", "bye metallic", "brooskettah", "cha bahta", "hahlah", "wooster sher",
  "coodent" / "shoodent" / "woodent" / "duzzent" / "izzent" for the contractions, and the verbs flite read as nouns
  ("to pre zent", "ruhfyooz", "Liv long"). `ButterBot-Common` is identical in both repos again.
  - `sdkconfig`: `CONFIG_LOG_MAXIMUM_EQUALS_DEFAULT` (maximum log level INFO instead of DEBUG, `main.cpp` sets WARN at
    runtime anyway) and silent assertions (`CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT`, expressions are still
    evaluated): the app is 207 KB smaller (8,818,544 -> 8,606,240 bytes), ~219 KB of the app partition are left.

- v7 - merged CircuitMess 1.1 (robot e3b8dd5, branch `merge-upstream-1.1`):
  - Upstream: `Ctrl::Settings` / `SettingsData` (front sensor Off / On, bottom sensor Off / Reduced / On, `ActivityMode`
    Off / Chill / Active / Hyper), `Idle::SettingsChange` / `SettingsSync`, `Scenario::SettingsActivity` voice commands,
    `SettingsActivityRoutine`, `Settings` blob fields, `BaseBoard` proximity modes, `IdleState` split into
    `AnyPowerRandomRoutines` / `UnpluggedRandomRoutines` with `applyActivityMode()` (5-10 min, 1-5 min, 15-60 s),
    `Bonds.h` (inline `BLE::isBonded`, our `Bonds.cpp` is gone), `factory` partition 8640k.
  - Removed (replaced by the above): our sensor filter (`Ctrl::Sensors*`, `BaseBoard::setProximityEnabled`), CHATTER
    (`ChatterMode`, `RobotConfigData::chatter`, `RobotConfig::chatter`, NVS key "Chatter") and `RambleKind::Toaster` /
    `Phrase::RambleToaster` (toaster lines are only used with the TALKIE TOASTER voice, via `Phrases::toasterMode`).
  - Kept on top of upstream's `IdleState`: ROAMING (no `WanderRoutine` with ROAMING OFF), `BreathRoutine` (VADER voice
    only), `ToasterFidgetRoutine` instead of Wander with the TOASTER voice, night mode, and the toaster talk: TOASTER
    voice + `ActivityMode::Hyper` = Observe and Person off, Ramble weight 8, fidget weight 4, no time-of-day or
    Thursday comments (`RambleRoutine`). `Com::sendData` still holds `txMutex`.
  - `Toaster_Settings*` lines for the ten new settings phrases; ActivityMode names in the TOASTER voice ("Idle activity
    toaster").
  - The partition table changed: all partitions have to be flashed once. `dependencies.lock` moved a component (0.6.0 to
    0.8.0), so the release images are built from the new build output.
- v7.1 (in testing) - quieter, shorter toaster:
  - `IdleState`: the talking routines (`RambleRoutine`, `ObserveRoutine`, `PersonRoutine`, see `isTalkRoutine`) run on
    their own timer `nextTalkTime` (`rescheduleTalk`, delays set in `applyActivityMode`): Chill 2-5 h, Active 30-90 min,
    Hyper 5-15 min, toaster talk (TOASTER voice + Hyper) 2-5 min. The silent ones (Wander, ToasterFidget, Breath) keep
    upstream's `nextRandomRoutineTime`; in toaster talk the fidget uses 1-5 min instead of 15-60 s. When both are due,
    the talk goes first. `pickRandomRoutine(talk)` picks from one pool; the toaster weights 8 / 4 are gone.
    `getDynamicTickInterval` sleeps until the earlier of the two times. Night mode reschedules both.
  - `Toaster_Ramble`: all 1,015 lines rewritten (median 53 characters, was 94; max 89, was 147), plain words, no
    verbatim Red Dwarf dialogue. Checked with the host flite build (`lex_lookup` per word plus `lts_apply` against the
    system dictionary); 97 `pronounced` strings differ from `shown`: "I am", "izzent" / "duzzent" / "woodent" /
    "diddent" / "havvent" / "wuzzent", "cha bahta", "Hahlah", "brooskettah", "Panettonay", "Vedge ee mite",
    "Liv long", "Cloze your eyes", "Bo ties", "ruhfyooz", "Car pay dee em", "Et too, Broo tay".
  - App 47 KB smaller (8,581,296 bytes).
  - `QuoteRoutine`: ULTRON overrides the voice to `VoicePreset::Normal`, and `isCharacterQuote` (Phrases.cpp) includes
    `Phrase::Ultron`, so Yoda mode leaves the monologue as it is, on the robot and on the controller.

## Flashing the robot

Do NOT flash the robot with esptool `--after hard_reset` (default of `idf.py flash`): it ends up stuck, ignores
the power button and drains the battery (looks like a dead battery or a crash).
Check the port first (16 MB = robot, 4 MB = controller), flash with `--before no_reset --after no_reset`,
then reset in software over USB (pulse RTS with DTR low, e.g. pyserial). The robot then powers off; turn it on
with the power button, held 4–5 s (boot to `app_main` takes ~2 s). Flashing only the app (0x10000) is enough
when the data partitions did not change. Upgrading from v1 / v2 to v3: flash everything once (`@flash_args`),
because the data partitions moved.
