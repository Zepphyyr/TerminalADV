// game.cpp — KODZIMIM engine. One thought per screen, prompt always at the bottom,
// plus a persistent player notebook with IDE-style keyword highlighting.
#include "game.h"
#include "minigame.h"
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace kd {

// defined further down, used earlier by decodedLine()
static std::vector<std::string> tokenize(const std::string& s);

std::string toLower(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = (char)std::tolower((unsigned char)c);
    return r;
}
std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) a++;
    while (b > a && std::isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}
void splitCommand(const std::string& raw, std::string& cmd, std::string& arg) {
    std::string t = trim(raw);
    size_t sp = t.find(' ');
    if (sp == std::string::npos) { cmd = toLower(t); arg = ""; }
    else { cmd = toLower(t.substr(0, sp)); arg = trim(t.substr(sp + 1)); }
}
static std::string digitsOnly(const std::string& s) {
    std::string r;
    for (char c : s) if (std::isdigit((unsigned char)c)) r += c;
    return r;
}
// Names map to docs/PALETTE.md. Both literal palette names and semantic
// aliases (orpheus/corvitae/nullpoint/cantor/halo) resolve here.
Color colorByName(const std::string& n) {
    std::string s = toLower(trim(n));
    // roles
    if (s == "white") return pal::white;
    if (s == "grey" || s == "gray") return pal::grey;
    // AI voices
    if (s == "cyan"  || s == "cantor") return pal::cyan;
    if (s == "blue"  || s == "halo" || s == "halo9") return pal::blue;
    if (s == "pale") return pal::pale;
    // manufacturer terminals
    if (s == "amber" || s == "orpheus") return pal::amber;
    if (s == "helion") return pal::helion;
    if (s == "argent") return pal::argent;
    if (s == "green" || s == "corvitae" || s == "cor_vitae") return pal::green;
    if (s == "red"   || s == "nullpoint") return pal::red;
    if (s == "tess"  || s == "tessellate") return pal::tess;
    // human name labels
    if (s == "maru"  || s == "vell") return pal::maru;
    if (s == "cassel") return pal::cassel;
    if (s == "okonkwo") return pal::okonkwo;
    return pal::amber;
}

Game::Game(IPlatform* platform) : p_(platform) {}

static bool endsWithPause(const std::string& w) {
    if (w.empty()) return false;
    char c = w.back();
    return c=='.'||c==','||c==';'||c==':'||c=='!'||c=='?'||c=='-';
}

std::vector<std::string> Game::wrap(const std::string& text, int cols) {
    std::vector<std::string> out;
    std::istringstream in(text);
    std::string raw;
    while (std::getline(in, raw)) {
        if ((int)raw.size() <= cols) { out.push_back(raw); continue; }
        std::vector<std::string> w;
        {
            std::istringstream ls(raw); std::string t;
            while (ls >> t) {
                while ((int)t.size() > cols) { w.push_back(t.substr(0, cols)); t = t.substr(cols); }
                if (!t.empty()) w.push_back(t);
            }
        }
        const int n = (int)w.size();
        if (n == 0) { out.push_back(""); continue; }
        const long long INF = 1000000000000LL;
        const long long BREAK_PENALTY = 60;
        std::vector<long long> dp(n + 1, INF);
        std::vector<int> nxt(n + 1, n);
        dp[n] = 0;
        for (int i = n - 1; i >= 0; --i) {
            int len = 0;
            for (int j = i; j < n; ++j) {
                len += (int)w[j].size() + (j > i ? 1 : 0);
                if (len > cols) break;
                long long cost;
                if (j == n - 1) cost = 0;
                else { long long slack = cols - len; cost = slack*slack; if (!endsWithPause(w[j])) cost += BREAK_PENALTY; }
                if (dp[j + 1] < INF && cost + dp[j + 1] < dp[i]) { dp[i] = cost + dp[j + 1]; nxt[i] = j + 1; }
            }
        }
        for (int i = 0; i < n; ) {
            int j = nxt[i]; std::string line = w[i];
            for (int k = i + 1; k < j; ++k) line += " " + w[k];
            out.push_back(line); i = j;
        }
    }
    return out;
}

void Game::pushLines(const std::vector<CLine>& lines) {
    Screen sc = p_->screen();
    const int contentRows = sc.rows - 2;
    for (size_t i = 0; i < lines.size(); ) {
        ScreenBuf s;
        for (int n = 0; n < contentRows && i < lines.size(); ++n) s.push_back(lines[i++]);
        screens_.push_back(s);
    }
}

// One "source" per screen. A screen never mixes, say, a terminal log with the
// player's grey thoughts. Colours are grouped into sources: the WORLD (white +
// human name/speech colours) is one source; grey (thoughts/UI) is one; every
// manufacturer colour is its own; every AI colour is its own. When the source
// changes mid-beat we start a new screen automatically — so authors never have
// to remember a '---' at every voice change.
static bool sameColor(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}
static int sourceKey(const Color& c) {
    // WORLD: environment + human dialogue (name label + white speech together)
    if (sameColor(c, pal::white) || sameColor(c, pal::maru) ||
        sameColor(c, pal::cassel) || sameColor(c, pal::okonkwo)) return 1;
    if (sameColor(c, pal::grey))   return 2;   // thoughts / UI
    if (sameColor(c, pal::amber))  return 3;   // ORPHEUS terminal
    if (sameColor(c, pal::helion)) return 4;
    if (sameColor(c, pal::argent)) return 5;
    if (sameColor(c, pal::green))  return 6;
    if (sameColor(c, pal::red))    return 7;
    if (sameColor(c, pal::tess))   return 8;
    if (sameColor(c, pal::cyan))   return 9;   // CANTOR
    if (sameColor(c, pal::blue))   return 10;  // HALO-9
    if (sameColor(c, pal::pale))   return 11;  // PALE
    return 0;
}

void Game::queueBeats(const std::string& text) {
    Screen sc = p_->screen();
    ScreenBuf beat;
    auto flush = [&]() {
        if (beat.empty()) return;
        std::vector<CLine> wrapped;
        for (const auto& cl : beat)
            for (const auto& w : wrap(cl.text, sc.cols))
                wrapped.push_back(CLine{cl.color, w, false});
        pushLines(wrapped);
        beat.clear();
    };
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::string t = trim(line);
        if (t == "---") { flush(); continue; }
        if (t.rfind("@color", 0) == 0) {
            Color nc = colorByName(t.substr(6));
            // new source => new screen (unless nothing is buffered yet)
            if (!beat.empty() && sourceKey(nc) != sourceKey(cur_)) flush();
            cur_ = nc;
            continue;
        }
        beat.push_back(CLine{cur_, line, false});
    }
    flush();
}

void Game::queueFile(const std::string& path) {
    std::string body;
    if (!p_->loadFile(path, body)) {
        cur_ = pal::red; queueBeats("missing file:\n" + path); cur_ = pal::amber; return;
    }
    queueBeats(body);
}

static Color classify(const std::string& tok, Color base) {
    int uppers = 0, lowers = 0, digits = 0, codey = 0;
    for (char c : tok) {
        if (std::isalpha((unsigned char)c)) { if (std::isupper((unsigned char)c)) uppers++; else lowers++; }
        else if (std::isdigit((unsigned char)c)) { digits++; codey++; }
        else if (c=='-'||c==':'||c=='/'||c=='.') codey++;
    }
    if (digits >= 2 && lowers == 0 && uppers == 0 && codey == (int)tok.size())
        return pal::green;
    if (lowers == 0 && uppers >= 2)
        return pal::cyan;
    return base;
}

