# gen_content.py — regenerates src/esp32/content_embedded.h from content/prologue/*.txt
#
# Runs automatically before every PlatformIO build (see platformio.ini:
# extra_scripts = pre:tools/gen_content.py), so the firmware texts can never
# drift out of sync with the .txt files. Can also be run by hand:
#     python tools/gen_content.py
import os

FILES = ["boot", "briefing", "mail", "roster", "shiplog", "status",
         "docking", "cantor"]
SRC_DIR = os.path.join("content", "prologue")
OUT = os.path.join("src", "esp32", "content_embedded.h")


def generate():
    out = [
        "// content_embedded.h  (AUTO-GENERATED from content/prologue/*.txt)",
        "// Story texts baked into the firmware: one .bin, no filesystem needed.",
        "// Do not edit by hand — edit the .txt files and rebuild.",
        "#ifndef KODZIMIM_CONTENT_EMBEDDED_H",
        "#define KODZIMIM_CONTENT_EMBEDDED_H",
        "#include <string>",
        "",
        "namespace kd {",
        "inline bool getEmbeddedContent(const std::string& path, std::string& out) {",
    ]
    first = True
    for name in FILES:
        path = os.path.join(SRC_DIR, name + ".txt")
        with open(path, encoding="utf-8") as fh:
            text = fh.read()
        if ')KD"' in text:
            raise SystemExit("raw-string delimiter clash in " + path)
        kw = "if" if first else "else if"
        first = False
        out.append('    %s (path == "prologue/%s.txt") { out = R"KD(%s)KD"; return true; }'
                   % (kw, name, text))
    out += ["    return false;", "}", "} // namespace kd", "#endif"]

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as fh:
        fh.write("\n".join(out) + "\n")
    print("gen_content: wrote %s (%d bytes)" % (OUT, os.path.getsize(OUT)))


generate()
