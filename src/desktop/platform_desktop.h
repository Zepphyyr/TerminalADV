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
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

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

    // ---- Minigame layer -------------------------------------------------
    // The device draws real pixels; on the desktop we render a coarse ASCII
    // canvas so minigames are still visible (and scriptable) in a terminal.
    int gfxW() const override { return DW; }
    int gfxH() const override { return DH; }
    int gfxCols() const override { return DW; }   // desktop: pixel == cell
    int gfxRows() const override { return DH; }

    void gfxClear(Color c) override {
        for (int i = 0; i < DW * DH; ++i) { cbuf_[i] = ' '; colbuf_[i] = c; }
    }
    void gfxRect(int x, int y, int w, int h, Color c) override {
        for (int j = 0; j < h; ++j) for (int i = 0; i < w; ++i) plot(x + i, y + j, glyphFor(c), c);
    }
    void gfxLine(int x1, int y1, int x2, int y2, Color c) override {
        int dx = std::abs(x2 - x1), dy = -std::abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1, sy = y1 < y2 ? 1 : -1, err = dx + dy;
        for (;;) {
            plot(x1, y1, glyphFor(c), c);
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }
    void gfxText(int x, int y, const std::string& s, Color c) override {
        for (size_t i = 0; i < s.size(); ++i) plot(x + (int)i, y, s[i], c);
    }
    void gfxPresent() override {
        std::cout << "\x1b[H";                       // home, overwrite (less flicker)
        for (int y = 0; y < DH; ++y) {
            for (int x = 0; x < DW; ++x) {
                Color c = colbuf_[y * DW + x];
                std::cout << "\x1b[38;2;" << (int)c.r << ';' << (int)c.g << ';' << (int)c.b << 'm'
                          << cbuf_[y * DW + x];
            }
            std::cout << "\x1b[0m\n";
        }
        std::cout.flush();
    }

    Key pollKey() override {
#ifdef _WIN32
        if (_kbhit()) return mapKey(_getch());
        return K_NONE;
#else
        fd_set fds; FD_ZERO(&fds); FD_SET(0, &fds);
        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 0;
        if (select(1, &fds, nullptr, nullptr, &tv) > 0) {
            unsigned char c;
            if (::read(0, &c, 1) > 0) return mapKey(c);
        }
        return K_NONE;
#endif
    }

    unsigned long nowMs() override {
        using namespace std::chrono;
        return (unsigned long)duration_cast<milliseconds>(
                   steady_clock::now().time_since_epoch()).count();
    }

private:
    static const int DW = 80, DH = 24;
    char  cbuf_[DW * DH];
    Color colbuf_[DW * DH];
    void plot(int x, int y, char ch, Color c) {
        if (x < 0 || y < 0 || x >= DW || y >= DH) return;
        cbuf_[y * DW + x] = ch; colbuf_[y * DW + x] = c;
    }
    static char glyphFor(Color c) {
        if (c.r == 200 && c.g == 140 && c.b == 255) return '~';  // pale
        if (c.r == 255 && c.g == 176 && c.b == 0)   return 'o';  // amber
        if (c.r == 150 && c.g == 150 && c.b == 150) return '.';  // grey
        return '#';
    }
    static Key mapKey(int c) {
        switch (c) {
            case 'w': case ';': return K_UP;
            case 's': case '.': return K_DOWN;
            case 'a': case ',': return K_LEFT;
            case 'd': case '/': return K_RIGHT;
            case ' ': case '\n': case '\r': return K_OK;
            case 'q': case 27: return K_BACK;
        }
        return K_NONE;
    }

    void bell() { std::cout << '\a'; std::cout.flush(); }
    std::string contentRoot_, saveFile_;
    Screen screen_;
    int delayPerChar_ = 6;
    bool beepOn_ = false;
};

} // namespace kd
#endif
