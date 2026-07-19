// platform_desktop.h — desktop implementation of IPlatform.
// Emulates the Cardputer-Adv screen (20x8) in a terminal, with real colors,
// so the whole game can be play-tested on a PC.
//
// Env overrides (handy for automated testing):
//   KD_DELAY=<ms>  per-character typing delay (default 6; 0 for tests)
//   KD_BEEP=1      audible bell on beeps (default off)
//   KD_COLS/KD_ROWS override the emulated screen size
#ifndef KODZIMIM_PLATFORM_DESKTOP_H
#define KODZIMIM_PLATFORM_DESKTOP_H

#include "../core/platform.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

namespace kd {

class DesktopPlatform : public IPlatform {
public:
    DesktopPlatform(const std::string& contentRoot, const std::string& saveFile)
        : contentRoot_(contentRoot), saveFile_(saveFile) {
        const char* d = std::getenv("KD_DELAY");
        delayPerChar_ = d ? std::atoi(d) : 6;
        beepOn_ = std::getenv("KD_BEEP") != nullptr;
        if (const char* c = std::getenv("KD_COLS")) screen_.cols = std::atoi(c);
        if (const char* r = std::getenv("KD_ROWS")) screen_.rows = std::atoi(r);
    }

    Screen screen() const override { return screen_; }

    void setColor(Color c) override {
        std::cout << "\x1b[38;2;" << (int)c.r << ";" << (int)c.g << ";"
                  << (int)c.b << "m";
        std::cout.flush();
    }

    void print(const std::string& s) override { std::cout << s; std::cout.flush(); }

    void typeOut(const std::string& s) override {
        int n = 0;
        for (char ch : s) {
            std::cout << ch; std::cout.flush();
            if (delayPerChar_ > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerChar_));
                if (beepOn_ && ch != ' ' && (++n % 3 == 0)) bell();
            }
        }
    }

    void clear() override { std::cout << "\x1b[2J\x1b[H"; std::cout.flush(); }
    void beep() override { if (beepOn_) bell(); }

    std::string readLine(const std::string& promptStr) override {
        std::cout << promptStr; std::cout.flush();
        std::string line;
        if (!std::getline(std::cin, line)) return "/shutdown";  // EOF
        return line;
    }

    void waitKey() override { std::string d; std::getline(std::cin, d); }

    void reboot() override {
        std::cout << "\x1b[0m\n";
        std::exit(0);          // desktop has no launcher; just quit
    }

    void delayMs(int ms) override {
        if (delayPerChar_ > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    bool loadFile(const std::string& path, std::string& out) override {
        std::ifstream f(contentRoot_ + "/" + path, std::ios::binary);
        if (!f) return false;
        std::ostringstream ss; ss << f.rdbuf(); out = ss.str();
        return true;
    }

    void saveState(const std::string& key, const std::string& value) override {
        std::ofstream f(saveFile_, std::ios::app);
        f << key << "=" << value << "\n";
    }
    std::string loadState(const std::string& key) override {
        std::ifstream f(saveFile_);
        std::string line, last;
        while (std::getline(f, line))
            if (line.rfind(key + "=", 0) == 0) last = line.substr(key.size() + 1);
        return last;
    }

private:
    void bell() { std::cout << '\a'; std::cout.flush(); }
    std::string contentRoot_, saveFile_;
    Screen screen_;
    int delayPerChar_ = 6;
    bool beepOn_ = false;
};

} // namespace kd
#endif
