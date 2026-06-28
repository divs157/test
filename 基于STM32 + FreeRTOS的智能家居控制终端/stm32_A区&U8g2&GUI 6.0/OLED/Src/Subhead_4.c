#include "stm32f1xx_hal.h"
#include "Subhead_4.h"
#include "Subhead_4-2.h"
#include "Subhead_4-1.h"
#include "usart.h"
#include "menu.h"
#include "GUI.h"
#include "oled.h"				//包含需要的头文件
#include "key.h"


extern QueueHandle_t g_xQueueMenu;

/*	Subhead_4名字	*/
const char* Subhead_4_Name = "Control Switch";

/*	Subhead_4菜单结构体	*/
Menu Subhead_4;	

void Subhead_4_Funsion(void)
{
	GUI_Display(&u8g2,&Subhead_4);
	//	为空	
}

void Subhead_4_Init(void)
{
	Menu_Init(&Subhead_4);
	Subhead_4.Funsion = Subhead_4_Funsion;
	Subhead_4.Name = Subhead_4_Name;
	
	Subhead_4_1_Init();
	Subhead_4_2_Init();
	
	Menu_SetFather(&Subhead_4_1, &Subhead_4);
	Menu_SetFather(&Subhead_4_2, &Subhead_4);

	
}

uint16_t Read_Switch_Status(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin) 
{
    return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}







