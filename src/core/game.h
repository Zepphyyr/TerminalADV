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
    bool hl;
    // NOTE: an explicit constructor (instead of `bool hl = false;`) keeps this
    // usable under C++11, which is what the Arduino ESP32 core compiles with.
    // A default member initializer would make CLine a non-aggregate there and
    // break every CLine{...} call site.
    CLine(Color c, const std::string& t, bool highlight = false)
        : color(c), text(t), hl(highlight) {}
};
using ScreenBuf = std::vector<CLine>;

// One conversation rule: if the player's sentence contains ALL words of any
// one trigger phrase, the AI answers with `reply` (which may be multi-beat and
// carry @color directives). Rules are scored by specificity (longest matched
// phrase wins), so "did you kill them" beats a bare "them".
struct DlgRule {
    std::vector<std::vector<std::string>> triggers;  // list of word-phrases
    std::string reply;
};

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

    // conversation ("talk") state
    bool  inTalk_ = false;
    std::string dlgIntro_;
    std::vector<DlgRule> dlgRules_;
    std::vector<std::string> dlgFallback_;
    size_t fbIdx_ = 0;

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

    // conversation
    void talkTo(const std::string& who);
    bool loadDialogue(const std::string& path);
    void talkAnswer(const std::string& question);

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
