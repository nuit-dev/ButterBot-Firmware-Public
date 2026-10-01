#!/usr/bin/env python3
# Usage: python3 tools/gen_voice_commands.py [--full OUT_DIR]
# Builds docs/VOICE-COMMANDS.md (what you can say) and docs/VISION.md (what the camera recognises) from the firmware
# sources (Scenarios.h, Phrases.cpp, ObjDet.cpp, routines). --full writes VOICE.md and VISION.md with every answer
# the robot gives into OUT_DIR instead.
import re, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMMON = ROOT + '/components/ButterBot-Common/src/'
FULL = '--full' in sys.argv
OUT_DIR = sys.argv[sys.argv.index('--full') + 1] if FULL else ROOT + '/docs'
OUT_VOICE = OUT_DIR + ('/VOICE.md' if FULL else '/VOICE-COMMANDS.md')
OUT_VISION = OUT_DIR + '/VISION.md'

src = open(COMMON + 'Phrases.cpp').read()
# arrays
arrays={}
for m in re.finditer(r'static constexpr std::array (\w+) = std::to_array<Phrases::PhraseOutput>\(\s*\{(.*?)\n\s*\}\s*\);', src, re.S):
    name, body = m.group(1), m.group(2)
    items=[]
    for e in re.finditer(r'\{\s*("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*,\s*([-0-9.f]+)\s*,\s*(true|false)\s*(?:,\s*("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*))?\s*\}', body):
        def s(x): return ''.join(re.findall(r'"((?:[^"\\]|\\.)*)"', x)) if x else ''
        p=s(e.group(1)); sh=s(e.group(4))
        items.append({'p':p,'shown':sh or p,'rare':e.group(3)=='true'})
    arrays[name]=items
maps={}
for m in re.finditer(r'm\[static_cast<size_t>\(Phrase::(\w+)\)\] = (\w+);', src):
    maps[m.group(1)]=m.group(2)
PH = arrays

# ---- voice activations ------------------------------------------------------------------------------------------
scen = open(COMMON + 'Scenarios.h').read()
acts = []
for m in re.finditer(r'\{ BB::Action::Scenario::(\w+), (.*?)"([^"]*)", "([^"]*)", "([^"]*)"(?:, ([\d.]+)f?)? \}', scen):
    acts.append(dict(scenario=m.group(1), data=m.group(2), text=m.group(3), ph=m.group(4), core=m.group(5)))

SPOKEN = {'turn 180': 'turn one eighty', 'rotate 180': 'rotate one eighty'}


def bold_core(text, ph, core):
    """Bold the words the fuzzy fallback leans on (the phoneme 'core' of the phrase)."""
    shown = ' '.join('I' if x == 'i' else x for x in SPOKEN.get(text, text).lower().split())
    words, toks, core_toks = shown.split(), ph.split(), core.split()
    if not core_toks or len(words) != len(toks):
        return shown
    out, ci = [], 0
    for w, t in zip(words, toks):
        if ci < len(core_toks) and t == core_toks[ci]:
            out.append('**' + w + '**')
            ci += 1
        else:
            out.append(w)
    return ' '.join(out) if ci == len(core_toks) else shown


def phrases_for(scenario, data_filter=None):
    seen, out = set(), []
    for a in acts:
        if a['scenario'] != scenario or (data_filter and data_filter not in a['data']):
            continue
        s = bold_core(a['text'], a['ph'], a['core'])
        if s.lower() in seen:
            continue
        seen.add(s.lower())
        out.append(s)
    return out


def lines(array, subs=()):
    res = []
    for e in PH[array]:
        t = e['shown']
        for old, new in subs:
            t = t.replace(old, new, 1)
        res.append(t)
    return res


