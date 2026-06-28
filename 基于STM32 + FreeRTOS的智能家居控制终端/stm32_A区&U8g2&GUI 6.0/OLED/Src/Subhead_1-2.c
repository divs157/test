#include "stm32f1xx_hal.h"
#include "usart.h"
#include "menu.h"
#include "oled.h"
#include "GUI.h"
#include "Subhead_1.h"
#include "Subhead_2.h"
#include "Subhead_3.h"
#include "Subhead_4.h"
#include "Subhead_5.h"
#include "Headline_1.h"
#include "main.h"             //包含需要的头文件
#include "wifi.h"	          //包含需要的头文件

/*	Subhead_1名字	*/


extern QueueHandle_t g_xQueueMenu;

const char* Subhead_1_2_Name = "Wifi Mode";

/*	Subhead_1菜单结构体	*/
Menu Subhead_1_2;	

void Subhead_1_2_Funsion(void)
{
	
	Set_Wifi(&u8g2,&wifi_mode);
}

void Subhead_1_2_Init(void)
{
	Menu_Init(&Subhead_1_2);
	Subhead_1_2.Funsion = Subhead_1_2_Funsion;
	Subhead_1_2.Name = Subhead_1_2_Name;
}



void Set_Wifi(u8g2_t *u8g2,uint8_t *wifi_mode)
{
	g_xQueueMenu = xQueueCreate(1,sizeof(struct Key));
	struct Key	key_data;
	u8g2_SetFontPosTop(u8g2);
	uint8_t mode = *wifi_mode;
	char dateStr[1];
    while(1)
    {
        u8g2_ClearBuffer(u8g2);
		
		u8g2_DrawRFrame(u8g2, 80, 32, 26, 12, (float)0.7); // x, y, width, height, radius
        u8g2_DrawRFrame(u8g2, 10, 10, 108, 40, 7);
		u8g2_DrawStr(u8g2, 13, 13, "Mode:");
		strcpy(dateStr, u8x8_u8toa(*wifi_mode, 1));
		u8g2_DrawStr(u8g2, 90, 13, dateStr);	
		
        
		
		if(*wifi_mode == 1)
		{
			u8g2_DrawRBox(u8g2, 82, 34, 22, 8, (float)0.7); // x, y, width, height, radius	
		}
		else
		{
			u8g2_DrawRBox(u8g2, 82, 34, 12, 8, (float)0.7); // x, y, width, height, radius
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
				*wifi_mode = mode;
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_LONG)
			{
				*wifi_mode = mode;
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_RIGHT)
			{
				*wifi_mode = 0;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
			{
				*wifi_mode = 1;
				
			}
		}
     
        u8g2_SendBuffer(u8g2);
    }
}





