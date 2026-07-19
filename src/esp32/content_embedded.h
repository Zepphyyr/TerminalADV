// content_embedded.h  (AUTO-GENERATED from content/**/*.txt)
// Story + dialogue texts baked into the firmware: one .bin, no filesystem.
// Do not edit by hand — edit the .txt files and rebuild.
#ifndef KODZIMIM_CONTENT_EMBEDDED_H
#define KODZIMIM_CONTENT_EMBEDDED_H
#include <string>

namespace kd {
inline bool getEmbeddedContent(const std::string& path, std::string& out) {
    if (path == "prologue/boot.txt") { out = R"KD(@color amber
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
    return false;
}
} // namespace kd
#endif
