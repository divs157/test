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

/*	Subhead_1-3名字	*/

extern QueueHandle_t g_xQueueMenu;


extern TaskHandle_t xSmartconfigTaskHandle;
extern TaskHandle_t xConWifiTaskHandle;

extern void ConWifiTask(void *params);

extern void SmartconfigTask(void *params);

const char* Subhead_1_3_Name = "AirKiss";

/*	Subhead_1菜单结构体	*/
Menu Subhead_1_3;	

void Subhead_1_3_Funsion(void)
{
	xTaskCreate(SmartconfigTask, "SmartconfigTask", 128, NULL, osPriorityNormal,&xSmartconfigTaskHandle);
//	vTaskSuspend(xShowTimeTaskHandle);
}

void Subhead_1_3_Init(void)
{
	Menu_Init(&Subhead_1_3);
	Subhead_1_3.Funsion = Subhead_1_3_Funsion;
	Subhead_1_3.Name = Subhead_1_3_Name;
}

void SmartconfigTask(void *params)
{
	
	if(WiFi_Smartconfig(60)) //配网
	{
		
		vTaskDelete(xSmartconfigTaskHandle);
	}
	else
	{
		xTaskCreate(ConWifiTask, "ConWifiTask", 128, NULL, osPriorityNormal+1,&xConWifiTaskHandle);
		vTaskDelay(1000);
		vTaskDelete(xSmartconfigTaskHandle);

	}
	
//	g_xQueueMenu = xQueueCreate(1,sizeof(struct Key));
//	struct Key	key_data;
//	
//	while(1)
//	{	
//		
//		u8g2_ClearBuffer(&u8g2);
//		u8g2_DrawRFrame(&u8g2, 10, 10, 108, 40, 7);
//		u8g2_DrawStr(&u8g2, 13, 23, "Open AirKiss...");
//		u8g2_SendBuffer(&u8g2);
//		
//		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,portMAX_DELAY))
//		{
//			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_CLICK)
//			{
//				
//			}
//			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
//			{
//				LinearClearScreen(&u8g2, CLEAR_LEFT, 10, 10);
//				return;
//			}
//			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_LONG)
//			{
//				LinearClearScreen(&u8g2, CLEAR_LEFT, 10, 10);
//				return;
//			}
//			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_RIGHT)
//			{
//				
//			}
//			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
//			{
//				
//				
//			}
//		}
		
					
//	}
}



