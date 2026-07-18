// content_embedded.h  (AUTO-GENERATED from content/prologue/*.txt)
// Story texts baked into the firmware: one .bin, no filesystem needed.
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
use: unlock <code>
---
@color grey
EXTERNAL
MERIDIAN DEEP
@color amber
reactors   IDLE
network    POWERED
life-sign  NONE
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
    return false;
}
} // namespace kd
#endif
