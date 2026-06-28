/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "key_capture.h"      //包含需要的头文件
#include "timer.h"            //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件
#include "wifi.h"             //包含需要的头文件
#include "i2c.h"
#include "Key.h"
#include "Encoder.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

/*-------------------------------------------------*/
/*函数名：串口2中断处理函数                         */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)
{	   	                              
	if(__HAL_UART_GET_FLAG(&U2_Control.UART_Handler,UART_FLAG_IDLE) != RESET){	                                 //如果发生空闲中断      	
		__HAL_UART_CLEAR_IDLEFLAG(&U2_Control.UART_Handler);                                                     //清除空闲中断标志 				
		HAL_UART_DMAStop(&U2_Control.UART_Handler);		                                                         //停止DMA
		U2_Control.U_RxCounter += (U2_DMARX_SIZE - __HAL_DMA_GET_COUNTER(&U2_Control.hdma_rx));                  //计算本次的接收数据量，定位结束位置
		U2_Control.URxDataInPtr->EndPtr = &U2_RxBuff[U2_Control.U_RxCounter];                                    //记录本次接收的结束位置		
		
		U2_Control.URxDataInPtr++;                                                                               //数据IN指针下移
		if(U2_Control.URxDataInPtr==U2_Control.URxDataEndPtr)                                                    //如果指针到处理接收数据的结构体数组尾部了
			U2_Control.URxDataInPtr = &U2_Control.UCB_RxBuffPtrCB[0];                                            //指针归位到处理接收数据的结构体数组的起始位置，也就是0号成员
		
		if(U2_RXBUFF_SIZE - U2_Control.U_RxCounter >= U2_DMARX_SIZE){                                            //如果剩余空间大于 DMA接收的单次数据量 进入if
			HAL_UART_Receive_DMA(&U2_Control.UART_Handler, &U2_RxBuff[U2_Control.U_RxCounter], U2_DMARX_SIZE);   //启动DMA接收
            U2_Control.URxDataInPtr->StartPtr = &U2_RxBuff[U2_Control.U_RxCounter];                              //记录下次接收的起始位置	
		}else{                                                                                                   //如果剩余空间不够单次接收量 进入else
			U2_Control.U_RxCounter = 0;                                                                          //U2_Control.U_RxCounter 清零
			HAL_UART_Receive_DMA(&U2_Control.UART_Handler, U2_RxBuff, U2_DMARX_SIZE);                            //启动DMA接收
			U2_Control.URxDataInPtr->StartPtr = U2_RxBuff;                                                       //记录下次接收的起始位置
		}
	}
}

