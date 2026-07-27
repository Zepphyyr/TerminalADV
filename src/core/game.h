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
    std::vector<std::string> replies;                // variants, separated by ~~~
    size_t lastReply = (size_t)-1;                   // avoid repeating verbatim
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

    // ---- Act I: deck navigation ---------------------------------------
    // Where the player stands. "" = aboard but not on a deck yet (post-prologue
    // station state). Persisted under save key "deck" so a reboot resumes here.
    std::string deck_;            // "", "helion", "argent", "act1_done"
    bool  pendingGo_   = false;   // Maru asked "go on?" — next line answers
    bool  haloHailed_  = false;   // HALO-9 has broken silence on HELION
    // Puzzle gates — a deck is passable once its card is in hand.
    bool  cardLang_    = false;   // HELION: won by solving the energy balance
    bool  cardOkoro_   = false;   // ARGENT: won by solving STOW
    bool  cantorConfronted_ = false; // decoded line brought to CANTOR (once)
    bool  glitchFired_ = false;   // PALE input glitch has fired (once, in STOW)
    bool  npSession_   = false;   // NULLPOINT: the Council's final session read

    // ---- conversation state -------------------------------------------
    // Who your plain words go to. Empty = nobody is on the channel.
    // It is STICKY: "/c ..." or "/h ..." switches the addressee, after which
    // you just type normally. Moving to a terminal sets it from nodeDefault_.
    std::string addressee_;
    std::string nodeDefault_ = "cantor";  // who lives at this terminal
    std::string dlgLoaded_;               // which talk file is in memory
    Color dlgVoice_ = pal::cyan;          // that speaker's colour
    bool  introShown_ = false;
    std::string dlgIntro_;
    std::vector<DlgRule> dlgRules_;
    std::vector<std::string> dlgFallback_;
    std::vector<std::string> dlgAmbient_;   // unprompted thoughts he drops in
    size_t fbIdx_ = 0;
    size_t lastFb_ = (size_t)-1;            // never repeat a deflection twice
    unsigned rng_ = 2463534242u;

    void queueBeats(const std::string& text);
    void queueFile(const std::string& path);
    void pushLines(const std::vector<CLine>& lines);
    std::vector<std::string> wrap(const std::string& text, int cols);

    void render();
    void advance();
    void printHighlighted(const std::string& text, Color base);

    bool handleCommand(const std::string& raw);   // "/command" (slash stripped)
    void handleSpeech(const std::string& raw);    // plain words = talking
    static bool isKnownCommand(const std::string& cmd);
    void closeChannel();
    bool handleStationCommand(const std::string& cmd, const std::string& arg);

    // Act I deck flow
    void enterAct1();                                        // station -> HELION
    void arriveDeck(const std::string& deck);               // show arrival, save
    void resumeDeck(const std::string& deck);               // reboot back onto deck
    bool handleDeckCommand(const std::string& cmd, const std::string& arg);
    bool helionCommand(const std::string& cmd, const std::string& arg);
    bool argentCommand(const std::string& cmd, const std::string& arg);
    bool corvitaeCommand(const std::string& cmd, const std::string& arg);  // Act II
    bool nullpointCommand(const std::string& cmd, const std::string& arg); // Act II
    int  btStage();                                        // Broken Terminal stage
    void deckLook();                                    // /look — the room
    void deckDir();                                     // /dir  — list documents
    void deckOpen(const std::string& arg);              // /open <name> (o <name>)
    void deckMail();                                    // /mail — held letters
    void helpDeck();
    void deckStatus();
    void askGoOn();                                          // Maru: "go on?"
    void advanceDeck();                                      // helion->argent->end
    std::string deckTitle(const std::string& deck) const;
    void unknownHere();                                      // shared error beat

    // Act I puzzles
    void solveEnergy(const std::string& arg);   // HELION: /solve <n> -> Lang card
    bool runStow();                             // ARGENT: STOW grid, returns solved
    bool decodedLine(const std::string& raw) const; // player spoke the decoded phrase

    // Act II: sysadmin password recovery (text flow). Player fails the login,
    // falls back to security questions (pet + child DOB from the read bio),
    // then sets a personal admin password saved under "adminpw" for later use.
    bool runPasswordFlow(const std::string& petName, const std::string& childDob);

    // Play text beats BLOCKING (one screen at a time, waitKey to advance), so a
    // scripted quest can interleave narrative with the blocking minigames.
    void playBeats(const std::string& text);
    void playFile(const std::string& path);     // same, from a content file

    // Act II set-piece: chains the whole "Broken Terminal" quest, persisting the
    // stage under "bt_stage" so a reboot resumes mid-quest. Returns true when
    // the recovered data has been read; false if the player left a minigame.
    bool runBrokenTerminal();

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
    bool ensureDialogue(const std::string& who);
    void speakTo(const std::string& who, const std::string& phrase);
    void arriveAt(const std::string& who);      // terminal sets the default
    std::string voiceName(const std::string& who) const;
    void talkTo(const std::string& who);
    bool loadDialogue(const std::string& path);
    void talkAnswer(const std::string& question);
    unsigned nextRand();
    std::string pickDeflection(const std::vector<std::string>& toks);

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
