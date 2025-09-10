#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include "Lcd/lcd.h"

// ===========================
// Step 모터 시퀀스 정의
// ===========================
unsigned char Step[8] = {0x90, 0x80, 0xc0, 0x40, 0x60, 0x20, 0x30, 0x10}; 

char motor_done = 0;  // 모터가 동작했는지 여부를 저장 (재실행 방지용)

// ===========================
// 키패드 초기화 함수
// 상위 4비트는 출력 (열), 하위 4비트는 입력 (행)으로 설정
// ===========================
void Keypad_Init() {
	DDRD = 0xF0;     // PD7~PD4: 출력, PD3~PD0: 입력
	PORTD = 0x0F;    // 하위 4비트 풀업 저항 활성화
}

// ===========================
// 키패드 입력 처리 함수
// 각 열을 차례로 LOW로 만들고 행에서 신호 확인
// ===========================
char Keypad_GetKey() {
	PORTD = 0x10; _delay_us(5);
	if (PIND & 0x01) return '1';
	if (PIND & 0x02) return '4';
	if (PIND & 0x04) return '7';
	if (PIND & 0x08) return '*';

	PORTD = 0x20; _delay_us(5);
	if (PIND & 0x01) return '2';
	if (PIND & 0x02) return '5';
	if (PIND & 0x04) return '8';
	if (PIND & 0x08) return '0';

	PORTD = 0x40; _delay_us(5);
	if (PIND & 0x01) return '3';
	if (PIND & 0x02) return '6';
	if (PIND & 0x04) return '9';
	if (PIND & 0x08) return '#';

	return 0; // 입력 없음
}

// ===========================
// 메인 루프 시작
// ===========================
int main(void) {
	char input[5] = {0};           // 사용자 입력 저장 버퍼 (4자리 비밀번호)
	uint8_t idx = 0;               // 입력된 키의 인덱스
	char input_mode = 0;           // 현재 동작 모드
	// 0: 대기, 1: 비밀번호 입력, 2: 변경모드(기존 PW), 3: 변경모드(새 PW)

	char password[5] = "1234";     // 초기 비밀번호

	// 외부 초기화 함수들
	Port_Init();                   // 포트 초기화 (정의는 외부에서 이뤄짐)
	LCD_Init();                    // LCD 초기화
	Keypad_Init();                 // 키패드 초기화

	// 초기 LCD 안내 메시지
	LCD_pos(0, 0);
	LCD_STR((unsigned char*)"Welcome *^v^*");
	LCD_pos(1, 0);
	LCD_STR((unsigned char*)"Press*->Enter PW");

	DDRB = 0xFF;    // PORTB: LED 출력용 포트 설정
	DDRF = 0xF0;    // 모터 제어용 상위 4비트 출력 설정
	int t = 0;      // Step 모터용 인덱스

	while (1) {
		char key = Keypad_GetKey();   // 키패드 입력 대기

		if (key) {
			// 1. 비밀번호 입력 모드 진입 (키 '#')
			if (!input_mode && key == '#') {
				LCD_Clear();
				LCD_pos(0, 0);
				LCD_STR((unsigned char*)"Enter Password");
				input_mode = 1;
				idx = 0;
				for (int i = 0; i < 5; i++) input[i] = 0;
				_delay_ms(200);
				continue;
			}

			// 2. 비밀번호 변경 모드 진입 (키 '*')
			if (!input_mode && key == '*') {
				LCD_Clear();
				LCD_pos(0, 0);
				LCD_STR((unsigned char*)"Enter Current PW");
				input_mode = 2;
				idx = 0;
				for (int i = 0; i < 5; i++) input[i] = 0;
				_delay_ms(200);
				continue;
			}

			// 3. 입력 중일 때 숫자만 처리 ('#', '*'는 무시)
			if (input_mode && key != '#' && key != '*') {
				if (idx == 0) {
					LCD_Clear();
					LCD_pos(0, 0);
				}
				LCD_CHAR('*');  // 보안을 위해 입력값은 '*'로 출력

				if (idx < 4) {
					input[idx++] = key;
					input[idx] = '\0';
				}

				// 4자리 입력 완료 시 처리
				if (idx == 4) {
					if (input_mode == 1) {
						// ===== 비밀번호 검증 모드 (#) =====
						if (!strcmp(input, password)) {
							LCD_Clear();
							LCD_pos(0, 0);
							LCD_STR((unsigned char*)"Door Open");
							PORTB = 0xFF;  // LED ON

							// 모터 동작 (한 번만 실행)
							if (!motor_done) {
								for (int i = 0; i < 36; i++) {
									PORTF = Step[t];
									t = (t + 1) % 8;
									_delay_ms(10);
								}
								_delay_ms(1000);
								for (int i = 0; i < 36; i++) {
									PORTF = Step[t];
									t = (t - 1 + 8) % 8;
									_delay_ms(10);
								}
								_delay_ms(1000);
								motor_done = 1;
							}
						} else {
							// ===== 비밀번호 틀림 =====
							LCD_Clear();
							LCD_pos(0, 0);
							LCD_STR((unsigned char*)"Try again");

							// LED 깜빡이기
							for (int i = 0; i < 2; i++) {
								PORTB = 0xFF;
								_delay_ms(500);
								PORTB = 0x00;
								_delay_ms(500);
							}
						}

						// 초기 상태로 복귀
						_delay_ms(1000);
						LCD_Clear();
						LCD_pos(0, 0);
						LCD_STR((unsigned char*)"Welcome *^v^*");
						LCD_pos(1, 0);
						LCD_STR((unsigned char*)"Press*->Enter PW");
						PORTB = 0x00;
						motor_done = 0;
						input_mode = 0;
						idx = 0;
						for (int i = 0; i < 5; i++) input[i] = 0;

					} else if (input_mode == 2) {
						// ===== 변경모드 - 현재 PW 확인 =====
						if (!strcmp(input, password)) {
							LCD_Clear();
							LCD_pos(0, 0);
							LCD_STR((unsigned char*)"Enter New PW");
							input_mode = 3;
							idx = 0;
							for (int i = 0; i < 5; i++) input[i] = 0;
						} else {
							LCD_Clear();
							LCD_pos(0, 0);
							LCD_STR((unsigned char*)"Wrong PW");
							_delay_ms(1000);
							// 다시 대기화면
							LCD_Clear();
							LCD_pos(0, 0);
							LCD_STR((unsigned char*)"Welcome *^v^*");
							LCD_pos(1, 0);
							LCD_STR((unsigned char*)"Press*->Enter PW");
							input_mode = 0;
							idx = 0;
							for (int i = 0; i < 5; i++) input[i] = 0;
						}
					} else if (input_mode == 3) {
						// ===== 새 비밀번호 저장 =====
						for (int i = 0; i < 4; i++) {
							password[i] = input[i];
						}
						password[4] = '\0';

						LCD_Clear();
						LCD_pos(0, 0);
						LCD_STR((unsigned char*)"PW Changed!");
						_delay_ms(1500);

						// 초기 화면으로 돌아감
						LCD_Clear();
						LCD_pos(0, 0);
						LCD_STR((unsigned char*)"Welcome *^v^*");
						LCD_pos(1, 0);
						LCD_STR((unsigned char*)"Press*->Enter PW");

						input_mode = 0;
						idx = 0;
						for (int i = 0; i < 5; i++) input[i] = 0;
					}
				}
				_delay_ms(200);  // 키 입력 딜레이
			}
		}
	}
}
