/*
 * Informer 213 Keyboard Firmware for Teensy 4.1 — v3.0 (новая распайка 2026-10-14)
 * macOS. Матрица 7 строк × 13 столбцов, диоды 1N4148 (ток из столбца в строку).
 * Строки: пины 33..39 (физ. ряды сверху вниз). Столбцы: пины 0..12 (12 = левый край).
 * Строку тянем в LOW, столбцы читаем с подтяжкой. Карта: ../MATRIX-MAP.md
 * USB Type: Serial + Keyboard + Mouse + Joystick (serialhid)
 *
 * Fn = FIELD MARK. Fn+IDENT(Esc) = загрузчик для перепрошивки. Fn+Backspace = Forward Delete.
 * Клавиша TEST (самая первая) не подключена.
 */
#include <Keyboard.h>

#define KEY_BRIGHTNESS_DOWN  (0x70 | 0xE400)
#define KEY_BRIGHTNESS_UP    (0x6F | 0xE400)
#define KEY_FN  0xFFFE

#define ROWS 7
#define COLS 13
const uint8_t ROW_PINS[ROWS] = {33, 34, 35, 36, 37, 38, 39};
// индекс столбца = номер пина (0..12)
#define DEBOUNCE_MS 10

bool     key_state[ROWS][COLS]  = {};
bool     raw_state[ROWS][COLS]  = {};
uint32_t debounce_t[ROWS][COLS] = {};
bool     fn_held = false;

// KEYMAP[строка][пин столбца]; физически слева направо = столбец 12 → 0
const uint16_t KEYMAP[ROWS][COLS] = {
  // пин 33 — PF: (TEST не подключён) PF1..PF12 = столбцы 11..0
  { KEY_MEDIA_VOLUME_INC,                 // 0  PF12
    KEY_MEDIA_VOLUME_DEC,                 // 1  PF11
    KEY_MEDIA_MUTE,                       // 2  PF10
    KEY_MEDIA_NEXT_TRACK,                 // 3  PF9
    KEY_MEDIA_PLAY_PAUSE,                 // 4  PF8
    KEY_MEDIA_PREV_TRACK,                 // 5  PF7
    KEY_F6,                               // 6  PF6
    KEY_F5,                               // 7  PF5
    KEY_F4,                               // 8  PF4
    KEY_F3,                               // 9  PF3
    KEY_BRIGHTNESS_UP,                    // 10 PF2
    KEY_BRIGHTNESS_DOWN,                  // 11 PF1
    0 },                                  // 12 TEST (не подключена)

  // пин 34 — тёмный ряд: IDENT FIELDMARK DUP ERASE PA3 CSEL CBLINK {} |\ ← ↑ ↓ →
  { KEY_RIGHT,                            // 0  →
    KEY_DOWN,                             // 1  ↓
    KEY_UP,                               // 2  ↑
    KEY_LEFT,                             // 3  ←
    KEY_BACKSLASH,                        // 4  | \  .
    KEY_RIGHT_BRACE,                      // 5  { }
    KEY_CAPS_LOCK,                        // 6  CURSR BLINK
    KEY_END,                              // 7  CURSR SEL
    KEY_HOME,                             // 8  PA3 ATTN
    KEY_PAGE_DOWN,                        // 9  ERASE EOF
    KEY_PAGE_UP,                          // 10 DUP
    KEY_FN,                               // 11 FIELD MARK
    KEY_ESC },                            // 12 IDENT

  // пин 35 — цифры: 1 2 3 4 5 6 7 8 9 0 - = `
  { KEY_TILDE, KEY_EQUAL, KEY_MINUS, KEY_0, KEY_9, KEY_8, KEY_7,
    KEY_6, KEY_5, KEY_4, KEY_3, KEY_2, KEY_1 },

  // пин 36 — Q W E R T Y U I O P [(!¢) Backspace
  { KEY_BACKSPACE, KEY_LEFT_BRACE, KEY_P, KEY_O, KEY_I, KEY_U,
    KEY_Y, KEY_T, KEY_R, KEY_E, KEY_W, KEY_Q, 0 },

  // пин 37 — Tab A S D F G H J K L ; ' Enter
  { KEY_ENTER, KEY_QUOTE, KEY_SEMICOLON, KEY_L, KEY_K, KEY_J,
    KEY_H, KEY_G, KEY_F, KEY_D, KEY_S, KEY_A, KEY_TAB },

  // пин 38 — LShift Z X C V B N M , . / RShift
  { MODIFIERKEY_RIGHT_SHIFT, KEY_SLASH, KEY_PERIOD, KEY_COMMA, KEY_M, KEY_N,
    KEY_B, KEY_V, KEY_C, KEY_X, KEY_Z, MODIFIERKEY_LEFT_SHIFT, 0 },

  // пин 39 — RESET(Ctrl) замок(Cmd) Space ALT(Opt) ENTER
  { KEY_ENTER,                            // 0  ENTER
    MODIFIERKEY_LEFT_ALT,                 // 1  ALT
    0, 0, 0,
    KEY_SPACE,                            // 5  пробел
    0, 0, 0, 0,
    MODIFIERKEY_LEFT_GUI,                 // 10 замок → Cmd
    MODIFIERKEY_LEFT_CTRL,                // 11 RESET → Ctrl
    0 },
};

void setup() {
  for (int c = 0; c < COLS; c++) pinMode(c, INPUT_PULLUP);
  for (int r = 0; r < ROWS; r++) pinMode(ROW_PINS[r], INPUT);
  Serial.begin(115200);
  Keyboard.begin();
  delay(300);
}

void loop() {
  uint32_t now = millis();
  for (int r = 0; r < ROWS; r++) {
    pinMode(ROW_PINS[r], OUTPUT);
    digitalWrite(ROW_PINS[r], LOW);
    delayMicroseconds(10);
    for (int c = 0; c < COLS; c++) {
      uint16_t key = KEYMAP[r][c];
      if (key == 0) continue;
      bool pressed = (digitalRead(c) == LOW);
      if (pressed != raw_state[r][c]) {
        raw_state[r][c] = pressed;
        debounce_t[r][c] = now;
      } else if ((now - debounce_t[r][c]) >= DEBOUNCE_MS && pressed != key_state[r][c]) {
        key_state[r][c] = pressed;
        Serial.print(pressed ? "PRESS row=" : "REL   row="); Serial.print(ROW_PINS[r]);
        Serial.print(" col="); Serial.println(c);
        if (key == KEY_FN) { fn_held = pressed; continue; }
        if (key == KEY_ESC && fn_held && pressed) _reboot_Teensyduino_();
        if (key == KEY_BACKSPACE) {
          if (pressed) Keyboard.press(fn_held ? KEY_DELETE : KEY_BACKSPACE);
          else { Keyboard.release(KEY_DELETE); Keyboard.release(KEY_BACKSPACE); }
          continue;
        }
        if (pressed) Keyboard.press(key); else Keyboard.release(key);
      }
    }
    pinMode(ROW_PINS[r], INPUT);
    delayMicroseconds(5);
  }
}
