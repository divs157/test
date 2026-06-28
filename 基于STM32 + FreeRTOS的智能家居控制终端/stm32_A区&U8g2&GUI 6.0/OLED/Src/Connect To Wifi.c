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

/*	Subhead_1名字	*/
const char* Subhead_1_Name = "Connect To Wifi";

/*	Subhead_1菜单结构体	*/
Menu Subhead_1;	

void Subhead_1_Funsion(void)
{
	if(WiFi_Connect_IoTServer()!=0)   	   //如果WiFi连接云服务器函数返回非零值，表示连接服务器失败，进入if
		NVIC_SystemReset();                 //重启
}

void Subhead_1_Init(void)
{
	Menu_Init(&Subhead_1);
	Subhead_1.Funsion = Subhead_1_Funsion;
	Subhead_1.Name = Subhead_1_Name;
}





