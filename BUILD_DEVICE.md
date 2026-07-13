# Сборка и прошивка пролога на Cardputer-Adv

Устройство-версия теперь **самодостаточная**: все тексты вшиты в прошивку
(`src/esp32/content_embedded.h`), поэтому получается **один `.bin`** и **один шаг
прошивки** — никакой отдельной загрузки файловой системы.

Готового `.bin` в репозитории нет: кастомную прошивку нужно один раз собрать у
себя (ESP32-тулчейн). Ниже — самый простой путь через PlatformIO. После первой
настройки пересборка = одна команда.

---

## Шаг 1. Поставить PlatformIO (один раз)

Самый удобный вариант — расширение для VS Code:

1. Установи [VS Code](https://code.visualstudio.com/).
2. В нём: вкладка Extensions (Ctrl+Shift+X) → найди **PlatformIO IDE** →
   Install. Подожди, пока внизу допишет "PlatformIO: ready".

Либо командной строкой (если не хочешь VS Code):

```
pip install -U platformio
```

## Шаг 2. Открыть проект

- VS Code: File → Open Folder → выбери папку `kodzimim` (в ней лежит
  `platformio.ini`). PlatformIO сам подтянет платформу ESP32, библиотеки
  M5Cardputer/M5Unified и тулчейн при первой сборке (нужен интернет,
  скачается ~несколько сотен МБ, идёт пару минут — только в первый раз).
- CLI: просто `cd kodzimim`.

## Шаг 3. Собрать `.bin`

```
pio run
```

Готовая прошивка появится здесь:

```
kodzimim/.pio/build/cardputer-adv/firmware.bin
```

(В VS Code то же самое — иконка «галочка» Build в нижней панели PlatformIO.)

## Шаг 4. Прошить на устройство

Подключи Cardputer-Adv по USB-C и:

```
pio run -t upload
```

Эта команда сама соберёт и зальёт всё, что нужно (загрузчик + таблицу
разделов + прошивку) — тебе не надо возиться с оффсетами. Порт определяется
автоматически; если несколько устройств — добавь `--upload-port COMx`.

В VS Code: иконка «стрелка вправо» (Upload).

Посмотреть, что происходит на устройстве (лог по serial):

```
pio device monitor
```

Если плата не прошивается: зажми кнопку **G0/BOOT** на Cardputer, коротко
нажми Reset, отпусти G0 — устройство войдёт в режим загрузки, повтори upload.

---

## Если нужен именно один файл `.bin` для внешнего флешера

`pio run -t upload` — самый простой путь. Но если хочешь прошивать сторонним
инструментом (esptool, M5Burner «custom firmware») одним файлом, собери
слитый образ:

```
pio run
esptool.py --chip esp32s3 merge_bin -o kodzimim_prologue_merged.bin \
  --flash_mode dio --flash_size 8MB \
  0x0 .pio/build/cardputer-adv/bootloader.bin \
  0x8000 .pio/build/cardputer-adv/partitions.bin \
  0x10000 .pio/build/cardputer-adv/firmware.bin
```

Полученный `kodzimim_prologue_merged.bin` шьётся на адрес `0x0`.

---

## Важные оговорки (это первый заход на железо)

- **Прошивка ещё не проверялась на реальном устройстве.** Код написан
  корректно и компилируется, но клавиатуру, звук (ES8311) и размер шрифта,
  скорее всего, придётся подстроить по факту. Это ожидаемо для первого билда.
- **`board = m5stack-stamps3`** в `platformio.ini` — под StampS3. Cardputer-Adv
  очень новый; если PlatformIO не знает борду или ADV отличается, поправь эту
  строку (варианты: `esp32-s3-devkitc-1`) и, при необходимости, обнови
  библиотеку M5Cardputer до последней — поддержка ADV именно в свежих версиях.
- **Экран/шрифт:** в `src/esp32/platform_m5.h` при `setTextSize(1)` получаем
  ~40 колонок. Если текст мелкий/крупный — крути `setTextSize` и цвет там же.
- **Звук «пиканья»:** `Speaker.tone(2200, 8)` — частота/длительность тоже
  подбираются на месте.

## Если правишь тексты

Тексты вшиты в `content_embedded.h`, сгенерированный из `content/prologue/*.txt`.
После редактирования .txt перегенерируй заголовок:

```
python3 - <<'PY'
import os
d="content/prologue"; files=["boot","briefing","mail","roster","shiplog","status","docking","cantor"]
o=["// AUTO-GENERATED from content/prologue/*.txt","#ifndef KODZIMIM_CONTENT_EMBEDDED_H","#define KODZIMIM_CONTENT_EMBEDDED_H","#include <string>","","namespace kd {","inline bool getEmbeddedContent(const std::string& path, std::string& out) {"]
first=True
for f in files:
    txt=open(os.path.join(d,f+".txt"),encoding="utf-8").read()
    kw="if" if first else "else if"; first=False
    o.append(f'    {kw} (path == "prologue/{f}.txt") {{ out = R"KD({txt})KD"; return true; }}')
o+=["    return false;","}","} // namespace kd","#endif"]
open("src/esp32/content_embedded.h","w",encoding="utf-8").write("\n".join(o)+"\n")
print("regenerated")
PY
```

Затем снова `pio run -t upload`.
