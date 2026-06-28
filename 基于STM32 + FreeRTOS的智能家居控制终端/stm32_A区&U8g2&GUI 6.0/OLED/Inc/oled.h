#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx_hal.h"
#include "u8g2.h"			  //包含需要的头文件

extern char seconds_str[3];                 //C语言，建立字符串变量
extern char minutes_str[3];
extern char hours_str[3];
extern char date_str[3];                 //C语言，建立字符串变量
extern char month_str[3];
extern char year_str[4];
extern char weekDay_str[3];

typedef enum{
  CLEAR_LEFT,
  CLEAR_RIGHT,
  CLEAR_UP,
  CLEAR_DOWN
} LinearClearDirction;

void Boot_animation(void);
void u8g2_Init(u8g2_t *u8g2);
void Time_animation(u8g2_t *u8g2);
void PrintStr_CharByChar(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t* text,uint8_t space, uint16_t delayMS);
void LinearClearScreen(u8g2_t *u8g2, LinearClearDirction direction, uint8_t speed, uint16_t delayMS);

typedef void (*DrawFuncCallback)(u8g2_t *u8g2, uint8_t x, uint8_t y, void* params); //带传参的句柄
 
//画空心矩形
void EasingDrawBox(u8g2_t *u8g2, uint8_t x, uint8_t y, void *params);
 
void BlinkDraw(u8g2_t *u8g2, DrawFuncCallback drawFunc, uint8_t x, uint8_t y, void *params,uint16_t delayMS,uint8_t t) ;

#endif
