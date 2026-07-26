// minigame.h — real-time graphical minigames for KODZIMIM (Act II+).
// Header-only. LOGIC is kept pure and platform-free (headless-testable);
// RENDERING/INPUT go through IPlatform's gfx/pollKey layer.
#ifndef KODZIMIM_MINIGAME_H
#define KODZIMIM_MINIGAME_H

#include "platform.h"
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

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

// ---- MAZE ---------------------------------------------------------------
// Top-down maze (STORY_BIBLE §12: crawl the station's guts to another
// terminal, then back). A dot moves on the arrows; reach the exit. The layout
// is data — content supplies it; a default is provided for testing/debug.
//   '#' wall   '.' open   'S' start   'E' exit
class Maze {
public:
    std::vector<std::string> grid;
    int px = 1, py = 1, ex = 1, ey = 1;
    bool solved_ = false;

    void init(const std::vector<std::string>& g) {
        grid = g; solved_ = false;
        for (int y = 0; y < (int)grid.size(); ++y)
            for (int x = 0; x < (int)grid[y].size(); ++x) {
                if (grid[y][x] == 'S') { px = x; py = y; }
                if (grid[y][x] == 'E') { ex = x; ey = y; }
            }
    }
    bool wall(int x, int y) const {
        if (y < 0 || y >= (int)grid.size() || x < 0 || x >= (int)grid[y].size()) return true;
        return grid[y][x] == '#';
    }
    void step(Key k) {
        if (solved_) return;
        int nx = px, ny = py;
        if      (k == K_UP)    ny--;
        else if (k == K_DOWN)  ny++;
        else if (k == K_LEFT)  nx--;
        else if (k == K_RIGHT) nx++;
        else return;
        if (!wall(nx, ny)) { px = nx; py = ny; }
        if (px == ex && py == ey) solved_ = true;
    }
    bool solved() const { return solved_; }
    bool done()   const { return solved_; }
};

inline std::vector<std::string> defaultMaze() {
    return {
        "###############",
        "#S............#",
        "#.###########.#",
        "#.###########.#",
        "#.###########.#",
        "#.###########.#",
        "#.###########.#",
        "#............E#",
        "###############",
    };
}

inline void renderMaze(IPlatform* p, const Maze& m) {
    const int W = p->gfxW(), H = p->gfxH();
    int rows = (int)m.grid.size(), cols = 0;
    for (size_t i = 0; i < m.grid.size(); ++i) cols = std::max(cols, (int)m.grid[i].size());
    int cw = cols ? W / cols : 1, ch = rows ? (H - 2) / rows : 1;
    int cell = std::min(cw, ch); if (cell < 1) cell = 1;
    const int oy = 1;
    p->gfxClear(kBlack);
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < (int)m.grid[y].size(); ++x) {
            char c = m.grid[y][x];
            if (c == '#')      p->gfxRect(x * cell, y * cell + oy, cell, cell, pal::grey);
            else if (c == 'E') p->gfxRect(x * cell, y * cell + oy, cell, cell, pal::green);
        }
    p->gfxRect(m.px * cell, m.py * cell + oy, cell, cell, pal::amber);
    p->gfxText(0, 0, "reach the exit   q: leave", pal::grey);
    p->gfxPresent();
}

inline bool runMaze(IPlatform* p, const std::vector<std::string>& layout) {
    Maze m; m.init(layout); renderMaze(p, m);
    while (!m.done()) {
        Key k = p->pollKey();
        if (k == K_BACK) return false;
        if (k != K_NONE) { m.step(k); renderMaze(p, m); }
        p->delayMs(16);
    }
    renderMaze(p, m); p->delayMs(400);
    return true;
}

} // namespace kd
#endif
