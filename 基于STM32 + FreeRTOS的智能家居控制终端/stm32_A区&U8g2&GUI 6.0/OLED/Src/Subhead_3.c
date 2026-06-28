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

extern QueueHandle_t g_xQueueMenu;

extern void Set_Date(u8g2_t *u8g2, uint8_t datePtr, DateComponent component,int8_t minVal, int8_t maxVal, const char *label);

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

void ShowSetTimeTask(void *params)
{
	GUI_SetTime(&u8g2);
	
	
}

void GUI_SetTime(u8g2_t *u8g2)
{
//	TaskHandle_t xTaskHandle = NULL;
//	UBaseType_t freeNum;
	g_xQueueMenu = xQueueCreate(1,sizeof(struct Key));
	struct Key	key_data;
	static int8_t options = 0;
	
	uint8_t draw_param[6][4] = {u8g2_GetStrWidth(u8g2,year_str) + 1,u8g2_GetMaxCharHeight(u8g2),14,9,
									u8g2_GetStrWidth(u8g2,month_str) + 1,u8g2_GetMaxCharHeight(u8g2),46,9,
									u8g2_GetStrWidth(u8g2,date_str) + 1,u8g2_GetMaxCharHeight(u8g2),66,9,
									30,25,14,22,
									30,25,48,22,
									30,25,90,22,
	};
	while(1)
	{
//		xTaskHandle = xTaskGetCurrentTaskHandle();
//		freeNum = uxTaskGetStackHighWaterMark(xTaskHandle);
//		u1_printf("FreeStack of Task %s : %d \n\r",pcTaskGetName(xTaskHandle),freeNum);
		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,0))
		{
			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_CLICK)
			{
				Set_Time(options,u8g2);
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
			{
				LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
				return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_LONG)
			{
				return;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_RIGHT)
			{
				options++;
				if(options>5)
				{
					options=5;
				}
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
			{
				options--;
				if(options<0)
				{
					options=0;
				}
				
			}
		}

		u8g2_ClearBuffer(u8g2);
		
		Time_animation(u8g2);
		
		BlinkDraw(u8g2, EasingDrawBox, draw_param[options][2],draw_param[options][3], draw_param[options], 500,0);	
		u8g2_SendBuffer(u8g2);

	}
	
}


void Set_Time(uint8_t i,u8g2_t *u8g2)
{
	switch(i)
	{
		case 0:
			Set_Date(u8g2,MyRtc.sdate.Year,DATE_COMPONENT_YEAR,0,99,"Year:");
		break;
		case 1:
			Set_Date(u8g2,MyRtc.sdate.Month, DATE_COMPONENT_MONTH, 1, 12, "Month:");
		break;
		case 2:
			Set_Date(u8g2, MyRtc.sdate.Date, DATE_COMPONENT_DAY, 1, 31, "Day:");
		break;
		case 3:
			Set_Date(u8g2, MyRtc.tdate.Hours, DATE_COMPONENT_HOUR, 0, 23, "Hour:");
		break;
		case 4:
			Set_Date(u8g2, MyRtc.tdate.Minutes, DATE_COMPONENT_MINUTE, 0, 59, "Minute:");
		break;
		case 5:
			Set_Date(u8g2, MyRtc.tdate.Seconds, DATE_COMPONENT_SECOND, 0, 59, "Second:");				
		break;
	}
}






