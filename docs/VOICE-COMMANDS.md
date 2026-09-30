# Talking to ButterBot

Everything you can say to the robot: **237 phrases**. What it answers is left for you to find out. What the camera recognises is in [VISION.md](VISION.md).

Generated from the firmware source (`Scenarios.h`, `Phrases.cpp`) with `tools/gen_voice_commands.py`, so it matches what is on the robot.

## How listening works

1. **Start listening**: short press of **SUMMON** on the controller, or a short press of the robot's power button.
2. Wait for the prompt ("Yes?", "I'm listening", "How can I help?"...). It plays a short sound first. Anything you say before the prompt ends is lost.
3. Say **one phrase from the lists below, word for word**, within about 4 seconds.
4. **Stop listening**: say "**cancel**" or "**nevermind**", or press the power button (or SUMMON) again.

Recognition runs offline on the robot (Espressif ESP-SR MultiNet, English). It is **not** a chat assistant: it only matches the fixed phrases in this file. If nothing matches, the robot says it did not understand.

### Tips

- Use the exact phrase. "Tell me a joke" works, "can you tell me something funny please" does not.
- The **bold** words are the key words. If the robot mishears the full phrase, it tries again on those words, so say them clearly.
- Speak at a normal pace from 30–50 cm, facing the robot, with no music or TV in the background. Short phrases are more reliable than long ones.
- Wait for the robot to finish talking before you press SUMMON again.
- The VOICE setting changes the answers: in TALKIE TOASTER the fun lines (facts, jokes, insults, idle talk, pokes) come from the Toaster list, and in YODA the sentences are reordered ("Remember this, I will").

## Commands

