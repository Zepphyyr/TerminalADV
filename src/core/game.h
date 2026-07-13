// game.h — KODZIMIM core engine.
// 20x8 screen. Layout:
//   rows 0-5 : content (one thought at a time)
//   row  6   : status hint  ("[enter] more" / "type 'help'")
//   row  7   : command prompt — ALWAYS visible, you can type at any moment
#ifndef KODZIMIM_GAME_H
#define KODZIMIM_GAME_H

#include "platform.h"
#include <string>
#include <vector>

namespace kd {

// One line of rendered text, with its own color (so a grey header and an
// amber body can share the same screen).
struct CLine {
    Color color;
    std::string text;
};
using ScreenBuf = std::vector<CLine>;

class Game {
public:
    explicit Game(IPlatform* platform);
    void run();

private:
    IPlatform* p_;
    std::vector<ScreenBuf> screens_;    // queue of screens waiting to be read
    Color cur_          = pal::amber;   // current color while parsing content
    bool  docked_       = false;
    bool  running_      = true;
    bool  cinematic_    = false;        // boot / finale: any input just advances
    bool  finale_       = false;
    bool  prologueDone_ = false;        // after the prologue you stay playable
    bool  typeIt_       = true;         // animate this screen? (first show only)

    // content -> screens
    void queueBeats(const std::string& text);
    void queueFile(const std::string& path);
    std::vector<std::string> wrap(const std::string& text, int cols);

    void render();      // draw current screen + hint, leave cursor on prompt row
    void advance();     // drop the current screen

    bool handleCommand(const std::string& raw);
    bool handleStationCommand(const std::string& cmd, const std::string& arg);

    void cmdHelp();
    void cmdDir();
    void cmdOpen(const std::string& arg);
    void cmdUnlock(const std::string& arg);
    void cmdShutdown();
    void cmdSave();
    void cmdReset();
    void helpStation();

    void queueFinale();
    void finish();
    void saveProgress(const std::string& value);
};

Color colorByName(const std::string& name);
std::string toLower(const std::string& s);
std::string trim(const std::string& s);
void splitCommand(const std::string& raw, std::string& cmd, std::string& arg);

} // namespace kd
#endif
