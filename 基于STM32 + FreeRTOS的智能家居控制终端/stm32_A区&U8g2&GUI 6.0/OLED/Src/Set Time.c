#include "stm32f1xx_hal.h"
#include "Subhead_1.h"
#include "Subhead_2.h"
#include "Subhead_3.h"
#include "Subhead_4.h"
#include "Subhead_5.h"
#include "usart.h"
#include "menu.h"
#include "GUI.h"
#include "main.h"             //包含需要的头文件
#include "key.h"

/*	Subhead_3名字	*/
const char* Subhead_3_Name = "Set Time";

/*	Subhead_3菜单结构体	*/
Menu Subhead_3;	

void Subhead_3_Funsion(void)
{
	GUI_SetTime(&u8g2);
}

void Subhead_3_Init(void)
{
	Menu_Init(&Subhead_3);
	Subhead_3.Funsion = Subhead_3_Funsion;
	Subhead_3.Name = Subhead_3_Name;
		
}





