/*
 * Project1.c
 *
 * Created: 2025-08-22 오전 9:11:38
 * Author : COMPUTER
 */ 

#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include "Lcd/lcd.h"

unsigned char Step[8]
= {0x90, 0x80, 0xc0, 0x40, 0x60, 0x20, 0x30, 0x10};		//모터

char motor_done = 0;  // 모터 실행 여부 저장

void Keypad_Init() {
	DDRD = 0xF0; // 상위 4비트 출력 (열), 하위 4비트 입력 (행)
	PORTD = 0x0F; // 풀업 저항 활성화
}

char Keypad_GetKey() {
	// 첫 번째 열 출력
	PORTD = 0x10;
	_delay_us(5);
	if (PIND & 0x01) return '1';
	if (PIND & 0x02) return '4';
	if (PIND & 0x04) return '7';
	if (PIND & 0x08) return '*';

	// 두 번째 열 출력
	PORTD = 0x20;
	_delay_us(5);
	if (PIND & 0x01) return '2';
	if (PIND & 0x02) return '5';
	if (PIND & 0x04) return '8';
	if (PIND & 0x08) return '0';

	// 세 번째 열 출력
	PORTD = 0x40;
	_delay_us(5);
	if (PIND & 0x01) return '3';
	if (PIND & 0x02) return '6';
	if (PIND & 0x04) return '9';
	if (PIND & 0x08) return '#';

	return 0; // 아무 키도 눌리지 않음
}

int main(void) {
	//Byte str[] = "Welcome *^v^*";									123123123
	char input[5] = {0}; // 입력한 키 저장용 (4글자 + 종료문자)
	uint8_t idx = 0;
char input_mode = 0;
	Port_Init();
	LCD_Init();
	Keypad_Init();

	LCD_pos(0, 0);
	LCD_STR((unsigned char*)"Welcome *^v^*");					//LCD_STR(str);			123123123
	
	LCD_pos(1, 0);
	LCD_STR((unsigned char*)"Press*->Enter PW");

	DDRB = 0xFF;	//LED

	int t=0;	//모터	
	   DDRF = 0xF0;

while (1) {
	char key = Keypad_GetKey();

	if (key) {
		// # 버튼으로 입력 시작
		if (!input_mode && key == '#') {
			LCD_Clear();
			LCD_pos(0, 0);
			LCD_STR((unsigned char*)"Enter Password");
			LCD_Clear();
			input_mode = 1;
			idx = 0;
			for (int i = 0; i < 5; i++) input[i] = 0;
			_delay_ms(200);
			continue;
		}

		// 입력 모드 중일 때만 입력 처리
		if (input_mode && key != '#' && key != '*') {
			LCD_CHAR('*');

			if (idx < 4) {
				input[idx++] = key;
				input[idx] = '\0';
			}

			// 4자리 입력 완료
			if (idx == 4) {
				if (input[0] == '1' && input[1] == '2' && input[2] == '3' && input[3] == '4') {
					LCD_Clear();
					LCD_pos(0, 0);
					LCD_STR((unsigned char*)"Door Open");
					PORTB = 0xFF;

					if (!motor_done) {
						for (int i = 0 ; i < 36 ; i++) {
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
					LCD_Clear();
					LCD_pos(0, 0);
					LCD_STR((unsigned char*)"Try again");
					PORTB = 0xFF;
					_delay_ms(500);
					PORTB = 0x00;
					_delay_ms(500);
					PORTB = 0xFF;
					_delay_ms(500);
					PORTB = 0x00;
					_delay_ms(500);
				}

				// 상태 초기화
				_delay_ms(1000);
				LCD_Clear();
				LCD_pos(0, 0);
				LCD_STR((unsigned char*)"Welcome *^v^*");
				LCD_pos(1, 0);
				LCD_STR((unsigned char*)"Press*->EnterPW");
				PORTB = 0x00;
				motor_done = 0;
				input_mode = 0;
				idx = 0;
				for (int i = 0; i < 5; i++) input[i] = 0;
			}
			_delay_ms(200);
		}
	}
	}
	

}
