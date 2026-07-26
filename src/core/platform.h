// platform.h — platform abstraction for KODZIMIM.
#ifndef KODZIMIM_PLATFORM_H
#define KODZIMIM_PLATFORM_H

#include <string>
#include <cstdint>

namespace kd {

// Cardputer-Adv is 240x135 px. With the 6x8 base font at size 2 (12x16 px)
// that is 20 columns x 8 rows. Small — so the whole presentation is built
// around it: ONE THOUGHT PER SCREEN, press a key for the next.
struct Screen {
    int cols = 20;
    int rows = 8;
};

// 24-bit color. The M5 frontend converts it to RGB565.
// (Passing raw 0xFFB000 straight to M5GFX was the "everything is red" bug:
//  M5GFX reads that value as RGB565, not RGB888.)
struct Color { uint8_t r, g, b; };

// Real-time input for minigames (non-blocking, per-frame). Text still uses the
// line-based readLine(); this is only for the graphical minigame loops.
enum Key { K_NONE = 0, K_UP, K_DOWN, K_LEFT, K_RIGHT, K_OK, K_BACK };

// Palette — the single source of truth is docs/PALETTE.md. Keep them in sync.
// Rules (short form):
//   grey  = the PLAYER'S own thoughts, and UI/system meta (prompts, menus).
//   white = the WORLD outside any terminal: environment + human SPEECH text.
//   Each AI has ONE colour and speaks entirely in it (name + words).
//   Each manufacturer's TERMINAL text is EXCLUSIVELY that brand's colour,
//     unless an AI is speaking through it.
//   A human's NAME label takes that person's colour; their speech is white.
namespace pal {
constexpr Color white {225, 225, 225};  // world/environment + human speech
constexpr Color grey  {150, 150, 150};  // player's thoughts + UI/system meta

// --- AI voices (each speaks fully in its own colour) ---
constexpr Color cyan  {110, 220, 255};  // CANTOR — light blue
constexpr Color blue  {  8,   0, 255};  // HALO-9 — dark blue
constexpr Color pale  {200, 140, 255};  // PALE SIGNAL — violet

// --- Manufacturer terminal identities (exclusive on their own terminals) ---
constexpr Color amber  {255, 176,   0};  // ORPHEUS INTERACTIVE — player's console
constexpr Color helion {255, 105,   0};  // HELION DYNAMICS — reactor orange
constexpr Color argent {176, 196, 222};  // ARGENT & COLE — silver / steel
constexpr Color green  { 90, 255, 140};  // COR VITAE BIOSYSTEMS — bio green
constexpr Color red    {255,  90,  70};  // NULLPOINT DEFENSE — security red
constexpr Color tess   {225, 110, 180};  // TESSELLATE SYS — network magenta

// --- Human name labels (name in colour, speech stays white) ---
constexpr Color maru    {240, 150, 175}; // MARU / Vell — rose
constexpr Color cassel  {220, 180,  90}; // CASSEL — gold
constexpr Color okonkwo {120, 205, 175}; // OKONKWO — teal-green
}

class IPlatform {
public:
    virtual ~IPlatform() {}
    virtual Screen screen() const = 0;
    virtual void setColor(Color c) = 0;
    virtual void print(const std::string& s) = 0;
    virtual void typeOut(const std::string& s) = 0;
    virtual void clear() = 0;
    virtual void beep() = 0;
    virtual std::string readLine(const std::string& promptStr) = 0;
    virtual void waitKey() = 0;                 // advance one screen
    virtual void delayMs(int ms) = 0;

    // Hard reboot the device. On the Cardputer this drops you back into
    // M5Launcher, which is how you leave the game.
    virtual void reboot() = 0;

    virtual bool loadFile(const std::string& path, std::string& out) = 0;
    virtual void saveState(const std::string& key, const std::string& value) = 0;
    virtual std::string loadState(const std::string& key) = 0;

    // ---- Minigame layer (graphics + real-time input + clock) -------------
    // Default stubs so text-only builds/tests keep working; each platform
    // overrides. Coordinates are in the platform's pixel space (gfxW x gfxH):
    // the device is 240x135; the desktop uses a coarse ASCII canvas.
    virtual int  gfxW() const { return 240; }              // pixel width
    virtual int  gfxH() const { return 135; }              // pixel height
    virtual int  gfxCols() const { return gfxW() / 6; }    // text grid width  (6x8 font)
    virtual int  gfxRows() const { return gfxH() / 8; }    // text grid height
    virtual void gfxClear(Color c) { (void)c; }
    // gfxRect/gfxLine use PIXEL coords; gfxText uses CHARACTER-CELL coords
    // (col,row) so text lays out identically on device and desktop.
    virtual void gfxRect(int x, int y, int w, int h, Color c) { (void)x;(void)y;(void)w;(void)h;(void)c; }
    virtual void gfxLine(int x1, int y1, int x2, int y2, Color c) { (void)x1;(void)y1;(void)x2;(void)y2;(void)c; }
    virtual void gfxText(int col, int row, const std::string& s, Color c) { (void)col;(void)row;(void)s;(void)c; }
    virtual void gfxPresent() {}                 // flush a frame to the screen
    virtual Key  pollKey() { return K_NONE; }     // a fresh keypress, or K_NONE
    virtual unsigned long nowMs() { return 0; }   // monotonic milliseconds
};

} // namespace kd
#endif
