# KODZIMIM — Colour palette (canon)

**Single source of truth for text colour.** Every text edit — content `.txt`
beats and engine `queueBeats` strings — must follow this. Code mirror lives in
`src/core/platform.h` (`namespace pal`) and name lookup in `colorByName`.

`@color <name>` in content files accepts every **name** below (literal or alias).

---

## 1. The five rules

1. **Player's thoughts → grey.** The nameless protagonist's inner voice. Also
   used for UI/system meta (prompts, menus, `/help`, errors, save notices).
2. **World outside any terminal → white.** Environment, description, action,
   and the **spoken words of humans**.
3. **Each AI speaks fully in its own colour** (name *and* words), even when it
   speaks through a manufacturer's terminal. This is the only exception to
   rule 4.
4. **A manufacturer's terminal text is EXCLUSIVELY that brand's colour.** Logs,
   files, mail, ledgers, readouts, system responses shown on that terminal —
   all one colour. No mixing in amber/grey/etc. (AI override = rule 3.)
5. **Human dialogue = name in the person's colour, speech in white.** Because
   the engine colours a whole line at once, write it vertically:

   ```
   @color okonkwo
   OKONKWO
   @color white
   I'll hold the lock.
   ```

   AIs do NOT split — the whole line is the AI colour: `CANTOR: ...` in cyan.

---

## 2. The colours

### Roles
| use | name | RGB |
|---|---|---|
| player's thoughts / UI meta | `grey` | 150,150,150 |
| world / environment / human speech | `white` | 225,225,225 |

### AI voices (whole utterance in colour)
| voice | name (aliases) | RGB | note |
|---|---|---|---|
| CANTOR | `cyan` / `cantor` | 110,220,255 | light blue |
| HALO-9 | `blue` / `halo` | 8,0,255 | dark blue |
| PALE SIGNAL | `pale` | 200,140,255 | violet |

### Manufacturer terminals (exclusive on their own decks)
| brand | name (aliases) | RGB |
|---|---|---|
| ORPHEUS INTERACTIVE (player console) | `amber` / `orpheus` | 255,176,0 |
| HELION DYNAMICS | `helion` | 255,105,0 |
| ARGENT & COLE | `argent` | 176,196,222 |
| COR VITAE BIOSYSTEMS | `green` / `corvitae` | 90,255,140 |
| NULLPOINT DEFENSE | `red` / `nullpoint` | 255,90,70 |
| TESSELLATE SYS | `tess` / `tessellate` | 225,110,180 |

### Human name labels (name coloured, speech white)
| person | name | RGB |
|---|---|---|
| MARU / Vell | `maru` / `vell` | 240,150,175 |
| CASSEL | `cassel` | 220,180,90 |
| OKONKWO | `okonkwo` | 120,205,175 |
| the player | — | (no label; thoughts are `grey`) |

Station dead (Lang, Junia, Okoro, Emil) are met as **terminal documents**, so
their text is the deck's manufacturer colour — they get no separate name colour
unless one day they speak in the world.

---

## 3. Worked examples

**Reading Lang's log on HELION** (terminal document → all HELION orange):
```
@color helion
LANG / ENGINEERING LOG
Cycle 118. Coils nominal...
```

**Walking onto a deck** (environment → white; the placard is the brand):
```
@color white
The lock opens on a room that still thinks it is working.
@color helion
HELION DYNAMICS / DECK H-2
```

**CANTOR through any terminal** (AI → cyan overrides the brand):
```
@color cyan
CANTOR: I kept the lights on.
```

**Cassel on the radio** (human → name gold, speech white):
```
@color cassel
CASSEL
@color white
You feel it? Air's wrong down here.
```

**A system prompt / error** (UI meta → grey):
```
@color grey
unknown directive.
```

---

## 4. Quick decision tree

- Is it the AI talking? → that AI's colour (whole line).
- Is it text on a manufacturer's terminal screen? → that brand's colour.
- Is it a human speaking in the world? → name line in their colour, speech white.
- Is it environment / description / action outside a terminal? → white.
- Is it the player thinking, or UI chrome? → grey.
