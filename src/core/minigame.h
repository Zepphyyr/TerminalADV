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
#include <cstdio>

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
    p->gfxText(0, p->gfxRows() - 1, "arrows: bend   q: leave", pal::grey);
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
    p->gfxText(0, 0, "reach the exit  q:leave", pal::grey);
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

// ---- QTE ----------------------------------------------------------------
// Timing bar (STORY_BIBLE §12: unscrew a panel / disassemble a terminal). An
// indicator sweeps a bar; press OK while it's over the (per-step, randomised)
// target zone. Do it `steps` times. Overshoot-safe: zone width >= speed.
class QTE {
public:
    int WIDTH = 120;
    int steps = 3, done_steps = 0, misses = 0;
    int pos = 0, dir = 1, speed = 4;
    int zoneLo = 0, zoneHi = 0, zoneW = 18;
    bool solved_ = false;
    unsigned rng_ = 2463534242u;

    unsigned rnd() { rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5; return rng_; }
    void newZone() { zoneLo = (int)(rnd() % (unsigned)(WIDTH - zoneW + 1)); zoneHi = zoneLo + zoneW; }

    void init(int nSteps, int zoneWidth, int spd) {
        steps = nSteps; zoneW = zoneWidth; speed = spd;
        done_steps = misses = 0; pos = 0; dir = 1; solved_ = false;
        rng_ = 2463534242u; newZone();
    }
    void tick() {                              // one frame of indicator movement
        if (solved_) return;
        pos += dir * speed;
        if (pos >= WIDTH) { pos = WIDTH; dir = -1; }
        if (pos <= 0)     { pos = 0;     dir =  1; }
    }
    bool inZone() const { return pos >= zoneLo && pos <= zoneHi; }
    void press() {
        if (solved_) return;
        if (inZone()) { if (++done_steps >= steps) solved_ = true; else newZone(); }
        else misses++;
    }
    bool solved() const { return solved_; }
    bool done()   const { return solved_; }
};

inline void renderQTE(IPlatform* p, const QTE& q, const char* label) {
    const int W = p->gfxW(), H = p->gfxH(), mid = H / 2;
    const int x0 = 6, x1 = W - 6, bw = x1 - x0;
    #define KD_SX(v) (x0 + (v) * bw / q.WIDTH)
    p->gfxClear(kBlack);
    p->gfxRect(x0, mid, bw, 1, pal::grey);
    p->gfxRect(KD_SX(q.zoneLo), mid - 4, KD_SX(q.zoneHi) - KD_SX(q.zoneLo) + 1, 9, pal::green);
    p->gfxRect(KD_SX(q.pos), mid - 6, 1, 13, pal::amber);
    #undef KD_SX
    p->gfxText(0, 0, std::string(label) + "  " +
               std::to_string(q.done_steps) + "/" + std::to_string(q.steps), pal::grey);
    p->gfxText(0, p->gfxRows() - 1, "space: hit   q: leave", pal::grey);
    p->gfxPresent();
}

inline bool runQTE(IPlatform* p, const char* label, int steps) {
    QTE q; q.init(steps, 18, 4);
    while (!q.done()) {
        Key k = p->pollKey();
        if (k == K_BACK) return false;
        if (k == K_OK)   q.press();
        q.tick();
        renderQTE(p, q, label);
        p->delayMs(24);
    }
    renderQTE(p, q, label); p->delayMs(400);
    return true;
}

// ---- HACK (Fallout-style terminal) --------------------------------------
// Decrypt slot (STORY_BIBLE §12). A memory dump of junk with candidate WORDS
// (all same length) hidden in it; one is the password. Guessing a word costs
// an attempt and reveals its "likeness" = letters in the correct position.
// Matched bracket pairs ( ) [ ] { } < > can be selected once to remove a dud
// (or, if none remain, replenish attempts). Board is generated from a word
// list content supplies + a seed, so logic is deterministic and testable.
enum EType { E_NONE = 0, E_WORD, E_BRACKET };
struct Entity { EType type; int row, col0, col1, wordIdx; bool active; };

class HackGame {
public:
    int R = 12, C = 16;
    std::vector<std::string> grid;     // R rows, length C
    std::vector<int> cell;             // R*C -> entity index or -1
    std::vector<Entity> ents;
    std::vector<std::string> words;
    int passIdx = 0;
    int attempts = 4, maxAttempts = 4;
    int cursor = 0;
    bool solved_ = false, failed_ = false;
    int lastLikeness = -1;
    std::string lastGuess, lastMsg;
    unsigned rng_ = 2463534242u;

    unsigned rnd() { rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5; return rng_; }
    int wordLen() const { return words.empty() ? 0 : (int)words[0].size(); }