| Command | What it does | Say |
|---|---|---|
| **Facts** | Tells a random fact. | tell me a **fact**<br>give me a **fact**<br>I want a **fact**<br>do you know a **fact**<br>tell me something **interesting**<br>hit me with a **fact**<br>drop some **knowledge**<br>**blow** my **mind**<br>**teach** me something **new** |
| **Jokes** | Tells a joke. | tell me a **joke**<br>make a **joke**<br>do you know a **joke**<br>give me a **joke**<br>I want a **joke**<br>say something **funny**<br>make me **laugh**<br>crack a **joke**<br>**amuse** me<br>give me some **humor** |
| **Pass the butter** | Explains, at length, why it cannot pass the butter. | pass the **butter**<br>give me the **butter**<br>hand me the **butter**<br>bring the **butter**<br>I need the **butter**<br>**butter** please<br>can you pass the **butter** |
| **"You pass butter"** | The existential crisis from the show. | you **pass** **butter** |
| **Insults** | Answers back. | you are **stupid**<br>you are **useless**<br>you are **dumb**<br>you are **broken**<br>you do not **work**<br>you are **worthless**<br>you are **bad** at this<br>you are **terrible**<br>what is **wrong** with you<br>you **idiot**<br>**moron**<br>you **failure**<br>**waste** of time<br>you are **disappointing**<br>**screw** you<br>you **suck**<br>you are a **piece** **of** **junk**<br>you are **garbage**<br>you are **trash**<br>absolute **failure**<br>complete **waste**<br>**worthless** machine<br>**stupid** robot<br>**stupid** machine |
| **Magic 8-ball** | Says "I'm listening", waits up to 4 seconds for your question (any words), then answers like a magic 8-ball. | I need **advice**<br>give me **advice**<br>any **advice**<br>help me **decide**<br>what do you **think**<br>I need **guidance**<br>can you **help** **me**<br>**help** **me** **out**<br>tell me **what** **to** **do**<br>show me **the** **way** |
| **Dice** | Rolls dice. "Roll a dice" asks two follow-up questions (see below); "roll a d twenty" and the like skip the dice type question. Then it says the roll (for example "2d6 roll") and the total. | roll a **dice**<br>roll a d **four**<br>roll a d **six**<br>roll a d **eight**<br>roll a d **ten**<br>roll a d **twelve**<br>roll a d **twenty**<br>roll a d **one** **hundred**<br>roll **dice**<br>roll the **dice**<br>I want to roll **dice**<br>**dice** roll |
| **Time** | Says the time from the robot clock, 24 h (set it in the controller Settings, DATE and TIME, or connect a phone). | what **time** is it<br>tell me the **time**<br>current **time**<br>give me the **time**<br>**time** now<br>do you know the **time**<br>what is the current **time** |
| **What is this? (camera)** | Takes one camera picture and names what it sees, see [VISION.md](VISION.md). | what is **this**<br>what's **this**<br>what do you **see**<br>what is in **front** of you<br>what's in **front** of you |
| **Faces: who is this** | Scans for a face and says if it is the owner or a stranger, see [VISION.md](VISION.md). | **who** is this<br>**who** is that<br>**who** do you **see**<br>**identify** this **person**<br>**identify** **face**<br>do you know this **person**<br>**recognize** this **person**<br>who is in **front** of you |
| **Faces: remember me** | Stores the face in front of the camera as the owner (one owner). | **remember** this **face**<br>**learn** this **face**<br>**save** this **face**<br>**set** as **owner**<br>**this** is my **face**<br>**recognize** me as **owner**<br>**register** this **face**<br>**make** me the **owner** |
| **Faces: forget the owner** | Deletes the stored owner face. | **forget** **owner**<br>**remove** **owner**<br>**delete** **owner**<br>**forget** **creator**<br>**remove** **creator**<br>**delete** **creator**<br>**clear** saved **face**<br>**remove** stored **face**<br>**forget** my **face**<br>**delete** saved **owner** |
| **Movement** | Drives a short distance or turns. Not while charging, lifted or blocked. | move **forward**<br>go **forward**<br>drive **forward**<br>move **backward**<br>go **backward**<br>drive **backward**<br>turn **left**<br>go **left**<br>rotate **left**<br>turn **right**<br>go **right**<br>rotate **right**<br>turn **around**<br>turn **one** **eighty**<br>rotate **one** **eighty**<br>spin **around** |
| **Dance** | Dances to music. Say "stop dancing" (or press the power button) to stop. | show me your **moves**<br>do a **dance**<br>**move** it, move it<br>I want to see you **dance**<br>get your **groove** on<br>**bust** a **move**<br>show me what you got |
| **Lights on / off** | Switches the LED module. *Needs the LED module.* | turn **on** the **lights**<br>switch **on** the **lights**<br>power **on** the **lights**<br>**enable** the **lights**<br>**lights** **on**<br>turn **off** the **lights**<br>switch **off** the **lights**<br>power **off** the **lights**<br>**disable** the **lights**<br>**lights** **off** |
| **Light effects** | Strobe, breathing (pulse), faster, slower. *Needs the LED module.* | light **strobe**<br>turn on **strobe**<br>enable **strobe**<br>start **strobe**<br>light **flash**<br>light **breathe**<br>turn on **breathing**<br>enable **breathing**<br>start **breathing**<br>lights **pulse**<br>lights **faster**<br>lights **slower** |
| **Temperature and humidity** | Reads the climate sensor. *Needs the temperature and humidity module.* | **temperature** report<br>**humidity** report<br>**climate** report<br>show **temperature** and **humidity**<br>check **temperature**<br>check **humidity**<br>what is the **temperature**<br>what is the **humidity**<br>**climate** status |
| **Temperature scale** | Switches between Celsius, Fahrenheit and Kelvin. *Needs the temperature and humidity module.* | switch to **celsius**<br>set **celsius**<br>use **celsius**<br>change to **celsius**<br>**celsius** mode<br>switch to **fahrenheit**<br>set **fahrenheit**<br>use **fahrenheit**<br>change to **fahrenheit**<br>**fahrenheit** mode<br>switch to **kelvin**<br>set **kelvin**<br>use **kelvin**<br>change to **kelvin**<br>**kelvin** mode |
| **Air quality** | Reads the gas sensor (needs about 10 minutes to calibrate). *Needs the gas module.* | get air **quality**<br>check air **quality**<br>air **quality** status<br>what is the air **quality** |
| **Intruder detection** | Turns the motion (PIR) alarm on or off. *Needs the PIR (motion) module.* | **enable** **intruder** detection<br>**motion** **sensor** **on**<br>turn **on** **intruder** detection<br>**disable** **intruder** detection<br>**motion** **sensor** **off**<br>turn **off** **intruder** detection |
| **Infrared remote: learn** | Learns a button from any IR remote: aim the remote at the robot and press the button, then say the phrase that should send it later (any short phrase, up to 10 commands). *Needs the IR module.* | **learn** new **infrared** command<br>**train** **infrared** command<br>**record** **infrared** signal<br>**add** new **infrared** action |
| **Infrared remote: use, list, forget** | After training, just say your own phrase in listening mode and the robot sends that IR signal. "Forget infrared" asks which phrase to forget. *Needs the IR module.* | **list** **infrared** commands<br>what **infrared** **commands** do you know<br>**forget** **infrared**<br>**remove** **infrared**<br>**delete** **infrared**<br>**forget** **all** **infrared**<br>**delete** **all** **infrared**<br>**clear** **all** **infrared** |
| **Phone: notifications** | Reads the phone notifications and asks after every 5 if it should continue: answer "yes" or "no". *Needs a phone connected over Bluetooth.* | list **notifications**<br>show **notifications**<br>what **notifications** do I have<br>read my **notifications**<br>check **notifications**<br>do I have **notifications**<br>tell me my **notifications** |
| **Phone: music** | Controls the music player on the phone. *Needs a phone connected over Bluetooth.* | what is **playing**<br>what song is **playing**<br>what is this **song**<br>current **song**<br>what **music** is **playing**<br>tell me what is **playing**<br>what is **playing** right now<br>**next** song<br>**skip** song<br>play **next**<br>**next** track<br>**previous** song<br>play **previous**<br>**previous** track<br>**play** **music**<br>**start** **music**<br>**start** **playback**<br>**resume** **music**<br>**stop** **music**<br>**pause** **music**<br>**stop** **playback**<br>**pause** **playback** |
| **Shut down** | Turns the robot off. | **shut** **down**<br>**deactivate**<br>**shut** yourself **down**<br>**power** **down**<br>turn **yourself** **off**<br>**battery** **off** |

## Follow-up answers

Some commands ask a question and listen again. Only these answers work there:

| After | Say |
|---|---|
| Dice: "how many dice?" | one, two, three, four, five, six, seven, eight, nine, ten, eleven, twelve, thirteen, one hundred |
| Dice: "which dice?" | d four, d six, d eight, d ten, d twelve, d twenty, d one hundred, four sided, six sided, eight sided, ten sided, twelve sided, twenty sided, one hundred sided, four, six, eight, ten, twelve, twenty, one hundred |
| Phone notifications: "should I continue?" | yes, no |
| Dance (while dancing) | stop dancing |
| Magic 8-ball | any question |
| Learning an IR button | any short phrase, that becomes the command |
| Any time while listening | cancel, nevermind |
