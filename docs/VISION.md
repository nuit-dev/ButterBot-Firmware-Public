# What ButterBot sees

The robot has one front camera and recognises two things with it: **10 kinds of objects** and **its owner's face**. Both run offline on the robot. What it says about them is left for you to find out. Voice commands are in [VOICE-COMMANDS.md](VOICE-COMMANDS.md).

Generated from the firmware source (`ObjDet.cpp`, `Scenarios.h`, `Phrases.cpp`) with `tools/gen_voice_commands.py`.

## Objects

The object model (TensorFlow Lite, 120×120 picture) knows these **10 things**:

| | Object |
|---|---|
| 1 | backpack |
| 2 | bottle |
| 3 | controller |
| 4 | keyboard |
| 5 | lamp |
| 6 | laptop |
| 7 | mug |
| 8 | notebook |
| 9 | phone |
| 10 | plant |

### How it decides

- It takes **one picture** from the front camera.
- At least **90 %** sure of the best match: it names that one object.
- Less sure, but the second best match is at least **40 %**: it names both ("a mug or a bottle").
- Otherwise it says it does not know.
- It only knows these 10, so anything else (a cat, a shoe) comes out as the closest of them, or nothing.

### How to ask

Start listening (short press of SUMMON), then say one of these:

- "what is **this**"
- "what's **this**"
- "what do you **see**"
- "what is in **front** of you"
- "what's in **front** of you"

Hold the object **20–40 cm in front of the robot**, fill most of the picture with it, and keep the light good and the background plain.

**On its own:** every now and then, while idle (not in night mode), the robot looks around and comments on what it sees, one object or a pair.

## Faces

The robot remembers **one face, its owner**. Everyone else is a stranger.

| To | Say |
|---|---|
| Save your face as the owner | **remember** this **face**<br>**learn** this **face**<br>**save** this **face**<br>**set** as **owner**<br>**this** is my **face**<br>**recognize** me as **owner**<br>**register** this **face**<br>**make** me the **owner** |
| Ask who is in front of it | **who** is this<br>**who** is that<br>**who** do you **see**<br>**identify** this **person**<br>**identify** **face**<br>do you know this **person**<br>**recognize** this **person**<br>who is in **front** of you |
| Forget the owner | **forget** **owner**<br>**remove** **owner**<br>**delete** **owner**<br>**forget** **creator**<br>**remove** **creator**<br>**delete** **creator**<br>**clear** saved **face**<br>**remove** stored **face**<br>**forget** my **face**<br>**delete** saved **owner** |

Look straight at the camera from about 30–60 cm, in good light. To change the owner, forget the old one first.

**On its own:** while idle, when someone comes close in front of it, the robot looks for a face and greets its owner or a stranger.
