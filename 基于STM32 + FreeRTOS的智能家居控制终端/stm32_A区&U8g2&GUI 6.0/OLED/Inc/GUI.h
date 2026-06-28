#ifndef __GUI_H__
#define __GUI_H__

#include "stm32f1xx_hal.h" 
#include "main.h"
#include "menu.h"

enum CursorStyle
{
	reverse,
	mouse,
	frame,
};

struct MenuProperty
{
	int8_t Catch_i;		
	int8_t Show_i; 		
	int8_t Cursor_i;	
	int8_t Incident;
	float Cursor_len_d0;
	float Cursor_len_d1;
	float Cursor_i_d0;
	float Cursor_i_d1;
	float Cursor_bar_0;
	float Cursor_bar_1;
	float Cursor_clock_0;
	float Cursor_clock_1;
	
	int8_t Show_d;
	int8_t Show_i_temp;
	enum CursorStyle CursorStyle; 
	float Slide_ActSpeed;		  
	
	int8_t Window_X;				
	int8_t Window_Y;                
	uint8_t Window_W;               
	uint8_t Window_H;               
};



enum{
	GUI_COMMAND_NONE,	
	GUI_COMMAND_UP,		
	GUI_COMMAND_DOWN,	
	GUI_COMMAND_LEFT,	
	GUI_COMMAND_RIGHT	
};

enum{
	
	LONG_PRESS = 4,
	DOUBLE_CLICK = 3,
	SINGLE_CLICK = 2,
	
	Turn_left = 1,
	NULL_CLICK = 0,
	Turn_right = -1,
	
};

void GUI_Init(void);

void Open_GUI(void);
void GUI_ReceiveCommand(uint8_t Command);

void GUI_Display(u8g2_t *u8g2,Menu *MU);
void ShowTimeTask(void *params);
void cesi(u8g2_t *u8g2,Menu *MU);
void GUI_SetTime(u8g2_t *u8g2);

#endif
