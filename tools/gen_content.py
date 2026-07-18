# gen_content.py — regenerates src/esp32/content_embedded.h from ALL .txt files
# under content/ (prologue/, talk/, ...). Runs automatically before every
# PlatformIO build so firmware texts never drift from the source files.
import os

CONTENT = "content"
OUT = os.path.join("src", "esp32", "content_embedded.h")


def collect():
    files = []
    for root, _dirs, names in os.walk(CONTENT):
        for n in sorted(names):
            if n.endswith(".txt"):
                full = os.path.join(root, n)
                logical = os.path.relpath(full, CONTENT).replace(os.sep, "/")
                files.append((logical, full))
    files.sort()
    return files


def generate():
    out = [
        "// content_embedded.h  (AUTO-GENERATED from content/**/*.txt)",
        "// Story + dialogue texts baked into the firmware: one .bin, no filesystem.",
        "// Do not edit by hand — edit the .txt files and rebuild.",
        "#ifndef KODZIMIM_CONTENT_EMBEDDED_H",
        "#define KODZIMIM_CONTENT_EMBEDDED_H",
        "#include <string>",
        "",
        "namespace kd {",
        "inline bool getEmbeddedContent(const std::string& path, std::string& out) {",
    ]
    first = True
    for logical, full in collect():
        with open(full, encoding="utf-8") as fh:
            text = fh.read()
        if ')KD"' in text:
            raise SystemExit("raw-string delimiter clash in " + full)
        kw = "if" if first else "else if"
        first = False
        out.append('    %s (path == "%s") { out = R"KD(%s)KD"; return true; }'
                   % (kw, logical, text))
    out += ["    return false;", "}", "} // namespace kd", "#endif"]
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as fh:
        fh.write("\n".join(out) + "\n")
    print("gen_content: %d files -> %s (%d bytes)" %
          (len(collect()), OUT, os.path.getsize(OUT)))


generate()
