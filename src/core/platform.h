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

// Palette — different terminals and different voices get different colors.
namespace pal {
constexpr Color amber {255, 176,   0};  // ORPHEUS / player console (default)
constexpr Color white {225, 225, 225};  // narration OUTSIDE the terminal
constexpr Color cyan  {110, 220, 255};  // CANTOR — the caretaker AI
constexpr Color green { 90, 255, 140};  // systems nominal / COR VITAE
constexpr Color red   {255,  90,  70};  // alerts, rejections / NULLPOINT
constexpr Color blue  {  8,   0, 255};  // HALO-9 "THE WARDEN" — hard police blue
constexpr Color pale  {200, 140, 255};  // PALE SIGNAL — the thing they built
constexpr Color grey  {150, 150, 150};  // headers, metadata, prompts
// Corporate module identities — each deck's brand reads at a glance.
constexpr Color helion {255, 105,   0};  // HELION DYNAMICS — reactor orange
constexpr Color argent {176, 196, 222};  // ARGENT & COLE — silver / steel
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
};

} // namespace kd
#endif
