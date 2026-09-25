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
