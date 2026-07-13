// game.cpp
// KODZIMIM core engine implementation. Prologue MVP.
#include "game.h"
#include <cctype>
#include <sstream>

namespace kd {

// ----------------------------------------------------------------------------
// string utilities
// ----------------------------------------------------------------------------
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
    if (sp == std::string::npos) {
        cmd = toLower(t);
        arg = "";
    } else {
        cmd = toLower(t.substr(0, sp));
        arg = trim(t.substr(sp + 1));
    }
}

// keep only digits (for lenient code entry: "04-12" == "0412")
static std::string digitsOnly(const std::string& s) {
    std::string r;
    for (char c : s) if (std::isdigit((unsigned char)c)) r += c;
    return r;
}

// ----------------------------------------------------------------------------
// Game
// ----------------------------------------------------------------------------
Game::Game(IPlatform* platform) : p_(platform) {}

void Game::nl() { p_->print("\n"); }
void Game::line(const std::string& s) { p_->print(s); p_->print("\n"); }
void Game::say(const std::string& s) { p_->typeOut(s); p_->print("\n"); }

std::vector<std::string> Game::wrap(const std::string& text, int cols) {
    std::vector<std::string> out;
    std::istringstream in(text);
    std::string raw;
    // Preserve explicit newlines; wrap each source line to `cols`.
    while (std::getline(in, raw)) {
        if ((int)raw.size() <= cols) { out.push_back(raw); continue; }
        std::string cur;
        std::istringstream ls(raw);
        std::string word;
        while (ls >> word) {
            if (cur.empty()) {
                cur = word;
            } else if ((int)(cur.size() + 1 + word.size()) <= cols) {
                cur += " " + word;
            } else {
                out.push_back(cur);
                cur = word;
            }
            // hard-break very long words
            while ((int)cur.size() > cols) {
                out.push_back(cur.substr(0, cols));
                cur = cur.substr(cols);
            }
        }
        out.push_back(cur);
    }
    return out;
}

void Game::page(const std::string& text) {
    Screen sc = p_->screen();
    std::vector<std::string> lines = wrap(text, sc.cols);
    int shown = 0;
    const int perPage = sc.rows - 2; // leave room for the more-prompt
    for (size_t i = 0; i < lines.size(); ++i) {
        p_->typeOut(lines[i]);
        p_->print("\n");
        if (++shown >= perPage && i + 1 < lines.size()) {
            p_->print("-- more -- (press any key)");
            p_->waitKey();
            p_->print("\n");
            shown = 0;
        }
    }
}

void Game::showFile(const std::string& path) {
    std::string body;
    if (!p_->loadFile(path, body)) {
        line("> file not found: " + path);
        return;
    }
    page(body);
}

// ----------------------------------------------------------------------------
// flow
// ----------------------------------------------------------------------------
void Game::run() {
    bootPrologue();
    if (running_) shipLoop();
    if (docked_) {
        dockingSequence();
        endPrologue();
    }
}

void Game::bootPrologue() {
    p_->clear();
    p_->beep();
    showFile("prologue/boot.txt");
    nl();
    line("> type 'help' for available directives.");
    nl();
}

void Game::shipLoop() {
    // The player is on their own ORPHEUS console aboard the tug FERRYMAN.
    // Goal: read the logs, find the docking auth code, unlock the clamps.
    while (running_ && !docked_) {
        std::string input = p_->readLine("FERRYMAN> ");
        if (!handleCommand(input)) break;
    }
}

bool Game::handleCommand(const std::string& raw) {
    std::string cmd, arg;
    splitCommand(raw, cmd, arg);
    if (cmd.empty()) return true;

    // one-letter shortcuts
    if (cmd == "o") cmd = "open";
    if (cmd == "h" || cmd == "?") cmd = "help";
    if (cmd == "l") cmd = "logs";

    if (cmd == "help")      { cmdHelp(); }
    else if (cmd == "dir")  { cmdDir(); }
    else if (cmd == "open") { cmdOpen(arg); }
    else if (cmd == "mail") { cmdMail(); }
    else if (cmd == "logs") { cmdLogs(); }
    else if (cmd == "status"){ cmdStatus(); }
    else if (cmd == "unlock" || cmd == "dock") { cmdUnlock(arg); }
    else if (cmd == "clear"){ p_->clear(); }
    else if (cmd == "shutdown" || cmd == "quit" || cmd == "exit") {
        say("> powering down local console.");
        running_ = false;
        return false;
    } else {
        line("> unknown directive. this system does not remember that word.");
    }
    return true;
}

void Game::cmdHelp() {
    line("> AVAILABLE DIRECTIVES:");
    line("    help    - this list");
    line("    dir     - list local documents");
    line("    open X  - open a document by name");
    line("    mail    - read your inbox");
    line("    logs    - ship system log");
    line("    status  - ship & dock status");
    line("    unlock C- enter a docking auth code");
    line("    clear   - clear the screen");
    line("    shutdown- power down console");
}

void Game::cmdDir() {
    line("> LOCAL DOCUMENTS:");
    line("    briefing   mission briefing (dispatch)");
    line("    roster     ferryman crew roster");
    line("  (use 'open <name>' to read; 'mail' and 'logs' are separate)");
}

void Game::cmdOpen(const std::string& arg) {
    std::string a = toLower(arg);
    if (a == "briefing" || a == "briefing.txt") showFile("prologue/briefing.txt");
    else if (a == "roster" || a == "roster.txt") showFile("prologue/roster.txt");
    else if (a == "vell" || a == "maru")         showFile("prologue/roster.txt");
    else if (a.empty()) line("> open what? try 'dir'.");
    else line("> no document named '" + arg + "'. try 'dir'.");
}

void Game::cmdMail()   { showFile("prologue/mail.txt"); }
void Game::cmdLogs()   { showFile("prologue/shiplog.txt"); }
void Game::cmdStatus() { showFile("prologue/status.txt"); }

void Game::cmdUnlock(const std::string& arg) {
    if (arg.empty()) { line("> unlock: provide a code. e.g. 'unlock 0000'"); return; }
    // Docking clamp auth = crewmate Vell Maru's birthday: 04-12 -> "0412".
    // (Hint in mail + date in roster: password-by-hint, the core loop.)
    if (digitsOnly(arg) == "0412") {
        p_->beep();
        say("> auth accepted. releasing docking clamps.");
        docked_ = true;
    } else {
        p_->beep();
        line("> AUTH REJECTED. clamps remain locked.");
        line("> (hint: the code is a date. read your mail and the roster.)");
    }
}

void Game::dockingSequence() {
    nl();
    showFile("prologue/docking.txt");
    p_->delayMs(400);
    nl();
    // The station network wakes. First contact with CANTOR.
    showFile("prologue/cantor.txt");
}

void Game::endPrologue() {
    nl();
    say("> [ PROLOGUE COMPLETE ]");
    p_->saveState("progress", "prologue_done");
    line("> Progress saved. ACT I: THE QUIET DECKS awaits.");
    line("> (MVP ends here. This is where the station proper begins.)");
}

} // namespace kd