void Game::printHighlighted(const std::string& text, Color base) {
    size_t i = 0;
    while (i < text.size()) {
        if (std::isspace((unsigned char)text[i])) {
            size_t j = i; while (j < text.size() && std::isspace((unsigned char)text[j])) j++;
            p_->setColor(base);
            std::string sp = text.substr(i, j - i);
            if (typeIt_) p_->typeOut(sp); else p_->print(sp);
            i = j;
        } else {
            size_t j = i; while (j < text.size() && !std::isspace((unsigned char)text[j])) j++;
            std::string tok = text.substr(i, j - i);
            p_->setColor(classify(tok, base));
            if (typeIt_) p_->typeOut(tok); else p_->print(tok);
            i = j;
        }
    }
}

void Game::render() {
    Screen sc = p_->screen();
    const int contentRows = sc.rows - 2;
    p_->clear();
    int used = 0;
    if (!screens_.empty()) {
        for (const auto& cl : screens_.front()) {
            if (cl.hl) { printHighlighted(cl.text, cl.color); }
            else { p_->setColor(cl.color); if (typeIt_) p_->typeOut(cl.text); else p_->print(cl.text); }
            p_->print("\n"); used++;
        }
    }
    for (int i = used; i < contentRows; ++i) p_->print("\n");
    if (screens_.size() > 1) { p_->setColor(pal::grey); p_->print("[enter] more"); }
    else if (!addressee_.empty()) { p_->setColor(dlgVoice_); p_->print(voiceName(addressee_)); }
    else if (screens_.empty()){ p_->setColor(pal::grey); p_->print("/help"); }
    p_->print("\n");
    typeIt_ = false;
}

void Game::advance() {
    if (!screens_.empty()) screens_.erase(screens_.begin());
    typeIt_ = true;
}

void Game::loadNotes() {
    notes_.clear();
    std::string blob = p_->loadState("notes");
    std::string cur;
    for (char c : blob) {
        if (c == '\x1f') { if (!cur.empty()) notes_.push_back(cur); cur.clear(); }
        else cur += c;
    }
    if (!cur.empty()) notes_.push_back(cur);
}
void Game::saveNotes() {
    std::string blob;
    for (size_t i = 0; i < notes_.size(); ++i) { if (i) blob += '\x1f'; blob += notes_[i]; }
    p_->saveState("notes", blob);
}
void Game::clearNotes() {
    notes_.clear(); saveNotes();
    cur_ = pal::grey; queueBeats("NOTEBOOK cleared."); cur_ = pal::amber;
}

void Game::addNote(const std::string& body) {
    std::string b = trim(body);
    std::string tag = "NOTE";
    if (b.size() >= 2 && b[0] == '*') {
        char k = (char)std::tolower((unsigned char)b[1]);
        if      (k == 'p') tag = "PWD";
        else if (k == 'w') tag = "WHO";
        else if (k == 'l') tag = "LEAD";
        else if (k == 'k' || k == '!') tag = "KEY";
        size_t sp = b.find(' ');
        b = (sp == std::string::npos) ? "" : trim(b.substr(sp + 1));
    }
    if (b.empty()) { showNotes(); return; }
    notes_.push_back(tag + " " + b);
    saveNotes();
    showNotes();
}

void Game::showNotes() {
    Screen sc = p_->screen();
    std::vector<CLine> lines;
    lines.push_back(CLine{pal::grey, "NOTEBOOK", false});
    if (notes_.empty()) {
        lines.push_back(CLine{pal::grey, "(empty)", false});
        lines.push_back(CLine{pal::grey, "add:  /note <text>", false});
        lines.push_back(CLine{pal::grey, "or:   *p <code>", false});
    } else {
        for (const auto& e : notes_)
            for (const auto& w : wrap(e, sc.cols))
                lines.push_back(CLine{pal::white, w, true});
    }
    pushLines(lines);
    typeIt_ = true;
}

void Game::run() {
    cur_ = pal::amber;
    p_->beep();
    typeIt_ = true;
    loadNotes();

    if (p_->loadState("progress") == "prologue_done") {
        prologueDone_ = true;
        cinematic_    = false;
        deck_ = trim(p_->loadState("deck"));
        if (!deck_.empty() && deck_ != "act1_done") {
            resumeDeck(deck_);
        } else {
            arriveAt("cantor");   // he is always there
            cur_ = pal::grey;
            queueBeats("SAVE FOUND\nprologue complete.\n"
                       "---\n"
                       "@color amber\nYou are aboard\nMERIDIAN DEEP.\n"
                       "@color cyan\nCANTOR is on the\nchannel.\n"
                       "@color grey\n/proceed to step\nonto the station.");
            cur_ = pal::amber;
        }
    } else {
        cinematic_ = true;
        queueFile("prologue/boot.txt");
    }

    while (running_) {
        render();
        p_->setColor(pal::amber);
        std::string in  = p_->readLine(addressee_.empty() ? "> " : "you> ");
        std::string cmd = trim(in);

        if (cinematic_) {
            advance();
            if (screens_.empty()) { cinematic_ = false; if (finale_) finish(); }
            continue;
        }

        if (cmd.empty()) { advance(); continue; }

        screens_.clear();
        typeIt_ = true;

        // Maru asked "go on?" — this line answers. yes advances; anything else
        // dismisses the prompt (and, if it was a real order, still runs below).
        if (pendingGo_) {
            pendingGo_ = false;
            std::string a = toLower(cmd);
            if (a=="yes"||a=="y"||a=="yeah"||a=="ok"||a=="go"||a=="onward") {
                advanceDeck(); continue;
            }
            if (a=="not yet"||a=="no"||a=="wait"||a=="stay"||a=="n") {
                cur_ = pal::white;
                queueBeats("@color maru\nMARU\n@color white\nAlright. When you're ready.");
                cur_ = pal::amber; continue;
            }
            // otherwise: fall through and treat the input normally
        }

        // INPUT MODEL:
        //   *note...  -> quick notebook entry
        //   /command  -> an order to the terminal
        //   anything  -> you are SPEAKING (to whoever is on the channel)
        if (cmd[0] == '*') { addNote(cmd); continue; }
        if (cmd[0] == '/') {
            std::string body = trim(cmd.substr(1));
            if (body.empty()) { cmdHelp(); continue; }
            if (!handleCommand(body)) break;
            if (docked_ && !finale_ && !prologueDone_) queueFinale();
            continue;
        }
        handleSpeech(cmd);
    }
}

