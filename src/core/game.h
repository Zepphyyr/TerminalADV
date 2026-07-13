// game.h — KODZIMIM core engine. Beat-based presentation for a 20x8 screen.
#ifndef KODZIMIM_GAME_H
#define KODZIMIM_GAME_H

#include "platform.h"
#include <string>
#include <vector>

namespace kd {

class Game {
public:
    explicit Game(IPlatform* platform);
    void run();

private:
    IPlatform* p_;
    bool docked_  = false;
    bool running_ = true;
    Color cur_ = pal::amber;   // current color, persists across beats

    // --- beat rendering -------------------------------------------------
    // Content is a sequence of BEATS separated by a line "---".
    // A line "@color <name>" switches the color from that beat onward.
    // Each beat gets its own screen: clear -> type -> wait for a key.
    void showBeats(const std::string& text);
    void showFile(const std::string& path);
    void renderBeat(const std::vector<std::string>& lines, Color c);
    std::vector<std::string> wrap(const std::string& text, int cols);

    // --- flow ------------------------------------------------------------
    void bootPrologue();
    void shipLoop();
    void dockingSequence();
    void endPrologue();

    // --- commands ---------------------------------------------------------
    bool handleCommand(const std::string& raw);
    void cmdHelp();
    void cmdDir();
    void cmdOpen(const std::string& arg);
    void cmdUnlock(const std::string& arg);
};

Color colorByName(const std::string& name);
std::string toLower(const std::string& s);
std::string trim(const std::string& s);
void splitCommand(const std::string& raw, std::string& cmd, std::string& arg);

} // namespace kd
#endif
