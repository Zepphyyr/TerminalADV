// game.cpp — KODZIMIM engine. One thought per screen, prompt always at the bottom,
// plus a persistent player notebook with IDE-style keyword highlighting.
#include "game.h"
#include <cctype>
#include <sstream>

namespace kd {

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
Color colorByName(const std::string& n) {
    std::string s = toLower(trim(n));
    if (s == "white") return pal::white;
    if (s == "cyan")  return pal::cyan;
    if (s == "green") return pal::green;
    if (s == "red")   return pal::red;
    if (s == "blue")  return pal::blue;
    if (s == "pale")  return pal::pale;
    if (s == "grey" || s == "gray") return pal::grey;
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
        if (t.rfind("@color", 0) == 0) { cur_ = colorByName(t.substr(6)); continue; }
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
    p_->setColor(pal::grey);
    if (screens_.size() > 1)   p_->print("[enter] more");
    else if (screens_.empty()) p_->print("type 'help'");
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
        lines.push_back(CLine{pal::grey, "add:  note <text>", false});
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
        cur_ = pal::grey;
        queueBeats("SAVE FOUND\nprologue complete.\n"
                   "---\n"
                   "@color amber\nYou are aboard\nMERIDIAN DEEP.\n"
                   "@color cyan\ntalk cantor\n"
                   "@color grey\nhelp / reset");
        cur_ = pal::amber;
    } else {
        cinematic_ = true;
        queueFile("prologue/boot.txt");
    }

    while (running_) {
        render();
        p_->setColor(pal::amber);
        std::string in  = p_->readLine(inTalk_ ? "you> " : "> ");
        std::string cmd = trim(in);

        if (cinematic_) {
            advance();
            if (screens_.empty()) { cinematic_ = false; if (finale_) finish(); }
            continue;
        }

        // conversation mode: empty = read on; a sentence = a question; bye = leave
        if (inTalk_) {
            if (cmd.empty()) { advance(); continue; }
            std::string lc = toLower(cmd);
            if (lc == "bye" || lc == "back" || lc == "leave" || lc == "exit" || lc == "q") {
                inTalk_ = false;
                screens_.clear(); typeIt_ = true;
                cur_ = pal::grey; queueBeats("[ channel closed ]"); cur_ = pal::amber;
                continue;
            }
            screens_.clear(); typeIt_ = true;
            talkAnswer(cmd);
            continue;
        }

        if (cmd.empty()) { advance(); continue; }

        screens_.clear();
        typeIt_ = true;
        if (!handleCommand(cmd)) break;
        if (docked_ && !finale_ && !prologueDone_) queueFinale();
    }
}

bool Game::handleCommand(const std::string& raw) {
    std::string cmd, arg;
    splitCommand(raw, cmd, arg);
    if (cmd == "o") cmd = "open";
    if (cmd == "h" || cmd == "?") cmd = "help";
    if (cmd == "l") cmd = "logs";
    if (cmd == "n") cmd = "notes";

    if (!raw.empty() && raw[0] == '*') { addNote(raw); return true; }
    if (cmd == "notes" || cmd == "note") {
        if (arg == "clear") clearNotes();
        else if (arg.empty()) showNotes();
        else addNote(arg);
        return true;
    }
    if (cmd == "shutdown" || cmd == "quit" || cmd == "exit") { cmdShutdown(); return true; }
    if (cmd == "save")  { cmdSave();  return true; }
    if (cmd == "reset") { cmdReset(); return true; }

    if (prologueDone_) return handleStationCommand(cmd, arg);

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
    (void)arg;
    if (cmd == "help") { helpStation(); return true; }
    if (cmd == "talk") { talkTo(arg.empty() ? "cantor" : arg); return true; }
    if (cmd == "status") {
        cur_ = pal::grey;
        queueBeats("MERIDIAN DEEP\n@color amber\nnetwork    ONLINE\ncrew       NONE\n"
                   "CANTOR     LISTENING\nHALO-9     ?");
        cur_ = pal::amber;
        return true;
    }
    if (cmd == "logs" || cmd == "mail" || cmd == "dir" || cmd == "open") {
        cur_ = pal::grey;
        queueBeats("FERRYMAN is behind\nthe airlock now.\n"
                   "---\nThe station's own\nterminals are not\nopen to you yet.\n"
                   "---\n@color amber\nACT I:\nTHE QUIET DECKS\nis not built yet.");
        cur_ = pal::amber;
        return true;
    }
    cur_ = pal::red;
    queueBeats("unknown directive.\nthis system does not\nremember that word.");
    cur_ = pal::amber;
    return true;
}

