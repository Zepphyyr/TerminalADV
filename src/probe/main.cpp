// probe/main.cpp — diagnostic firmware, not part of the game.
//
// Purpose: split the "M5Launcher refuses to install our .bin" problem in half.
// This is the smallest possible app built with EXACTLY the same platformio
// settings as KODZIMIM (same platform version, same board, same framework).
//
//   probe.bin installs, kodzimim-app.bin does not  -> the fault is in our app
//       itself: its size, its libraries, or something in the image content.
//   probe.bin also refuses to install               -> the fault is in the build
//       configuration (platform version, board, flash settings), not our code.
//
// It draws one screen and does nothing else, so if it boots we also learn that
// the board definition is sane.
#include <M5Unified.h>

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setTextSize(2);
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.setCursor(0, 0);
    M5.Display.println("PROBE OK");
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.println("build config");
    M5.Display.println("is fine.");
}

void loop() { delay(1000); }
