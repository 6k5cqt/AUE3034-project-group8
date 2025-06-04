// --- 핀 설정 ---
#define SENSOR_PIN 0x04 // 적외선 센서를 연결한 interrupt pin == 2번핀
#define SERVO_PIN PB1 // 동전 배출 서보모터 == 9번핀


// --- 변수 설정 ---
volatile int coinCount = 0; // 인식한 동전 개수
volatile unsigned long lastSensorTriggerTime = 0; // 센서 마지막 감지 시간 저장
const unsigned long debounceDelayTime = 100; // 센서 디바운싱 시간 밀리초


// --- 가위바위보 관련 ---
enum RPS { SCISSORS = 0, ROCK = 1, PAPER = 2 };


// --- 함수 ---
void dispenseCoin();
void showLEDs(int player, int ai);
int judge(int player, int ai);
void resetGameState();

// ---
void setup() {
  // Input
  DDRD &= ~SENSOR_PIN; // pin 2 for IR Sensor

  // Output
  DDRB |= (1 << SERVO_PIN); // pin 9 for Coin Dispenser Servo
  DDRC |= 0x3F; // PORTC(A0~5)사용. 0~2:플레이어, 3~5:게임기. 가위,바위,보 순서.
  PORTC = 0; // all PORTC LEDs OFF

  // Register settings for INT0 for IR Sensor
  EICRA |= 0x02; // falling edge
  EIMSK |= 0x01; // INT0(pin2) Enable

  // Enable Interrupt
  sei();

  // Timer1 Setting for Coin Dispenser Servo
  TCCR1A = _BV(COM1A1) | _BV(WGM11); // Clear OC1A on Compare Match (Set output to low level) | for Fast PWM
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11); // for Fast PWM | clk/8 (from prescaler)
  // 16MHz / 8 = 2,000,000 클럭/s
  // 50Hz(20ms)동안 클럭 수 2,000,000 * 0.02 = 40,000
  ICR1 = 39999; // 타이머는 0부터 시작해서 39,999로 설정.
  OCR1A = 2000 + (int)((120 - 90) * (2000 / 120)); // 초기값 90도(방향 반대로: 30도)

  // Enable Serial Monitor
  Serial.begin(9600);
}

// ---
void loop() {
  Serial.println(coinCount);

  // --- 동전 2개 이상이면 게임 시작 ---
  if (coinCount >= 2) {
    resetGameState(); // 게임 상태 초기화

    Serial.println("가위바위보 게임 시작!");
    
    while(1){
      Serial.println("가위, 바위, 보 중 하나를 입력하세요:");

      while(Serial.available()) Serial.read(); // 시리얼 버퍼 초기화
      while(!Serial.available()); // 데이터 입력까지 무한 대기

      String userInput = Serial.readStringUntil('\n'); // 가위바위보 입력
      userInput.trim();

      resetGameState(); // 게임 표시 상태 초기화

      // --- 플레이어 값 변환 ---
      int playerChoice = 3;
      if (userInput == "가위") playerChoice = SCISSORS;
      else if (userInput == "바위") playerChoice = ROCK;
      else if (userInput == "보") playerChoice = PAPER;
      else {
        Serial.println("잘못된 입력입니다. 다시 입력하세요.");
        continue;
      }

      int aiChoice = random(0, 3); // 게임기 값 생성

      showLEDs(playerChoice, aiChoice); // 각각의 값 LED로 보여주기

      // --- 승패 판단 ---
      int result = judge(playerChoice, aiChoice);
      if (result == 1) {
      Serial.println("플레이어 승! (계속 진행)");
        break; // 룰렛(6번)으로 진행
      }
      else if (result == -1) {
        Serial.println("게임기 승!");
        Serial.println("게임 종료");
        coinCount = 0;
        break; // 게임 종료
      }
      else {
        Serial.println("비겼습니다. 다시 입력하세요.");
      }
    }  
  

    // 6번 룰렛(생략)


    // 7번: 플레이어 승리 시 동전 반환
    if (coinCount > 1) {
      dispenseCoin();
      Serial.println("게임 종료.");
      coinCount = 0;
      delay(1000);
    }
  }
  delay(500);
}

// --- INT0 Interrupt for IR sensor ---
ISR(INT0_vect){
  unsigned long currentTime = millis(); // 현재 시간 가져오기 (채터링 방지)
  if(currentTime - lastSensorTriggerTime > debounceDelayTime){ // 채터링 방지
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

// --- Coin Dispensing ---
void dispenseCoin(){
  // 1. 반시계방향(정방향)으로 0도 이동 → 방향 반대로: 120도(120-0)
  OCR1A = 2000 + (int)(120 * (2000 / 120)); // 4000
  delay(500);

  // 2. 시계방향(역방향)으로 110도 이동 (0도에서 110도로 이동) → 방향 반대로: 120-110=10도
  //OCR1A = 2000 + (int)(10 * (2000 / 120)); // 2167
  //delay(1000);

  // 3. 초기 위치(90도)로 복귀 → 방향 반대로: 120-90=30도
  OCR1A = 2000 + (int)(30 * (2000 / 120)); // 2500
  delay(500);
}


// --- Status Display LEDs ---
void showLEDs(int player, int ai){
  PORTC |= (1 << player); // 플레이어 선택 ON
  PORTC |= (1 << ai + 3); // 게임기 선택 ON
}


// --- Juding ---
int judge(int player, int ai){
  if (player == ai){
    return 0;
  }
  else if ((player == SCISSORS && ai == PAPER) || (player == ROCK && ai == SCISSORS) || (player == PAPER && ai == ROCK)){
    return 1;
  }
  else {
    return -1;
  }
}


// --- Restart Game ---
void resetGameState(){
  PORTC = 0; // 가위바위보 상태 LED 전부 끄기
  while (Serial.available()) Serial.read(); // 시리얼 버퍼 초기화
}

