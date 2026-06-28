#include "menu.h"
#include "oled.h"
#include "stm32f1xx_hal.h" 
#include <string.h>
#include "Key.h"
#include "Encoder.h"
#include "GUI.h"
#include "Subhead_1.h"
#include "Subhead_2.h"
#include "Subhead_3.h"
#include "Subhead_4.h"
#include "Subhead_5.h"
#include "Headline_1.h"

extern int16_t Key_Get(void);

extern QueueHandle_t g_xQueueMenu;
extern QueueHandle_t g_xQueueReceiver;	

extern TaskHandle_t xMyIotTaskHandle;
extern TaskHandle_t xShowTimeTaskHandle;
extern TaskHandle_t xShowHomeTaskHandle;
extern TaskHandle_t xShowConWifiTaskHandle;
extern TaskHandle_t xShowSetTimeTaskHandle;
extern TaskHandle_t xShowSetDataTaskHandle;
extern TaskHandle_t xConWifiTaskHandle;
extern TaskHandle_t xShowMatrixTaskHandle;
extern TaskHandle_t xSmartconfigTaskHandle;

static uint8_t g_QueueReceiverBuf[10*sizeof(struct Receiver)];
static StaticQueue_t g_xQueueReceiverStaticstruct;


struct MenuProperty Menu_Global = {
	
	.Catch_i = 0,	
	.Show_i= 0,		
	.Cursor_i= 0,	
	.Incident= 0,	
	.Cursor_len_d0= 0,
	.Cursor_len_d1= 0,
	.Cursor_i_d0= 0,
	.Cursor_i_d1= 0,
	.Show_d= 0,
	.Show_i_temp= 0,
	.CursorStyle = reverse,	 
	.Slide_ActSpeed = 8,	 
	.Cursor_clock_0 = 43,
	.Cursor_clock_1 = 0,
	
	.Window_X = 0,	 
	.Window_Y = 16,	 
	.Window_W = 128, 
	.Window_H = 48,	 
};


void GUI_Init(void)
{
	Headline_1_Init();
	
	Subhead_1_Init();
	Subhead_2_Init();
	Subhead_3_Init();
	Subhead_4_Init();
	Subhead_5_Init();
}


//static void GUI_ToChild(void)
//{
//	Menu_ToChild();
//}


//static void GUI_ToFunsion(void)
//{
//	Menu_ToFunsion();
//}


void Menu_ShowCursor(Menu Page,enum CursorStyle CurStyle,u8g2_t *u8g2)
{
	Menu_Global.Cursor_i_d1 = (Menu_Global.Cursor_i) * WORD_H;					
	Menu_Global.Cursor_len_d1 = strlen(Page.Child[Menu_Global.Catch_i].Name) * 8 +2.5;
	Menu_Global.Cursor_bar_1 = Menu_Global.Catch_i*64/Page.len;
	

	if((Menu_Global.Cursor_bar_1 - Menu_Global.Cursor_bar_0) > 1) {Menu_Global.Cursor_bar_0 += (Menu_Global.Cursor_bar_1 - Menu_Global.Cursor_bar_0) / Menu_Global.Slide_ActSpeed + 1;}		//如果当前位置不是盿标位罿,当前位置向目标位罿移�?它们之间距翻乘速度的倒数;
	else if((Menu_Global.Cursor_bar_1 - Menu_Global.Cursor_bar_0) < -1) {Menu_Global.Cursor_bar_0 += (Menu_Global.Cursor_bar_1 - Menu_Global.Cursor_bar_0) / Menu_Global.Slide_ActSpeed - 1;}
	else {Menu_Global.Cursor_bar_0 = Menu_Global.Cursor_bar_1;}

	if((Menu_Global.Cursor_i_d1 - Menu_Global.Cursor_i_d0) > 1) {Menu_Global.Cursor_i_d0 += (Menu_Global.Cursor_i_d1 - Menu_Global.Cursor_i_d0) / Menu_Global.Slide_ActSpeed + 1;}		//如果当前位置不是�?标位�?,当前位置向目标位�?移动 它们之间距�?�乘速度的倒数;
	else if((Menu_Global.Cursor_i_d1 - Menu_Global.Cursor_i_d0) < -1) {Menu_Global.Cursor_i_d0 += (Menu_Global.Cursor_i_d1 - Menu_Global.Cursor_i_d0) / Menu_Global.Slide_ActSpeed - 1;}
	else {Menu_Global.Cursor_i_d0 = Menu_Global.Cursor_i_d1;}
	
	if((Menu_Global.Cursor_len_d1 - Menu_Global.Cursor_len_d0) > 1) {Menu_Global.Cursor_len_d0 += (Menu_Global.Cursor_len_d1 - Menu_Global.Cursor_len_d0) / Menu_Global.Slide_ActSpeed+1;}
	else if((Menu_Global.Cursor_len_d1 - Menu_Global.Cursor_len_d0) < -1) {Menu_Global.Cursor_len_d0 += (Menu_Global.Cursor_len_d1 - Menu_Global.Cursor_len_d0) / Menu_Global.Slide_ActSpeed - 1;}
	else {Menu_Global.Cursor_len_d0 = Menu_Global.Cursor_len_d1;}
	
	if (CurStyle == reverse)
	{
		u8g2_SetDrawColor(u8g2,2);
		u8g2_DrawRBox(u8g2,0, Menu_Global.Cursor_i_d0, Menu_Global.Cursor_len_d0, WORD_H,(float)0.8);
	}
	else if (CurStyle == mouse)
	{
		u8g2_SetFont(u8g2,u8g2_font_6x13_tf);
		u8g2_DrawStr(u8g2,Menu_Global.Cursor_len_d0+1, Menu_Global.Cursor_i_d0+2,"<-"); 
		
	}
	else if (CurStyle == frame)
	{
		u8g2_DrawRFrame(u8g2,0, Menu_Global.Cursor_i_d0, Menu_Global.Cursor_len_d0, WORD_H, 3);
	}
	u8g2_DrawRBox(u8g2,125,Menu_Global.Cursor_bar_0,2,64/Page.len,1);
}