bool Game::handleCommand(const std::string& raw) {
    std::string cmd, arg;
    splitCommand(raw, cmd, arg);
    if (cmd == "o") cmd = "open";
    if (cmd == "?") cmd = "help";   // NB: "h" is reserved for addressing HALO-9
    if (cmd == "l") cmd = "logs";
    if (cmd == "n") cmd = "notes";

    if (!raw.empty() && raw[0] == '*') { addNote(raw); return true; }
    if (cmd == "notes" || cmd == "note") {
        if (arg == "clear") clearNotes();
        else if (arg.empty()) showNotes();
        else addNote(arg);
        return true;
    }
    if (cmd == "c" || cmd == "cantor") { speakTo("cantor", arg); return true; }
    if (cmd == "h" || cmd == "halo")   { speakTo("halo",   arg); return true; }
    if (cmd == "version" || cmd == "build") {
        cur_ = pal::grey;
        queueBeats(std::string("KODZIMIM\nbuild\n@color amber\n")
                   + __DATE__ + "\n" + __TIME__);
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "sine") {                       // debug: try the Act II minigame
        bool ok = runSineMatch(p_);
        cur_ = pal::grey;
        queueBeats(ok ? "SINE: locked." : "SINE: left.");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "maze") {                        // debug: top-down maze
        bool ok = runMaze(p_, defaultMaze());
        cur_ = pal::grey;
        queueBeats(ok ? "MAZE: out." : "MAZE: left.");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "qte") {                         // debug: timing bar
        bool ok = runQTE(p_, "UNSCREW", 3);
        cur_ = pal::grey;
        queueBeats(ok ? "QTE: done." : "QTE: left.");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "hack") {                         // debug: Fallout-style terminal
        bool ok = runHack(p_, defaultHackWords(), 0);   // password = SIGNAL
        cur_ = pal::grey;
        queueBeats(ok ? "HACK: in." : "HACK: out.");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "pw") {                           // debug: password recovery flow
        bool ok = runPasswordFlow("rocket", "0412");
        cur_ = pal::grey;
        queueBeats(ok ? "PW: admin set." : "PW: aborted.");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "broken") {                        // debug: whole quest orchestrator
        bool ok = runBrokenTerminal();
        cur_ = pal::grey;
        queueBeats(ok ? "QUEST: recovered." : "QUEST: paused (resume with /broken).");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "corvitae") {                       // debug: read the COR VITAE logs
        playFile("act2/corvitae/arrive.txt");
        playFile("act2/corvitae/genetics.txt");
        playFile("act2/corvitae/children.txt");
        playFile("act2/corvitae/cryo.txt");
        playFile("act2/corvitae/scientist.txt");
        cur_ = pal::grey; queueBeats("COR VITAE: read."); cur_ = pal::amber;
        return true;
    }
    if (cmd == "nullpoint") {                      // debug: read the NULLPOINT deck
        playFile("act2/nullpoint/arrive.txt");
        playFile("act2/nullpoint/tribunals.txt");
        playFile("act2/nullpoint/elders.txt");
        playFile("act2/nullpoint/council_fall.txt");
        cur_ = pal::grey; queueBeats("NULLPOINT: read."); cur_ = pal::amber;
        return true;
    }
    if (cmd == "shutdown" || cmd == "quit" || cmd == "exit") { cmdShutdown(); return true; }
    if (cmd == "save")  { cmdSave();  return true; }
    if (cmd == "reset") { cmdReset(); return true; }

    if (prologueDone_) {
        if (!deck_.empty()) return handleDeckCommand(cmd, arg);
        return handleStationCommand(cmd, arg);
    }

    if      (cmd == "help")   cmdHelp();
    else if (cmd == "dir")    cmdDir();
    else if (cmd == "open")   cmdOpen(arg);
    else if (cmd == "mail")   queueFile("prologue/mail.txt");
    else if (cmd == "logs")   queueFile("prologue/shiplog.txt");
    else if (cmd == "status") queueFile("prologue/status.txt");
    else if (cmd == "unlock" || cmd == "dock") cmdUnlock(arg);
    else if (cmd == "clear")  { }
    else {
        cur_ = pal::red;
        queueBeats("unknown directive.\nthis system does not\nremember that word.");
        cur_ = pal::amber;
    }
    return true;
}

bool Game::handleStationCommand(const std::string& cmd, const std::string& arg) {
    if (cmd == "help") { helpStation(); return true; }
    if (cmd == "talk") { talkTo(arg.empty() ? "cantor" : arg); return true; }
    if (cmd == "proceed" || cmd == "enter" || cmd == "begin" || cmd == "go") {
        enterAct1(); return true;
    }
    if (cmd == "status") {
        cur_ = pal::grey;
        queueBeats("MERIDIAN DEEP\n@color amber\nnetwork    ONLINE\ncrew       NONE\n"
                   "CANTOR     LISTENING\nHALO-9     ?");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "logs" || cmd == "mail" || cmd == "dir" || cmd == "open") {
        cur_ = pal::grey;
        queueBeats("You are aboard, but\nstill at the airlock\npanel.\n"
                   "---\nThe station opens\nfurther in.\n"
                   "---\n@color amber\n/proceed\nto step onto the\nfirst deck.");
        cur_ = pal::amber;
        return true;
    }
    unknownHere();
    return true;
}

// ------------------------------------------------------------- Act I decks
std::string Game::deckTitle(const std::string& deck) const {
    if (deck == "helion") return "HELION / DECK H-2";
    if (deck == "argent") return "ARGENT / DECK A-5";
    if (deck == "act1_done") return "MERIDIAN DEEP";
    return "MERIDIAN DEEP";
}

void Game::unknownHere() {
    cur_ = pal::grey;
    queueBeats("@color grey\nunknown directive.\nthis system does not\nremember that word.\n"
               "---\n/help for what this\nterminal understands.");
    cur_ = pal::amber;
}

// Step off the airlock panel onto the first station deck.
void Game::enterAct1() {
    cur_ = pal::white;
    queueBeats("@color white\nYou leave the airlock panel behind. The station opens around you.");
    cur_ = pal::amber;
    arriveDeck("helion");
}

// Show a deck's arrival, set the channel, and persist the location.
void Game::arriveDeck(const std::string& deck) {
    deck_ = deck;
    p_->saveState("deck", deck_);
    arriveAt("cantor");                 // CANTOR is station-wide; plain words reach him
    if (deck == "helion") {
        queueFile("act1/helion/arrive.txt");
    } else if (deck == "argent") {
        queueFile("act1/argent/arrive.txt");
        queueFile("act1/argent/cassel.txt");   // an unbidden transmission, not a file
    }
    cur_ = pal::white;
    queueBeats("@color maru\nMARU\n@color white\nI'm on comms. Look around. Say /go when you want to move on.");
    cur_ = pal::amber;
}

// Reboot landed us mid-Act-I: re-orient without replaying the arrival cinematic.
void Game::resumeDeck(const std::string& deck) {
    deck_ = deck;
    // Reaching a later deck means the earlier cards were already earned; restore
    // them so a reboot never strands the player behind a gate.
    cardLang_  = (deck == "argent" || deck == "act1_done");
    cardOkoro_ = (deck == "act1_done");
    arriveAt("cantor");
    std::string acc = (deck == "helion") ? "helion" : (deck == "argent") ? "argent" : "grey";
    cur_ = pal::grey;
    queueBeats("@color grey\nRESUME\n@color " + acc + "\n" + deckTitle(deck) +
               "\n@color grey\n/look  where am I\n/help  orders");
    cur_ = pal::amber;
}

bool Game::handleDeckCommand(const std::string& cmd, const std::string& arg) {
    if (cmd == "help")   { helpDeck();   return true; }
    if (cmd == "status") { deckStatus(); return true; }
    if (cmd == "talk")   { talkTo(arg.empty() ? "cantor" : arg); return true; }
    if (cmd == "go" || cmd == "proceed" || cmd == "next") { askGoOn(); return true; }
    // Shared terminal verbs, same model as the prologue: dir lists documents,
    // open (aliased "o") reads one, mail holds letters.
    if (cmd == "look") { deckLook(); return true; }
    if (cmd == "dir")  { deckDir();  return true; }
    if (cmd == "open") { deckOpen(arg); return true; }
    if (cmd == "mail") { deckMail(); return true; }
    if (deck_ == "helion") return helionCommand(cmd, arg);
    if (deck_ == "argent") return argentCommand(cmd, arg);
    // act1_done or unknown deck: end-of-content
    cur_ = pal::grey;
    queueBeats("END OF ACT I\n---\n@color amber\nThe decks beyond are\nnot yet built.\n"
               "@color grey\nTalk to CANTOR or\nHALO-9 in the\nmeantime.");
    cur_ = pal::amber;
    return true;
}

