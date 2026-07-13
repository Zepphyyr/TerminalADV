// platform_desktop.h
// Desktop implementation of IPlatform. Emulates the Cardputer-Adv screen
// (40x16) in a normal terminal so the whole game can be tested on a PC.
//
// Environment overrides (handy for automated testing):
//   KD_DELAY=<ms>   per-character typing delay   (default 6, use 0 for tests)
//   KD_BEEP=1       emit an audible bell on beeps (default off)
//   KD_COLS / KD_ROWS  override emulated screen size
#ifndef KODZIMIM_PLATFORM_DESKTOP_H
#define KODZIMIM_PLATFORM_DESKTOP_H

#include "../core/platform.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

namespace kd {

class DesktopPlatform : public IPlatform {
public:
    explicit DesktopPlatform(const std::string& contentRoot,
                             const std::string& saveFile)
        : contentRoot_(contentRoot), saveFile_(saveFile) {
        const char* d = std::getenv("KD_DELAY");
        delayPerChar_ = d ? std::atoi(d) : 6;
        beepOn_ = std::getenv("KD_BEEP") != nullptr;
        const char* c = std::getenv("KD_COLS");
        const char* r = std::getenv("KD_ROWS");
        if (c) screen_.cols = std::atoi(c);
        if (r) screen_.rows = std::atoi(r);
    }

    Screen screen() const override { return screen_; }

    void print(const std::string& s) override {
        std::cout << s;
        std::cout.flush();
    }

    void typeOut(const std::string& s) override {
        int beepEvery = 3; // beep every few chars for the "terminal" feel
        int n = 0;
        for (char ch : s) {
            std::cout << ch;
            std::cout.flush();
            if (delayPerChar_ > 0) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(delayPerChar_));
                if (beepOn_ && ch != ' ' && (++n % beepEvery == 0)) bell();
            }
        }
    }

    void clear() override {
        // ANSI clear + home. On the device this is a display fill.
        std::cout << "\x1b[2J\x1b[H";
        std::cout.flush();
    }

    void beep() override { if (beepOn_) bell(); }

    std::string readLine(const std::string& promptStr) override {
        std::cout << promptStr;
        std::cout.flush();
        std::string line;
        if (!std::getline(std::cin, line)) {
            // EOF: end the session cleanly.
            return "shutdown";
        }
        return line;
    }

    void waitKey() override {
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void delayMs(int ms) override {
        if (delayPerChar_ > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    bool loadFile(const std::string& path, std::string& out) override {
        std::ifstream f(contentRoot_ + "/" + path, std::ios::binary);
        if (!f) return false;
        std::ostringstream ss;
        ss << f.rdbuf();
        out = ss.str();
        return true;
    }

    void saveState(const std::string& key, const std::string& value) override {
        std::ofstream f(saveFile_, std::ios::app);
        f << key << "=" << value << "\n";
    }

    std::string loadState(const std::string& key) override {
        std::ifstream f(saveFile_);
        std::string line, last;
        while (std::getline(f, line)) {
            if (line.rfind(key + "=", 0) == 0)
                last = line.substr(key.size() + 1);
        }
        return last;
    }

private:
    void bell() { std::cout << '\a'; std::cout.flush(); }

    std::string contentRoot_;
    std::string saveFile_;
    Screen screen_;
    int delayPerChar_ = 6;
    bool beepOn_ = false;
};

} // namespace kd

#endif // KODZIMIM_PLATFORM_DESKTOP_H
