// game.cpp — KODZIMIM engine. One thought per screen, prompt always at the bottom.
#include "game.h"
#include <cctype>
#include <sstream>

namespace kd {

// ---------------------------------------------------------------- utils
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
    if (s == "pale")  return pal::pale;
    if (s == "grey" || s == "gray") return pal::grey;
    return pal::amber;
}

Game::Game(IPlatform* platform) : p_(platform) {}

// A line break is "natural" after a pause.
static bool endsWithPause(const std::string& w) {
    if (w.empty()) return false;
    char c = w.back();
    return c=='.'||c==','||c==';'||c==':'||c=='!'||c=='?'||c=='-';
}

// Clause-aware, balanced word wrap: choose the breaks that minimise raggedness
// AND prefer to end a line after punctuation, so lines never stop mid-thought.
std::vector<std::string> Game::wrap(const std::string& text, int cols) {
    std::vector<std::string> out;
    std::istringstream in(text);
    std::string raw;
    while (std::getline(in, raw)) {
        if ((int)raw.size() <= cols) { out.push_back(raw); continue; }

        std::vector<std::string> w;
        {
            std::istringstream ls(raw);
            std::string t;
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
                else {
                    long long slack = cols - len;
                    cost = slack * slack;
                    if (!endsWithPause(w[j])) cost += BREAK_PENALTY;
                }
                if (dp[j + 1] < INF && cost + dp[j + 1] < dp[i]) {
                    dp[i] = cost + dp[j + 1];
                    nxt[i] = j + 1;
                }
            }
        }
        for (int i = 0; i < n; ) {
            int j = nxt[i];
            std::string line = w[i];
            for (int k = i + 1; k < j; ++k) line += " " + w[k];
            out.push_back(line);
            i = j;
        }
    }
    return out;
}

// Content -> beats ("---" separated) -> screens (<= contentRows lines each).
// "@color <name>" switches color; it may appear INSIDE a beat, so a grey
// header and an amber body can live on the same screen.
void Game::queueBeats(const std::string& text) {
    Screen sc = p_->screen();
    const int contentRows = sc.rows - 2;

    ScreenBuf beat;
    auto flush = [&]() {
        if (beat.empty()) return;
        ScreenBuf wrapped;
        for (const auto& cl : beat) {
            for (const auto& w : wrap(cl.text, sc.cols))
                wrapped.push_back(CLine{cl.color, w});
        }
        for (size_t i = 0; i < wrapped.size(); ) {
            ScreenBuf s;
            for (int n = 0; n < contentRows && i < wrapped.size(); ++n)
                s.push_back(wrapped[i++]);
            screens_.push_back(s);
        }
        beat.clear();
    };

    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();   // CRLF safety
        std::string t = trim(line);
        if (t == "---") { flush(); continue; }
        if (t.rfind("@color", 0) == 0) { cur_ = colorByName(t.substr(6)); continue; }
        beat.push_back(CLine{cur_, line});
    }
    flush();
}

void Game::queueFile(const std::string& path) {
    std::string body;
    if (!p_->loadFile(path, body)) {
        cur_ = pal::red;
        queueBeats("missing file:\n" + path);
        cur_ = pal::amber;
        return;
    }
    queueBeats(body);
}

// Draw the current screen and the hint row, then leave the cursor on the
// prompt row — so the command line is ALWAYS visible and you can type at
// any moment (e.g. read `dir`, then type `open briefing` right away).
void Game::render() {
    Screen sc = p_->screen();
    const int contentRows = sc.rows - 2;

    p_->clear();
    int used = 0;
    if (!screens_.empty()) {
        for (const auto& cl : screens_.front()) {
            p_->setColor(cl.color);
            if (typeIt_) p_->typeOut(cl.text); else p_->print(cl.text);
            p_->print("\n");
            used++;
        }
    }
    for (int i = used; i < contentRows; ++i) p_->print("\n");

    p_->setColor(pal::grey);
    if (screens_.size() > 1)   p_->print("[enter] more");
    else if (screens_.empty()) p_->print("type 'help'");
    p_->print("\n");

    typeIt_ = false;   // re-draws of the same screen are instant, not re-typed
}

void Game::advance() {
    if (!screens_.empty()) screens_.erase(screens_.begin());
    typeIt_ = true;
}