bool Game::helionCommand(const std::string& cmd, const std::string& arg) {
    if (cmd == "power" || cmd == "load") {
        queueFile("act1/helion/power.txt");
        if (!haloHailed_) {              // the silent channel breaks, once
            haloHailed_ = true;
            queueFile("act1/helion/halo_hail.txt");
        }
        return true;
    }
    if (cmd == "solve" || cmd == "balance") { solveEnergy(arg); return true; }
    unknownHere();
    return true;
}

// HELION energy balance. Buses 604+396 = 1000 kW; known draws sum to 993 (with
// FOLD COILS listed as 0.30 MW to force a kW conversion); the missing 7 kW is
// the answer. The same 7 is the Caesar shift for HALO-9's cipher. Narratively,
// the deck's audit interlock holds Lang's card until the ledger reconciles;
// entering the unaccounted figure reconciles it and releases the card.
void Game::solveEnergy(const std::string& arg) {
    std::string d = digitsOnly(arg);
    if (d.empty()) {
        cur_ = pal::grey;
        queueBeats("@color grey\nsolve: give a number.\n/solve <n>\n---\nbus total minus the\nknown draws.");
        cur_ = pal::amber; return;
    }
    p_->beep();
    if (d == "7") {
        cardLang_ = true;
        cur_ = pal::helion;
        queueBeats("@color helion\nAUDIT RECONCILED\nunaccounted load: 7 kW\n"
                   "---\n@color grey\nSomething the size of one habitation unit still draws power. In a dead city.\n"
                   "---\n@color helion\nThe console decides the deck is not being tampered with, and releases the card seated in Lang's reader.\n"
                   "ACCESS CARD:\nLANG, T.\n"
                   "---\n@color blue\n(7 is also the shift HALO-9 asked for.)\n"
                   "@color grey\n/go when ready.");
        cur_ = pal::amber;
    } else {
        cur_ = pal::helion;
        queueBeats("@color helion\nLEDGER STILL OFF.\nthe interlock holds.\n---\n@color grey\nsum BOTH buses for\nthe total. convert\nthe MW line. then\nsubtract. try again.");
        cur_ = pal::amber;
    }
}

bool Game::argentCommand(const std::string& cmd, const std::string& arg) {
    (void)arg;
    if (cmd == "comms" || cmd == "ferryman" || cmd == "cassel" || cmd == "hail") {
        // Cassel already called on arrival; hailing back gets only static.
        cur_ = pal::white;
        queueBeats("@color white\nYou hail FERRYMAN.\n---\n@color grey\nStatic. Cassel does\nnot answer.\n"
                   "He said he would not.");
        cur_ = pal::amber; return true;
    }
    if (cmd == "archive" || cmd == "card") {
        if (cardOkoro_) {
            cur_ = pal::grey;
            queueBeats("You already hold\nOkoro's card.\n@color grey\n/go to move on.");
            cur_ = pal::amber; return true;
        }
        queueFile("act1/argent/stow.txt");
        return true;
    }
    if (cmd == "stow" || cmd == "loader") {
        if (cardOkoro_) {
            cur_ = pal::grey; queueBeats("The loader is done.\nYou have the card.");
            cur_ = pal::amber; return true;
        }
        bool solved = runStow();          // blocking sub-loop; paints its own frames
        if (solved) {
            cardOkoro_ = true;
            cur_ = pal::white;
            queueBeats("@color white\nThe aisle clears. The loader arm swings.\n"
                       "---\n@color argent\nACCESS CARD lifted:\nOKORO, S.\n"
                       "---\n@color maru\nMARU\n@color white\nThat's it. /go to slot it and move on.");
            cur_ = pal::amber;
        } else {
            cur_ = pal::white;
            queueBeats("@color white\nYou step back from the loader.\n@color grey\n/stow to try again.");
            cur_ = pal::amber;
        }
        return true;
    }
    unknownHere();
    return true;
}

void Game::deckLook() {
    if      (deck_ == "helion") queueFile("act1/helion/arrive.txt");
    else if (deck_ == "argent") queueFile("act1/argent/arrive.txt");
    else { cur_ = pal::grey; queueBeats("nothing to see here\nyet."); cur_ = pal::amber; }
}

void Game::deckDir() {
    if (deck_ == "helion") {
        queueBeats("@color helion\nDOCUMENTS / H-2\n@color grey\n"
                   "log\n  Lang's engineering\n  logs\n"
                   "diary\n  scratched notes\n"
                   "---\n/open <name>\n/mail for letters");
    } else if (deck_ == "argent") {
        queueBeats("@color argent\nDOCUMENTS / A-5\n@color grey\n"
                   "ledger\n  ration issue log\n"
                   "notes\n  Emil's slips\n"
                   "---\n/open <name>\n/mail for letters");
    } else {
        cur_ = pal::grey; queueBeats("no documents here.");
    }
    cur_ = pal::amber;
}

void Game::deckOpen(const std::string& arg) {
    std::string a = toLower(trim(arg));
    if (a.empty()) { cur_ = pal::grey; queueBeats("open what?\ntry /dir."); cur_ = pal::amber; return; }
    if (deck_ == "helion") {
        if (a == "log" || a == "logs" || a == "lang")   { queueFile("act1/helion/lang_log.txt"); return; }
        if (a == "diary" || a == "junia")               { queueFile("act1/helion/junia_diary.txt"); return; }
    } else if (deck_ == "argent") {
        if (a == "ledger" || a == "rations" || a == "okoro") { queueFile("act1/argent/okoro_ledger.txt"); return; }
        if (a == "notes"  || a == "emil" || a == "bay")      { queueFile("act1/argent/emil.txt"); return; }
    }
    cur_ = pal::red;
    queueBeats("no document named\n'" + arg + "'.\ntry /dir.");
    cur_ = pal::amber;
}

void Game::deckMail() {
    if      (deck_ == "helion") queueFile("act1/helion/mail.txt");
    else if (deck_ == "argent") queueFile("act1/argent/mail.txt");
    else { cur_ = pal::grey; queueBeats("no mail here."); cur_ = pal::amber; }
}

void Game::helpDeck() {
    if (deck_ == "helion") {
        queueBeats("@color helion\nHELION / H-2\n@color grey\n"
                   "/look   the deck\n/dir    documents\n/open <name>\n"
                   "/mail   letters\n/power  load ledger\n/solve <n> answer\n"
                   "---\n/c <w> CANTOR\n/h <w> HALO-9\n"
                   "/notes  notebook\n/go     move on\n"
                   "---\nplain words are\nspoken on the\nchannel.");
    } else if (deck_ == "argent") {
        queueBeats("@color argent\nARGENT / A-5\n@color grey\n"
                   "/look   the deck\n/dir    documents\n/open <name>\n"
                   "/mail   letters\n/archive the card\n/stow   the loader\n"
                   "---\n/comms  hail crew\n/c <w> CANTOR\n/h <w> HALO-9\n"
                   "/notes  notebook\n/go     move on\n"
                   "---\nplain words are\nspoken on the\nchannel.");
    } else {
        helpStation();
    }
    cur_ = pal::amber;
}

void Game::deckStatus() {
    cur_ = pal::grey;
    std::string halo = haloHailed_ ? "STIRRING" : "?";
    std::string acc = (deck_ == "helion") ? "helion" : (deck_ == "argent") ? "argent" : "grey";
    queueBeats("@color " + acc + "\n" + deckTitle(deck_) +
               "\n@color grey\nnetwork   ONLINE\ncrew      NONE\n"
               "CANTOR    LISTENING\nHALO-9    " + halo);
    cur_ = pal::amber;
}

