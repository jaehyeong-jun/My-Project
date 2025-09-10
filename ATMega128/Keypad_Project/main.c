/*
 * Project1.c
 *
 * Created: 2025-08-22 오전 9:11:38
 * Author : COMPUTER
 *
 * 기능: 
 *  - UART 시리얼 통신으로 비밀번호 입력을 받은 후
		1. LCD에 상태 표시
		2. 맞으면 스텝 모터를 작동시키고 LED 점등
		3. 틀리면 LED 점멸 및 다시 시도 유도
		4. 터미널에도 상태 메시지 출력 (에코 및 결과 메시지)
 */

#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include "Lcd/lcd.h"	//lcd.h 파일 가져오기

unsigned char Step[8] = {0x90, 0x80, 0xc0, 0x40, 0x60, 0x20, 0x30, 0x10};

char motor_done = 0;  // 모터가 이미 동작했는지 여부 저장

// USART1 초기화 함수 (통신 속도 설정, 송수신 활성화)
void Init_USART()
{
	DDRE = 0x02;  // PE 0번핀 입력, 1번 핀 출력 (USART1 통신)
	UCSR0A = 0x00;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);    // 송신, 수신 활성화
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 데이터 비트 8비트 설정
	UBRR0H = 0x00;
	UBRR0L = 0x07; // 보레이트 설정 (115200bps에 가까움)
}

// USART1로 1바이트 송신 함수
void USART0_tx(unsigned char data)
{
	while(!(UCSR0A & (1 << UDRE0))); // 전송 버퍼가 비어질 때까지 대기
	UDR0 = data;                    // 데이터 전송
}

// UART로 1바이트 수신 함수 (수신될 때까지 대기)
unsigned char USART0_rx()
{
	while(!(UCSR0A & (1 << RXC0))); // 데이터 수신될 때까지 대기
	return UDR0;                    // 받은 데이터 반환
}

// UART로 문자열 송신 함수 ('\0' 종료까지 송신)
void USART0_str(unsigned char* str)
{
	while(*str){
		USART0_tx(*str++);
	}
}

int main(void) {
	
	Init_USART();    // UsART1 초기화
	
	DDRB = 0xFF;     // PORTB를 출력으로 설정 (LED 제어용)
	PORTB = 0x00;    // LED OFF 초기화
	
	char input[5] = {0}; // 입력한 비밀번호(4자리 + 종료문자)
	uint8_t idx = 0;     // 입력 인덱스
	char input_mode = 0; // 입력 모드 플래그 (0:입력 안함, 1:입력 중)
	
	Port_Init();    // LCD 및 기타 포트 초기화 함수 (외부 헤더에 정의됨)
	LCD_Init();     // LCD 초기화 함수
	
	// 초기 LCD 화면 출력
	LCD_pos(0, 0);
	LCD_STR((unsigned char*)"Welcome *^v^*");
	LCD_pos(1, 0);
	LCD_STR((unsigned char*)"Press*->Enter PW");
	
	DDRB = 0xFF;    // LED용 출력 핀 설정 (중복 설정)
	int t=0;        // 모터 단계 변수
	DDRF = 0xF0;    // PORTF 상위 4비트 출력 (스텝모터 제어용)
	
	while (1) {
		unsigned char Tkey = USART0_rx();	// USART1로 키 입력 대기
		USART0_tx(Tkey);						// 입력받은 키를 그대로 터미널에 다시 출력 (에코)
		
		if (Tkey) {
			// # 키를 누르면 비밀번호 입력 시작 모드로 전환
			if (!input_mode && Tkey == '#') {
				LCD_Clear();
				LCD_pos(0, 0);
				LCD_STR((unsigned char*)"Enter Password");
				LCD_Clear();
				input_mode = 1;    // 입력 모드 ON
				idx = 0;           // 입력 인덱스 초기화
				for (int i = 0; i < 5; i++) input[i] = 0; // 입력 배열 초기화
				_delay_ms(200);
				continue;
			}
			
			// 입력 모드 중일 때 숫자 입력 처리 (#, * 제외)
			if (input_mode && Tkey != '#' && Tkey != '*') {
				LCD_CHAR('*');    // 입력 키 대신 '*' 문자 LCD에 표시 (비밀번호 보안)
				
				if (idx < 4) {
					input[idx++] = Tkey;  // 입력값 저장
					input[idx] = '\0';    // 문자열 종료 문자 추가
				}
				
				// 4자리 입력 완료시 비밀번호 확인
				if (idx == 4) {
					if (input[0] == '1' && input[1] == '2' && input[2] == '3' && input[3] == '4') {
						// 비밀번호가 1234 입력 됐을 때.
						LCD_Clear();
						LCD_pos(0, 0);
						LCD_STR((unsigned char*)"Door Open");
						PORTB = 0xFF;  // LED ON (성공 표시)
						
						USART0_str((unsigned char*)"Password correct. Door Open.\r\n"); // 터미널 출력
						
						if (!motor_done) { // 모터가 동작하지 않았으면
							// 스텝 모터 정방향 36스텝 (회전)
							for (int i = 0 ; i < 36 ; i++) {
								PORTF = Step[t];
								t = (t + 1) % 8;
								_delay_ms(10);
							}
							_delay_ms(1000);
							// 스텝 모터 역방향 36스텝 (복귀)
							for (int i = 0; i < 36; i++) {
								PORTF = Step[t];
								t = (t - 1 + 8) % 8;
								_delay_ms(10);
							}
							_delay_ms(1000);
							
							motor_done = 1; // 모터 동작 완료 플래그 설정
						}
					} else {
						// 비밀번호 틀렸을 때
						LCD_Clear();
						LCD_pos(0, 0);
						LCD_STR((unsigned char*)"Try again");
						
						PORTB = 0xFF; // LED 점멸로 오류 표시
						_delay_ms(500);
						PORTB = 0x00;
						_delay_ms(500);
						PORTB = 0xFF;
						_delay_ms(500);
						PORTB = 0x00;
						_delay_ms(500);
						
						USART0_str((unsigned char*)"Incorrect password. Try again.\r\n"); // 터미널 출력
					}
					
					// 입력 상태 초기화 및 초기 화면 복귀
					_delay_ms(1000);
					LCD_Clear();
					LCD_pos(0, 0);
					LCD_STR((unsigned char*)"Welcome *^v^*");
					LCD_pos(1, 0);
					LCD_STR((unsigned char*)"Press*->EnterPW");
					
					PORTB = 0x00; // LED OFF
					motor_done = 0; // 모터 동작 플래그 초기화
					input_mode = 0;  // 입력 모드 종료
					idx = 0;         // 입력 인덱스 초기화
					for (int i = 0; i < 5; i++) input[i] = 0; // 입력 버퍼 초기화
				}
				_delay_ms(200);
			} 
		}
	}
}