# ---- categories --------------------------------------------------------------------------------------------------
# (title, [(scenario, data filter)], what it does, needs, [(answer label, array, placeholder subs)])
OBJ = (('%s', '[object]'), ('%s', '[object]'))
CATS = [
    ('Facts', [('Fact', None)], 'Tells a random fact.', None,
     [('Answers', 'FactPhrases', ())]),
    ('Jokes', [('Joke', None)], 'Tells a joke.', None,
     [('Answers', 'JokePhrases', ())]),
    ('Pass the butter', [('PassTheButter', None)], 'Explains, at length, why it cannot pass the butter.', None,
     [('Answers', 'PassTheButterPhrases', ())]),
    ('"You pass butter"', [('YouPassButter', None)], 'The existential crisis from the show.', None,
     [('Answers', 'YouPassButterPhrases', ())]),
    ('Insults', [('Profanity', None)], 'Answers back.', None,
     [('Answers', 'ProfanityPhrases', ())]),
    ('Magic 8-ball', [('EightBall', None)],
     'Says "I\'m listening", waits up to 4 seconds for your question (any words), then answers like a magic 8-ball.',
     None,
     [('Answers', 'EightBall', ()), ('No question heard', 'EightBallNoQuestionPhrases', ())]),
    ('Dice', [('DiceRoll', None)],
     'Rolls dice. "Roll a dice" asks two follow-up questions (see below); "roll a d twenty" and the like skip '
     'the dice type question. Then it says the roll (for example "2d6 roll") and the total.', None,
     [('Asks how many', 'DiceRollAskCountPhrases', ()), ('Asks which dice', 'DiceRollAskTypePhrases', ()),
      ('No answer to "how many"', 'DiceRollCountTimeoutPhrases', ()),
      ('Number not understood', 'DiceRollCountInvalidPhrases', ()),
      ('No answer to "which dice"', 'DiceRollTypeTimeoutPhrases', ()),
      ('Dice type not understood', 'DiceRollTypeInvalidPhrases', ())]),
    ('Time', [('CurrentTime', None)],
     'Says the time from the robot clock, 24 h (set it in the controller Settings, DATE and TIME, or connect a phone).',
     None,
     [('Answers', 'CurrentTimeShowPhrases', (('%s', '[14:05]'),)),
      ('Clock not set', 'CurrentTimeNotConfiguredPhrases', ())]),
    ('What is this? (camera)', [('WhatsThis', None)],
     'Takes one camera picture and names what it sees, see [VISION.md](VISION.md).',
     None,
     [('One object', 'WhatsThisOnePhrases', OBJ), ('Two candidates', 'WhatsThisTwoPhrases', OBJ),
      ('Nothing recognised', 'WhatsThisNonePhrases', ())]),
    ('Faces: who is this', [('FaceDetect', 'Detect')],
     'Scans for a face and says if it is the owner or a stranger, see [VISION.md](VISION.md).', None,
     [('Scanning', 'FaceScanStartPhrases', ()), ('Owner', 'FaceOwnerRecognizedPhrases', ()),
      ('Stranger', 'FaceStrangerRecognizedPhrases', ()), ('No face', 'FaceNotDetectedPhrases', ())]),
    ('Faces: remember me', [('FaceDetect', 'Remember')],
     'Stores the face in front of the camera as the owner (one owner).', None,
     [('Saved', 'FaceRegisteredPhrases', ()), ('Owner already set', 'FaceAlreadyOwnerPhrases', ()),
      ('No face', 'FaceNotDetectedPhrases', ())]),
    ('Faces: forget the owner', [('FaceDetectForget', None)], 'Deletes the stored owner face.', None,
     [('Forgotten', 'FaceForgottenPhrases', ()), ('No owner stored', 'FaceNoOwnerPhrases', ())]),
    ('Movement', [('VoiceControl', 'Forward'), ('VoiceControl', 'Backward'), ('VoiceControl', 'Left'),
                  ('VoiceControl', 'Right'), ('VoiceControl', 'Rotate')],
     'Drives a short distance or turns. Not while charging, lifted or blocked.', None,
     [('Forward', 'VoiceForwardPhrases', ()), ('Backward', 'VoiceBackwardPhrases', ()),
      ('Left', 'VoiceTurnLeftPhrases', ()), ('Right', 'VoiceTurnRightPhrases', ()),
      ('Turn around', 'VoiceTurn180Phrases', ()), ('Cannot move', 'CannotMovePhrases', ()),
      ('Charging', 'CannotPerformPhrases', ())]),
    ('Dance', [('Dance', None)], 'Dances to music. Say "stop dancing" (or press the power button) to stop.', None,
     [('Start', 'DanceStartPhrases', ()), ('During the dance', 'DanceInterludePhrases', ()),
      ('Stopped', 'DanceStoppedPhrases', ()), ('Interrupted', 'DanceInterruptedPhrases', ())]),
    ('Lights on / off', [('LEDTurnOn', None), ('LEDTurnOff', None)], 'Switches the LED module.', 'the LED module',
     [('On', 'LEDTurnONPhrases', ()), ('Already on', 'LEDAlreadyTurnONPhrases', ()),
      ('Off', 'LEDTurnOFFPhrases', ()), ('Already off', 'LEDAlreadyTurnOFFPhrases', ()),
      ('No LED module', 'LEDModuleMissingPhrases', ())]),
    ('Light effects', [('LEDStrobe', None), ('LEDBreathe', None), ('LEDFaster', None), ('LEDSlower', None)],
     'Strobe, breathing (pulse), faster, slower.', 'the LED module',
     [('Strobe', 'LEDStrobePhrases', ()), ('Breathe', 'LEDBreathePhrases', ()),
      ('Faster', 'LEDFasterPhrases', ()), ('Already fastest', 'LEDAlreadyFasterPhrases', ()),
      ('Slower', 'LEDSlowerPhrases', ()), ('Already slowest', 'LEDAlreadySlowerPhrases', ())]),
    ('Temperature and humidity', [('TempHumModule', None)], 'Reads the climate sensor.',
     'the temperature and humidity module',
     [('Answers', 'TempHumReadingPhrases', (('%s', '[23 degrees]'), ('%d', '[45]'))),
      ('No module', 'TempHumModuleMissingPhrases', ())]),
    ('Temperature scale', [('TempHumScaleCelsius', None), ('TempHumScaleFahrenheit', None),
                           ('TempHumScaleKelvin', None)], 'Switches between Celsius, Fahrenheit and Kelvin.',
     'the temperature and humidity module',
     [('Celsius', 'TempHumScaleCelsiusPhrases', ()), ('Fahrenheit', 'TempHumScaleFahrenheitPhrases', ()),
      ('Kelvin', 'TempHumScaleKelvinPhrases', ())]),
    ('Air quality', [('GasModule', None)], 'Reads the gas sensor (needs about 10 minutes to calibrate).',
     'the gas module',
     [('Good', 'AirQualityOK', ()), ('Bad', 'AirQualityBad', ()), ('No module', 'GasModuleMissingPhrases', ())]),
    ('Intruder detection', [('IntruderDetectionOn', None), ('IntruderDetectionOff', None)],
     'Turns the motion (PIR) alarm on or off.', 'the PIR (motion) module',
     [('On', 'IntruderDetectionOnPhrases', ()), ('Off', 'IntruderDetectionOffPhrases', ()),
      ('No module', 'PIRModuleMissingPhrases', ())]),
    ('Infrared remote: learn', [('IR_train', None)],
     'Learns a button from any IR remote: aim the remote at the robot and press the button, then say the phrase '
     'that should send it later (any short phrase, up to 10 commands).', 'the IR module',
     [('Ready', 'IRTrainScanningPhrases', ()), ('Button received', 'IRTrainScanSuccessPhrases', ()),
      ('Asks for the phrase', 'IRTrainAskPhrasePhrases', ()), ('Saved', 'IRTrainSavedPhrases', ()),
      ('No signal', 'IRTrainScanTimeoutPhrases', ()), ('Unknown signal', 'IRTrainInvalidPhrases', ()),
      ('No phrase heard', 'IRTrainPhraseTimeoutPhrases', ()), ('Too similar', 'IRTrainTooSimilarPhrases', ()),
      ('Phrase taken', 'IRActionReservedPhrases', ()), ('Memory full', 'IRTrainFullPhrases', ())]),
    ('Infrared remote: use, list, forget', [('IR_list', None), ('IR_forget', None), ('IR_forgetAll', None)],
     'After training, just say your own phrase in listening mode and the robot sends that IR signal. '
     '"Forget infrared" asks which phrase to forget.', 'the IR module',
     [('Sent', 'IRActionDonePhrases', ()), ('List', 'IRListPrefixPhrases', ()), ('Nothing stored', 'IRListEmptyPhrases', ()),
      ('Asks which one', 'IRForgetAskCommandPhrases', ()), ('Forgotten', 'IRForgetSuccessPhrases', ()),
      ('Unknown phrase', 'IRForgetUnknownPhrases', ()), ('All forgotten', 'IRForgetAllSuccessPhrases', ()),
      ('Nothing to forget', 'IRForgetAllEmptyPhrases', ())]),
    ('Phone: notifications', [('PhoneListNotifs', None)],
     'Reads the phone notifications and asks after every 5 if it should continue: answer "yes" or "no".',
     'a phone connected over Bluetooth',
     [('Count', 'PhoneNotifCountPhrases', (('%d', '[3]'),)), ('Continue?', 'PhoneAskContinuePhrases', (('%d', '[2]'),)),
      ('Nothing new', 'PhoneNoNotifsPhrases', ()), ('All read', 'PhoneAllReadPhrases', ()),
      ('No phone', 'PhoneNotConnectedPhrases', ())]),
    ('Phone: music', [('PhoneWhatsPlaying', None), ('PhoneNextSong', None), ('PhonePrevSong', None),
                      ('PhonePlayMusic', None), ('PhoneStopMusic', None)],
     'Controls the music player on the phone.', 'a phone connected over Bluetooth',
     [('What is playing', 'PhonePlayingPhrases', (('%s', '[song]'), ('%s', '[artist]'))),
      ('Paused', 'PhonePausedPhrases', (('%s', '[song]'), ('%s', '[artist]'))),
      ('Nothing playing', 'PhoneNotPlayingPhrases', ()), ('Next', 'PhoneNextTrackPhrases', ()),
      ('Previous', 'PhonePrevTrackPhrases', ()), ('Play', 'PhonePlayMusicPhrases', ()),
      ('Stop', 'PhoneStopMusicPhrases', ())]),
    ('Shut down', [('Shutdown', None)], 'Turns the robot off.', None,
     [('Answers', 'TurningOff', ())]),
]

