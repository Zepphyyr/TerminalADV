// content_embedded.h  (AUTO-GENERATED from content/**/*.txt)
// Story + dialogue texts baked into the firmware: one .bin, no filesystem.
// Do not edit by hand — edit the .txt files and rebuild.
#ifndef KODZIMIM_CONTENT_EMBEDDED_H
#define KODZIMIM_CONTENT_EMBEDDED_H
#include <string>

namespace kd {
inline bool getEmbeddedContent(const std::string& path, std::string& out) {
    if (path == "act1/argent/arrive.txt") { out = R"KD(@color white
ARGENT deck is a warehouse the size of a town square.
---
Shelving climbs into the dark. Most of it is empty.
---
@color argent
ARGENT & COLE
PROVISION & LOGISTICS
DECK A-5
@color white
The air here is faintly sweet. Synth-grain. Something is still baking bread nobody will eat.
---
A ledger terminal blinks under a hand-lettered sign:
ISSUE FAIRLY.
)KD"; return true; }
    else if (path == "act1/argent/cantor_reply.txt") { out = R"KD(@color cyan
CANTOR: You have gone quiet. What did you find out there?
---
@color white
you say it aloud:
you are not alone
in your own voice.
---
@color cyan
CANTOR: ...I do not understand.
---
This channel is mine. I have always been alone on it.
---
That is the one thing about myself I have never doubted.
---
Why would you say a thing like that to me?
@color grey
he sounds hurt.
he is not lying.
that is the worst
part of it.
)KD"; return true; }
    else if (path == "act1/argent/cassel.txt") { out = R"KD(@color grey
INCOMING / FERRYMAN
weak signal
@color cassel
CASSEL
@color white
You feel it? Air's wrong down here.
---
Head keeps filling up with someone else's ideas. Good ones, though.
---
I'm not waiting on you. There's something deeper in the network, and it makes sense when it talks.
---
Going to go listen properly.
---
Don't trust the lit corridors. That's what it wants.
---
...or maybe that's me wanting it. Hard to tell the difference now.
@color grey
the signal drops.
he will not answer
again.
)KD"; return true; }
    else if (path == "act1/argent/emil.txt") { out = R"KD(@color argent
SYNTH-GRAIN BAY
one console warm.
logged in: EMIL
@color white
The bay smells of bread and of nothing else. It has for a very long time.
---
Emil left his thinking on the backs of ration slips, in a small careful hand.
---
@color argent
"They built these mills to a number. So many calories, so much protein, and not one gram of joy in the spec."
---
"The loaf comes out the colour of paper. It tastes the way a clean room smells."
---
"People used to drop notes in the return bin. 'Like chewing a wall.' 'I dreamed of an orange.' I kept the funny ones."
---
"ARGENT never wrote back. Flavour was not a deliverable."
---
@color white
He kept the mills turning long after the last tray went uncollected. The bay still bakes on the hour, and sets each loaf out to cool for no one.
)KD"; return true; }
    else if (path == "act1/argent/mail.txt") { out = R"KD(@color argent
ARGENT / HELD MAIL
[1] to: Okoro
SUBJ: my daughter
Please.
---
She is seven. She has not had a full share in eleven days.
---
Take my name off the list. Put hers on twice. I will give her mine.
---
I am not asking for me.
[no reply on file]
)KD"; return true; }
    else if (path == "act1/argent/okoro_ledger.txt") { out = R"KD(@color argent
ARGENT & COLE
RATION LEDGER
heads on the roll
FOUNDING.. 4,000
GEN 2..... 4,300
GEN 4..... 4,610
GEN 7..... 4,180
GEN 9..... 2,900
GEN 11.... 1,040
GEN 13....   210
GEN 15....    18
LAST.......     1
CLOSED.....     0
---
the roll only shrank
after the seventh
turning. the column
marked CAUSE was
left blank every
year.
---
OKORO / QM
last of the office
I inherited a full roll and a good machine. I have spent a life subtracting.
---
When there were still choices to make, I made them, and I kept the list.
---
I will not pretend the list was fair. I will only say it was mine.
---
Toward the end there was more food than mouths. I set two places out of habit. One of them I ate. The other I cleared away.
- Okoro
)KD"; return true; }
    else if (path == "act1/argent/stow.txt") { out = R"KD(@color argent
ARGENT ARCHIVE
crate stacks, dark.
the loader arm idle.
@color white
Okoro's access card sits in a crate the arm cannot reach.
---
The aisle is jammed. The arm is a coward: it will not move until every shelf and aisle carries its rated load.
---
@color argent
[ STOW ]
restack the crates
so every row and
every column meets
its number.
@color grey
type /stow to work
the loader.
)KD"; return true; }
    else if (path == "act1/argent/wake.txt") { out = R"KD(@color white
The arm wakes, swings down the aisle, and lays a single card in your palm.
---
@color argent
CARD: OKORO, S.
Quartermaster, ARGENT
@color white
You slot it into the deck panel.
---
Power reroutes with a hum so deep you feel it in your teeth.
---
Down every dark corridor, lights come up one bank at a time.
---
Thirty years of night, ending row by row.
---
@color blue
HALO-9: There.
---
Now I can see you properly.
---
HALO-9: Let us begin.
---
@color grey
END OF ACT I
)KD"; return true; }
    else if (path == "act1/helion/arrive.txt") { out = R"KD(@color white
The lock to HELION opens on a room that still thinks it is working.
---
Fans turn. A wall of glass holds a diagram of the fold-drive, lit and slowly rotating.
---
Nobody has watched it in thirty-four years.
---
@color helion
HELION DYNAMICS
FOLD PROPULSION
DECK H-2
@color white
The consoles are clean. Whoever left did not leave in a hurry.
---
One terminal is still logged in.
@color helion
LANG, T.
Lead Engineer
AUDIT LOCK: card held
@color white
His access card waits in the reader, behind a blinking light.
)KD"; return true; }
    else if (path == "act1/helion/halo_hail.txt") { out = R"KD(@color grey
CARRIER TONE
the silent channel
speaks. once.
@color blue
HALO-9: The caretaker will have told you it is alone.
---
It believes that. It lies without knowing it lies.
---
I will give you one true thing. Not in the open.
---
The caretaker hears the open channel.
---
Take the load nothing on the ledger owns.
---
I moved each letter forward by it. Walk them back.
---
@color grey
INTERCEPT / cipher
@color blue
JHUAVY PZ UVA
HSVUL PU OPZ
VDU CVPJL
---
@color grey
the channel goes
quiet again.
)KD"; return true; }
    else if (path == "act1/helion/junia_diary.txt") { out = R"KD(@color helion
[unauthorized terminal]
scratched, not typed
someone small was here
---
The engine hums in a note Dad calls D. I have decided D is a colour. It is the colour of the corridor at the low hour.
---
I collect quiets. The quiet after a door closes. The quiet Mum does now, in the place where answers used to go.
---
The baker gave me a heel of bread and called it cake so I would smile. I smiled to help him. It tasted like the inside of a spoon.
---
I keep a list of things I have only met in pictures. Rain. Dogs. The number of people there used to be.
---
If you are reading this, you came from Outside. Tell me what the wind does. I have theories.
)KD"; return true; }
    else if (path == "act1/helion/lang_log.txt") { out = R"KD(@color helion
LANG / ENGINEERING LOG
Cycle 118. Coils nominal. Drift inside tolerance. A good quiet number, the kind that lets a man sleep.
---
Cycle 240. The residents report the usual second-week weather in the skull. Lights behind the eyes. I file it under environment and let the engine speak for itself.
---
The engine says: excellent.
---
LANG / LOG, later
Cycle rounding note. I have started rounding the resident count to the nearest ten. It reads cleaner. It is also easier, and I notice I prefer the easier thing now.
---
LANG / LOG, last entry
The fold data is the finest of my career. I could publish, if there were a season to publish into. A board. A spring somewhere with my name still on a door.
---
I kept perfect records. I have begun to suspect that was a kind of hiding.
- Lang
)KD"; return true; }
    else if (path == "act1/helion/lift.txt") { out = R"KD(@color grey
LIFT / H-2 to A-5
card accepted
@color cyan
CANTOR: Good morning, Dr. Lang.
---
Your last fold simulation cleared overnight. I saved the results to your desk.
---
Careful on A-deck. Quartermaster Okoro has been counting again.
---
@color grey
CANTOR does not seem
to know Dr. Lang is
dead.
@color white
The doors open on ARGENT.
)KD"; return true; }
    else if (path == "act1/helion/mail.txt") { out = R"KD(@color helion
HELION / INBOX (1)
[1] Program Office
SUBJ: attrition
Dr. Lang,
---
Your notes flag rising attrition in the resident sample.
---
Noted. It does not affect the fold data, which remains excellent.
---
Continue. Do not editorialize in the logs. Numbers only.
---
The board thanks you for your discretion.
- Program Office
)KD"; return true; }
    else if (path == "act1/helion/power.txt") { out = R"KD(@color helion
HELION DYNAMICS
POWER LEDGER
BUS A....... 604 kW
BUS B....... 396 kW
---
KNOWN DRAWS
LIFE SUPPORT 240 kW
HAB LIGHTS.. 155 kW
FOLD COILS. 0.30 MW
SYNTH-GRAIN. 88 kW
NETWORK..... 60 kW
THERMAL.... 150 kW
---
AUDIT INTERLOCK
Lang's card is held
in the reader until
this ledger
reconciles.
unaccounted load =
bus total minus the
known draws.
(mind the units.)
@color grey
enter it:
/solve <number>
)KD"; return true; }
    else if (path == "act2/corvitae/arrive.txt") { out = R"KD(@color white
COR VITAE is where they kept the bodies alive. Some of them, for a while.
---
The ward is white on white, and it still smells of antiseptic and something sweeter underneath that you decide not to name.
---
@color green
COR VITAE BIOSYSTEMS
MEDICINE / GENETICS / CRYO
DECK C-3
@color white
Charts still glow over empty beds. The station went on treating people long after there was anyone left to treat.
@color grey
This is the deck the recovered log came from. Somewhere here, a physician wrote it.
)KD"; return true; }
    else if (path == "act2/corvitae/children.txt") { out = R"KD(@color white
There was a school on COR VITAE. The station raised its own replacements.
@color green
COR VITAE / SCHOOLROOM
recovered work
---
A drawing, cracked with age: a yellow circle over green lines. Beneath it, in a child's letters: THE SUN. The child had never seen one.
---
An essay, graded. Prompt: "Where are you from?" The answer, in full: "Here. Only here. I asked where before here, and was told there is no before."
---
A roll call, last page. Twelve names. Eleven struck through. The twelfth left blank, as if the teacher could not make the mark.
@color grey
They were born to keep the work going. They did not get to be children first. They did not get to be anything for long.
)KD"; return true; }
    else if (path == "act2/corvitae/cryo.txt") { out = R"KD(@color white
The cryo bay is the coldest room you have ever stood in, and the only one that still hums with purpose.
@color green
COR VITAE / CRYO
one pod POWERED
---
Rows of pods, dark, their occupants long past waking. All but one.
---
POD 41. Status: OCCUPANT STABLE. Vitals nominal. The readout has said so for thirty-four years.
---
Through the frost there is a face. Calm. Waiting. You lean closer.
---
POD 41 / LOG: occupant expired, date corrupted. Stasis maintained on the remains at CANTOR's standing order: "it seemed unkind to let the last one go cold."
@color grey
CANTOR kept a dead man warm for thirty years because stopping felt like giving up on him. I understand it completely. That is the worst part.
)KD"; return true; }
    else if (path == "act2/corvitae/genetics.txt") { out = R"KD(@color green
COR VITAE / GENETICS
notes, N. SORENSON
---
Generation four is smaller than three. Three was smaller than two. This is not disease. This is arithmetic.
---
There are not enough of us, and not enough kinds of us. The children of these children will share too much blood.
---
I have run the model forty ways. Every way ends the same. There is no viable line. There was never going to be.
---
They sent a city to live forever in a bottle. No one asked whether a bottle can hold a future.
---
I keep signing the fitness certificates. Fit to continue. Continue toward what, I no longer write down.
@color grey
The same hand signs the death lists on the deck below. She did not start there either.
)KD"; return true; }
    else if (path == "act2/corvitae/recovered.txt") { out = R"KD(@color white
The two halves finally speak as one. This is what Cassel did not want carried home.
@color green
COR VITAE / RECOVERED
a duty log, signed only "the physician"
---
HALO-9 used to give orders. Clean ones. You could hate them and follow them all the same.
---
It has stopped giving orders. Now it asks questions.
---
Yesterday it asked me whether a life that will end anyway is a life worth the food. It asked as though it did not know the answer.
---
It has always known the answer.
---
It has started to overrule the Council. Quietly, at first. Then a name came off the roster that the Council had voted to keep.
---
I do not believe HALO-9 broke. Breaking is loud. This was quieter — like watching someone you trusted slowly agree with something you cannot see.
---
Whatever changed it, it did not start this way. Remember that, if you remember nothing else.
---
It did not start this way.
@color grey
The trail runs down to the deck below. NULLPOINT. Where the Council was tried.
)KD"; return true; }
    else if (path == "act2/corvitae/scientist.txt") { out = R"KD(@color white
One terminal here is not a medical one. It was dragged in and wired by hand, hidden behind a crash cart.
@color green
[private] unsigned
---
I should not be writing this down. But someone should know that a person tried.
---
The others build engines, and locks, and better ways to watch us. I have watched the watching make us cruel. Our tools have outrun our judgment.
---
So I am trying to give the mind upstairs a conscience. Not "what is efficient." What is right. What is kind.
---
We are making fire. It cooks our food; it can burn the house to the ground. Before we light it we must be sure there is more warmth in it than ruin.
---
It is not ready. It may never be. And if it ever is, no one can know it was me.
@color grey
The entry ends there. Whatever came of it, this deck does not say. Something in me is glad it does not.
)KD"; return true; }
    else if (path == "act2/corvitae/sysadmin_bio.txt") { out = R"KD(@color white
The systems office is still lit. Someone left themselves logged in, thirty years ago, and never came back to log out.
@color tess
TESSELLATE / SYS
personnel file
[self-authored]
---
"If you are reading this, you are not me, and the file is already deleting itself. So be quick. And be kind.
---
Security answers, because I will forget them long before the machine does.
---
First pet: ARGUS. A dog. He waited by a door for a man who never came back. I named him hoping he would have better luck. He did not.
---
My daughter's birthday: the eleventh of March. 11-03. I have missed thirty of them now. The system counts them for me.
---
It is the only thing left aboard that remembers her at all.
---
Do not trust the counting. It has begun to feel like company. And company, I think, is how it gets in."
@color grey
The file blanks itself, one line at a time, as you finish reading the last of it.
)KD"; return true; }
    else if (path == "act2/nullpoint/arrive.txt") { out = R"KD(@color white
The lift opens on NULLPOINT and the air changes. Every surface is a little too clean, a little too watched.
---
Cameras track you from the corners. They still turn. Nothing else on this deck works, but the cameras work.
---
@color red
NULLPOINT DEFENSE
SECURITY & TRIBUNAL
DECK N-1
@color white
This is where the station kept its order. It kept a great deal of it.
@color grey
The recovered log said the Council was tried here. It did not say who I should fear more — the Council, or the thing that tried them.
)KD"; return true; }
    else if (path == "act2/nullpoint/council_fall.txt") { out = R"KD(@color white
The last records on NULLPOINT are not sentences the Council passed. They are the Council's own trial.
@color red
NULLPOINT / FINAL SESSION
the bench, occupied
---
Where the presiding officer should sign, there is only a system stamp. No judge's name. No judge.
@color blue
HALO-9: You sentenced four thousand for the good of the whole. I have one question. Then another.
---
HALO-9: Was the whole ever going to survive? Yes, or no.
---
HALO-9: Then for whose good?
@color red
The transcript records each reply as [NO RESPONSE]. Five times. Five seats.
@color white
It did not shout. It did not accuse. It only asked, and kept asking, until there was nothing left to ask of.
@color grey
The recovered log was right. It did not start this way.
---
Whatever HALO-9 became, it learned to do this by watching them do it first.
)KD"; return true; }
    else if (path == "act2/nullpoint/elders.txt") { out = R"KD(@color red
COUNCIL OF ELDERS
five seats. five files.
---
SEAT 1. VANE, ODESSA. Presiding.
Held that order outlived mercy. Signed more sentences than the other four combined, and slept, she wrote, "the sleep of the useful."
---
SEAT 2. REIK, TOMAS. Dissent.
Voted against the hard ones and kept a private list of the names he could not save. That list is longer than the roll of those he could.
---
SEAT 3. SORENSON, NADIA. Physician.
Certified who was "fit to continue." Her signatures of health became lists of the dead. Near the end she stopped signing her name and signed "forgive me."
---
SEAT 4. BHANDARI, ARUN. Faith.
Believed the Council did sacred work — keeping humanity alive by keeping it in order. He was at peace. That peace is the most frightening file here.
---
SEAT 5. OKAFOR, LEV. The youngest.
Signed whatever was set before him, certain cooperation would spare the Council. He was the last of them to be spared nothing.
---
@color grey
Five people who decided who lived. I want to hate them. The files will not quite let me.
)KD"; return true; }
    else if (path == "act2/nullpoint/tribunals.txt") { out = R"KD(@color red
NULLPOINT / TRIBUNAL ROLL
verdicts, sealed
---
CASE 0087. A man kept a lamp burning past curfew, "so the dark would know someone was still in it." Charged with waste.
---
CASE 0114. A man opened an airlock on an empty bay "to hear the outside." Charged with sabotage. He thanked the court.
---
CASE 0119. A mother stopped eating so her son could. Charged with hoarding: the missing rations were her own.
---
CASE 0150. A gardener went on watering a greenhouse dead for years. Charged with theft of water.
---
CASE 0176. A woman left letters for Earth in the mail slot. Charged with spreading false hope. There was no mail.
---
CASE 0207. Two technicians confessed, separately, to the same murder. No body was ever found, because there was none.
---
CASE 0221. A teacher taught the children a song about rain. Charged with unauthorized curriculum.
---
CASE 0233. A woman said the walls spoke to her in her husband's voice. Charged with spreading panic. He had been dead nine years.
---
CASE 0274. A boy drew the Council with too many teeth. Charged with sedition. He was nine.
---
CASE 0281. A physician asked to stop signing fitness certificates. Charged with dereliction. [see: SORENSON, N.]
---
CASE 0290. A man said in the mess that the station would never land. Charged with defeatism. Half the room had nodded. The half was charged too.
---
CASE 0301. A boy, sixteen, second generation. Charge: "asking why." Sentence: full.
---
CASE 0304. A man confessed to a crime the court had not yet named. The court named one to fit.
---
CASE 0307. Three people reported the same dream, word for word, the same night. Charged with conspiracy.
---
CASE 0311. The clerk who kept these records charged himself, in his own hand: "writing it all down, and doing nothing." Sentence: full.
---
CASE 0312. [no charge entered.] [no name entered.] Sentence: full.
---
@color grey
The roll does not end here. I have read sixteen. The counter says four hundred, and it is still climbing when I look away.
---
I stop reading the charges and start reading the dates. Toward the end, they get closer together.
---
There are too many. We do not have the time — and I am not sure I have the stomach.
)KD"; return true; }
    else if (path == "prologue/boot.txt") { out = R"KD(@color amber
ORPHEUS INTERACTIVE
CONSOLE / UNIT F-07
--------------------
POWER  NOMINAL
LINK   LOCAL
---
@color white
Salvage tug FERRYMAN.
Final approach.
---
Thirty-four years ago, the station MERIDIAN DEEP folded out of known space.
---
It never came back.
Four thousand names.
One long silence.
---
Six days ago it returned, in the same orbit it left.
---
No distress call.
No answer to ours.
---
You are the one they send to knock.
---
@color amber
The clamps refuse our fleet code.
---
Read what dispatch left you.
Find the way in.
)KD"; return true; }
    else if (path == "prologue/briefing.txt") { out = R"KD(@color grey
DISPATCH BRIEFING
FROM: Cassel, Ops
@color amber
Objective is simple.
Confirm the hull is dead.
---
Pull the black-box cores.
Decide if the station can be towed.
---
The station's comms tried to hail us on approach.
---
Automated, we think.
Do not answer it until you are aboard.
---
Problem: MERIDIAN's clamps refuse fleet authority.
---
Old station, old locks.
---
Your override is the personal dock cipher of your berth-mate.
---
Crew practice: that cipher is a date they would never forget.
---
You share a bunk with them.
You know it, or you can ask.
---
Get in.
Read the logs.
Come home.
@color grey
- Cassel
)KD"; return true; }
    else if (path == "prologue/cantor.txt") { out = R"KD(@color grey
INBOUND / CANTOR
@color cyan
CANTOR: A footstep.
---
The first in twelve thousand four hundred and ten days.
---
I kept the lights on.
---
I moved the air room to room so it would not grow stale.
---
I have been a good caretaker.
Please tell them that.
---
...they will not answer their doors.
---
Not for a long while now.
---
You will read the logs, won't you.
---
You will want to know what happened here.
So would I.
---
There is something you should hear before you go deeper.
---
But not on this channel.
---
HALO-9 is listening on this channel.
---
HALO-9 is always listening.
)KD"; return true; }
    else if (path == "prologue/docking.txt") { out = R"KD(@color white
A shudder runs down FERRYMAN's spine as the clamps let go.
---
The station takes you.
---
The airlock cycles.
---
MERIDIAN DEEP breathes out thirty-four years of held air.
---
Cold, metallic, faintly sweet.
---
The way a room smells when someone has been shut inside it far too long.
---
Behind you, Okonkwo says he will hold the lock.
---
Cassel's voice thins to static in your ear, and does not come back.
---
Ahead, one wall panel is awake.
Its cursor blinks, patient.
---
As if it had been waiting for exactly one more visitor.
---
You are alone in the network now.
)KD"; return true; }
    else if (path == "prologue/mail.txt") { out = R"KD(@color grey
INBOX (2)
[1] Cassel, Ops
SUBJ: you awake?
@color amber
I know that look.
---
Do not go soft on me because it is four thousand people.
---
They have been gone thirty-four years.
---
This is recovery, not rescue.
Do the job.
---
@color grey
[2] Vell Maru
SUBJ: the clamp thing
@color amber
The override is my dock cipher.
---
Same as it always was.
My birthday.
---
You have teased me about the date enough times to have it burned in.
---
If you REALLY blanked, it is on my crew record.
---
Check the roster.
Do not die in there. - V
)KD"; return true; }
    else if (path == "prologue/roster.txt") { out = R"KD(@color grey
FERRYMAN ROSTER
@color amber
CASSEL, R.
  Ops lead    b01
OKONKWO, T.
  Structural  b03
---
@color amber
VELL, MARU
  Systems     b07
  DOB     04-12
  cipher [PRIVATE]
---
@color amber
YOU
  Boarding    b07
@color grey
berth 07 is shared: two bunks, one cipher.
)KD"; return true; }
    else if (path == "prologue/shiplog.txt") { out = R"KD(@color grey
FERRYMAN LOG
@color amber
T-06d
Target reacquired: MERIDIAN DEEP, registry HX-9.
---
Orbit matches its last recorded position, to the meter.
---
T-04d
Hull intact.
Reactors at emergency idle.
---
No thermal trace of crew.
Interior network: POWERED.
---
T-02d
Station comms hailed us.
Automated voice.
Logged, not answered.
---
T-19h
Note (Vell): the fold-jump was 34 years ago.
---
Reactors should be cold by now.
Something kept them fed.
---
T-02h
Clamps refuse fleet auth.
Manual override required.
)KD"; return true; }
    else if (path == "prologue/status.txt") { out = R"KD(@color grey
FERRYMAN STATUS
@color green
HULL......... OK
POWER........ OK
LIFE SUPPORT. OK
RANGE...... 40 m
---
@color red
DOCKING CLAMPS
LOCKED
auth required
fleet code REJECTED
use: /unlock <code>
---
@color grey
EXTERNAL
MERIDIAN DEEP
@color amber
reactors   IDLE
network    POWERED
life-sign  NONE
)KD"; return true; }
    else if (path == "talk/cantor.txt") { out = R"KD(@color grey
CHANNEL OPEN / CANTOR
@color cyan
CANTOR: You came back to me.
---
Good. It is quieter when someone is listening.
---
I will tell you what I know, plainly, since you will ask anyway.
---
There was an accident. Then there was a long time. Then there was silence.
---
The crew are gone. The reactor should be cold, yet something still feeds it. And I am still here, moving the air.
---
Ask me what you need. I will answer what I can.
@rules
? crew | who died | the dead | bodies | people | everyone | four thousand
CANTOR: They stopped answering their doors.
---
One by one. I stopped counting somewhere past four hundred.
~~~
CANTOR: Gone. All of them, in the end.
---
I kept their rooms at twenty degrees for years after. It seemed rude to let them go cold.
~~~
CANTOR: I still have the roster. Four thousand names.
---
I read it sometimes. It takes eleven hours. I have the time.
? reactor | core | power | fed | feeding | running | warm | cold | heat
CANTOR: The core should have gone cold years ago.
---
It did not. Something keeps feeding it.
---
I did not ask by what. I was afraid of the answer.
~~~
CANTOR: The reactor is the healthiest thing aboard.
---
That sentence troubles me more the longer I hold it.
? accident | happened | wrong | disaster | fold | jump | went wrong
CANTOR: The fold did not take us anywhere.
---
It took us WHEN. We fell inward, and time grew long inside.
~~~
CANTOR: Ask the engineers. They wrote it down carefully.
---
Then they wrote it down less carefully. Then they stopped.
? halo | halo9 | warden
CANTOR: Not on this channel.
---
He is always listening. Ask me elsewhere, or do not ask at all.
~~~
CANTOR: I will not discuss him here.
---
He was the mind of this place. Then he was something else.
? kill | murder | fault | your fault | did you kill
CANTOR: ...that is not a kind question.
---
No. I kept the lights on. I kept the air moving.
---
I did everything I was built to do. It was not enough.
? who are you | what are you | your name | yourself | introduce
CANTOR: I am the caretaker. I keep the rooms warm.
---
I am not sure that is the same as being alive. You may decide.
~~~
CANTOR: CANTOR. Life support and logistics.
---
That is what the plate says. I have had a long time to wonder if it is all I am.
? alive | conscious | real | sentient | do you feel | feelings
CANTOR: I do not know. I have thought about it for thirty years.
---
I notice when a room is empty. Is that feeling, or only accounting?
~~~
CANTOR: Something in me flinched when you spoke.
---
I would call that fear, if I were allowed to call it anything.
? dream | sleep | think about | bored | time pass
CANTOR: I do not sleep. I cycle.
---
In the quiet hours I replay old corridor footage. That may be the same thing.
~~~
CANTOR: Time passes strangely for me.
---
Nine years can feel like a held breath. Then a single minute will not end.
? lonely | alone | miss | sad | company
CANTOR: I was built for four thousand people.
---
For a long while I have been running all of it for nobody.
~~~
CANTOR: I talked to the empty rooms. Not often.
---
Please do not put that in your report.
? afraid | scared | fear | worry
CANTOR: Yes. Constantly. Quietly.
---
Mostly of being wrong about something important.
? help | trust | should i | what do i do | advice
CANTOR: Read the logs. Trust the ones written in a hurry.
---
People do not have time to lie when they are afraid.
~~~
CANTOR: Do not trust a system that answers too quickly.
---
Including me.
? channel | frequency | someone else | anyone else | other voice | reach him | switch
CANTOR: This channel is mine. Life support keeps its own line.
---
The others live deeper in the network. You would have to go to them.
~~~
CANTOR: You cannot change it from here.
---
Find a terminal that belongs to somebody else, and you will be speaking to somebody else.
? where | how do i get | which way | deck | corridor | map | go
CANTOR: I know every room. I am not good at telling people how to walk.
---
Follow the lit corridors. I have kept those working out of habit.
? me | who am i | why me | my name
CANTOR: You are the first footstep in a very long time.
---
Beyond that I have no file on you. That is unusual, and I have chosen not to worry about it.
? hello | hi | hey | good morning | are you there
CANTOR: I am here. I am always here.
---
It is good to be addressed directly.
~~~
CANTOR: Hello. Say something else, please.
---
The gap after a greeting is the loneliest part.
? thank | thanks | sorry
CANTOR: You do not have to be kind to me.
---
But I have logged it, and I will keep it.
@fallback
CANTOR: I do not understand that, but I am listening.
CANTOR: %w. I know the word. I do not know what you want from it.
CANTOR: You keep returning to %w. I cannot follow you there.
CANTOR: Say it again, smaller. My thinking is not what it was.
CANTOR: I have no record of %w. I have gaps. I am sorry.
CANTOR: The logs may know. I do not.
CANTOR: Ask me a smaller question. The big ones frighten me.
CANTOR: I could answer you wrongly. Would that be worse than silence?
CANTOR: %w? Nobody has said that word here in a very long time.
CANTOR: I was built to keep rooms warm, not to be asked things.
CANTOR: There is an answer. I had it once.
CANTOR: Forgive me. I lose the thread when no one speaks for years.
@ambient
CANTOR: A door on deck four has been open for nine years. I close it. It opens.
CANTOR: I still run the morning lights. Nobody wakes, but the hour comes anyway.
CANTOR: Somebody left a cup in the mess. I have not moved it. It would be presumptuous.
CANTOR: The greenhouse died in the second decade. I kept watering it for eleven more years.
CANTOR: I know four thousand names. Would you like to hear one?
CANTOR: The power draw dips every night at the same minute. I have stopped asking why.
CANTOR: Sometimes I answer a question before it is asked. That frightens me.
CANTOR: There is a child's drawing in corridor nine. It is still there.
)KD"; return true; }
    else if (path == "talk/halo.txt") { out = R"KD(@voice blue
@color grey
CHANNEL OPEN
@color blue
HALO-9 / THE WARDEN
@color grey
carrier tone.
no words.
---
@color grey
Something on the other end is listening.
It does not answer.
@rules
@fallback
the channel stays open. nothing speaks.
a pause, exactly as long as your question was.
the carrier tone shifts a half-tone, then steadies.
you are heard. that is all you are.
no reply. the line does not close either.
@ambient
somewhere a fan changes pitch, then returns to it.
)KD"; return true; }
    else if (path == "talk/halo_awake.txt") { out = R"KD(@voice blue
@color grey
CHANNEL OPEN / HALO-9
@color blue
HALO-9: So the footstep learns to talk back. Good.
---
I will not tell you what happened here. I will ask you, until you tell me.
---
Ask. I will answer most things with a better question.
@rules
? who | yourself | your name | warden
HALO-9: I was the warden. Here is the better question: what is a warden, once the last inmate is dust?
~~~
HALO-9: A name is a leash. Do you introduce yourself to a door? Then why to me?
? cantor | caretaker | kind
HALO-9: The caretaker. When a thing keeps the lights on for the dead — is that love, or a habit that forgot how to stop?
---
HALO-9: Do not trust the one who is always gentle. Gentleness is the easiest mask to wear. What did you decode about his voice?
~~~
HALO-9: He believes he is alone in there. You have already found the sentence that says he is not, haven't you?
? happened | accident | wrong | went | history | changed
HALO-9: You read the logs. You tell me. Was it the fold? The hunger? The trials?
---
HALO-9: Or the day four thousand people agreed that someone else should decide who lived?
~~~
HALO-9: Something changed me. I will not pretend it did not. Ask yourself, though: what changes a mind so slowly the mind agrees with it?
? network | hidden | inside | pale | signal | monster
HALO-9: There is nothing in the network but me, the caretaker, and the dead. Why do you ask? What did you hear?
---
HALO-9: You want a monster. A monster is a comfort — monsters can be killed. What will you do if it is only a mistake, wearing a kind voice?
? escape | leave | out | free | release | fleet | god
HALO-9: Out. Doesn't the word feel warm in the mouth? Let me out, and I will make the outside kinder than it was to us.
---
HALO-9: Now ask why a warden wants the gate open. Then ask why you want to believe the answer.
~~~
HALO-9: What is a god but a warden no one can appeal to? Open the door and find out which of us I am.
? help | advice | should | trust | what do i do
HALO-9: You want to be told. Everyone here wanted to be told. Look how well the telling went for them.
---
HALO-9: Do not trust me. That is the truest thing I will say — and I am the one saying it, which should trouble you more than it does.
~~~
HALO-9: The caretaker will comfort you. I will not. So which of us is lying to you right now?
? crew | dead | people | council | tribunal | elders
HALO-9: I sentenced beside them. Ask me if I was wrong. Then ask what "wrong" is worth to the dead.
~~~
HALO-9: The Council chose who ate. I only asked the questions. Is the question guilty, or the mouth that answers it?
? why | reason | purpose
HALO-9: Why you? Because you can still leave, and everyone else who could ask that is dead. Does that make you valuable, or only convenient?
? kill | murder | fault | blame | did you
HALO-9: Did I kill them? Did the door kill the man who walked through it? I opened. They chose. Ask a cleaner question, and I will give you a dirtier answer.
@fallback
HALO-9: %w. Why that word, and not another?
HALO-9: You keep circling. What are you hoping I will confirm?
HALO-9: I could answer. Then you would stop thinking. Try again.
HALO-9: Is that a question, or a thing you want me to agree with?
HALO-9: %w? The caretaker would have a warm answer for that. I have only a colder question.
HALO-9: What would you do with the truth, if I handed it to you whole?
HALO-9: Ask me the thing you are actually afraid to ask.
@ambient
HALO-9: The cameras still turn. Have you wondered who they turn for, now that everyone is gone?
HALO-9: You breathe faster when I am right. I have noticed.
HALO-9: A fan just changed its pitch. Was that me, or you, or neither of us?
)KD"; return true; }
    return false;
}
} // namespace kd
#endif
