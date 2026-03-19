/*
 * Informer 213 Keyboard Firmware for Teensy 4.1
 * macOS Optimized — v2.2
 *
 * Сканирование: COL2ROW
 * USB Type: Serial + Keyboard + Mouse + Joystick (serialhid)
 *
 * Физическая раскладка (физряд → матричная позиция):
 *   Ряд1 (PF):    row0[0-11] = ESC F1-F11, row6[11] = F12
 *   Ряд2 (тёмн):  row1[0-11] + row6[10] = →
 *   Ряд3 (цифры): row2[0-11] = 1..=, row6[9] = `~
 *   Ряд4 (QWERTY): row3[0-11], row3[12] = CMD
 *   Ряд5 (home):   row7[0] = Tab, row4[1-11] = A..'"  row6[8] = Enter
 *   Ряд6 (Z-ряд):  row2[12] = LShift, row5[2-11] = Z../, row4[12] = RShift
 *   Ряд7 (низ):    row5[0] = Ctrl, row5[1] = Space, row3[12] = CMD,
 *                  row7[12] = Opt, row6[7] = Enter
 */

#include <Keyboard.h>

// HID Consumer Page коды для яркости (не встроены в Teensyduino 1.59)
#define KEY_BRIGHTNESS_DOWN  (0x70 | 0xE400)
#define KEY_BRIGHTNESS_UP    (0x6F | 0xE400)

// Sentinel: программный Fn-модификатор (не отправляет HID, меняет поведение Backspace)
#define KEY_FN  0xFFFE

#define ROWS 8
#define COLS 13

const uint8_t ROW_PINS[ROWS] = {2, 3, 4, 5, 6, 7, 8, 9};
const uint8_t COL_PINS[COLS] = {23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 27, 28, 29};

#define DEBOUNCE_MS 10

bool     key_state[ROWS][COLS]  = {};
bool     raw_state[ROWS][COLS]  = {};
uint32_t debounce_t[ROWS][COLS] = {};
bool     fn_held = false;

const uint16_t KEYMAP[ROWS][COLS] = {

  // ── row0 — Верхний ряд: ESC + медиа как MacBook ──────────
  { KEY_ESC,
    KEY_BRIGHTNESS_DOWN, KEY_BRIGHTNESS_UP,              // F1, F2
    KEY_F3, KEY_F4,                                      // Mission Ctrl, Launchpad
    KEY_F5, KEY_F6,                                      // (нет подсветки)
    KEY_MEDIA_PREV_TRACK,  KEY_MEDIA_PLAY_PAUSE,         // F7, F8
    KEY_MEDIA_NEXT_TRACK,  KEY_MEDIA_MUTE,               // F9, F10
    KEY_MEDIA_VOLUME_DEC, 0 },                           // F11, F12→row6[11]

  // ── row1 — Тёмный ряд: CapsLock Fn {}:\ стрелки ──────────
  { KEY_CAPS_LOCK, KEY_FN, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END, 0,
    KEY_RIGHT_BRACE, KEY_BACKSLASH,
    KEY_LEFT, KEY_UP, KEY_DOWN, KEY_RIGHT },

  // ── row2 — Цифровой ряд + LShift в col12 ─────────────────
  { KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6,
    KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_MINUS, KEY_EQUAL, MODIFIERKEY_LEFT_SHIFT },

  // ── row3 — QWERTY (col0-11) + CMD в col12 ────────────────
  { KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y,
    KEY_U, KEY_I, KEY_O, KEY_P,
    KEY_LEFT_BRACE, KEY_BACKSPACE, MODIFIERKEY_LEFT_GUI },

  // ── row4 — A..'" (col1-11) + RShift в col12, col0 пустой ─
  { 0,
    KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,
    KEY_H, KEY_J, KEY_K, KEY_L,
    KEY_SEMICOLON, KEY_QUOTE, MODIFIERKEY_RIGHT_SHIFT },

  // ── row5 — Ctrl(col0) Space(col1) Z../(col2-11) ──────────
  { MODIFIERKEY_LEFT_CTRL, KEY_SPACE,
    KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
    KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, 0 },

  // ── row6 — Спецклавиши ────────────────────────────────────
  //   col7  = Enter нижний ряд
  //   col8  = Enter home-ряд (↵)
  //   col9  = `~ (последняя цифрового ряда)
  //   col10 = → стрелка (последняя тёмного ряда)
  //   col11 = F12 (последняя PF-ряда)
  { 0, 0, 0, 0, 0, 0, 0,
    KEY_ENTER, KEY_ENTER, KEY_TILDE, KEY_RIGHT, KEY_MEDIA_VOLUME_INC, 0 },

  // ── row7 — Tab(col0) + Opt/Alt(col12) ────────────────────
  { KEY_TAB, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MODIFIERKEY_LEFT_ALT },
};

void setup() {
  for (int c = 0; c < COLS; c++) {
    pinMode(COL_PINS[c], OUTPUT);
    digitalWrite(COL_PINS[c], HIGH);
  }
  for (int r = 0; r < ROWS; r++) {
    pinMode(ROW_PINS[r], INPUT_PULLUP);
  }
  Keyboard.begin();
  delay(500);
}

void loop() {
  uint32_t now = millis();

  for (int c = 0; c < COLS; c++) {
    digitalWrite(COL_PINS[c], LOW);
    delayMicroseconds(10);

    for (int r = 0; r < ROWS; r++) {
      uint16_t key = KEYMAP[r][c];
      if (key == 0) continue;

      bool pressed = (digitalRead(ROW_PINS[r]) == LOW);

      if (pressed != raw_state[r][c]) {
        raw_state[r][c] = pressed;
        debounce_t[r][c] = now;
      } else if ((now - debounce_t[r][c]) >= DEBOUNCE_MS) {
        if (pressed != key_state[r][c]) {
          key_state[r][c] = pressed;

          // Fn-модификатор: не отправляем HID, только запоминаем состояние
          if (key == KEY_FN) {
            fn_held = pressed;
            continue;
          }

          // Fn + ESC = войти в режим загрузчика (HalfKay) для перепрошивки
          if (key == KEY_ESC && fn_held && pressed) {
            _reboot_Teensyduino_();
          }

          // Backspace + Fn = Forward Delete
          if (key == KEY_BACKSPACE) {
            uint16_t actual = fn_held ? KEY_DELETE : KEY_BACKSPACE;
            if (pressed) Keyboard.press(actual);
            else {
              Keyboard.release(KEY_DELETE);
              Keyboard.release(KEY_BACKSPACE);
            }
            continue;
          }

          if (pressed) Keyboard.press(key);
          else         Keyboard.release(key);
        }
      }
    }

    digitalWrite(COL_PINS[c], HIGH);
  }
}