// Maru asks whether to move to the next sector. The gate is a placeholder until
// the real card/puzzle checks land: HELION needs the ledger seen, ARGENT the
// archive. If not ready, Maru says so instead of arming the prompt.
void Game::askGoOn() {
    bool ready = (deck_ == "helion") ? cardLang_
               : (deck_ == "argent") ? cardOkoro_
               : true;
    if (!ready) {
        cur_ = pal::white;
        if (deck_ == "helion")
            queueBeats("@color maru\nMARU\n@color white\nThe lift wants a card we don't have.\n"
                       "@color grey\nthat load ledger is nagging at me.\n(try /power, /solve)");
        else
            queueBeats("@color maru\nMARU\n@color white\nWe can't leave without Okoro's card.\n"
                       "@color grey\nit's in the archive.\n(try /archive, /stow)");
        cur_ = pal::amber;
        return;
    }
    pendingGo_ = true;
    cur_ = pal::white;
    queueBeats("@color maru\nMARU\n@color white\nGo on to the next sector?\n"
               "@color grey\nyes  /  not yet");
    cur_ = pal::amber;
}

// helion -> (lift) -> argent -> (wake) -> act1_done
void Game::advanceDeck() {
    if (deck_ == "helion") {
        queueFile("act1/helion/lift.txt");     // CANTOR greets the dead Dr. Lang
        arriveDeck("argent");
        return;
    }
    if (deck_ == "argent") {
        queueFile("act1/argent/wake.txt");     // power restored, HALO-9 wakes
        deck_ = "act1_done";
        p_->saveState("deck", deck_);
        cur_ = pal::grey;
        queueBeats("[ ACT I COMPLETE ]\n---\n@color amber\nAct II is not built\nyet. Your progress\nis saved.");
        cur_ = pal::amber;
        return;
    }
}

// Act II — sysadmin password recovery. The login is a dead end on purpose:
// Maru suggests the obvious duds, they fail, and the way in is the "forgot
// password" path — the security answers come from the sysadmin bio the player
// read earlier. Ends by having the player SET a personal admin password, which
// we persist under "adminpw" for reuse later in the game.
bool Game::runPasswordFlow(const std::string& petName, const std::string& childDob) {
    p_->clear();
    p_->setColor(pal::grey); p_->print("SYS TERMINAL\nadmin locked\n\n");
    int tries = 0;
    for (;;) {
        p_->setColor(pal::amber);
        std::string pw = trim(toLower(p_->readLine("password> ")));
        if (pw == "forgot" || pw == "reset" || pw == "?") break;
        if (pw == "q" || pw == "quit") return false;
        p_->setColor(pal::red); p_->print("access denied\n");
        if (++tries == 1) {
            p_->setColor(pal::maru);  p_->print("MARU\n");
            p_->setColor(pal::white); p_->print("try admin, or 1234.\n");
        } else {
            p_->setColor(pal::grey);  p_->print("[forgot password? type 'forgot']\n");
        }
    }
    for (;;) {                                   // security questions
        p_->clear();
        p_->setColor(pal::grey);  p_->print("RECOVERY\nsecurity check\n\n");
        p_->setColor(pal::amber);
        std::string pet = trim(toLower(p_->readLine("first pet> ")));
        std::string dob = digitsOnly(p_->readLine("child dob (ddmm)> "));
        if (pet == toLower(petName) && dob == digitsOnly(childDob)) break;
        p_->setColor(pal::red);  p_->print("\nmismatch.\n");
        p_->setColor(pal::grey); p_->print("it is in the file you read.\n");
        p_->delayMs(800);
    }
    std::string np;                              // player sets a personal password
    for (;;) {
        p_->clear();
        p_->setColor(pal::green); p_->print("VERIFIED\n\n");
        p_->setColor(pal::amber);
        np = trim(p_->readLine("set admin pw> "));
        if (np.size() >= 3) break;
        p_->setColor(pal::red); p_->print("min 3 chars.\n"); p_->delayMs(700);
    }
    p_->saveState("adminpw", np);
    p_->clear();
    p_->setColor(pal::grey);
    p_->print("ADMIN PW SET\n\nremember it. it opens\nadmin terminals from\nhere on.\n\n");
    p_->waitKey();
    return true;
}

// Blocking text playback: reuse the pager (render + advance) but drive it here
// with waitKey, so the quest orchestrator can put narrative between minigames.
void Game::playBeats(const std::string& text) {
    screens_.clear();
    typeIt_ = true;
    queueBeats(text);
    while (!screens_.empty()) {
        render();
        p_->waitKey();
        advance();
    }
}

void Game::playFile(const std::string& path) {
    std::string body;
    if (p_->loadFile(path, body)) playBeats(body);
    else playBeats("@color red\nmissing: " + path);
}

// The "Broken Terminal" set-piece (STORY_BIBLE §12). A linear stage machine:
// each stage saves "bt_stage" on success, so a reboot resumes; leaving a
// minigame (q) returns false WITHOUT advancing, so re-entry retries that stage.
// Narrative is placeholder for now — real content is authored later and slots
// straight into these playBeats() calls.
bool Game::runBrokenTerminal() {
    int stage = std::atoi(p_->loadState("bt_stage").c_str());
    // Themed decrypt board (COR VITAE / genetics); password answers below match
    // the sysadmin bio the player reads before the login.
    std::vector<std::string> hackWords =
        {"VIABLE","VESSEL","MARROW","GAMETE","ALLELE","SPLICE","FROZEN","DONORS"};

    if (stage < 1) {
        playBeats("@color white\nThe log on the screen is a bad one — the kind you read once and carry forever. You are halfway down it when a shadow falls over the glass.\n"
                  "@color cassel\nCASSEL\n@color white\nStop. Don't read that.\n"
                  "---\nWe are not meant to know this. It is too dangerous to carry home.\n"
                  "@color white\nHe drives his elbow into the screen. Sparks, and the log is gone.\n"
                  "---\nYou catch his arm on the backswing and put him down. He folds, muttering, and goes still.\n"
                  "@color grey\nHe was not wrong to be afraid. That is the part that frightens me.");
        stage = 1; p_->saveState("bt_stage", "1");
    }
    if (stage < 2) {
        playBeats("@color white\nThe casing is still warm. The drive sits behind four screws. Get it out.");
        if (!runQTE(p_, "UNSCREW", 3)) return false;
        playBeats("@color grey\nThe little disk comes loose in your palm.");
        stage = 2; p_->saveState("bt_stage", "2");
    }
    if (stage < 3) {
        playBeats("@color white\nCOR VITAE's dead terminals will not read it. You crawl the maintenance run looking for one with a pulse.");
        if (!runMaze(p_, defaultMaze())) return false;
        stage = 3; p_->saveState("bt_stage", "3");
    }
    if (stage < 4) {
        playBeats("@color white\nThe drive is sealed. Work the lock.");
        if (!runHack(p_, hackWords, 0)) return false;    // password = VIABLE
        playBeats("@color grey\nIt opens — and it is only half. The rest is on another unit, deeper in.");
        stage = 4; p_->saveState("bt_stage", "4");
    }
    if (stage < 5) {
        playBeats("@color white\nBack the way you came, past the one you started at, to the far terminal.");
        if (!runMaze(p_, defaultMaze())) return false;
        stage = 5; p_->saveState("bt_stage", "5");
    }
    if (stage < 6) {
        playBeats("@color white\nThis one is whole. You will have to open it to marry the two halves.");
        if (!runQTE(p_, "DISASSEMBLE", 3)) return false;
        stage = 6; p_->saveState("bt_stage", "6");
    }
    if (stage < 7) {
        playBeats("@color white\nYou seat the disk. It clicks home like it was always meant to be there.\n"
                  "---\nYou close the panel.");
        stage = 7; p_->saveState("bt_stage", "7");
    }
    if (stage < 8) {
        playBeats("@color white\nThe rebuilt unit asks for an admin login you do not have.");
        playFile("act2/corvitae/sysadmin_bio.txt");      // learn pet + child DOB
        if (!runPasswordFlow("argus", "1103")) return false;
        stage = 8; p_->saveState("bt_stage", "8");
    }
    if (stage < 9) {
        playFile("act2/corvitae/recovered.txt");         // the HALO-change revelation
        stage = 9; p_->saveState("bt_stage", "9");
    }
    return true;
}

