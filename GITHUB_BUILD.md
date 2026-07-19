# Получить `.bin` без установки тулчейна (сборка в облаке)

Компилятор ESP32 нельзя достать из моей песочницы (провайдер блокирует все
серверы Espressif/PlatformIO/Arduino). Поэтому сборку делает **GitHub Actions** —
облачный раннер сам скачает тулчейн, соберёт прошивку и отдаст готовый `.bin`.
Тебе ничего ставить не нужно: только аккаунт GitHub и браузер.

Workflow уже лежит в проекте: `.github/workflows/build-firmware.yml`.

## Шаги

1. Заведи бесплатный аккаунт на https://github.com (если нет).
2. Создай новый репозиторий (кнопка **New**, имя любое, можно **Private**).
3. Залей туда содержимое папки `kodzimim`:
   - Проще всего: на странице репозитория **Add file → Upload files**,
     перетащи всю папку `kodzimim` целиком, **Commit**.
   - ВАЖНО: убедись, что попала скрытая папка `.github/workflows/` с файлом
     `build-firmware.yml` — без неё сборка не запустится. Если drag-and-drop
     её пропустил, создай файл вручную: **Add file → Create new file**, имя
     `.github/workflows/build-firmware.yml`, вставь содержимое из проекта.
4. После коммита открой вкладку **Actions** — сборка **build-firmware**
   запустится сама (2–4 минуты). Дождись зелёной галочки.
5. Зайди в завершённый запуск → блок **Artifacts** внизу → скачай
   **kodzimim-firmware** (zip). Либо, что удобнее, бери файлы из раздела
   **Releases** — там всегда лежит свежая сборка.

## ВАЖНО: два разных .bin, они НЕ взаимозаменяемы

| Файл | Что это | Чем шить |
|---|---|---|
| **kodzimim-app.bin** | только приложение | **M5Launcher** (SD / URL / WebUI), OTA |
| **kodzimim-full.bin** | весь флеш: загрузчик + разделы + приложение | **esptool** / веб-флешер, адрес **0x0** |

Если скормить лаунчеру `full`-образ, он попытается прочитать заголовок
загрузчика как приложение и выдаст **«flash read failed / Update error»**.
Ровно так же наоборот: `app`-образ, записанный на 0x0 голым esptool, не
загрузится, потому что в нём нет загрузчика.

Постоянные ссылки:

```
https://github.com/Zepphyyr/TerminalADV/releases/latest/download/kodzimim-app.bin
https://github.com/Zepphyyr/TerminalADV/releases/latest/download/kodzimim-full.bin
```

## Прошить `.bin` тоже без установки (веб-флешер)

1. Открой в Chrome или Edge веб-флешер Espressif:
   https://espressif.github.io/esptool-js/
2. Подключи Cardputer-Adv по USB-C → **Connect** → выбери COM-порт.
3. Add file: выбери `kodzimim_prologue_merged.bin`, адрес **0x0**.
4. **Program**. После прошивки — Reset. Должен подняться пролог.

(Если предпочитаешь локально: `esptool --chip esp32s3 write_flash 0x0
kodzimim_prologue_merged.bin`.)

## Если сборка упадёт

Самое вероятное — PlatformIO не знает борду `m5stack-stamps3` для совсем нового
ADV, либо версия M5Cardputer. Открой лог упавшего шага в Actions, пришли мне
последние строки ошибки — поправлю `platformio.ini`/`board` и код под ADV.
Прошивка ещё не проверялась на живом устройстве, так что шрифт/клавиатуру/звук,
скорее всего, тоже подстроим по первому запуску.