void GUI_Display(u8g2_t *u8g2,Menu *Page)
{
//	TaskHandle_t xTaskHandle = NULL;
//	UBaseType_t freeNum;
	g_xQueueMenu = xQueueCreate(10,sizeof(struct Key));
	struct Key	key_data;
	Menu_Global.Cursor_i = 0;		
	Menu_Global.Catch_i = 0;
	while(1)
	{
		u8g2_ClearBuffer(u8g2);
		u8g2_SetFontPosTop(u8g2);  
		u8g2_SetFont(u8g2,u8g2_font_t0_13b_tf);
		
		for(int8_t i = -1; i < 6; i++)
		{
			if(Menu_Global.Show_i + i > Page->len - 1 ) {break;}	
			u8g2_DrawStr(u8g2,2,i*WORD_H+2+Menu_Global.Show_d,Page->Child[Menu_Global.Show_i+i].Name);  
		}
		Menu_ShowCursor(*Page,Menu_Global.CursorStyle,u8g2);
		
		

//		xTaskHandle = xTaskGetCurrentTaskHandle();
//		freeNum = uxTaskGetStackHighWaterMark(xTaskHandle);
//		u1_printf("FreeStack of Task %s : %d \n\r",pcTaskGetName(xTaskHandle),freeNum);
		
		
		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,0))
		{
			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_CLICK)
			{
				if (Page->Child[Menu_Global.Catch_i].Funsion)
				{
					Page->Child[Menu_Global.Catch_i].Funsion();
				}
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
			{
//				if (Page->Father != NULL) {
//					// �ָ�������
//					u1_printf("f");
//					if (Page->Father->Funsion != NULL)
//					{
//						u1_printf("f");
//						Page->Father->Funsion();
//					}

//				}
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
				Menu_Global.Incident = 1;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
			{
				Menu_Global.Incident = -1;
			}
		}
		
		if(Menu_Global.Incident)					
		{
			Menu_Global.Cursor_i += Menu_Global.Incident;		
			Menu_Global.Catch_i += Menu_Global.Incident;
			
			Menu_Global.Incident = 0;
			
			if(Menu_Global.Catch_i < 0) {Menu_Global.Catch_i = 0;}			
			if(Menu_Global.Catch_i > Page->len - 1) {Menu_Global.Catch_i = Page->len - 1;}
			
			if(Menu_Global.Cursor_i < 0) {Menu_Global.Cursor_i = 0;}		
			if(Menu_Global.Cursor_i > 3) {Menu_Global.Cursor_i = 3;}
			if(Menu_Global.Cursor_i > Page->len - 1) {Menu_Global.Cursor_i = Page->len - 1;}
		}
		
		Menu_Global.Show_i = Menu_Global.Catch_i - Menu_Global.Cursor_i;				
		
		if(1)		
		{
			if(Menu_Global.Show_i - Menu_Global.Show_i_temp)		  			
			{
				Menu_Global.Show_d = (Menu_Global.Show_i - Menu_Global.Show_i_temp) * WORD_H;	
				Menu_Global.Show_i_temp = Menu_Global.Show_i;		
			}
			if(Menu_Global.Show_d) {Menu_Global.Show_d /= 1.5;}				
		}
		
		u8g2_SendBuffer(u8g2);
	}
	
}






void ShowTimeTask(void *params)
{
//	TaskHandle_t xTaskHandle = NULL;
//	UBaseType_t freeNum;

	vTaskSuspend(xMyIotTaskHandle);
//	vTaskSuspend(xConWifiTaskHandle);
	vTaskSuspend(xShowMatrixTaskHandle);
	
	g_xQueueMenu = xQueueCreate(1,sizeof(struct Key));
	g_xQueueReceiver = xQueueCreateStatic(1,sizeof(struct Receiver),g_QueueReceiverBuf,&g_xQueueReceiverStaticstruct);
	struct Key	key_data;
	while(1)
	{

		Time_animation(&u8g2);
		
//		xTaskHandle = xTaskGetCurrentTaskHandle();
//		freeNum = uxTaskGetStackHighWaterMark(xTaskHandle);
//		u1_printf("FreeStack of Task %s : %d \n\r",pcTaskGetName(xTaskHandle),freeNum);
		
		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,0))
		{
			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
			{
				GUI_Display(&u8g2,&Headline_1);
			}
		}

	}
}