def anchor(title):
    a = title.lower()
    a = re.sub(r'[^a-z0-9 -]', '', a).strip().replace(' ', '-')
    return a


def answers_block(label, items):
    body = '\n'.join('- ' + t for t in items)
    if len(items) <= 4:
        return f'*{label}:*\n\n{body}\n'
    return f'<details><summary>{label} ({len(items)})</summary>\n\n{body}\n\n</details>\n'


md = []
w = md.append
w('# Talking to ButterBot')
w('')
n_phr = len({a["text"].lower() for a in acts})
n_ans = sum(len(v) for v in PH.values())
if FULL:
    w(f'Everything the robot understands when you speak to it (**{n_phr} phrases**) and everything it answers. '
      'What the camera recognises is in [VISION.md](VISION.md).')
else:
    w(f'Everything you can say to the robot: **{n_phr} phrases**. What it answers is left for you to find out. '
      'What the camera recognises is in [VISION.md](VISION.md).')
w('')
w('Generated from the firmware source (`Scenarios.h`, `Phrases.cpp`) with '
  '`tools/gen_voice_commands.py`, so it matches what is on the robot.')
w('')
w('## How listening works')
w('')
w('1. **Start listening**: short press of **SUMMON** on the controller, or a short press of the robot\'s power button.')
w('2. Wait for the prompt ("Yes?", "I\'m listening", "How can I help?"...). It plays a short sound first. '
  'Anything you say before the prompt ends is lost.')
