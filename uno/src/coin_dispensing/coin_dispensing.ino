#define BUTTON_PIN 2

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(9, OUTPUT);

  // Timer1 설정 (분주 8, Fast PWM, TOP=ICR1)
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
  ICR1 = 39999; // 50Hz (20ms)
  OCR1A = 2000 + (int)((120 - 90) * (2000.0 / 120)); // 초기값 90도(방향 반대로: 30도)
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {

      // 1. 반시계방향(정방향)으로 0도 이동 → 방향 반대로: 120도(120-0)
      OCR1A = 2000 + (int)(120 * (2000.0 / 120)); // 4000
      delay(500);

      // 2. 시계방향(역방향)으로 110도 이동 (0도에서 110도로 이동) → 방향 반대로: 120-110=10도
      //OCR1A = 2000 + (int)(10 * (2000.0 / 120)); // 2167
      //delay(1000);

      // 3. 초기 위치(90도)로 복귀 → 방향 반대로: 120-90=30도
      OCR1A = 2000 + (int)(30 * (2000.0 / 120)); // 2500
      delay(500);
    }
  }
}
