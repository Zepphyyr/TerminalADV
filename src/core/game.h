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

// One rendered line. `hl` = syntax-highlight this line (used by the notebook):
// codes/passwords go green, ALLCAPS names/systems go cyan, the rest stays base.
struct CLine {
    Color color;
    std::string text;
    bool hl = false;
};
using ScreenBuf = std::vector<CLine>;

class Game {
public:
    explicit Game(IPlatform* platform);
    void run();

private:
    IPlatform* p_;
    std::vector<ScreenBuf> screens_;
    std::vector<std::string> notes_;
    Color cur_          = pal::amber;
    bool  docked_       = false;
    bool  running_      = true;
    bool  cinematic_    = false;
    bool  finale_       = false;
    bool  prologueDone_ = false;
    bool  typeIt_       = true;

    void queueBeats(const std::string& text);
    void queueFile(const std::string& path);
    void pushLines(const std::vector<CLine>& lines);
    std::vector<std::string> wrap(const std::string& text, int cols);

    void render();
    void advance();
    void printHighlighted(const std::string& text, Color base);

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

    void loadNotes();
    void saveNotes();
    void addNote(const std::string& body);
    void showNotes();
    void clearNotes();

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