w('3. Say **one phrase from the lists below, word for word**, within about 4 seconds.')
w('4. **Stop listening**: say "**cancel**" or "**nevermind**", or press the power button (or SUMMON) again.')
w('')
w('Recognition runs offline on the robot (Espressif ESP-SR MultiNet, English). It is **not** a chat assistant: it '
  'only matches the fixed phrases in this file. If nothing matches, the robot says it did not understand.')
w('')
w('### Tips')
w('')
w('- Use the exact phrase. "Tell me a joke" works, "can you tell me something funny please" does not.')
w('- The **bold** words are the key words. If the robot mishears the full phrase, it tries again on those words, so '
  'say them clearly.')
w('- Speak at a normal pace from 30–50 cm, facing the robot, with no music or TV in the background. '
  'Short phrases are more reliable than long ones.')
w('- Wait for the robot to finish talking before you press SUMMON again.')
w('- The VOICE setting changes the answers: in TALKIE TOASTER the fun lines (facts, jokes, insults, idle '
  'talk, pokes) come from the Toaster list, and in YODA the sentences are reordered ("Remember this, I will").')
w('')
w('## Commands')
w('')
if FULL:
    for title, *_ in CATS:
        w(f'- [{title}](#{anchor(title)})')
    w('- [Follow-up answers](#follow-up-answers)')
    w('')
