#include "stm32f1xx_hal.h"
#include "usart.h"
#include "menu.h"
#include "oled.h"
#include "GUI.h"
#include "Subhead_1.h"
#include "Subhead_1-1.h"
#include "Subhead_1-2.h"
#include "Subhead_1-3.h"
#include "Subhead_2.h"
#include "Subhead_3.h"
#include "Subhead_4.h"
#include "Subhead_5.h"
#include "Headline_1.h"
#include "main.h"             //包含需要的头文件

extern TaskHandle_t xShowConWifiTaskHandle;



/*	Subhead_1名字	*/
const char* Subhead_1_Name = "Connect To Wifi";

/*	Subhead_1菜单结构体	*/
Menu Subhead_1;	

void Subhead_1_Funsion(void)
{
	GUI_Display(&u8g2,&Subhead_1);
}

void Subhead_1_Init(void)
{
	Menu_Init(&Subhead_1);
	Subhead_1.Funsion = Subhead_1_Funsion;
	Subhead_1.Name = Subhead_1_Name;
	
	Subhead_1_1_Init();
	Subhead_1_2_Init();
	Subhead_1_3_Init();
	
	Menu_SetFather(&Subhead_1_1, &Subhead_1);
	Menu_SetFather(&Subhead_1_2, &Subhead_1);
	Menu_SetFather(&Subhead_1_3, &Subhead_1);
}





