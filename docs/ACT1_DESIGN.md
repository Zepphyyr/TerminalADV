# Act I — "The Quiet Decks" — design & puzzle specs

Companion to STORY_BIBLE.md §11. The bible owns the *story*; this file owns the
*mechanics*: exact puzzle data, solutions, and the engine work still to do.

Content already written: `content/act1/helion/*.txt`.

---

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
5. **ARGENT** (content TBD): Okoro's ration ledgers = the death curve; Emil in
   the background. Player **decrypts** HALO-9 with shift 19 -> a true line that
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
THERMAL      = 138 kW       known total = 981 kW
```

**Solution:** 1000 - 981 = **19 kW unmetered.**

Deliberate friction: total is split across two buses (must be summed); one draw
is stated in MW (must be converted). Everything else is plain subtraction.

Thematic point: 19 kW is about one habitation unit — something the size of a
single occupant is still drawing power in a city with no occupants.

**Answer 19 is reused as** the card code and the cipher shift. The reuse is the
intended "aha": the number you dug out of the ledger is the key to everything
else on this deck.

## Puzzle 2 — HALO-9's Caesar cipher

**Ciphertext (in `halo_hail.txt`):**

```
VTGMHK BL GHM TEHGX BG ABL HPG OHBVX
```

**Key:** shift 19 (from puzzle 1). Decrypt = shift each letter *back* 19.

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

## Engine work still to do (content is ahead of code)

The .txt beats exist but the engine cannot yet drive Act I. Needed:

1. **Deck/room navigation** beyond the prologue's station mode: a notion of
   "current deck" and gated transitions (HELION -> lift -> ARGENT -> next).
2. **Access-card state** + the lift gate: holding a card enables a transition
   and picks the lift greeting.
3. **Interactive puzzle screens**: a puzzle takes over input/render, reports
   success/failure back to the flow. First client is STOW; the arithmetic and
   cipher are solved in the player's head, so they only need an *answer check*
   (player types 19 / the decoded line), not a bespoke screen.
4. **The "go on?" beat**: Maru asks at each sector edge; `yes` / `not yet`.
5. **PALE glitch hook**: a one-shot render filter that prints a different number
   than the player typed, exactly once, on ARGENT. See STORY_BIBLE §8.
6. **Progress flags**: which decks are done, which cards are held — into the
   existing NVS save.
