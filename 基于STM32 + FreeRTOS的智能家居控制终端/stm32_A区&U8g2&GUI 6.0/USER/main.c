/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*STM32G0xx_HAL_Driver  ：官方提供的库文件             */
/*-----------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "delay.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "led.h"              //包含需要的头文件
#include "iic.h"              //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "AHT10.h"            //包含需要的头文件
#include "bh1750.h"           //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件
#include "spi.h"              //包含需要的头文件
#include "wifi.h"	          //包含需要的头文件
#include "timer.h"            //包含需要的头文件
#include "adc.h"              //包含需要的头文件
#include "iot.h"              //包含需要的头文件
#include "FreeRTOS.h"		  //包含需要的头文件
#include "task.h"			  //包含需要的头文件
#include "cmsis_os.h"		  //包含需要的头文件
#include "u8g2.h"				//包含需要的头文件
#include "oled.h"				//包含需要的头文件
#include "i2c.h"
#include "GUI.h"
#include "rcc.h"
#include "menu.h"
#include "Key.h"
#include "Encoder.h"
#include "Message.h"
#include "rtc.h"
#include "receiver.h"
#include "Headline_1.h"


Sys_CB  SysCB;                //用于各种系统参数的结构体
Attribute_CB AttributeCB;     //用于保存各种属性功能的结构体
AliyunInfo_CB     AliInfoCB;  //EEPROM内保存的阿里云证书信息结构体

/* USER CODE BEGIN Variables */


/* USER CODE END Variables */

/* Definitions for defaultTask */
u8g2_t u8g2;

TaskHandle_t xMyIotTaskHandle = NULL;
TaskHandle_t xShowTimeTaskHandle = NULL;
TaskHandle_t xShowHomeTaskHandle = NULL;
TaskHandle_t xShowConWifiTaskHandle = NULL;
TaskHandle_t xShowSetTimeTaskHandle = NULL;
TaskHandle_t xConWifiTaskHandle = NULL;
TaskHandle_t xShowMatrixTaskHandle = NULL;
TaskHandle_t xSmartconfigTaskHandle = NULL;


extern void IotTask(void *params);
extern void ShowTimeTask(void *params);
extern void ShowHomeTask(void *params);
extern void ShowConWifiTask(void *params);
extern void ShowSetTimeTask(void *params);
extern void ShowSetDataTask(void *params);
extern void ConWifiTask(void *params);
extern void ReceiverTask(void *params);
extern void ShowMatrixTask(void *params); 
extern void SmartconfigTask(void *params);
extern void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName);

static signed char pcWriteBuffer[200];

void vApplicationIdleHook(void);

extern QueueHandle_t g_xQueueReceiver;	
extern QueueHandle_t g_xQueueMenu;

uint32_t RGBdata;

/**
  * @brief  应用程序入口点.
  * @retval int
  */