// Did the player speak HALO-9's decoded line? ("CANTOR IS NOT ALONE IN HIS OWN
// VOICE") We match loosely on its load-bearing words so paraphrases still land.
bool Game::decodedLine(const std::string& raw) const {
    std::vector<std::string> t = tokenize(raw);
    bool alone = false, voice = false, notw = false;
    for (size_t i = 0; i < t.size(); ++i) {
        if (t[i] == "alone") alone = true;
        if (t[i] == "voice" || t[i] == "voices" || t[i] == "head") voice = true;
        if (t[i] == "not"   || t[i] == "another" || t[i] == "someone") notw = true;
    }
    return alone && (voice || notw);
}

// ------------------------------------------------------------------- STOW
// ARGENT archive puzzle: fill a 5x5 shelf grid so every row and column carries
// its rated crate count, without touching the pillar cells. Line-based input:
//   "c4"  toggle a cell    "ok" check    "?" help    "q" leave
// Returns true only when solved. The first crate the player places triggers the
// one-and-only PALE glitch: the panel registers a DIFFERENT cell for a beat.
bool Game::runStow() {
    const int N = 5;
    bool pillar[5][5] = {{false}};
    pillar[1][0] = true;   // B1
    pillar[2][2] = true;   // C3
    pillar[3][4] = true;   // D5
    const int rowT[5] = {3, 2, 2, 2, 3};
    const int colT[5] = {3, 2, 2, 2, 3};
    bool fill[5][5] = {{false}};
    // Status shown in the input prompt (the screen is only 8 rows: header +
    // 5 grid rows + column footer already fill 7, leaving the prompt row).
    std::string msg = "place a crate";

    auto rowSum = [&](int r){ int s=0; for(int c=0;c<N;c++) s+=fill[r][c]?1:0; return s; };
    auto colSum = [&](int c){ int s=0; for(int r=0;r<N;r++) s+=fill[r][c]?1:0; return s; };

    auto paint = [&]() {
        p_->clear();
        // 2-space indent so column numbers sit over the cells (rows begin "A ").
        p_->setColor(pal::grey); p_->print("  1 2 3 4 5\n");
        for (int r = 0; r < N; ++r) {
            p_->setColor(pal::grey);
            std::string head(1, (char)('A' + r)); head += " ";
            p_->print(head);
            for (int c = 0; c < N; ++c) {
                if      (pillar[r][c]) { p_->setColor(pal::red);   p_->print("X"); }
                else if (fill[r][c])   { p_->setColor(pal::green); p_->print("#"); }
                else                   { p_->setColor(pal::grey);  p_->print("."); }
                p_->print(" ");
            }
            int rs = rowSum(r);
            p_->setColor(rs == rowT[r] ? pal::green : pal::amber);
            p_->print("|"); p_->print(std::to_string(rowT[r])); p_->print("\n");
        }
        p_->setColor(pal::grey); p_->print("  ");
        for (int c = 0; c < N; ++c) {
            p_->setColor(colSum(c) == colT[c] ? pal::green : pal::amber);
            p_->print(std::to_string(colT[c])); p_->print(" ");
        }
        p_->print("\n");
    };

    for (;;) {
        paint();
        p_->setColor(pal::amber);
        std::string in = trim(toLower(p_->readLine(msg + "> ")));
        if (in == "q" || in == "quit" || in == "exit" || in == "bye") return false;
        if (in == "?" || in == "help") { msg = "c4 to place, ok"; continue; }
        if (in == "ok" || in == "done" || in == "check") {
            bool solved = true;
            for (int r = 0; r < N; ++r) if (rowSum(r) != rowT[r]) solved = false;
            for (int c = 0; c < N; ++c) if (colSum(c) != colT[c]) solved = false;
            if (solved) return true;
            msg = "not balanced yet.";
            continue;
        }
        // parse a cell reference like "c4" (row letter A-E, col digit 1-5)
        if (in.size() >= 2 && in[0] >= 'a' && in[0] <= 'e'
                           && in[1] >= '1' && in[1] <= '5') {
            int r = in[0] - 'a', c = in[1] - '1';
            if (pillar[r][c]) { p_->beep(); msg = "that's a pillar."; continue; }
            // PALE glitch: the first placement registers on a different cell,
            // for exactly one frame, then the panel "corrects" itself.
            if (!glitchFired_) {
                glitchFired_ = true;
                int gr = (r + 1) % N, gc = (c + 2) % N;
                if (!pillar[gr][gc]) {
                    fill[gr][gc] = !fill[gr][gc];   // a ghost cell, for one beat
                    paint(); p_->beep(); p_->delayMs(650);
                    fill[gr][gc] = !fill[gr][gc];   // revert the ghost cell
                }
                msg = "panel misread you.";
            }
            fill[r][c] = !fill[r][c];
            continue;
        }
        p_->beep(); msg = "try c4 or ok";
    }
}

void Game::helpStation() {
    queueBeats(
        "@color grey\nCOMMANDS\n@color amber\n"
        "/help     this list\n"
        "/status   the station\n"
        "/notes    notebook\n"
        "/c <words> CANTOR\n"
        "/h <words> HALO-9\n"
        "/version build id\n"
        "---\n@color amber\n"
        "*p <code> note code\n"
        "/save    /reset\n"
        "/shutdown reboot\n"
        "---\n@color grey\n"
        "plain words go to\nwhoever is on the\nchannel. /c and /h\nswitch, and stick.");
    cur_ = pal::amber;
}

void Game::cmdShutdown() {
    p_->clear(); p_->setColor(pal::grey); p_->print("\npowering down.\n");
    p_->delayMs(700); p_->reboot(); running_ = false;
}
void Game::cmdSave() {
    saveProgress(prologueDone_ ? "prologue_done" : "prologue");
    saveNotes();
    cur_ = pal::green; queueBeats("PROGRESS SAVED."); cur_ = pal::amber;
}
void Game::cmdReset() {
    p_->saveState("progress", "");
    p_->saveState("notes", "");
    p_->saveState("deck", "");
    p_->saveState("bt_stage", "");
    p_->clear(); p_->setColor(pal::grey); p_->print("\nsave erased.\nrebooting.\n");
    p_->delayMs(900); p_->reboot(); running_ = false;
}
void Game::saveProgress(const std::string& value) { p_->saveState("progress", value); }

