// game.h
// KODZIMIM core game engine (portable C++). Drives the PROLOGUE MVP.
#ifndef KODZIMIM_GAME_H
#define KODZIMIM_GAME_H

#include "platform.h"
#include <string>
#include <vector>

namespace kd {

class Game {
public:
    explicit Game(IPlatform* platform);

    // Entry point. Runs until the player quits or the prologue ends.
    void run();

private:
    IPlatform* p_;
    bool docked_ = false;   // has the player unlocked the docking clamps?
    bool running_ = true;   // main loop flag

    // ---- output helpers ----
    void nl();                                  // newline
    void line(const std::string& s);            // instant line
    void say(const std::string& s);             // typed line (atmospheric)
    void showFile(const std::string& path);     // load + page a content file
    void page(const std::string& text);         // word-wrap + paginate any text
    std::vector<std::string> wrap(const std::string& text, int cols);

    // ---- flow ----
    void bootPrologue();
    void shipLoop();          // ORPHEUS ship-terminal command loop
    void dockingSequence();   // cutscene after the puzzle is solved
    void endPrologue();

    // ---- command handling on the ship terminal ----
    // Returns false if the command ended the session (shutdown/quit).
    bool handleCommand(const std::string& raw);
    void cmdHelp();
    void cmdDir();
    void cmdOpen(const std::string& arg);
    void cmdMail();
    void cmdLogs();
    void cmdStatus();
    void cmdUnlock(const std::string& arg);
};

// ---- small string utilities (kept here so both frontends can reuse) ----
std::string toLower(const std::string& s);
std::string trim(const std::string& s);
// Split "open briefing" -> cmd="open", arg="briefing".
void splitCommand(const std::string& raw, std::string& cmd, std::string& arg);

} // namespace kd

#endif // KODZIMIM_GAME_H
