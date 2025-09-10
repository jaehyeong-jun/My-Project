/*
 * lcd.c
 *
 * Created: 2025-08-20 오후 1:16:06
 * Author : COMPUTER
 */ 
/*
#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

void Port_Init(void)
{
	DDRG = 0x0F;    // 포트 G를 제어핀 출력으로 설정
	DDRC = 0xFF;    // 포트 C 출력으로 설정
}

// LCD에 데이터를 전송하는 함수이다
void LCD_DATA(Byte data)
{
	LCD_CTRL |= (1 << LCD_RS);
	LCD_CTRL &= ~(1 << LCD_RW);
	LCD_CTRL |= (1 << LCD_EN);
	_delay_us(50);
	LCD_WDATA = data;
	_delay_us(50);
	LCD_CTRL &= ~(1 << LCD_EN);
}

    LCD_Comm(0x80 + (row * 0x40) + col);
    }

    void LCD_Clear(void)
    {
	    LCD_Comm(0x01);
	    _delay_ms(2);
    }

    // LCD 초기화 함수이다
    void LCD_Init(void)
    {
	    LCD_Comm(0x38);
	    _delay_ms(2);
	    LCD_Comm(0x38);
	    _delay_ms(2);
	    LCD_Comm(0x38);
	    _delay_ms(2);
	    LCD_Comm(0x0E);
	    _delay_ms(2);
	    LCD_Comm(0x06);
	    _delay_ms(2);
	    LCD_Clear();
    }

*/
#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>      // 딜레이 헤더 파일

#define LCD_WDATA PORTC      // LCD 데이터 포트 정의
#define LCD_WINST PORTC
#define LCD_CTRL PORTG       // LCD 제어 포트 정의
#define LCD_EN 2             // LCD 제어(PING0=2) 정의
#define LCD_RW 1
#define LCD_RS 0

#define Byte char   // Byte 선언
#define On 1
#define Off 0

void Port_Init(void) {
	DDRC = 0xFF;	//DDRD = 0xFF;
	DDRG = 0x0F;	//DDRF = 0x0F;
}


void LCD_Data(Byte ch) {
	LCD_CTRL |= (1 << LCD_RS);  // RS=1, R/W=0 → 데이터 쓰기 싸이클
	LCD_CTRL &= ~(1 << LCD_RW);
	LCD_CTRL |= (1 << LCD_EN);  // LCD 사용
	_delay_us(50);
	LCD_WDATA = ch;             // 데이터 출력
	_delay_us(50);
	LCD_CTRL &= ~(1 << LCD_EN); // LCD 사용안함
}

void LCD_Comm(Byte ch) {
	// PG0-EN, PG1-RW, PG2-RS, PG4-TOSC1핀(사용안함)
	LCD_CTRL &= ~(1 << LCD_RS); // RS=0, R/W=0로 정의
	LCD_CTRL &= ~(1 << LCD_RW);
	LCD_CTRL |= (1 << LCD_EN);  // LCD 사용함
	_delay_us(50);
	LCD_WINST = ch;             // 명령어 쓰기
	_delay_us(50);
	LCD_CTRL &= ~(1 << LCD_EN); // LCD 사용안함
}

void LCD_CHAR(Byte c) {
	// CGROM 문자코드의 0x31 ~ 0xFF = 아스키코드와 일치함
	LCD_Data(c);
	_delay_ms(1);
}

void LCD_STR(Byte *str) {
	// 문자열을 한문자씩 출력함수로 전달
	while (*str != 0) {
		LCD_CHAR(*str);
		str++;
	}
}

void LCD_pos(unsigned char col, unsigned char row) {
	// LCD 표시위치 설정
	LCD_Comm(0x80 | (row+col*0x40)); // row=행 / col=열 , DDRAM주소 설정
}

void LCD_Clear(void) {
	// 화면 클리어
	LCD_Comm(0x01);
	_delay_ms(2); // 1.6ms 이상의 실행시간으로 딜레이 필요
}

void LCD_Init(void) {  // LCD 초기화
	LCD_Comm(0x38); // 데이터 8비트 사용, 5X7도트, LCD2열로 사용(6)
	_delay_ms(2);
	LCD_Comm(0x38);
	_delay_ms(2);
	LCD_Comm(0x38);
	_delay_ms(2);
	LCD_Comm(0x0e); // Display ON/OFF
	_delay_ms(2);
	LCD_Comm(0x06); // 주소 +1, 커서를 우측으로 이동 (3)
	_delay_ms(2);
	LCD_Clear();
}
