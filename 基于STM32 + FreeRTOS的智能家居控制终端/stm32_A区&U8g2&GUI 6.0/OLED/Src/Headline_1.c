#include "stm32f1xx_hal.h"
#include "usart.h"
#include "menu.h"
#include "Subhead_1.h"
#include "Subhead_2.h"
#include "Subhead_3.h"
#include "Subhead_4.h"
#include "Subhead_5.h"
#include "Headline_1.h"
#include "main.h"


extern TaskHandle_t xShowHomeTaskHandle;

/**
  * 函    数：游戏菜单的功能函数
  * 参    数：无
  * 返 回 值：无
  * 说    明：用于游戏菜单的功能函数
  */
void Headline_1_Funsion(void)
{
	
}



/*全局变量*********************/

/*	Headline_1结构体	*/
Menu Headline_1;	



/*	Headline名字	*/
const char* Headline_1_Name = "Home";



/*********************全局变量*/

void Headline_1_Init(void)
{
	Menu_Init(&Headline_1);
	Headline_1.Funsion = Headline_1_Funsion;
	Headline_1.Name = Headline_1_Name;
	
	Menu_SetFather(&Subhead_1, &Headline_1);
	Menu_SetFather(&Subhead_2, &Headline_1);
	Menu_SetFather(&Subhead_3, &Headline_1);
	Menu_SetFather(&Subhead_4, &Headline_1);
	Menu_SetFather(&Subhead_5, &Headline_1);
}

void ShowHomeTask(void *params)
{
	GUI_Display(&u8g2,&Headline_1);
}