int main(void) 
{  
	RGBdata = 0x55555555;
	HAL_Init();               //初始化HAL	
	RccClock_HSE_Init();       //初始化延迟计数器 配置时钟函数
	U1_Init(9600);          //初始化串口1
	u1_printf("跳转A区成功...\r\n");              //串口提示数据
	IIC_Init();               //初始化IIC接口
	LED_Init();               //初始化LED
	u8g2_Init(&u8g2);
	ADC1_Init();              //初始化ADC1
	AHT10_Init();             //初始化AHT10	
    WiFi_ResetIO_Init();      //初始化WiFi的复位IO
	W25Qxx_Init();            //初始化W25Qxx
	I2C_Init();               //初始化硬件I2C接口
	EEprom_ReadInfo();        //EEprom读取阿里云证书信息
	TIM1_Init(10000,7200,0);    //定时器1 1s定时，作为计时单元
	TIM3_Init(0xFFFF,1-1);
	TIM4_Init(3500,7200);                                                                      
	GUI_Init();
	MSG_Init();
	RTC_Init(24,10,29,2,23,59,59);                                                                           //初始化RTC ，2021-9-15 12:0:0 周三                                               
	IRReceiver_Init();
	RGB_Init();
	
	Boot_animation();
	
	LinearClearScreen(&u8g2, CLEAR_LEFT, 10, 10);
	
//	RGB_SetRGB(0,255,0,4,4);
//	while(1)
//	{
//		
//		u8g2_ClearBuffer(&u8g2);
//		u8g2_SetFontPosTop(&u8g2); 
//        u8g2_DrawRFrame(&u8g2, 0, 0, 128, 64, 7);
//		u8g2_DrawStr(&u8g2, 3, 3, "L");
//		u8g2_DrawStr(&u8g2, 3, 19, "R");
//		u8g2_DrawStr(&u8g2, 3, 34, "G");
//		u8g2_DrawStr(&u8g2, 3, 48, "B");
//		
//		
//		u8g2_DrawRBox(&u8g2, 20, 7, 100, 4, (float)0.5);
//		u8g2_DrawRBox(&u8g2, 20, 23, 100, 4, (float)0.5);
//		u8g2_DrawRBox(&u8g2, 20, 39, 100, 4, (float)0.5);
//		u8g2_DrawRBox(&u8g2, 20, 53, 100, 4, (float)0.5);
//		
//		
//		uint8_t draw_param[4][4] = {107,15,17,2,
//									107,15,17,18,
//									107,15,17,33,
//									107,15,17,47,
//		};
//		BlinkDraw(&u8g2, EasingDrawBox, draw_param[3][2],draw_param[3][3], draw_param[3], 500,1);	

//		
//		
//		u8g2_SendBuffer(&u8g2);
//	}
                                                                       //主循环			
//	RGB_SetData(RBG_SetColoerData(RBG_GetColoerData(99,00,55),77),2); //设置彩灯的值			/设置彩灯的值									
	
//	RGB_SetRGB(0,0,255,2,4);
	
	xTaskCreate(ShowTimeTask, "ShowTimeTask", 164, NULL, osPriorityNormal,&xShowTimeTaskHandle);
	xTaskCreate(ShowMatrixTask, "ShowMatrixTask", 150, NULL, osPriorityNormal,&xShowMatrixTaskHandle);
	xTaskCreate(ReceiverTask, "ReceiverTaskTask", 64, NULL, osPriorityNormal,NULL);						//红外遥控转编码器
//	xTaskCreate(ConWifiTask, "ConWifiTask", 128, NULL, osPriorityNormal,&xConWifiTaskHandle);
	xTaskCreate(IotTask, "IOTTask", 200, NULL, osPriorityNormal,&xMyIotTaskHandle);



	vTaskStartScheduler();  //开启调度器                
}

void vApplicationIdleHook(void)
{
	vTaskList(pcWriteBuffer);
	u1_printf("%s\r\n",pcWriteBuffer);
}

void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName)
{
	u1_printf("任务%s溢出\r\n",pcTaskName);
}


static void ReceiverTask(void *params)
{
//	TaskHandle_t xTaskHandle = NULL;
//	UBaseType_t freeNum;
	struct Receiver receiver_data;
	struct Key	key_data;
	while(1)
	{
//		xTaskHandle = xTaskGetCurrentTaskHandle();
//		freeNum = uxTaskGetStackHighWaterMark(xTaskHandle);
//		u1_printf("FreeStack of Task %s : %d \n\r",pcTaskGetName(xTaskHandle),freeNum);
		//读队列
		xQueueReceive(g_xQueueReceiver,&receiver_data,portMAX_DELAY);
		
		//处理数据
		if(receiver_data.Receiver_val == IR_KEY_ADD)/* 上 */
		{
			key_data.Key_Data = CM_ROATATING;
			key_data.Key_Param = EK_LEFT;
		}
		if (receiver_data.Receiver_val == IR_KEY_DEC) /* 下 */
		{
			key_data.Key_Data = CM_ROATATING;
			key_data.Key_Param = EK_RIGHT;
		}

		if (receiver_data.Receiver_val == IR_KEY_PLAY)  /* 单击 */
		{
			key_data.Key_Data = CM_KEY;
			key_data.Key_Param = VK_CLICK;
		}
		if (receiver_data.Receiver_val == IR_KEY_RETURN)  /* 双击 */
		{
			key_data.Key_Data = CM_KEY;
			key_data.Key_Param = VK_DOUBLE;
		}
		if (receiver_data.Receiver_val == IR_KEY_POWER)  /* 长按 */
		{
			key_data.Key_Data = CM_KEY;
			key_data.Key_Param = VK_LONG;
		}
		
		//写队列
		xQueueSend(g_xQueueMenu,&key_data,0);
	}
}




/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