if not FULL:
    w('| Command | What it does | Say |')
    w('|---|---|---|')
    for title, scs, what, needs, ans in CATS:
        phr = []
        for sc, flt in scs:
            phr += phrases_for(sc, flt)
        does = what.replace('|', '/') + (f' *Needs {needs}.*' if needs else '')
        w(f'| **{title}** | {does} | ' + '<br>'.join(phr) + ' |')
    w('')
for title, scs, what, needs, ans in (CATS if FULL else []):
    w(f'### {title}')
    w('')
    w(what + (f' Needs {needs}.' if needs else ''))
    w('')
    phr = []
    for sc, flt in scs:
        phr += phrases_for(sc, flt)
    w('*Say:* ' + ' · '.join(f'"{p}"' for p in phr))
    w('')
    for label, arr, subs in ans:
        w(answers_block(label, lines(arr, subs)))
    w('')

# ---- follow-ups ------------------------------------------------------------------------------------------------
dice = open(ROOT + '/main/src/Routines/DiceRollRoutine.cpp').read()
counts = re.findall(r'\{ \{ "([^"]+)", "[^"]+", "[^"]+"(?:, [\d.]+f)? \}, \d+ \}', dice)
types = []
for t in re.findall(r'\{ \{ "([^"]+)", "[^"]+", "[^"]+"(?:, [\d.]+f)? \}, DiceRollData::DiceType::\w+ \}', dice):
    if t not in types:
        types.append(t)
w('## Follow-up answers')
w('')
w('Some commands ask a question and listen again. Only these answers work there:')
w('')
w('| After | Say |')
w('|---|---|')
w('| Dice: "how many dice?" | ' + ', '.join(counts) + ' |')
w('| Dice: "which dice?" | ' + ', '.join(types) + ' |')
w('| Phone notifications: "should I continue?" | yes, no |')
w('| Dance (while dancing) | stop dancing |')
w('| Magic 8-ball | any question |')
w('| Learning an IR button | any short phrase, that becomes the command |')
w('| Any time while listening | cancel, nevermind |')
w('')

def save(path, lines_):
    text = '\n'.join(lines_).replace('\n\n\n', '\n\n')
    assert '\u2014' not in text, 'em dash in output'
    open(path, 'w').write(text.rstrip() + '\n')
    print(path)


save(OUT_VOICE, md)

# ---- vision ----------------------------------------------------------------------------------------------------
od = open(ROOT + '/main/src/Services/ObjDet.cpp').read()
classes = re.findall(r'\{ ObjClass::\w+, "(\w+)" \}', od)
voice_doc = 'VOICE.md' if FULL else 'VOICE-COMMANDS.md'
md = []
w = md.append
w('# What ButterBot sees')
w('')
w('The robot has one front camera and recognises two things with it: **10 kinds of objects** and **its owner\'s '
  'face**. Both run offline on the robot. ' +
  ('Everything it says about them is listed below. ' if FULL else 'What it says about them is left for you to find out. ') +
  f'Voice commands are in [{voice_doc}]({voice_doc}).')
