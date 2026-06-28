/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*STM32G0xx_HAL_Driver  ：官方提供的库文件             */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "boot.h"             //包含需要的头文件
#include "delay.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "timer.h"            //包含需要的头文件
#include "stmflash.h"         //包含需要的头文件
#include "rtc.h"              //包含需要的头文件
#include "iic.h"              //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件
#include "spi.h"              //包含需要的头文件
  
uint32_t          BootEventFlag;       //发生各种事件的标志变量                                                   
Update_CB         Update;              //本地/OTA更新固件时结构体变量
AliyunInfo_CB     AliInfoCB;           //EEPROM内保存的阿里云证书信息结构体

void RccClock_HSE_Init(void);

uint8_t rdata_buff[16*256];



int main(void) 
{  
	HAL_Init();                                      //初始化HAL
	RccClock_HSE_Init();                              //初始化延迟计数器 配置时钟函数	
	U1_Init(9600);                                 //初始化串口1
	//RTC_Init();                                      //初始化RTC 
    IIC_Init();                                      //初始化IIC接口
	W25Qxx_Init();                                   //初始化W25Qxx
	EEprom_ReadInfo();                               //EEprom读取阿里云证书信息 
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET){    //查看PWR_FLAG_SB标志，如果置位表示系统从待机模式唤醒，进入if
		u1_printf("系统从待机模式唤醒\r\n");         //串口输出信息
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);           //清除PWR_FLAG_SB标志	
	}else u1_printf("正常开机\r\n");                 //串口输出信息			
	
	BootLoader_Branch();                             //BootLoader分支走向，跳转到A区应用程序，还是向下进入BootLoader主循环,还是OTA升级	
	TIM1_Init(30000,7200,0);                           //定时器1 周期3S  用于定时发送C
	/*---------------------------------*/
	/*             主循环              */
	/*---------------------------------*/
	while(1){                           
		/*-----------------------------------------------------------------------------------------*/
		/*                             处 理 串 口 1 接 收 到 的 数 据                             */
		/*-----------------------------------------------------------------------------------------*/
		if(U1_Control.URxDataOutPtr != U1_Control.URxDataInPtr){                                             //如果串口1的 URxDataOutPtr指针 和 URxDataInPtr指针不相等，则if成立，说明串口1有数据接收到了                     					   						                                                                                         
			U1_Control.U_Rxdatalen = U1_Control.URxDataOutPtr->EndPtr - U1_Control.URxDataOutPtr->StartPtr;  //计算本次接收的数据量		                                           //清空缓冲区
			memcpy(U1Copy_RxBuff,U1_Control.URxDataOutPtr->StartPtr,U1_Control.U_Rxdatalen);                 //提取数据		
			BootLoader_Event(U1Copy_RxBuff,U1_Control.U_Rxdatalen);                                          //处理BootLoader命令或事件	
			U1_Control.URxDataOutPtr ++;                                                                     //串口1的 URxDataOutPtr指针 下移
			if(U1_Control.URxDataOutPtr==U1_Control.URxDataEndPtr)                                           //如果到达处理接收数据的结构体数组尾部了
				U1_Control.URxDataOutPtr = &U1_Control.UCB_RxBuffPtrCB[0];                                   //串口1的 URxDataOutPtr指针 回到处理接收数据的结构体数组开头  
		}
		/*----------------------------------------------------------------------------------------*/
		/*                                     主 动 事 件                                        */
		/*----------------------------------------------------------------------------------------*/	
		ActiveEvent();	
	}
}

/**
  * @brief  发生错误时执行该功能。
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
