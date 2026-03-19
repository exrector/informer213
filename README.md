# Informer 213 Keyboard Firmware

![Изображение](https://github.com/user-attachments/assets/af887412-c078-4108-a875-0ca18df30807)
![B975B99A-3C3A-40FC-80E9-567C41E198F9_1_105_c](https://github.com/user-attachments/assets/3fdb0fbd-2968-49c6-be26-6a5a0b1fcfaa)
![F42247DC-CE73-49D2-A771-158E9641E0AA_1_105_c](https://github.com/user-attachments/assets/ad6d0564-1af9-44da-b2ef-8b6e35693b25)
<img width="1141" alt="Снимок экрана 2024-08-25 в 14 38 59" src="https://github.com/user-attachments/assets/dd727615-5b1f-4d9e-a92d-a8897e97037f">
<img width="935" alt="Снимок экрана 2024-08-25 в 04 04 10" src="https://github.com/user-attachments/assets/e95b6fd4-dd0b-48b5-a117-4ec0692f6539">
<img width="1052" alt="Снимок экрана 2024-08-25 в 04 04 14" src="https://github.com/user-attachments/assets/8a3abfda-1943-4e7a-ac1a-2fa57cbf5253">
<img width="1021" alt="Снимок экрана 2024-08-25 в 04 03 39" src="https://github.com/user-attachments/assets/44dd2be7-6e8a-4aba-bd80-f0eeb2391b68">

Прошивка для клавиатуры терминала **Informer 213** (США, 1989–1992) на базе **Teensy 4.1**.
Внутри корпуса установлен Mac Mini 2014. Прошивка оптимизирована под macOS.

---

## Как перепрошить

### Команда прошивки

```bash
cd "/Users/exrector/Documents/ПРОЕКТЫ/informer213-arduino"
arduino-cli compile --fqbn "teensy:avr:teensy41:usb=serialhid,speed=600,opt=o2std,keys=en-us" informer213-arduino && \
arduino-cli upload -p /dev/cu.usbmodem* --fqbn "teensy:avr:teensy41:usb=serialhid,speed=600,opt=o2std,keys=en-us" informer213-arduino
```

Если порт не найден — используй teensy_loader_cli:

```bash
sudo teensy_loader_cli --mcu=TEENSY41 -w -v \
  "/Users/exrector/Documents/ПРОЕКТЫ/informer213-arduino/build/informer213-arduino.ino.hex"
```

### Кнопка Reset

**Teensy 4.x не поддерживает программный вход в загрузчик** — это аппаратная защита PJRC.
Для перепрошивки всегда нужно нажать физическую кнопку Reset на плате Teensy внутри корпуса.

> Fn+ESC перезапускает прошивку (мягкий reboot), но не входит в загрузчик.

---

## Физическая раскладка клавиатуры

```
РЯД 1:  [ESC ] [F1 ▼] [F2 ▲] [F3 ] [F4 ] [F5 ] [F6 ] [F7 ⏮] [F8 ⏯] [F9 ⏭] [F10 🔇] [F11 🔉] [F12 🔊]

РЯД 2:  [CAPS] [FN ] [PgUp] [PgDn] [Home] [End] [----] [  }] [  \] [←] [↑] [↓] [→]

РЯД 3:  [ 1 ] [ 2 ] [ 3 ] [ 4 ] [ 5 ] [ 6 ] [ 7 ] [ 8 ] [ 9 ] [ 0 ] [ - ] [ = ] [ `~ ]

РЯД 4:  [ Q ] [ W ] [ E ] [ R ] [ T ] [ Y ] [ U ] [ I ] [ O ] [ P ] [ [ ] [BSPC]

РЯД 5:  [TAB] [ A ] [ S ] [ D ] [ F ] [ G ] [ H ] [ J ] [ K ] [ L ] [ ; ] [ ' ] [ENTER]

РЯД 6:  [LSHFT] [ Z ] [ X ] [ C ] [ V ] [ B ] [ N ] [ M ] [ , ] [ . ] [ / ] [RSHFT]

РЯД 7:  [CTRL] [CMD] [         SPACE         ] [OPT] [ENTER]
```

---

## Fn-комбинации

| Комбинация | Действие |
|---|---|
| Fn + Backspace | Forward Delete (удалить символ справа) |
| Fn + ESC | Мягкий перезапуск прошивки |

---

## F1–F12 (MacBook-стиль)

| Клавиша | Действие |
|---|---|
| F1 | Яркость ↓ |
| F2 | Яркость ↑ |
| F3 | Mission Control (назначить в System Preferences) |
| F4 | Launchpad (назначить в System Preferences) |
| F7 | Предыдущий трек |
| F8 | Play/Pause |
| F9 | Следующий трек |
| F10 | Mute |
| F11 | Громкость ↓ |
| F12 | Громкость ↑ |

---

## Матрица клавиатуры

- **Сканирование:** COL2ROW, 8 строк × 13 столбцов
- **ROW пины:** 2, 3, 4, 5, 6, 7, 8, 9
- **COL пины:** 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 27, 28, 29
- **Debounce:** 10 мс

## Физическая раскладка → матрица

| Физический ряд | Матричные позиции |
|---|---|
| Ряд 1 (ESC/F-keys) | row0[0-11] + row6[11]=F12 |
| Ряд 2 (тёмный) | row1[0-12] |
| Ряд 3 (цифры) | row2[0-11] + row6[9]=`~ |
| Ряд 4 (QWERTY) | row3[0-11] + row3[12]=CMD |
| Ряд 5 (home) | row7[0]=Tab, row4[1-11], row6[8]=Enter |
| Ряд 6 (Z-ряд) | row2[12]=LShift, row5[2-11], row4[12]=RShift |
| Ряд 7 (низ) | row5[0]=Ctrl, row5[1]=Space, row7[12]=Opt, row6[7]=Enter |

## Раскладка тёмного ряда (ряд 2)

| Позиция | Клавиша |
|---|---|
| col 1 (row1[0]) | Caps Lock |
| col 2 (row1[1]) | **Fn** (программный модификатор) |
| col 3 (row1[2]) | Page Up |
| col 4 (row1[3]) | Page Down |
| col 5 (row1[4]) | Home |
| col 6 (row1[5]) | End |
| col 7 (row1[6]) | — пусто — |
| col 8 (row1[7]) | ] |
| col 9 (row1[8]) | \ |
| col 10 (row1[9]) | ← |
| col 11 (row1[10]) | ↑ |
| col 12 (row1[11]) | ↓ |
| col 13 (row1[12]) | → |

---

## Версии прошивки

| Версия | Изменения |
|---|---|
| v1.0 | Первая рабочая версия, базовые клавиши |
| v2.0 | Все стандартные Mac-клавиши + F1-F12 как медиа |
| v2.1 | Fn-модификатор, Fn+Backspace=Delete, тёмный ряд |
| v2.2 | Fn+ESC = мягкий reboot |
