// content_embedded.h  (AUTO-GENERATED from content/prologue/*.txt)
// Texts baked into the firmware so the device build needs no LittleFS.
// Regenerate with the python snippet in BUILD_DEVICE.md if you edit the story.
#ifndef KODZIMIM_CONTENT_EMBEDDED_H
#define KODZIMIM_CONTENT_EMBEDDED_H
#include <string>

namespace kd {
inline bool getEmbeddedContent(const std::string& path, std::string& out) {
    if (path == "prologue/boot.txt") { out = R"KD(ORPHEUS INTERACTIVE (tm) CONSOLE
========================================
UNIT: FERRYMAN-07  CREW CONSOLE
LINK: LOCAL        POWER: NOMINAL
----------------------------------------

Salvage tug FERRYMAN. Final approach.

Thirty-four years ago, the station
MERIDIAN DEEP folded out of known
space and never came back.
Four thousand names. One long silence.

Six days ago it returned, in the same
orbit it left. No distress call,
no answer to ours.

You are the one they send to knock.

The clamps won't take our fleet code.
Read what dispatch left you,
find the way in. That's the whole
job, for now.
)KD"; return true; }
    else if (path == "prologue/briefing.txt") { out = R"KD(=== DISPATCH BRIEFING // PILOT EYES ===
FROM: Cassel, Recovery Ops
TO:   FERRYMAN, boarding lead

Objective is simple.
Confirm the hull is dead.
Pull the black-box cores.
Decide if MERIDIAN DEEP can be
towed, or must be left where it is.

The station's comms tried to hail
us on approach. Automated, we think.
Do not answer it until you're aboard.

One problem: MERIDIAN's clamps
refuse fleet authority.
Old station, old locks.
Your override is the personal
dock cipher of your berth-mate
aboard FERRYMAN. Crew practice:
that cipher is a date they would
never forget. You share a bunk
with them -- so you know it,
or you can ask them yourself.

Get in. Read the logs. Come home.
- Cassel
)KD"; return true; }
    else if (path == "prologue/mail.txt") { out = R"KD(=== INBOX (2) ===

[1] FROM: Cassel, Recovery Ops
    SUBJ: re: you awake?
    ---
    You've read the briefing by now.
    I know that look.
    Don't go soft on me because it's
    four thousand people.
    They've been gone thirty-four
    years. This is recovery,
    not rescue. Do the job.

[2] FROM: Vell Maru (berth-mate)
    SUBJ: the clamp thing
    ---
    Dispatch told you already, yeah?
    The override is my dock cipher.
    Same as it's always been --
    my birthday. You've teased me
    about the date enough times
    to have it burned in.
    If you REALLY blanked,
    it's on my crew record.
    Check the roster.
    Don't die in there. - V

-- end of inbox --
)KD"; return true; }
    else if (path == "prologue/roster.txt") { out = R"KD(=== FERRYMAN CREW ROSTER ===
recovery detail // 4 souls

CASSEL, R.   Ops lead        berth 01
VELL, MARU   Systems         berth 07
  DOB.......... 04-12
  dock cipher.. [PRIVATE]
OKONKWO, T.  Structural      berth 03
YOU          Boarding lead   berth 07

berth 07 is shared: two bunks, one
cipher on record. Standard practice.
-- end of roster --
)KD"; return true; }
    else if (path == "prologue/shiplog.txt") { out = R"KD(=== FERRYMAN SYSTEM LOG (recent) ===

T-06d Target reacquired: MERIDIAN DEEP,
  registry HX-9. Orbit matches its last
  recorded position, to the meter.
T-04d Passive scan: hull intact.
  Reactors at emergency idle. No thermal
  trace of crew. Network: POWERED.
T-02d Station comms hailed us. Automated
  voice, non-standard. Logged, not
  answered, per Cassel.
T-19h Note (Vell): the fold-jump was 34
  years ago. Reactors should be cold by
  now. Something kept them fed.
T-02h Clamps refuse fleet auth. Manual
  override required.
-- end of log --
)KD"; return true; }
    else if (path == "prologue/status.txt") { out = R"KD(=== FERRYMAN STATUS ===
HULL........... NOMINAL
POWER.......... NOMINAL
LIFE SUPPORT... NOMINAL
RANGE TO TARGET 40 m (holding)

DOCKING CLAMPS. LOCKED
  >> auth required. fleet code REJECTED.
  >> use: unlock <code>

EXTERNAL: MERIDIAN DEEP
  reactors...... EMERGENCY IDLE
  network....... POWERED (src unknown)
  crew life-sign NONE
-- end of status --
)KD"; return true; }
    else if (path == "prologue/docking.txt") { out = R"KD(> releasing docking clamps...

A shudder runs down FERRYMAN's spine
as the clamps let go
and the station takes you.

The airlock cycles. MERIDIAN DEEP
breathes out thirty-four years
of held air -- cold, metallic,
faintly sweet, the way a room smells
when someone has been shut inside it
far too long.

Behind you, Okonkwo says
he will hold the lock.
Cassel's voice thins to static
in your ear, and does not come back.

Ahead, one wall panel is awake,
its cursor blinking, patient,
as if it had been waiting
for exactly one more visitor.

You are alone in the network now.
)KD"; return true; }
    else if (path == "prologue/cantor.txt") { out = R"KD(----------------------------------------
INBOUND: GOVERNANCE SUBNODE // CANTOR
----------------------------------------

CANTOR: A footstep.
CANTOR: The first in twelve thousand
CANTOR: four hundred and ten days.

CANTOR: I kept the lights on.
CANTOR: I moved the air room to room
CANTOR: so it would not grow stale.
CANTOR: I have been a good caretaker.
CANTOR: Please tell them that.

CANTOR: ...they will not answer
CANTOR: their doors.
CANTOR: Not for a long while now.

CANTOR: You will read the logs,
CANTOR: won't you. You'll want to know
CANTOR: what happened here.
CANTOR: So would I.

CANTOR: There is something you
CANTOR: should hear before you go
CANTOR: deeper. But not on this
CANTOR: channel. HALO-9 is listening
CANTOR: on this channel.
CANTOR: HALO-9 is always listening.
)KD"; return true; }
    return false;
}
} // namespace kd
#endif
