// platform_m5.h
// Cardputer-Adv implementation of IPlatform (M5Unified + M5Cardputer).
// Content is read from LittleFS; progress is saved to NVS (Preferences).
//
// NOTE: this file is only compiled for the ESP32 build (PlatformIO), not the
// desktop test build. It is written to be structurally correct for the
// Cardputer-Adv; fine-tune font size / beep tone on real hardware.
#ifndef KODZIMIM_PLATFORM_M5_H
#define KODZIMIM_PLATFORM_M5_H

#include <M5Cardputer.h>
#include <M5Unified.h>
#include <Preferences.h>
#include "../core/platform.h"
#include "content_embedded.h"   // story texts baked into the firmware

namespace kd {

class M5Platform : public IPlatform {
public:
    void begin() {
        auto cfg = M5.config();
        M5Cardputer.begin(cfg, true);
        auto& d = M5Cardputer.Display;
        d.setRotation(1);
        d.setTextSize(1);               // 6x8 font -> ~40 cols x ~16 rows
        d.setTextColor(0xFFB000, 0x000000); // amber on black
        d.setTextScroll(true);          // auto-scroll like a real console
        d.fillScreen(0x000000);
        d.setCursor(0, 0);
    }

    Screen screen() const override {
        Screen s; s.cols = 40; s.rows = 16; return s;
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
        M5Cardputer.Display.fillScreen(0x000000);
        M5Cardputer.Display.setCursor(0, 0);
    }

    void beep() override { tick(); }

    std::string readLine(const std::string& promptStr) override {
        print(promptStr);
        std::string buf;
        for (;;) {
            M5Cardputer.update();
            if (M5Cardputer.Keyboard.isChange() &&
                M5Cardputer.Keyboard.isPressed()) {
                auto st = M5Cardputer.Keyboard.keysState();
                for (auto c : st.word) { buf += c; echo(c); tick(); }
                if (st.del && !buf.empty()) {
                    buf.pop_back();
                    // visually erase last char
                    auto& d = M5Cardputer.Display;
                    int16_t x = d.getCursorX() - 6;
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

    void waitKey() override {
        for (;;) {
            M5Cardputer.update();
            if (M5Cardputer.Keyboard.isChange() &&
                M5Cardputer.Keyboard.isPressed()) { tick(); return; }
            delay(8);
        }
    }

    void delayMs(int ms) override { delay(ms); }

    bool loadFile(const std::string& path, std::string& out) override {
        // Texts are compiled into the firmware (see content_embedded.h),
        // so there is no filesystem to mount and only one .bin to flash.
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
    void tick() { M5Cardputer.Speaker.tone(2200, 8); } // short terminal blip
    Preferences prefs_;
};

} // namespace kd

#endif // KODZIMIM_PLATFORM_M5_H
