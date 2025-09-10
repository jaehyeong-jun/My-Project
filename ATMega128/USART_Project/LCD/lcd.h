/*
 * lcd.h
 *
 * Created: 2025-08-20 오후 1:15:47
 * Author : COMPUTER
 */
#ifndef LCD_H_
#define LCD_H_

#define LCD_WDATA  PORTC   // LCD 데이터
#define LCD_WINST  PORTC   // LCD 명령
#define LCD_CTRL   PORTG   // LCD 제어

#define LCD_RS  0   // LCD RS 신호
#define LCD_RW  1   // LCD RW 신호
#define LCD_EN  2   // LCD E 신호

#define Byte     unsigned char
#define On       1
#define Off      0

//#define TLCD_EN { TLCD_E = 0; TLCD_E = 1; }   // LCD 데이터 신호
//#define DATA PORTC

void Port_Init(void);          // 128a 포트 초기화 함수
void LCD_DATA(Byte);           // LCD에 데이터 입력 함수
void LCD_Comm(Byte);           // LCD 명령 함수
void LCD_CHAR(Byte);           // LCD에 문자 출력 함수
void LCD_STR(Byte*);
void LCD_pos(Byte, Byte);
void LCD_Clear(void);
void LCD_Init(void);				//LCD초기화함수

#endif /* LCD_H_ */	