// main.cpp (desktop)
// Test/dev build of KODZIMIM. Runs the prologue in a normal terminal.
//
// Usage:
//   kodzimim [content_dir] [save_file]
// Defaults: content_dir="content", save_file="save.dat"
#include "platform_desktop.h"
#include "../core/game.h"
#include <string>

int main(int argc, char** argv) {
    std::string content = (argc > 1) ? argv[1] : "content";
    std::string save    = (argc > 2) ? argv[2] : "save.dat";

    kd::DesktopPlatform platform(content, save);
    kd::Game game(&platform);
    game.run();
    return 0;
}