    void init(const std::vector<std::string>& candidateWords, int correctIdx, unsigned seed) {
        words = candidateWords; passIdx = correctIdx;
        attempts = maxAttempts; solved_ = failed_ = false;
        lastLikeness = -1; lastGuess.clear(); lastMsg.clear();
        cursor = 0; rng_ = seed ? seed : 1u;
        grid.assign(R, std::string(C, ' '));
        cell.assign(R * C, -1);
        ents.clear();
        static const std::string junk = "!@#$%^&*-+=/|;:?_~";
        for (int r = 0; r < R; ++r)
            for (int c = 0; c < C; ++c) grid[r][c] = junk[rnd() % junk.size()];
        int len = wordLen();
        // place words
        for (int i = 0; i < (int)words.size(); ++i) {
            for (int t = 0; t < 300; ++t) {
                int r = rnd() % R, c = (len < C) ? (int)(rnd() % (C - len + 1)) : 0;
                bool ok = true;
                for (int k = 0; k < len; ++k) if (cell[r * C + c + k] != -1) { ok = false; break; }
                if (!ok) continue;
                for (int k = 0; k < len; ++k) { grid[r][c + k] = words[i][k]; cell[r * C + c + k] = (int)ents.size(); }
                Entity e; e.type = E_WORD; e.row = r; e.col0 = c; e.col1 = c + len - 1; e.wordIdx = i; e.active = true;
                ents.push_back(e); break;
            }
        }
        // place a few bracket pairs on free junk
        static const std::string ob = "([{<", cb = ")]}>";
        for (int b = 0; b < 3; ++b) {
            for (int t = 0; t < 300; ++t) {
                int g = 2 + (int)(rnd() % 4);
                if (g >= C) continue;
                int r = rnd() % R, c = (int)(rnd() % (C - g));
                bool ok = true;
                for (int k = 0; k <= g; ++k) if (cell[r * C + c + k] != -1) { ok = false; break; }
                if (!ok) continue;
                int bi = rnd() % 4;
                grid[r][c] = ob[bi]; grid[r][c + g] = cb[bi];
                cell[r * C + c] = (int)ents.size();     // trigger on the opening bracket
                Entity e; e.type = E_BRACKET; e.row = r; e.col0 = c; e.col1 = c + g; e.wordIdx = -1; e.active = true;
                ents.push_back(e); break;
            }
        }
    }

    int likeness(const std::string& a) const {
        const std::string& pw = words[passIdx]; int n = 0;
        for (size_t i = 0; i < a.size() && i < pw.size(); ++i) if (a[i] == pw[i]) n++;
        return n;
    }
    int hovered() const { return cell[cursor]; }

    void move(Key k) {
        int r = cursor / C, c = cursor % C;
        if      (k == K_UP    && r > 0)     cursor -= C;
        else if (k == K_DOWN  && r < R - 1) cursor += C;
        else if (k == K_LEFT  && c > 0)     cursor -= 1;
        else if (k == K_RIGHT && c < C - 1) cursor += 1;
    }

    void pressOK() {
        if (solved_ || failed_) return;
        int e = cell[cursor];
        if (e < 0) return;
        Entity& en = ents[e];
        if (en.type == E_WORD && en.active) {
            lastGuess = words[en.wordIdx];
            lastLikeness = likeness(lastGuess);
            if (lastLikeness == wordLen()) { solved_ = true; lastMsg = "ACCESS GRANTED"; }
            else { if (--attempts <= 0) failed_ = true; lastMsg = "denied"; }
        } else if (en.type == E_BRACKET && en.active) {
            en.active = false;
            int dud = -1;
            for (int i = 0; i < (int)ents.size(); ++i)
                if (ents[i].type == E_WORD && ents[i].active && ents[i].wordIdx != passIdx) { dud = i; break; }
            if (dud >= 0) {
                ents[dud].active = false;
                for (int c = ents[dud].col0; c <= ents[dud].col1; ++c) {
                    grid[ents[dud].row][c] = '.'; cell[ents[dud].row * C + c] = -1;
                }
                lastMsg = "dud removed";
            } else { attempts = maxAttempts; lastMsg = "allowance replenished"; }
        }
    }

    bool solved() const { return solved_; }
    bool failed() const { return failed_; }
    bool done()   const { return solved_ || failed_; }
};

inline void renderHack(IPlatform* p, const HackGame& g) {
    const int rows = p->gfxRows();
    p->gfxClear(kBlack);
    std::string att; for (int i = 0; i < g.attempts; ++i) att += '#';
    p->gfxText(0, 0, "TERMINAL LOCKED  ATT:" + att, pal::red);
    int hov = g.cell[g.cursor];
    for (int r = 0; r < g.R; ++r) {
        int y = 1 + r;
        if (y >= rows - 2) break;
        char adr[10]; std::snprintf(adr, sizeof(adr), "0x%04X ", (0xF900 + r * g.C) & 0xFFFF);
        std::string a(adr); int rx = (int)a.size();
        p->gfxText(0, y, a, pal::grey);
        p->gfxText(rx, y, g.grid[r], pal::green);
        if (hov >= 0 && g.ents[hov].row == r) {
            const Entity& e = g.ents[hov];
            p->gfxText(rx + e.col0, y, g.grid[r].substr(e.col0, e.col1 - e.col0 + 1), pal::amber);
        }
        if (g.cursor / g.C == r) {
            int cc = g.cursor % g.C;
            p->gfxText(rx + cc, y, std::string(1, g.grid[r][cc]), pal::white);
        }
    }
    if (g.lastLikeness >= 0)
        p->gfxText(0, rows - 2, "> " + g.lastGuess + "  " +
                   std::to_string(g.lastLikeness) + "/" + std::to_string(g.wordLen()) + " " + g.lastMsg, pal::grey);
    p->gfxText(0, rows - 1, "move:arrows  pick:space  q:leave", pal::grey);
    p->gfxPresent();
}

inline bool runHack(IPlatform* p, const std::vector<std::string>& words, int correctIdx) {
    HackGame g; g.init(words, correctIdx, 0x51ACE7u);  // seed
    renderHack(p, g);
    while (!g.done()) {
        Key k = p->pollKey();
        if (k == K_BACK) return false;
        if (k == K_UP || k == K_DOWN || k == K_LEFT || k == K_RIGHT) { g.move(k); renderHack(p, g); }
        else if (k == K_OK) { g.pressOK(); renderHack(p, g); }
        p->delayMs(16);
    }
    renderHack(p, g); p->delayMs(700);
    return g.solved();
}

inline std::vector<std::string> defaultHackWords() {
    return {"SIGNAL","SILVER","SISTER","MASTER","MATTER","LETTER","BITTER","LATTER"};
}

} // namespace kd
#endif
