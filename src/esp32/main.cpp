// main.cpp (ESP32 / Cardputer-Adv)
// Arduino entry point. Boots the platform and runs the prologue.
#include "platform_m5.h"
#include "../core/game.h"

static kd::M5Platform platform;

void setup() {
    platform.begin();
    kd::Game game(&platform);
    game.run();   // blocking; drives the whole prologue
}

void loop() {
    // The game runs to completion inside setup(). Nothing to do here.
    delay(1000);
}
