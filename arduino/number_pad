#include <Keypad.h>

// 키패드 설정
const byte ROWS = 4; // 키패드의 행 개수
const byte COLS = 4; // 키패드의 열 개수
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {3, 4, 5, 6}; // 행 핀 연결
byte colPins[COLS] = {7, 8, 9, 10}; // 열 핀 연결
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    Serial.begin(9600); // 시리얼 통신 시작
}

void loop() {
    char key = keypad.getKey(); // 키패드 입력 감지

    if (key) { // 키 입력이 있으면
        handleKeyInput(key); // 키 입력 처리
    }
}

// 키 입력을 처리하는 함수
void handleKeyInput(char key) {
    if (key) { // 키 입력이 감지되면
        if (key == '8') { // 위로 이동
            Serial.println("U"); // 위로 이동 명령
        } else if (key == '2') { // 아래로 이동
            Serial.println("W"); // 아래로 이동 명령
        } else if (key == '5') { // 선택
            Serial.println("S"); // 선택 명령
        } else {
            Serial.print("Key Pressed: ");
            Serial.println(key); // 입력된 키 출력
        }
    }
}