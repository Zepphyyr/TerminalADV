// minigame.h — real-time graphical minigames for KODZIMIM (Act II+).
// Header-only. LOGIC is kept pure and platform-free (headless-testable);
// RENDERING/INPUT go through IPlatform's gfx/pollKey layer.
#ifndef KODZIMIM_MINIGAME_H
#define KODZIMIM_MINIGAME_H

#include "platform.h"
#include <cmath>
#include <string>

namespace kd {

static const Color kBlack {0, 0, 0};

// ---- SINE MATCH ---------------------------------------------------------
// Two waves: a "source" wave (PALE's, violet) and the player's (amber). The
// player bends theirs with the arrows — LEFT/RIGHT change the wavelength
// ("length"), UP/DOWN the amplitude ("width") — to overlay the source.
// The PALE twist (STORY_BIBLE §12): the instant the player FIRST matches,
// PALE shifts the source wavelength once, and the player must match again.
struct Wave { int len; int amp; };

class SineMatch {
public:
    static const int LMIN = 1, LMAX = 8;   // wavelength steps (cycles across)
    static const int AMIN = 1, AMAX = 6;   // amplitude steps

    Wave source {6, 4};
    Wave player {1, 1};
    bool paleFired = false;
    bool solved_   = false;
    const char* msg = "match the wave";

    bool matched() const { return player.len == source.len && player.amp == source.amp; }
    bool solved()  const { return solved_; }
    bool done()    const { return solved_; }

    // Pure step: apply one key, detect the rising edge into a match.
    void step(Key k) {
        if (solved_) return;
        bool was = matched();
        switch (k) {
            case K_LEFT:  if (player.len > LMIN) player.len--; break;
            case K_RIGHT: if (player.len < LMAX) player.len++; break;
            case K_DOWN:  if (player.amp > AMIN) player.amp--; break;
            case K_UP:    if (player.amp < AMAX) player.amp++; break;
            default: break;
        }
        if (matched() && !was) {              // just snapped onto the source
            if (!paleFired) { paleFired = true; source.len = 3; msg = "the wave slipped..."; }
            else            { solved_ = true;  msg = "locked."; }
        }
    }
};

// Plot one wave as a polyline across the full width.
inline void plotWave(IPlatform* p, const Wave& w, Color col) {
    const int W = p->gfxW(), H = p->gfxH();
    const int mid = H / 2;
    const double PI = 3.14159265358979;
    int px = 0, py = mid;
    for (int x = 0; x < W; ++x) {
        double phase = (double)x / (double)W * (double)w.len * 2.0 * PI;
        int a = w.amp * (mid - 2) / SineMatch::AMAX;
        int y = mid - (int)(a * std::sin(phase));
        if (x > 0) p->gfxLine(px, py, x, y, col);
        px = x; py = y;
    }
}

inline void renderSine(IPlatform* p, const SineMatch& g) {
    p->gfxClear(kBlack);
    plotWave(p, g.source, pal::pale);     // PALE's wave
    plotWave(p, g.player, pal::amber);    // yours
    p->gfxText(0, 0, g.msg, pal::grey);
    p->gfxText(0, p->gfxH() - 1, "arrows: bend   q: leave", pal::grey);
    p->gfxPresent();
}

// Blocking real-time loop. Returns true if solved, false if the player left.
inline bool runSineMatch(IPlatform* p) {
    SineMatch g;
    renderSine(p, g);
    while (!g.done()) {
        Key k = p->pollKey();
        if (k == K_BACK) return false;
        if (k != K_NONE) { g.step(k); renderSine(p, g); }
        p->delayMs(16);
    }
    renderSine(p, g);
    p->delayMs(600);
    return true;
}

} // namespace kd
#endif