w('')
w('Generated from the firmware source (`ObjDet.cpp`, `Scenarios.h`, `Phrases.cpp`) with `tools/gen_voice_commands.py`.')
w('')
w('## Objects')
w('')
w('The object model (TensorFlow Lite, 120×120 picture) knows these **10 things**:')
w('')
w('| | Object |')
w('|---|---|')
for i, c in enumerate(classes, 1):
    w(f'| {i} | {c.lower()} |')
w('')
w('### How it decides')
w('')
w('- It takes **one picture** from the front camera.')
w('- At least **90 %** sure of the best match: it names that one object.')
w('- Less sure, but the second best match is at least **40 %**: it names both ("a mug or a bottle").')
w('- Otherwise it says it does not know.')
w('- It only knows these 10, so anything else (a cat, a shoe) comes out as the closest of them, or nothing.')
w('')
w('### How to ask')
w('')
w('Start listening (short press of SUMMON), then say one of these:')
w('')
for p_ in phrases_for('WhatsThis'):
    w(f'- "{p_}"')
w('')
w('Hold the object **20–40 cm in front of the robot**, fill most of the picture with it, and keep the light good '
  'and the background plain.')
w('')
w('**On its own:** every now and then, while idle (not in night mode), the robot looks around and comments on what '
  'it sees, one object or a pair.')
w('')
if FULL:
    for label, arr, subs in [('One object', 'WhatsThisOnePhrases', OBJ), ('Two candidates', 'WhatsThisTwoPhrases', OBJ),
                             ('Nothing recognised', 'WhatsThisNonePhrases', ())]:
        w(answers_block('"What is this?", ' + label.lower(), lines(arr, subs)))
    w('### Idle comments per object')
    w('')
    for c in classes:
        w(answers_block(c, lines('Observe' + c + 'Phrases')))
    w('### Idle comments for two objects seen together')
    w('')
    for k in [k for k in PH if re.match(r'Observe[A-Z][a-z]+[A-Z][a-z]+Phrases$', k)]:
        nm = re.sub(r'([a-z])([A-Z])', r'\1 + \2', k[len('Observe'):-len('Phrases')]).lower()
        w(answers_block(nm, lines(k)))
    w(answers_block('Nothing recognised', lines('ObserveNonePhrases')))
w('## Faces')
w('')
w('The robot remembers **one face, its owner**. Everyone else is a stranger.')
w('')
w('| To | Say |')
w('|---|---|')
w('| Save your face as the owner | ' + '<br>'.join(phrases_for('FaceDetect', 'Remember')) + ' |')
w('| Ask who is in front of it | ' + '<br>'.join(phrases_for('FaceDetect', 'Detect')) + ' |')
w('| Forget the owner | ' + '<br>'.join(phrases_for('FaceDetectForget')) + ' |')
w('')
w('Look straight at the camera from about 30–60 cm, in good light. To change the owner, forget the old one first.')
w('')
w('**On its own:** every now and then, while idle, the robot turns around looking for a face, drives up to it and '
  'greets its owner or a stranger. With ROAMING off (controller Settings) it stays put and only looks straight ahead.')
w('')
if FULL:
    for label, arr in [('Scanning', 'FaceScanStartPhrases'), ('Owner', 'FaceOwnerRecognizedPhrases'),
                       ('Stranger', 'FaceStrangerRecognizedPhrases'), ('No face', 'FaceNotDetectedPhrases'),
                       ('Saved', 'FaceRegisteredPhrases'), ('Owner already set', 'FaceAlreadyOwnerPhrases'),
                       ('Forgotten', 'FaceForgottenPhrases'), ('No owner stored', 'FaceNoOwnerPhrases'),
                       ('Idle greeting, owner', 'PersonOwnerGreetingPhrases'),
                       ('Idle greeting, stranger', 'PersonStrangerGreetingPhrases')]:
        w(answers_block(label, lines(arr)))
save(OUT_VISION, md)
