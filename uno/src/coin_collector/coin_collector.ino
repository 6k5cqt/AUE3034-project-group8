#define SENSOR_PIN 0x04 // 적외선 센서를 연결한 interrupt pin == 8번핀
#define RESET_BUTTON 0x80 // 동전 카운팅 초기화 시킬 버튼 == 7번핀
#define LED_1 0x01 // 동전 카운팅 LED 1 == 8번핀
#define LED_2 0x02 // 동전 카운팅 LED 2 == 9번핀

volatile int coinCount = 0;
volatile unsigned long lastSensorTriggerTime = 0; // 센서 마지막 감지 시간 저장
const unsigned long debounceDelayTime = 100; // 센서 디바운싱 시간 밀리초

void setup() {
  // Output
  DDRB = LED_1 | LED_2; // pin 8,9 as Output

  // Input
  DDRD &= ~SENSOR_PIN;
  DDRD &= ~RESET_BUTTON; // pin 2,7 as Input

  PORTD |= RESET_BUTTON;

  // Register settings for INT0
  EICRA |= 0x02; // falling edge
  EIMSK |= 0x01; // INT0(pin2) Enable

  // Register settings for PCINT2
  PCICR |= 0x04; // PCIE2 Enable
  PCMSK2 |= RESET_BUTTON; // PCINT23(pin7) Enable

  // Enable Interrupt
  sei();

  //Serial.begin(9600);
}

void loop() {
  //Serial.println(coinCount);

  if(coinCount == 0){
    PORTB &= ~LED_1; // turn off
    PORTB &= ~LED_2; // turn off
  }
  else if(coinCount == 1){
    PORTB |= LED_1; // turn on
    PORTB &= ~LED_2; // turn off
  }
  else if(coinCount >= 2){
    PORTB |= LED_1; // turn on
    PORTB |= LED_2; // turn on
  }

  delay(50);
}

// INT0 Interrupt for IR sensor
ISR(INT0_vect){
  unsigned long currentTime = millis(); // 현재 시간 가져오기

  if(currentTime - lastSensorTriggerTime > debounceDelayTime){
    if(coinCount < 2){
      coinCount++;
    }
    lastSensorTriggerTime = currentTime; // 마지막 감지 시간 업데이트
  }

  // for delaying function inside ISR
  for(uint16_t i = 0; i <= 60000; i++){
    asm("nop");
  }
}

// PCINT2 Interrupt for Reset Button
ISR(PCINT2_vect){
  if(!(PIND & RESET_BUTTON))
  // 버튼 안누르면 1xxxxxxx & 10000000 = 10000000인데 ! 때문에 false 됨. 
  // 누르면 0xxxxxxx & 10000000 = 00000000인데 ! 때문에 true 됨.
  {
    coinCount = 0;
  }

  // for delaying function inside ISR
  for(uint16_t i = 0; i <= 30000; i++){
    asm("nop");
  }
}