// ------------------------------------------------------------------ flow
void Game::run() {
    cur_ = pal::amber;
    p_->beep();
    typeIt_ = true;

    // Resume: progress is autosaved at every checkpoint.
    if (p_->loadState("progress") == "prologue_done") {
        prologueDone_ = true;
        cinematic_    = false;
        cur_ = pal::grey;
        queueBeats("SAVE FOUND\nprologue complete.\n"
                   "---\n"
                   "@color amber\nYou are aboard\nMERIDIAN DEEP.\n"
                   "@color grey\ntype 'help'\n'reset' starts over");
        cur_ = pal::amber;
    } else {
        cinematic_ = true;
        queueFile("prologue/boot.txt");
    }

    while (running_) {
        render();
        p_->setColor(pal::amber);
        std::string in  = p_->readLine("> ");
        std::string cmd = trim(in);

        // During boot / finale, any input just moves the story forward.
        if (cinematic_) {
            advance();
            if (screens_.empty()) {
                cinematic_ = false;
                if (finale_) finish();
            }
            continue;
        }

        // Empty line = page forward.
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

    // Always available, in any state.
    if (cmd == "shutdown" || cmd == "quit" || cmd == "exit") { cmdShutdown(); return true; }
    if (cmd == "save")  { cmdSave();  return true; }
    if (cmd == "reset") { cmdReset(); return true; }

    // After the prologue you are on the station, not on the ship.
    if (prologueDone_) return handleStationCommand(cmd, arg);

    if      (cmd == "help")   cmdHelp();
    else if (cmd == "dir")    cmdDir();
    else if (cmd == "open")   cmdOpen(arg);
    else if (cmd == "mail")   queueFile("prologue/mail.txt");
    else if (cmd == "logs")   queueFile("prologue/shiplog.txt");
    else if (cmd == "status") queueFile("prologue/status.txt");
    else if (cmd == "unlock" || cmd == "dock") cmdUnlock(arg);
    else if (cmd == "clear")  { /* screens_ already cleared */ }
    else {
        cur_ = pal::red;
        queueBeats("unknown directive.\nthis system does not\nremember that word.");
        cur_ = pal::amber;
    }
    return true;
}

// ---- station mode: the prologue is over, but you are never stuck ----------
bool Game::handleStationCommand(const std::string& cmd, const std::string& arg) {
    (void)arg;
    if (cmd == "help") { helpStation(); return true; }
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
        "@color grey\n"
        "COMMANDS\n"
        "@color amber\n"
        "help     this list\n"
        "status   the station\n"
        "save     save now\n"
        "reset    start over\n"
        "shutdown reboot\n");
    cur_ = pal::amber;
}

// ---- always-available commands -------------------------------------------
void Game::cmdShutdown() {
    p_->clear();
    p_->setColor(pal::grey);
    p_->print("\npowering down.\n");
    p_->delayMs(700);
    p_->reboot();          // on the Cardputer this drops you back to M5Launcher
    running_ = false;      // desktop / safety net
}

void Game::cmdSave() {
    saveProgress(prologueDone_ ? "prologue_done" : "prologue");
    cur_ = pal::green;
    queueBeats("PROGRESS SAVED.");
    cur_ = pal::amber;
}

void Game::cmdReset() {
    p_->saveState("progress", "");
    p_->clear();
    p_->setColor(pal::grey);
    p_->print("\nsave erased.\nrebooting.\n");
    p_->delayMs(900);
    p_->reboot();
    running_ = false;
}

void Game::saveProgress(const std::string& value) {
    p_->saveState("progress", value);
}

void Game::cmdHelp() {
    cur_ = pal::amber;
    queueBeats(
        "@color grey\n"
        "COMMANDS\n"
        "@color amber\n"
        "help   this list\n"
        "dir    list docs\n"
        "open X open a doc\n"
        "mail   your inbox\n"
        "logs   ship log\n"
        "---\n"
        "@color amber\n"
        "status ship + dock\n"
        "unlock <code>\n"
        "save   save now\n"
        "shutdown reboot\n");
    cur_ = pal::amber;
}

void Game::cmdDir() {
    queueBeats(
        "@color grey\n"
        "DOCUMENTS\n"
        "@color amber\n"
        "briefing\n"
        "  dispatch orders\n"
        "roster\n"
        "  ferryman crew\n");
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
    if (arg.empty()) {
        cur_ = pal::grey;
        queueBeats("unlock: give a code.\ne.g. unlock 0000");
        cur_ = pal::amber;
        return;
    }
    p_->beep();
    // Docking clamp auth = Vell Maru's birthday, 04-12 -> "0412".
    // Cross-referenced in mail (the hint) and roster (the date).
    if (digitsOnly(arg) == "0412") {
        docked_ = true;
    } else {
        cur_ = pal::red;
        queueBeats("AUTH REJECTED\nclamps stay locked.\n"
                   "---\n"
                   "hint: the code is a date.\n"
                   "read your mail,\nthen the roster.");
        cur_ = pal::amber;
    }
}

void Game::queueFinale() {
    screens_.clear();
    cur_ = pal::green;
    queueBeats("AUTH ACCEPTED\nreleasing docking clamps.");
    queueFile("prologue/docking.txt");
    queueFile("prologue/cantor.txt");
    cur_ = pal::grey;
    queueBeats("[PROLOGUE COMPLETE]\n---\nACT I:\nTHE QUIET DECKS\nawaits.");
    cinematic_ = true;
    finale_    = true;
    typeIt_    = true;
}

// End of the prologue. Autosave, and drop the player into station mode —
// they can still type commands, save, reset or reboot. Never a dead end.
void Game::finish() {
    saveProgress("prologue_done");
    prologueDone_ = true;
    finale_       = false;
    cinematic_    = false;
    cur_ = pal::grey;
    queueBeats("progress saved.\n@color amber\ntype 'help'\n'shutdown' reboots");
    cur_ = pal::amber;
    typeIt_ = true;
}

} // namespace kd