void Game::cmdHelp() {
    cur_ = pal::amber;
    queueBeats(
        "@color grey\nCOMMANDS\n@color amber\n"
        "/help   this list\n"
        "/dir    list docs\n"
        "/open X open a doc\n"
        "/mail   your inbox\n"
        "/logs   ship log\n"
        "---\n@color amber\n"
        "/status ship + dock\n"
        "/unlock <code>\n"
        "/notes  notebook\n"
        "/c <words> speak\n"
        "/version build id\n"
        "*p <code> quick note\n"
        "---\n@color grey\n"
        "plain words go to\nwhoever is on the\nchannel. /c and /h\nswitch, and stick.");
    cur_ = pal::amber;
}

void Game::cmdDir() {
    queueBeats("@color grey\nDOCUMENTS\n@color amber\n"
        "briefing\n  dispatch orders\nroster\n  ferryman crew\n");
    cur_ = pal::amber;
}

void Game::cmdOpen(const std::string& arg) {
    std::string a = toLower(arg);
    if (a == "briefing") queueFile("prologue/briefing.txt");
    else if (a == "roster" || a == "vell" || a == "maru") queueFile("prologue/roster.txt");
    else if (a.empty()) { cur_ = pal::grey; queueBeats("open what?\ntry /dir."); cur_ = pal::amber; }
    else { cur_ = pal::red; queueBeats("no document named\n'" + arg + "'.\ntry /dir."); cur_ = pal::amber; }
}

void Game::cmdUnlock(const std::string& arg) {
    if (arg.empty()) { cur_ = pal::grey; queueBeats("unlock: give a code.\ne.g. /unlock 0000"); cur_ = pal::amber; return; }
    p_->beep();
    if (digitsOnly(arg) == "0412") { docked_ = true; }
    else {
        cur_ = pal::red;
        queueBeats("AUTH REJECTED\nclamps stay locked.\n---\n"
                   "hint: the code is a date.\nread your mail,\nthen the roster.");
        cur_ = pal::amber;
    }
}

// ------------------------------------------------------------------ dialogue
// Tokenize a sentence into lowercase word tokens (punctuation dropped).
static std::vector<std::string> tokenize(const std::string& s) {
    std::vector<std::string> out; std::string cur;
    for (char c : s) {
        if (std::isalnum((unsigned char)c)) cur += (char)std::tolower((unsigned char)c);
        else { if (!cur.empty()) { out.push_back(cur); cur.clear(); } }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}
static bool hasToken(const std::vector<std::string>& toks, const std::string& w) {
    for (const auto& t : toks) if (t == w) return true;
    return false;
}

// Dialogue file format (content/talk/<who>.txt):
//   ... intro beats (@color / --- like any content file) ...
//   @rules
//   ? crew | who died | the dead        <- trigger phrases, '|' separated
//   CANTOR: they stopped answering.      <- reply beats until next '?' / @fallback
//   ---
//   one by one.
//   ? reactor | core | fed
//   CANTOR: the core has been fed.
//   @fallback
//   CANTOR: i do not understand, but i am listening.
//   CANTOR: you keep circling that.
bool Game::loadDialogue(const std::string& path) {
    std::string body;
    if (!p_->loadFile(path, body)) return false;
    dlgIntro_.clear(); dlgRules_.clear(); dlgFallback_.clear();
    dlgAmbient_.clear(); fbIdx_ = 0; lastFb_ = (size_t)-1;
    dlgVoice_ = pal::cyan;

    std::istringstream in(body);
    std::string line;
    int section = 0;                 // 0 = intro, 1 = rules, 2 = fallback
    DlgRule cur; bool haveRule = false; std::string curReply;
    auto commit = [&]() {
        if (!haveRule) return;
        // a reply block may hold several variants separated by a "~~~" line
        std::istringstream rs(curReply);
        std::string ln, acc;
        while (std::getline(rs, ln)) {
            if (trim(ln) == "~~~") { if (!trim(acc).empty()) cur.replies.push_back(acc); acc.clear(); }
            else { acc += ln; acc += "\n"; }
        }
        if (!trim(acc).empty()) cur.replies.push_back(acc);
        dlgRules_.push_back(cur);
        cur = DlgRule(); curReply.clear(); haveRule = false;
    };

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::string t = trim(line);
        if (t.rfind("@voice", 0) == 0) { dlgVoice_ = colorByName(t.substr(6)); continue; }
        if (t == "@rules")    { section = 1; continue; }
        if (t == "@fallback") { commit(); section = 2; continue; }
        if (t == "@ambient")  { commit(); section = 3; continue; }
        if (t == "@end")      break;

        if (section == 0) { dlgIntro_ += line; dlgIntro_ += "\n"; }
        else if (section == 1) {
            if (!t.empty() && t[0] == '?') {
                commit();
                haveRule = true;
                std::string spec = t.substr(1);
                // split on '|'
                size_t start = 0;
                while (true) {
                    size_t bar = spec.find('|', start);
                    std::string alt = trim(spec.substr(start, bar == std::string::npos ? std::string::npos : bar - start));
                    if (!alt.empty()) cur.triggers.push_back(tokenize(alt));
                    if (bar == std::string::npos) break;
                    start = bar + 1;
                }
            } else if (haveRule) {
                curReply += line; curReply += "\n";
            }
        } else if (section == 2) {
            if (!t.empty()) dlgFallback_.push_back(t);
        } else {                       // ambient
            if (!t.empty()) dlgAmbient_.push_back(t);
        }
    }
    commit();
    if (dlgFallback_.empty()) dlgFallback_.push_back("CANTOR: ...");
    return true;
}

// Plain words are speech. If a channel is open the AI answers; if not, we say
// so in-fiction — and if the player typed a bare command name, we gently point
// out that orders need a slash.
bool Game::isKnownCommand(const std::string& c) {
    static const char* cmds[] = {"help","dir","open","mail","logs","status","unlock","version","build","c","h",
                                 "dock","notes","note","save","reset","shutdown",
                                 "quit","exit","clear","talk","bye",
                                 // Act I deck verbs
                                 "proceed","enter","begin","go","next","look",
                                 "diary","junia","lang","power","ledger","load",
                                 "rations","okoro","bay","emil","grain",
                                 "comms","ferryman","cassel","archive","stow","card",
                                 "solve","balance","loader"};
    for (size_t i = 0; i < sizeof(cmds)/sizeof(cmds[0]); ++i)
        if (c == cmds[i]) return true;
    return false;
}

void Game::closeChannel() {
    addressee_.clear();
    cur_ = pal::grey;
    queueBeats("[ channel closed ]\n@color grey\n/help for commands");
    cur_ = pal::amber;
}

void Game::handleSpeech(const std::string& raw) {
    std::string lc = toLower(raw);

    if (!addressee_.empty()) {
        if (lc == "bye" || lc == "goodbye" || lc == "leave") { closeChannel(); return; }
        // Bringing HALO-9's decoded truth to CANTOR: he sincerely denies it.
        // Only meaningful once the player has the cipher shift (Lang's card).
        if (addressee_ == "cantor" && cardLang_ && !cantorConfronted_ && decodedLine(raw)) {
            cantorConfronted_ = true;
            queueFile("act1/argent/cantor_reply.txt");
            return;
        }
        talkAnswer(raw);
        return;
    }

    std::string cmd, arg;
    splitCommand(raw, cmd, arg);
    if (isKnownCommand(cmd)) {
        cur_ = pal::grey;
        queueBeats("orders need a slash.\n---\n@color amber\n/" + cmd +
                   "\n@color grey\nplain words are\nspoken aloud.");
        cur_ = pal::amber;
        return;
    }
    cur_ = pal::grey;
    queueBeats("you say it aloud.\n---\nnothing on this\nchannel answers.\n"
               "---\n@color grey\n/talk cantor\nto open a channel");
    cur_ = pal::amber;
}


