// platform_m5.h — Cardputer-Adv implementation of IPlatform (M5Unified + M5Cardputer).
// Story texts are compiled in (content_embedded.h): one .bin, no filesystem.
#ifndef KODZIMIM_PLATFORM_M5_H
#define KODZIMIM_PLATFORM_M5_H

#include <M5Cardputer.h>
#include <M5Unified.h>
#include <Preferences.h>
#include "../core/platform.h"
#include "content_embedded.h"

namespace kd {

// Screen: 240x135. Base font is 6x8; at size 2 that is 12x16 px
// => 20 columns x 8 rows. Big enough to actually read in the hand.
static const int KD_TEXT_SIZE = 2;
static const int KD_CHAR_W    = 6 * KD_TEXT_SIZE;   // 12 px

class M5Platform : public IPlatform {
public:
    void begin() {
        auto cfg = M5.config();
        M5Cardputer.begin(cfg, true);
        auto& d = M5Cardputer.Display;
        d.setRotation(1);
        d.setTextSize(KD_TEXT_SIZE);
        d.setTextScroll(true);
        d.fillScreen(0x0000);
        setColor(pal::amber);
        d.setCursor(0, 0);
    }

    Screen screen() const override { Screen s; s.cols = 20; s.rows = 8; return s; }

    // IMPORTANT: M5GFX expects RGB565. Passing a raw 0xRRGGBB makes everything
    // look red — that was the original bug. color565() converts properly.
    void setColor(Color c) override {
        auto& d = M5Cardputer.Display;
        d.setTextColor(d.color565(c.r, c.g, c.b), (uint16_t)0x0000);
    }

    void print(const std::string& s) override {
        M5Cardputer.Display.print(s.c_str());
    }

    void typeOut(const std::string& s) override {
        int n = 0;
        for (char ch : s) {
            M5Cardputer.Display.print(ch);
            delay(6);
            if (ch != ' ' && (++n % 3 == 0)) tick();
        }
    }

    void clear() override {
        M5Cardputer.Display.fillScreen(0x0000);
        M5Cardputer.Display.setCursor(0, 0);
    }

    void beep() override { tick(); }

    std::string readLine(const std::string& promptStr) override {
        print(promptStr);
        std::string buf;
        for (;;) {
            M5Cardputer.update();
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                auto st = M5Cardputer.Keyboard.keysState();
                for (auto c : st.word) { buf += c; echo(c); tick(); }
                if (st.del && !buf.empty()) {
                    buf.pop_back();
                    auto& d = M5Cardputer.Display;
                    int16_t x = d.getCursorX() - KD_CHAR_W;
                    if (x < 0) x = 0;
                    d.setCursor(x, d.getCursorY());
                    d.print(' ');
                    d.setCursor(x, d.getCursorY());
                }
                if (st.enter) { print("\n"); return buf; }
            }
            delay(8);
        }
    }

    // Advance one screen: wait for a fresh key press.
    void waitKey() override {
        for (;;) {
            M5Cardputer.update();
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                tick();
                return;
            }
            delay(8);
        }
    }

    // Hard reboot -> the bootloader / M5Launcher takes over again.
    void reboot() override {
        M5Cardputer.Display.fillScreen(0x0000);
        delay(200);
        ESP.restart();
    }

    void delayMs(int ms) override { delay(ms); }

    // ---- Minigame layer: real pixels on the 240x135 panel ---------------
    int gfxW() const override { return M5Cardputer.Display.width(); }
    int gfxH() const override { return M5Cardputer.Display.height(); }
    void gfxClear(Color c) override { M5Cardputer.Display.fillScreen(rgb(c)); }
    void gfxRect(int x, int y, int w, int h, Color c) override {
        M5Cardputer.Display.fillRect(x, y, w, h, rgb(c));
    }
    void gfxLine(int x1, int y1, int x2, int y2, Color c) override {
        M5Cardputer.Display.drawLine(x1, y1, x2, y2, rgb(c));
    }
    // (col,row) in the size-1 6x8 text grid.
    int gfxCols() const override { return M5Cardputer.Display.width() / 6; }
    int gfxRows() const override { return M5Cardputer.Display.height() / 8; }
    void gfxText(int col, int row, const std::string& s, Color c) override {
        auto& d = M5Cardputer.Display;
        d.setTextSize(1);
        d.setTextColor(rgb(c), (uint16_t)0x0000);
        d.setCursor(col * 6, row * 8);
        d.print(s.c_str());
        d.setTextSize(KD_TEXT_SIZE);
    }
    void gfxPresent() override {}   // M5GFX draws immediately

    Key pollKey() override {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            auto st = M5Cardputer.Keyboard.keysState();
            if (st.enter) return K_OK;
            for (auto c : st.word) {
                switch (c) {
                    case 'w': case ';': return K_UP;
                    case 's': case '.': return K_DOWN;
                    case 'a': case ',': return K_LEFT;
                    case 'd': case '/': return K_RIGHT;
                    case ' ': return K_OK;
                    case '`': return K_BACK;
                }
            }
        }
        return K_NONE;
    }
    unsigned long nowMs() override { return millis(); }

    bool loadFile(const std::string& path, std::string& out) override {
        return getEmbeddedContent(path, out);
    }

    void saveState(const std::string& key, const std::string& value) override {
        prefs_.begin("kodzimim", false);
        prefs_.putString(key.c_str(), value.c_str());
        prefs_.end();
    }
    std::string loadState(const std::string& key) override {
        prefs_.begin("kodzimim", true);
        String v = prefs_.getString(key.c_str(), "");
        prefs_.end();
        return std::string(v.c_str());
    }

private:
    void echo(char c) { M5Cardputer.Display.print(c); }
    void tick() { M5Cardputer.Speaker.tone(2200, 8); }
    static uint16_t rgb(Color c) { return M5Cardputer.Display.color565(c.r, c.g, c.b); }
    Preferences prefs_;
};

} // namespace kd
#endif