void Game::helpStation() {
    queueBeats(
        "@color grey\nCOMMANDS\n@color amber\n"
        "help     this list\n"
        "status   the station\n"
        "talk cantor  speak\n"
        "notes    notebook\n"
        "---\n@color amber\n"
        "*p <code> note code\n"
        "save     save now\n"
        "reset    start over\n"
        "shutdown reboot\n");
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
    p_->clear(); p_->setColor(pal::grey); p_->print("\nsave erased.\nrebooting.\n");
    p_->delayMs(900); p_->reboot(); running_ = false;
}
void Game::saveProgress(const std::string& value) { p_->saveState("progress", value); }

void Game::cmdHelp() {
    cur_ = pal::amber;
    queueBeats(
        "@color grey\nCOMMANDS\n@color amber\n"
        "help   this list\n"
        "dir    list docs\n"
        "open X open a doc\n"
        "mail   your inbox\n"
        "logs   ship log\n"
        "---\n@color amber\n"
        "status ship + dock\n"
        "unlock <code>\n"
        "notes  notebook\n"
        "*p <code> quick note\n"
        "save   shutdown");
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
    else if (a.empty()) { cur_ = pal::grey; queueBeats("open what?\ntry 'dir'."); cur_ = pal::amber; }
    else { cur_ = pal::red; queueBeats("no document named\n'" + arg + "'.\ntry 'dir'."); cur_ = pal::amber; }
}

void Game::cmdUnlock(const std::string& arg) {
    if (arg.empty()) { cur_ = pal::grey; queueBeats("unlock: give a code.\ne.g. unlock 0000"); cur_ = pal::amber; return; }
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
    dlgIntro_.clear(); dlgRules_.clear(); dlgFallback_.clear(); fbIdx_ = 0;

    std::istringstream in(body);
    std::string line;
    int section = 0;                 // 0 = intro, 1 = rules, 2 = fallback
    DlgRule cur; bool haveRule = false;
    auto commit = [&]() { if (haveRule) { dlgRules_.push_back(cur); cur = DlgRule(); haveRule = false; } };

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::string t = trim(line);
        if (t == "@rules")    { section = 1; continue; }
        if (t == "@fallback") { commit(); section = 2; continue; }
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
                cur.reply += line; cur.reply += "\n";
            }
        } else { // fallback
            if (!t.empty()) dlgFallback_.push_back(t);
        }
    }
    commit();
    if (dlgFallback_.empty()) dlgFallback_.push_back("CANTOR: ...");
    return true;
}

void Game::talkTo(const std::string& who) {
    std::string id = toLower(trim(who));
    if (id.empty()) id = "cantor";
    if (!loadDialogue("talk/" + id + ".txt")) {
        cur_ = pal::red;
        queueBeats("no answer from\n'" + id + "'.");
        cur_ = pal::amber;
        return;
    }
    inTalk_ = true;
    cur_ = pal::cyan;
    queueBeats(dlgIntro_);              // the AI speaks first: the big picture
    cur_ = pal::amber;
}

// Match the player's sentence to the best rule (longest matched phrase wins).
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
    cur_ = pal::cyan;
    if (best >= 0) {
        queueBeats(dlgRules_[best].reply);
    } else {
        // ELIZA-style deflection, cycling so repeats vary.
        queueBeats(dlgFallback_[fbIdx_ % dlgFallback_.size()]);
        fbIdx_++;
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

void Game::finish() {
    saveProgress("prologue_done");
    prologueDone_ = true; finale_ = false; cinematic_ = false;
    cur_ = pal::grey;
    queueBeats("progress saved.\n"
               "@color cyan\nCANTOR is still\nlistening.\n"
               "---\n@color amber\ntalk cantor\n"
               "@color grey\n  ask him anything\n"
               "help / shutdown");
    cur_ = pal::amber; typeIt_ = true;
}

} // namespace kd
