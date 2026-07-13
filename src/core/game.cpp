// game.cpp — KODZIMIM engine. Prologue, rendered one thought per screen.
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

// ------------------------------------------------------------------ Game
Game::Game(IPlatform* platform) : p_(platform) {}

// A line break is "natural" after a pause.
static bool endsWithPause(const std::string& w) {
    if (w.empty()) return false;
    char c = w.back();
    return c=='.'||c==','||c==';'||c==':'||c=='!'||c=='?'||c=='-';
}

// Clause-aware, balanced word wrap.
// A plain greedy fill breaks sentences at arbitrary points ("Final /
// approach."). This chooses the set of line breaks that minimises raggedness
// AND prefers to break after punctuation, so a line ends on a natural pause
// instead of mid-thought. (Knuth-style DP; the text is tiny, so this is free.)
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
                while ((int)t.size() > cols) {          // hard-split giant words
                    w.push_back(t.substr(0, cols));
                    t = t.substr(cols);
                }
                if (!t.empty()) w.push_back(t);
            }
        }
        const int n = (int)w.size();
        if (n == 0) { out.push_back(""); continue; }

        const long long INF = 1000000000000LL;
        const long long BREAK_PENALTY = 60;   // cost of ending a line mid-clause
        std::vector<long long> dp(n + 1, INF);
        std::vector<int> nxt(n + 1, n);
        dp[n] = 0;
        for (int i = n - 1; i >= 0; --i) {
            int len = 0;
            for (int j = i; j < n; ++j) {
                len += (int)w[j].size() + (j > i ? 1 : 0);
                if (len > cols) break;
                long long cost;
                if (j == n - 1) {
                    cost = 0;                              // last line: no penalty
                } else {
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

// One beat = one screen. If it does not fit, it is split into more screens.
void Game::renderBeat(const std::vector<std::string>& lines, Color c) {
    Screen sc = p_->screen();
    std::vector<std::string> wrapped;
    for (const auto& l : lines) {
        auto w = wrap(l, sc.cols);
        wrapped.insert(wrapped.end(), w.begin(), w.end());
    }
    if (wrapped.empty()) return;

    const int perScreen = sc.rows - 2;   // keep a row for the "press key" hint
    size_t i = 0;
    while (i < wrapped.size()) {
        p_->clear();
        p_->setColor(c);
        int n = 0;
        while (i < wrapped.size() && n < perScreen) {
            p_->typeOut(wrapped[i++]);
            p_->print("\n");
            n++;
        }
        for (; n < perScreen; ++n) p_->print("\n");
        p_->setColor(pal::grey);
        p_->print("[ press any key ]");
        p_->waitKey();
        p_->setColor(c);
    }
}

// Parse "---" separated beats and "@color <name>" directives, then render.
void Game::showBeats(const std::string& text) {
    std::istringstream in(text);
    std::string line;
    std::vector<std::string> beat;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();  // CRLF safety
        std::string t = trim(line);
        if (t == "---") { renderBeat(beat, cur_); beat.clear(); continue; }
        if (t.rfind("@color", 0) == 0) { cur_ = colorByName(t.substr(6)); continue; }
        beat.push_back(line);
    }
    renderBeat(beat, cur_);
}

void Game::showFile(const std::string& path) {
    std::string body;
    if (!p_->loadFile(path, body)) {
        p_->setColor(pal::red);
        p_->print("missing: " + path + "\n");
        p_->waitKey();
        return;
    }
    showBeats(body);
}

// ------------------------------------------------------------------ flow
void Game::run() {
    bootPrologue();
    if (running_) shipLoop();
    if (docked_) { dockingSequence(); endPrologue(); }
}

void Game::bootPrologue() {
    cur_ = pal::amber;
    p_->beep();
    showFile("prologue/boot.txt");
}

void Game::shipLoop() {
    while (running_ && !docked_) {
        p_->clear();
        p_->setColor(pal::grey);
        p_->print("type 'help'\n\n");
        p_->setColor(pal::amber);
        std::string input = p_->readLine("> ");
        if (!handleCommand(input)) break;
    }
}

bool Game::handleCommand(const std::string& raw) {
    std::string cmd, arg;
    splitCommand(raw, cmd, arg);
    if (cmd.empty()) return true;
    if (cmd == "o") cmd = "open";
    if (cmd == "h" || cmd == "?") cmd = "help";
    if (cmd == "l") cmd = "logs";

    if      (cmd == "help")   cmdHelp();
    else if (cmd == "dir")    cmdDir();
    else if (cmd == "open")   cmdOpen(arg);
    else if (cmd == "mail")   showFile("prologue/mail.txt");
    else if (cmd == "logs")   showFile("prologue/shiplog.txt");
    else if (cmd == "status") showFile("prologue/status.txt");
    else if (cmd == "unlock" || cmd == "dock") cmdUnlock(arg);
    else if (cmd == "clear")  { /* the loop clears anyway */ }
    else if (cmd == "shutdown" || cmd == "quit" || cmd == "exit") {
        cur_ = pal::grey;
        showBeats("powering down local console.");
        running_ = false;
        return false;
    } else {
        cur_ = pal::red;
        showBeats("unknown directive.\n---\nthis system does not remember that word.");
        cur_ = pal::amber;
    }
    return true;
}

void Game::cmdHelp() {
    showBeats(
        "@color grey\n"
        "COMMANDS\n"
        "--------------------\n"
        "---\n"
        "@color amber\n"
        "help   this list\n"
        "dir    list docs\n"
        "open X open a doc\n"
        "mail   your inbox\n"
        "---\n"
        "logs   ship log\n"
        "status ship + dock\n"
        "unlock <code>\n"
        "shutdown\n");
    cur_ = pal::amber;
}

void Game::cmdDir() {
    showBeats(
        "@color grey\n"
        "LOCAL DOCUMENTS\n"
        "--------------------\n"
        "---\n"
        "@color amber\n"
        "briefing\n"
        "  dispatch orders\n"
        "roster\n"
        "  ferryman crew\n"
        "---\n"
        "@color grey\n"
        "use: open <name>\n"
        "mail and logs are\n"
        "separate commands\n");
    cur_ = pal::amber;
}

void Game::cmdOpen(const std::string& arg) {
    std::string a = toLower(arg);
    if (a == "briefing") showFile("prologue/briefing.txt");
    else if (a == "roster" || a == "vell" || a == "maru") showFile("prologue/roster.txt");
    else if (a.empty()) { cur_ = pal::grey; showBeats("open what?\ntry 'dir'."); cur_ = pal::amber; }
    else { cur_ = pal::red; showBeats("no document named\n'" + arg + "'.\n---\ntry 'dir'."); cur_ = pal::amber; }
}

void Game::cmdUnlock(const std::string& arg) {
    if (arg.empty()) {
        cur_ = pal::grey;
        showBeats("unlock: give a code.\ne.g. unlock 0000");
        cur_ = pal::amber;
        return;
    }
    p_->beep();
    // Docking clamp auth = Vell Maru's birthday, 04-12 -> "0412".
    // Cross-referenced in mail (the hint) and roster (the date).
    if (digitsOnly(arg) == "0412") {
        cur_ = pal::green;
        showBeats("AUTH ACCEPTED\n---\nreleasing docking clamps.");
        docked_ = true;
    } else {
        cur_ = pal::red;
        showBeats("AUTH REJECTED\nclamps stay locked.\n"
                  "---\n"
                  "hint: the code is a date.\n---\nread your mail,\nthen the roster.");
        cur_ = pal::amber;
    }
}

void Game::dockingSequence() {
    showFile("prologue/docking.txt");
    p_->delayMs(300);
    showFile("prologue/cantor.txt");
}

void Game::endPrologue() {
    cur_ = pal::grey;
    showBeats("[ PROLOGUE\n  COMPLETE ]\n---\nprogress saved.\n---\nACT I:\nTHE QUIET DECKS\nawaits.");
    p_->saveState("progress", "prologue_done");
}

} // namespace kd