/*-------------------------------------------------*/
/*函数名：DMA通道4中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel4_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hi2c2.hdmatx);
}

/*-------------------------------------------------*/
/*函数名：DMA通道5中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hi2c2.hdmarx);
}

/*-------------------------------------------------*/
/*函数名：I2C1_EV中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void I2C2_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&hi2c2);
}

extern void ConWifiTask(void *params);
extern TaskHandle_t xConWifiTaskHandle;


/*-------------------------------------------------*/
/*函数名：定时器1中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM1_UP_IRQHandler(void)
{
	if(__HAL_TIM_GET_IT_SOURCE(&htim1, TIM_IT_UPDATE) != RESET){     //如果定时器1溢出更新中断，进入if
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);                   //清除中断标志位
		
		/*--------------------------------------------*/
		/*      全局计时的变量+1,表示过去了1秒钟      */
		/*--------------------------------------------*/
		SysCB.SystemTimer++;                                                                       		
		/*--------------------------------------------*/
		/*      如果MQTT CONNECT报文成功事件发生      */
		/*         连接上服务器事件的计数器+1         */
		/*--------------------------------------------*/
		if(SysCB.SysEventFlag&CONNECT_PACK){                                
		    SysCB.ConnectTimer++;
		}
		/*--------------------------------------------*/
		/*             如果PING检查事件发生           */
		/*         等待服务器5s内回复PINGRESP包	      */
		/*--------------------------------------------*/
		if(SysCB.SysEventFlag&PING_CHECK){                 //判断事件是否发生                  
		    SysCB.PingTimeouter++;                         //PING检查事件超时计数器+1
			if(SysCB.PingTimeouter>=5){                    //5内没有PINGRESP回复包，重启
				u1_printf("连接掉线，重启\r\n");           //串口输出信息
				NVIC_SystemReset();                        //重启
			}
		}
		/*--------------------------------------------*/
		/*        连接上服务器的计时器 到了5s了       */
		/*          上传同步服务器固件版本号          */
		/*--------------------------------------------*/	
		if(SysCB.ConnectTimer==5){                                                               
            u1_printf("同步当前固件版本号\r\n");                 //串口输出信息 
			WiFi_PropertyVersion();		                         //同步当前固件版本号	
		}
		/*--------------------------------------------*/
		/*             连接上服务器的计时器           */
		/*             每60s发送一个PING包            */
		/*--------------------------------------------*/	
		if((SysCB.ConnectTimer - SysCB.PingTimer)>=60){           //当前连接上服务器时间 和 发送PING数据包时间至少差60s，进入if 
			SysCB.PingTimer = SysCB.ConnectTimer;                 //把当前的连接上服务器时间，记录到发送PING数据包计时变量
			if(SysCB.SysEventFlag&PING_SENT){                     //判断PING_SENT标志，如果置位，进入if，说明上传的PING数据包发送后，没有收到服务器的PINGRESP回复包，可能掉线了
                u1_printf("PING检查事件发生\r\n");                //串口输出信息
				MQTT_PingREQ();                                   //将一个PING数据包加入发送缓冲区
				SysCB.SysEventFlag |= PING_CHECK;                 //表示PING检查事件发生
				SysCB.PingTimeouter = 0;                          //PING检查事件计数器 从0开始计数
			}else{				
				MQTT_PingREQ();                                   //将一个PING数据包加入发送缓冲区			
				SysCB.SysEventFlag |= PING_SENT;                  //表示PING保活包发送事件发生				
			}
		}
		/*--------------------------------------------*/
		/*                分  割  线                  */
	    /*       分割线以下，添加自定义定时事件       */
		/*--------------------------------------------*/
		if((SysCB.ConnectTimer - AttributeCB.TempTimer)>=20){      //当前连接上服务器时间 和 温度计时时间至少差20s，进入if
			AttributeCB.TempTimer = SysCB.ConnectTimer;            //把当前的连接上服务器时间，记录到温度计时变量
			SysCB.SysEventFlag |= UP_TEMP_EVENT;                   //事件标志置位，表示该上传温度数据了	
		}
		if((SysCB.ConnectTimer - AttributeCB.HumiTimer)>=30){      //当前连接上服务器时间 和 湿度计时时间至少差30s，进入if
			AttributeCB.HumiTimer = SysCB.ConnectTimer;            //把当前的连接上服务器时间，记录到湿度计时变量
			SysCB.SysEventFlag |= UP_HUMI_EVENT;                   //事件标志置位，表示该上传湿度数据了	
		}
		if((SysCB.ConnectTimer - AttributeCB.LightTimer)>=27){     //当前连接上服务器时间 和 光照度计时时间至少差27s，进入if
			AttributeCB.LightTimer = SysCB.ConnectTimer;           //把当前的连接上服务器时间，记录到光照度计时变量
			SysCB.SysEventFlag |= UP_LIGHT_EVENT;                  //事件标志置位，表示该上传光照度数据了	
		}
		if((SysCB.ConnectTimer - AttributeCB.ADCTimer)>=37){       //当前连接上服务器时间 和 ADC计时时间至少差37s，进入if
			AttributeCB.ADCTimer = SysCB.ConnectTimer;             //把当前的连接上服务器时间，记录到ADC计时变量
			SysCB.SysEventFlag |= UP_ADC_EVENT;                    //事件标志置位，表示该上传ADC数据了	
		}
	} 
}

/*-------------------------------------------------*/
/*函数名：定时器2中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}

/*-------------------------------------------------*/
/*函数名：定时器3中断处理函数                    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&tim3);
}

/*-------------------------------------------------*/
/*函数名：定时器4中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&tim4);
}

/*-------------------------------------------------*/
/*函数名：GPIO 10-15 中断处理函数                   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}
/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void SVC_Handler(void)
//{
//	
//}
/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void PendSV_Handler(void)
//{
//	
//}
/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void SysTick_Handler(void)
//{  
//	HAL_IncTick();	
//}
