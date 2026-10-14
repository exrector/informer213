// Независимая проверка: каждая пара в изоляции, обе полярности, остальные пины INPUT_DISABLE
const int NP = 42;
void allZ() { for (int p = 0; p < NP; p++) if (p != 13) pinMode(p, INPUT_DISABLE); }
// true если X, вытянутый в lvl, перетягивает Y (на Y обратная подтяжка)
bool conducts(int x, int y, bool lvl) {
  allZ();
  pinMode(y, lvl ? INPUT_PULLDOWN : INPUT_PULLUP);
  pinMode(x, OUTPUT); digitalWrite(x, lvl);
  delayMicroseconds(50);
  bool r = digitalRead(y) == lvl;
  allZ();
  return r;
}
void setup() { Serial.begin(115200); while (!Serial && millis() < 3000) {} }
void loop() {
  if (!Serial.available()) return;
  while (Serial.available()) Serial.read();
  int n = 0;
  for (int a = 0; a < NP; a++) for (int b = a + 1; b < NP; b++) {
    if (a == 13 || b == 13) continue;
    // ток a->b: a=HIGH тянет b (pulldown) вверх, или b=LOW тянет a (pullup) вниз
    bool ab = conducts(a, b, HIGH) || conducts(b, a, LOW);
    bool ba = conducts(b, a, HIGH) || conducts(a, b, LOW);
    if (ab || ba) {
      Serial.print("pin "); Serial.print(a); Serial.print(" - pin "); Serial.print(b);
      Serial.println(ab && ba ? "  ОБЕ стороны (замыкание)" : (ab ? "  одна сторона (диод)" : "  одна сторона (диод)"));
      n++;
    }
  }
  Serial.print("pairs="); Serial.println(n);
  Serial.println("END");
}
