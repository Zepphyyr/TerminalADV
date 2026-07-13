// platform.h
// Platform abstraction for KODZIMIM.
// The game core talks ONLY to this interface, so the same engine runs on
// the desktop (for testing) and on the Cardputer-Adv (real hardware).
#ifndef KODZIMIM_PLATFORM_H
#define KODZIMIM_PLATFORM_H

#include <string>

namespace kd {

// Screen geometry the core assumes. Both frontends emulate this so the
// text/pager layout looks identical on PC and on the device.
// Cardputer-Adv: 240x135 px. At a ~6px mono font that is ~40 cols x ~16 rows.
struct Screen {
    int cols = 40;
    int rows = 16;
};

// Abstract platform. Implemented by platform_desktop.* and platform_m5.*.
class IPlatform {
public:
    virtual ~IPlatform() {}

    // Screen dimensions (in character cells).
    virtual Screen screen() const = 0;

    // Print text instantly (no typing effect). Handles '\n'.
    virtual void print(const std::string& s) = 0;

    // Print text with the "terminal typing" effect: char by char, with a
    // short delay and a periodic beep. This is the atmospheric output path.
    virtual void typeOut(const std::string& s) = 0;

    // Clear the screen.
    virtual void clear() = 0;

    // Short terminal beep (used on key input / events).
    virtual void beep() = 0;

    // Print a prompt, then read one line of user input (with local echo).
    // Returns the entered line WITHOUT the trailing newline.
    virtual std::string readLine(const std::string& promptStr) = 0;

    // Wait for any single keypress (used by the pager "-- more --").
    virtual void waitKey() = 0;

    // Sleep for the given number of milliseconds.
    virtual void delayMs(int ms) = 0;

    // Load a content file into `out`. Returns false if not found.
    // Paths are logical, e.g. "prologue/briefing.txt".
    virtual bool loadFile(const std::string& path, std::string& out) = 0;

    // Persist and read a small progress value (save system).
    virtual void  saveState(const std::string& key, const std::string& value) = 0;
    virtual std::string loadState(const std::string& key) = 0;
};

} // namespace kd

#endif // KODZIMIM_PLATFORM_H
