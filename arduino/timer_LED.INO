#include <DS1302.h>
#include <LiquidCrystal_I2C.h>

// DS1302 핀 설정
#define CLK_PIN 11
#define DATA_PIN 12
#define RST_PIN 13

DS1302 rtc(RST_PIN, DATA_PIN, CLK_PIN);

// LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 버튼 핀 설정
const int button1Pin = 7;
const int button2Pin = 6;
const int button3Pin = 5; // 세 번째 버튼
const int led1Pin = 2;    // 정답 맞췄을 때 켜지는 LED1
const int led2Pin = 3;    // 정답 틀렸을 때 켜지는 LED2

// 상태 변수
bool timerRunning = false;   // 타이머 실행 상태
bool timerPaused = false;    // 타이머 멈춤 상태
bool showingClock = true;    // 날짜, 시간을 보여주는 상태
unsigned long timerStart = 0; // 타이머 시작 시간
unsigned long timerDuration = 0; // 타이머 경과 시간 저장

void setup() {
  rtc.halt(false);
  rtc.writeProtect(false);

  // LCD 초기화
  lcd.begin();
  lcd.backlight();

  // 버튼 및 LED 초기화
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  Serial.begin(9600);

  // RTC 초기 시간 설정 (필요시 주석 해제)
  rtc.setDate(19, 12, 2024); // 일, 월, 연도
  rtc.setDOW(5);             // 목요일
  rtc.setTime(15, 32, 30);   // 시, 분, 초

  // 초기 화면은 날짜와 시간 표시
  printTimeToLCD();
}

void loop() {
  if (showingClock) {
    // 날짜, 시간 화면에서 주기적으로 갱신
    if (millis() % 1000 == 0) {
      printTimeToLCD();
    }
  }

  checkButton1(); // 버튼1 상태 확인
  checkButton2(); // 버튼2 상태 확인
  checkButton3(); // 버튼3 상태 확인

  if (timerRunning) {
    updateTimerDisplay(); // 타이머 화면 갱신
  }

  handleSerialCommands(); // Serial 명령 처리 (LED 및 S 입력)
}

void printTimeToLCD() {
  Time t = rtc.getTime();
  String day = dayAsString(t.dow);

  lcd.setCursor(0, 0);
  lcd.print(t.year);
  lcd.print('-');
  lcd.print(t.mon);
  lcd.print('-');
  lcd.print(t.date);

  lcd.setCursor(0, 1);
  lcd.print(t.hour);
  lcd.print(':');
  lcd.print(t.min);
  lcd.print(':');
  lcd.print(t.sec);
  lcd.print(' ');
  lcd.print(day);
}

String dayAsString(int day) {
  switch (day) {
    case 1: return "Sun";
    case 2: return "Mon";
    case 3: return "Tue";
    case 4: return "Wed";
    case 5: return "Thu";
    case 6: return "Fri";
    case 7: return "Sat";
    default: return "(unknown)";
  }
}

void checkButton1() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(button1Pin);

  if (currentState == LOW && lastState == HIGH) {
    if (showingClock) {
      showingClock = false;
      timerRunning = true;
      timerPaused = false;
      timerStart = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Timer Running...");
      Serial.println("Timer Started");
    } else if (timerRunning) {
      timerRunning = false;
      timerPaused = true;
      timerDuration += millis() - timerStart;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Timer Paused:");
      lcd.setCursor(0, 1);
      lcd.print(timerDuration / 1000);
      lcd.print(" sec");
      Serial.println("Timer Paused");
    } else if (timerPaused) {
      timerRunning = true;
      timerPaused = false;
      timerStart = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Timer Resumed...");
      Serial.println("Timer Resumed");
    }
  }

  lastState = currentState;
}

void checkButton2() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(button2Pin);

  if (currentState == LOW && lastState == HIGH) {
    if (!showingClock) {
      timerRunning = false;
      timerPaused = false;
      showingClock = true;
      timerDuration = 0;
      lcd.clear();
      printTimeToLCD(); // 시계 화면 복귀
      Serial.println("Timer Reset and Returning to Clock");
    }
  }

  lastState = currentState;
}

void checkButton3() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(button3Pin);

  if (currentState == LOW && lastState == HIGH) {
    // 세 번째 버튼 누르면 HTML 페이지 전환 신호 전송
    Serial.println("Change to quiz_index.html");
  }

  lastState = currentState;
}

void handleSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        Serial.print("Received command: "); // 디버깅용
        Serial.println(command);

        if (command == "LED1_ON") {
            Serial.println("Turning on LED1");
            digitalWrite(led1Pin, HIGH);
            delay(2000); 
            digitalWrite(led1Pin, LOW);
            Serial.println("Turning off LED1");
        } else if (command == "LED2_ON") {
            Serial.println("Turning on LED2");
            digitalWrite(led2Pin, HIGH);
            delay(2000); 
            digitalWrite(led2Pin, LOW);
            Serial.println("Turning off LED2");
        }
    }
}

void updateTimerDisplay() {
  unsigned long elapsed = (millis() - timerStart + timerDuration) / 1000;

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(elapsed);
  lcd.print(" sec   ");
}
