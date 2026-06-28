#include "stm32f1xx_hal.h"
#include "usart.h"
#include "menu.h"
#include "oled.h"
#include "GUI.h"
#include "Subhead_4.h"
#include "main.h"             //包含需要的头文件
#include "iot.h"              //包含需要的头文件


extern QueueHandle_t g_xQueueMenu;

extern void Control_Switch_1(u8g2_t *u8g2);

/*	Subhead_1名字	*/
const char* Subhead_4_1_Name = "Switch_1";

/*	Subhead_1菜单结构体	*/
Menu Subhead_4_1;	

void Subhead_4_1_Funsion(void)
{
	Control_Switch_1(&u8g2);
}

void Subhead_4_1_Init(void)
{
	Menu_Init(&Subhead_4_1);
	Subhead_4_1.Funsion = Subhead_4_1_Funsion;
	Subhead_4_1.Name = Subhead_4_1_Name;
}

void Control_Switch_1(u8g2_t *u8g2)
{
	g_xQueueMenu = xQueueCreate(1,sizeof(struct Key));
	struct Key	key_data;
	u8g2_SetFontPosTop(u8g2);
    while(1)
    {
        u8g2_ClearBuffer(u8g2);
		
		u8g2_DrawRFrame(u8g2, 80, 32, 26, 12, (float)0.7); // x, y, width, height, radius
        u8g2_DrawRFrame(u8g2, 10, 10, 108, 40, 7);
		u8g2_DrawStr(u8g2, 13, 13, "Status:");

		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == RESET)
		{
			u8g2_DrawRBox(u8g2, 82, 34, 22, 8, (float)0.7); // x, y, width, height, radius	
			u8g2_DrawStr(u8g2, 84, 13, "ON");	
		}
		else
		{
			u8g2_DrawRBox(u8g2, 82, 34, 12, 8, (float)0.7); // x, y, width, height, radius
			u8g2_DrawStr(u8g2, 84, 13, "OFF");	
		}
		
		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,0))
		{
			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_CLICK)
			{
				LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
			{
				
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_LONG)
			{
				
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_RIGHT)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
			}
		}
		

     
        u8g2_SendBuffer(u8g2);
    }
}



