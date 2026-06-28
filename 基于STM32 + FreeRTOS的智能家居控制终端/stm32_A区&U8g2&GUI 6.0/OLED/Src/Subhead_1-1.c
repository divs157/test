#include "stm32f1xx_hal.h"
#include "usart.h"
#include "menu.h"
#include "oled.h"
#include "GUI.h"
#include "Subhead_1.h"
#include "Subhead_1-2.h"
#include "Subhead_1-1.h"
#include "main.h"             //包含需要的头文件
#include "iot.h"              //包含需要的头文件

extern TaskHandle_t xConWifiTaskHandle;
extern TaskHandle_t xShowTimeTaskHandle;

extern void ConWifiTask(void *params);

/*	Subhead_1名字	*/
const char* Subhead_1_1_Name = "Connect";

/*	Subhead_1菜单结构体	*/
Menu Subhead_1_1;	

void Subhead_1_1_Funsion(void)
{
	xTaskCreate(ConWifiTask, "ConWifiTask", 128, NULL, osPriorityNormal+1,&xConWifiTaskHandle);
//	WiFi_Connect_IoTServer();   	   //如果WiFi连接云服务器函数返回非零值，表示连接服务器失败，进入if
}

void Subhead_1_1_Init(void)
{
	Menu_Init(&Subhead_1_1);
	Subhead_1_1.Funsion = Subhead_1_1_Funsion;
	Subhead_1_1.Name = Subhead_1_1_Name;
	
	
}

void ConWifiTask(void *params)
{
	if(WiFi_Connect_IoTServer()!=0)   	   //如果WiFi连接云服务器函数返回非零值，表示连接服务器失败，进入if
	{
		vTaskDelete(xConWifiTaskHandle);
		//NVIC_SystemReset();                //重启
	}
	else
	{
		vTaskDelete(xConWifiTaskHandle);

	}
}