std::string Game::voiceName(const std::string& who) const {
    if (who == "cantor") return "CANTOR";
    if (who == "halo")   return "HALO-9";
    if (who.empty())     return "";
    std::string up = who;
    for (size_t i = 0; i < up.size(); ++i) up[i] = (char)std::toupper((unsigned char)up[i]);
    return up;
}

// Load a speaker's script once and keep it resident.
bool Game::ensureDialogue(const std::string& who) {
    if (dlgLoaded_ == who) return true;
    if (!loadDialogue("talk/" + who + ".txt")) return false;
    dlgLoaded_ = who;
    introShown_ = false;
    return true;
}

// "/c ..." / "/h ...": switch who you are addressing, then (optionally) speak.
// The switch sticks, so the next lines need no prefix.
void Game::speakTo(const std::string& who, const std::string& phrase) {
    if (!ensureDialogue(who)) {
        cur_ = pal::red;
        queueBeats("no channel for\n'" + voiceName(who) + "'.");
        cur_ = pal::amber;
        return;
    }
    bool switched = (addressee_ != who);
    addressee_ = who;
    if (switched && !introShown_) {
        introShown_ = true;
        cur_ = dlgVoice_;
        queueBeats(dlgIntro_);
        cur_ = pal::amber;
        if (trim(phrase).empty()) return;
    } else if (switched) {
        cur_ = pal::grey;
        queueBeats("[ " + voiceName(who) + " on the\n  channel ]");
        cur_ = pal::amber;
        if (trim(phrase).empty()) return;
    }
    if (!trim(phrase).empty()) talkAnswer(phrase);
}

// Walking up to a terminal decides who you are talking to by default.
void Game::arriveAt(const std::string& who) {
    nodeDefault_ = who;
    if (who.empty()) { addressee_.clear(); return; }
    if (ensureDialogue(who)) addressee_ = who;
}

void Game::talkTo(const std::string& who) {
    std::string id = toLower(trim(who));
    if (id.empty()) id = "cantor";
    if (!ensureDialogue(id)) {
        cur_ = pal::red;
        queueBeats("no answer from\n'" + id + "'.");
        cur_ = pal::amber;
        return;
    }
    addressee_ = id; introShown_ = true;
    cur_ = dlgVoice_;
    queueBeats(dlgIntro_);
    cur_ = pal::amber;
}

// Match the player's sentence to the best rule (longest matched phrase wins).

// Cheap deterministic PRNG (xorshift) — no <random> weight on the device.
unsigned Game::nextRand() {
    rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5;
    return rng_;
}

// The word the player seems to care about, so CANTOR can echo it back —
// the old ELIZA trick that makes a canned line feel like a reply.
static std::string salientWord(const std::vector<std::string>& toks) {
    static const char* stop[] = {"what","who","why","how","when","where","is","are","was",
        "were","the","a","an","you","your","yours","me","my","mine","do","did","does","done",
        "i","it","its","to","of","and","or","in","on","at","that","this","these","those",
        "can","could","would","will","shall","tell","about","know","knew","there","they",
        "them","their","he","she","we","us","for","with","but","not","no","yes","if","then",
        "have","has","had","been","be","am","get","got","say","said","think","really","just"};
    std::string best;
    for (size_t i = 0; i < toks.size(); ++i) {
        const std::string& t = toks[i];
        if (t.size() < 4) continue;
        bool isStop = false;
        for (size_t k = 0; k < sizeof(stop)/sizeof(stop[0]); ++k)
            if (t == stop[k]) { isStop = true; break; }
        if (isStop) continue;
        if (t.size() > best.size()) best = t;
    }
    return best;
}

// Pick a deflection: never the same one twice in a row, and prefer templates
// that can quote the player's own word ("%w") when we found one.
std::string Game::pickDeflection(const std::vector<std::string>& toks) {
    if (dlgFallback_.empty()) return "CANTOR: ...";
    std::string w = salientWord(toks);
    const size_t n = dlgFallback_.size();
    for (int tries = 0; tries < 16; ++tries) {
        size_t idx = nextRand() % n;
        if (n > 1 && idx == lastFb_) continue;
        std::string line = dlgFallback_[idx];
        size_t at = line.find("%w");
        if (at != std::string::npos) {
            if (w.empty()) continue;           // needs a word, we have none
            line = line.substr(0, at) + w + line.substr(at + 2);
        }
        lastFb_ = idx;
        return line;
    }
    for (size_t i = 0; i < n; ++i)
        if (dlgFallback_[i].find("%w") == std::string::npos) { lastFb_ = i; return dlgFallback_[i]; }
    return dlgFallback_[0];
}

void Game::talkAnswer(const std::string& question) {
    std::vector<std::string> toks = tokenize(question);
    int best = -1; size_t bestLen = 0;
    for (size_t r = 0; r < dlgRules_.size(); ++r) {
        for (const auto& phrase : dlgRules_[r].triggers) {
            if (phrase.empty()) continue;
            bool all = true;
            for (const auto& w : phrase) if (!hasToken(toks, w)) { all = false; break; }
            if (all && phrase.size() > bestLen) { bestLen = phrase.size(); best = (int)r; }
        }
    }
    cur_ = dlgVoice_;
    if (best >= 0) {
        DlgRule& r = dlgRules_[best];
        size_t pick = 0;
        if (r.replies.size() > 1) {
            for (int t = 0; t < 8; ++t) {
                pick = nextRand() % r.replies.size();
                if (pick != r.lastReply) break;
            }
        }
        r.lastReply = pick;
        queueBeats(r.replies.empty() ? std::string("CANTOR: ...") : r.replies[pick]);
    }
    else           queueBeats(pickDeflection(toks));

    // Sometimes he volunteers a thought nobody asked for. ONLY after he has
    // actually answered something: tacking a non-sequitur onto "I did not
    // understand you" just reads as broken. Rendered grey so it is visibly an
    // aside rather than a reply.
    if (best >= 0 && !dlgAmbient_.empty() && (nextRand() % 4) == 0) {
        cur_ = dlgVoice_;   // an aside, but still HIS voice — keep his colour
        queueBeats(dlgAmbient_[nextRand() % dlgAmbient_.size()]);
    }
    cur_ = pal::amber;
}

void Game::queueFinale() {
    screens_.clear();
    cur_ = pal::green;
    queueBeats("AUTH ACCEPTED\nreleasing docking clamps.");
    queueFile("prologue/docking.txt");
    queueFile("prologue/cantor.txt");
    cur_ = pal::grey;
    queueBeats("[PROLOGUE COMPLETE]\n---\nACT I:\nTHE QUIET DECKS\nawaits.");
    cinematic_ = true; finale_ = true; typeIt_ = true;
}

// End of the prologue. CANTOR just spoke to you, so his channel stays OPEN:
// the player's first instinct is to ask him something, and that should simply
// work. Plain words = speech, "/..." = orders, "bye" closes the channel.
void Game::finish() {
    saveProgress("prologue_done");
    prologueDone_ = true; finale_ = false; cinematic_ = false;

    arriveAt("cantor");           // his node: plain words reach him by default
    cur_ = pal::grey;
    queueBeats("@color grey\nprogress saved.\n"
               "@color cyan\nCANTOR: I am still here. Ask me.\n"
               "@color maru\nMARU\n@color white\nA deck lock is ahead. /proceed when you're ready.\n"
               "@color grey\njust type to speak\n/help for orders");
    cur_ = pal::amber; typeIt_ = true;
}

} // namespace kd
