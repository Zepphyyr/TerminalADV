# KODZIMIM — Art direction (core game)

Companion to `STORY_BIBLE.md` (world) and `PALETTE.md` (text colour). This is the
**visual** design code for the 3D walking-sim. The goal: the cyberpunk-brutalist,
"built for machine vision" look — but **authored from primary sources and from
our own fiction**, not borrowed from Marathon (2026).

---

## 1. The one idea that makes it ours

**MERIDIAN DEEP is signed for machines, not for people.** It is an AI-run,
surveilled, corporate station that PALE re-optimised past human legibility. So
its surfaces speak a machine language: high-contrast glyphs, fiducial markers,
colour-coded wayfinding — legible to a camera, alienating to an eye. The human
presence is **graffiti scrawled over the machine's signage**: a hand-lettered
"ISSUE FAIRLY", a child's drawing in corridor nine.

That is the whole trick. The style is not decoration; it is **what the station
is**. Marathon uses this look as a style; we use it as a fact about the world.
The comparison fades when the aesthetic has a reason.

Continuity: this is the 3D extension of `PALETTE.md`. Each corporate module
already has a machine-legible brand colour — that system becomes the wayfinding.

---

## 2. Design rules (generative, not decorative)

- **Machine-legible surfaces.** Walls carry fiducial markers, index codes, and
  bold glyphs meant for the station's cameras. Real ArUco/AprilTag-style markers,
  or custom "Artcodes". Human-readable text is small, secondary, often overwritten.
- **Colour = system, per `PALETTE.md`.** Wayfinding and signage use the module's
  brand colour. A corridor's colour tells a machine which corporation owns it.
  Acid, saturated, high-contrast — but each hue *means* something.
- **Sharp angles, hard edges, limited materials.** Few surfaces, few materials,
  strong silhouettes. Flat bold colour + emissive signage over PBR realism.
  (Marathon's AD literally cites "limited materials".)
- **Brutalist mass.** Heavy concrete/metal volumes, oppressive scale, right
  angles, repetition. The architecture is infrastructure, not hospitality.
- **Human warmth only as intrusion.** Anything soft/personal (a mug, a drawing,
  scrawled words) reads as a violation of the machine order. Rare, and it lands
  harder for being rare. This carries the emotional core (the dead city).
- **Clean, not grungy.** Lean toward crisp graphic-realism (flat, high-contrast)
  rather than dirty PS1 fuzz. Use retro-render tricks sparingly, for mood, not as
  the whole identity.

---

## 3. Primary sources for the moodboard (go here, not to Marathon)

Marathon's own art director named these — draw from the same well:

- **The Designers Republic** — the studio behind WipEout's identity. The root of
  this whole acid-graphic / machine-signage language (1990s).
- **Ghost in the Shell** (Oshii), **Akira / Otomo**, **Koji Morimoto** — dense
  machine cities, signage, cold scale.
- **Neville Brody / David Rudnick / rave & jungle flyer design** — brutal
  typography, acid colour, graphic aggression.
- **The New Aesthetic (James Bridle)** and **fiducial markers** (ArUco, AprilTag,
  Artcodes), warehouse-robot floor markings, CV-dazzle camouflage — the real
  "design for machine eyes" material Marathon only brushes.
- **Mirror's Edge** — clean colour-coded wayfinding in a stark world.

Deliberately NOT a reference: Marathon (2026). We share ancestors, not assets.

---

## 4. How it meets the gameplay

- Walking-sim: the space *is* the content. Architecture and signage do the
  storytelling between terminals.
- **Terminals** are the bridge to everything we already built: the one-thought-
  per-screen text engine, the colour-voice palette, the dialogue format — all
  reimplemented as in-world screens (and mirrored on the Cardputer companion).
- Wayfinding-by-colour doubles as level design: the player learns to read the
  machine's colour language to navigate — then PALE starts corrupting it.
