/*
 * Teensy 4.1 — универсальный пробник матрицы клавиатуры
 * Не нужно знать распиновку: каждый пин 0..41 по очереди тянется к GND,
 * остальные читаются с подтяжкой. Нажатие = пара пинов.
 * Вывод: "DOWN drv=A sns=B" / "UP drv=A sns=B"
 * Пины, замкнутые на землю в покое, — в отчёте STUCK (пишется при старте и по '?').
 */
const int NP = 42;
bool usable[NP];
bool prev[NP][NP];
bool base[NP][NP];

void scanAll(bool out[NP][NP]) {
  for (int d = 0; d < NP; d++) {
    if (!usable[d]) continue;
    pinMode(d, OUTPUT); digitalWrite(d, LOW); delayMicroseconds(20);
    for (int s = 0; s < NP; s++) out[d][s] = (s != d && usable[s] && digitalRead(s) == LOW);
    pinMode(d, INPUT_PULLUP); delayMicroseconds(10);
  }
}

void report() {
  Serial.print("STUCK(idle LOW):");
  bool any = false;
  for (int p = 0; p < NP; p++) if (!usable[p]) { Serial.print(' '); Serial.print(p); any = true; }
  if (!any) Serial.print(" none");
  Serial.println();
  for (int d = 0; d < NP; d++) {
    bool h = false;
    for (int s = 0; s < NP; s++) if (base[d][s]) {
      if (!h) { Serial.print("BASE "); Serial.print(d); Serial.print(" ->"); h = true; }
      Serial.print(' '); Serial.print(s);
    }
    if (h) Serial.println();
  }
  Serial.println("READY");
}

void setup() {
  Serial.begin(115200);
  for (int p = 0; p < NP; p++) pinMode(p, INPUT_PULLUP);
  delay(50);
  for (int p = 0; p < NP; p++) usable[p] = digitalRead(p) == HIGH;
  usable[13] = false; // LED
  memset(base, 0, sizeof(base));
  for (int i = 0; i < 5; i++) { bool t2[NP][NP]; scanAll(t2); for (int d=0; d<NP; d++) for (int s=0; s<NP; s++) base[d][s] |= t2[d][s]; delay(5); }
  memcpy(prev, base, sizeof(prev));
  uint32_t t = millis();
  while (!Serial && millis() - t < 3000) {}
  report();
}

bool gprev[NP];
void loop() {
  // щуп: пины, притянутые к GND снаружи, когда сами ничего не тянем
  bool gnow[NP]; bool gch = false;
  for (int p = 0; p < NP; p++) { gnow[p] = usable[p] && digitalRead(p) == LOW; if (gnow[p] != gprev[p]) gch = true; }
  if (gch) {
    Serial.print("GND:");
    bool any = false;
    for (int p = 0; p < NP; p++) if (gnow[p]) { Serial.print(' '); Serial.print(p); any = true; }
    Serial.println(any ? "" : " none");
    memcpy(gprev, gnow, sizeof(gprev));
  }
  if (gch || memchr(gnow, 1, NP)) { delay(5); return; }
  if (Serial.available()) { while (Serial.available()) Serial.read(); report(); }
  for (int d = 0; d < NP; d++) {
    if (!usable[d]) continue;
    pinMode(d, OUTPUT);
    digitalWrite(d, LOW);
    delayMicroseconds(20);
    for (int s = 0; s < NP; s++) {
      if (s == d || !usable[s]) continue;
      bool now = digitalRead(s) == LOW;
      if (now != prev[d][s]) {
        Serial.print((now != base[d][s]) ? "DOWN drv=" : "UP drv=");
        if (base[d][s]) Serial.print("(base-break) ");
        Serial.print(d); Serial.print(" sns="); Serial.println(s);
        prev[d][s] = now;
      }
    }
    pinMode(d, INPUT_PULLUP);
    delayMicroseconds(10);
  }
  delay(2);
}
