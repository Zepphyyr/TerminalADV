# Act I — "The Quiet Decks" — design & puzzle specs

Companion to STORY_BIBLE.md §11. The bible owns the *story*; this file owns the
*mechanics*: exact puzzle data, solutions, and the engine work still to do.

Content already written: `content/act1/helion/*.txt`.

---

## Deck interaction model (same as the prologue)

Terminals share one command vocabulary, so the player never re-learns controls:

- `/dir` lists the deck's **documents**; `/open <name>` (alias `/o <name>`)
  reads one. HELION: `log`, `diary`. ARGENT: `ledger`, `notes`.
- `/mail` holds **letters** (distinct from documents). One per deck.
- `/look` re-describes the room; `/status` the deck; `/help` the command list.
- System consoles are direct verbs: HELION `/power` (+`/solve`), ARGENT
  `/archive` + `/stow`.
- **Cassel is not a document.** His transmission fires as an unbidden event on
  arriving at ARGENT; `/comms` afterward only raises static (he won't answer).
- Plain words are speech to whoever is on the channel (CANTOR by default). So
  bare `o log` talks to CANTOR — the open command needs the slash.
- Corp colour identities: HELION reactor-orange, ARGENT silver. CANTOR is always
  cyan (his unprompted asides are cyan too, not grey).

## Player spine (HELION -> ARGENT)

1. **Arrive HELION** (`arrive.txt`). Read Lang's logs (`lang_log.txt`) and
   Junia's diary (`junia_diary.txt`). Cold report voice vs. living child voice.
2. **Energy-balance puzzle** (`power.txt`). Player derives the unmetered load by
   hand. Result = **19**. That number is:
   - the code for **Lang's access card** (opens the lift), and
   - the **Caesar shift** for HALO-9's cipher.
3. **HALO-9 breaks silence** (`halo_hail.txt`): one clean line + ciphertext.
4. **Lift to ARGENT** (`lift.txt`). CANTOR greets "Dr. Lang" warmly — it does
   not know he is dead. First crack in CANTOR's reliability.
5. **ARGENT**: Okoro's ration ledger = a census-necrology across generations
   (centuries, not a 35-year starvation); the synth-grain / "bread like paper"
   theme via Emil; Emil in
   the background. Player **decrypts** HALO-9 with shift 7 -> a true line that
   **contradicts CANTOR**. First contradiction caught. Cassel appears once.
   First PALE glitch fires here. **STOW** puzzle yields Okoro's card -> restores
   power to the next decks -> HALO-9 wakes fully. Act ends.

---

## Puzzle 1 — Energy balance (arithmetic, pen & paper)

**Data (from `power.txt`):**

```
BUS A = 604 kW
BUS B = 396 kW              bus total = 1000 kW

LIFE SUPPORT = 240 kW
HAB LIGHTS   = 155 kW
FOLD COILS   = 0.30 MW      <- trap: convert to 300 kW
SYNTH-GRAIN  =  88 kW
NETWORK      =  60 kW
THERMAL      = 150 kW       known total = 993 kW
```

**Solution:** 1000 - 993 = **7 kW unaccounted.**

Deliberate friction: total is split across two buses (must be summed); one draw
is stated in MW (must be converted). Everything else is plain subtraction.

Thematic point: 7 kW is about one habitation unit — something the size of a
single occupant is still drawing power in a city with no occupants.

**Why entering 7 releases the card (narrative):** HELION consoles hold their
access cards under a *power-audit interlock* — while the ledger does not
reconcile, the system treats the discrepancy as possible tampering and will not
release credentials. Entering the unaccounted figure reconciles the audit, so
the console frees Lang's card from the reader. (The irony: the "tampering" is
PALE's 7 kW draw, unexplained for thirty years.)

**Answer 7 is reused as the cipher shift** — the number you dug out of the
ledger is the key to HALO-9's message too.

## Puzzle 2 — HALO-9's Caesar cipher

**Ciphertext (in `halo_hail.txt`):**

```
JHUAVY PZ UVA HSVUL PU OPZ VDU CVPJL
```

**Key:** shift 7 (from puzzle 1). HALO says "I moved each letter forward by it,
walk them back" — encryption is +7 (A->H), so decrypt = shift each letter
*back* 7.

**Plaintext:** `CANTOR IS NOT ALONE IN HIS OWN VOICE`

This is HALO-9's one true statement. It contradicts CANTOR's prologue claim
("this channel is mine... I am always here... alone") and points, without naming
it, at PALE riding inside CANTOR. Player catches the contradiction on ARGENT.

Hint chain so a stuck player is never dead: HALO says "the number the coils
forgot" (-> unmetered load, listed right under FOLD COILS) and "read the rest
backward by it" (-> Caesar, shift = that number).

## Puzzle 3 — STOW (graphical door, ARGENT)

Endfield-style fill grid, dressed as an ARGENT archive. The loader arm can only
reach the crate holding Okoro's card once every aisle and shelf carries its
rated count of crates.

- Grid **5x5**. Cursor moves with keys; a key toggles a cell filled/empty.
- Each **row** and each **column** has a target count printed at its edge.
- 2-3 cells are **pillars** (pre-blocked, cannot fill) to force real reasoning.
- Win: every row and column count satisfied simultaneously.
- Solvable on paper (it is a small "Aquarium"/binary-count logic puzzle).

Renders in the full 20x8 during the puzzle (drop the one-thought layout while a
puzzle is active). Draft render, target grid, and a worked solution live in a
follow-up once we wire it.

**Not used:** sine-wave matching — poor fit for 20 wide. Parked for Act II.

---

## Engine work — status

Foundation is built and tested end-to-end on the desktop build (fresh save ->
prologue -> `/proceed` -> HELION -> `/power` (HALO hail) -> `/go`/`yes` -> lift ->
ARGENT -> `/archive` -> `/go`/`yes` -> wake -> act1_done; plus reboot-resume).

- [x] **Deck navigation**: `deck_` state ("", helion, argent, act1_done), entered
  from the station with `/proceed`, per-deck content commands, `/help` `/status`.
- [x] **"Go on?" beat**: Maru asks at each sector edge; `yes` / `not yet`.
- [x] **Progress persistence**: current deck saved under NVS key `deck`; reboot
  resumes onto it; `/reset` clears it.
- [x] **HALO breaks silence once** on HELION (after `/power`).

- [x] **Access-card state** — `cardLang_` / `cardOkoro_` gate the two sector
  transitions; resume restores them by deck. Lift greeting plays on advance.
- [x] **Energy-balance answer check** — `/solve <n>` on HELION; `19` grants
  Lang's card and confirms the cipher shift.
- [x] **Cipher payoff** — speaking the decoded line to CANTOR (once, after the
  shift is known) fires `cantor_reply.txt`; matched loosely on its key words.
- [x] **STOW** — interactive 5x5 fill grid (`runStow`), line-based input
  (`c4` / `ok` / `?` / `q`); solving grants Okoro's card and opens the gate.
- [x] **PALE glitch** — one-shot: the first crate placed in STOW registers on a
  different cell for one frame, then the panel "corrects." Fires once per run.

All verified on the desktop build: full puzzle path fresh-save -> act1_done,
wrong-answer rejection, gate refusal without a card, and the CANTOR payoff.

### Puzzle solutions (for reference)
- Energy balance: **7** (kW). See the ledger table above.
- Caesar: shift **7** (decode back 7) -> `CANTOR IS NOT ALONE IN HIS OWN VOICE`.
  Ciphertext `JHUAVY PZ UVA HSVUL PU OPZ VDU CVPJL`.
- STOW: rows/cols target [3,2,2,2,3]; pillars B1, C3, D5. One valid fill:
  A1 A3 A5 / B2 B4 / C1 C5 / D2 D4 / E1 E3 E5. (Any fill meeting the counts
  and avoiding pillars wins.)
