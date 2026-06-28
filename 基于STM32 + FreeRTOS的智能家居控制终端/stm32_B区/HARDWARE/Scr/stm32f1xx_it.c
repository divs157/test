/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "timer.h"            //包含需要的头文件
#include "spi.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

/*-------------------------------------------------*/
/*函数名：串口1中断处理函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART1_IRQHandler(void)
{		                              
	if(__HAL_UART_GET_FLAG(&U1_Control.UART_Handler,UART_FLAG_IDLE) != RESET){	                                 //如果发生空闲中断      	
		__HAL_UART_CLEAR_IDLEFLAG(&U1_Control.UART_Handler);                                                     //清除空闲中断标志 				
		HAL_UART_DMAStop(&U1_Control.UART_Handler);		                                                         //停止DMA
		U1_Control.U_RxCounter += (U1_DMARX_SIZE - __HAL_DMA_GET_COUNTER(&U1_Control.hdma_rx));                  //计算本次的接收数据量，定位结束位置
		U1_Control.URxDataInPtr->EndPtr = &U1_RxBuff[U1_Control.U_RxCounter];                                    //记录本次接收的结束位置		
				
		U1_Control.URxDataInPtr++;                                                                               //数据IN指针下移
		if(U1_Control.URxDataInPtr==U1_Control.URxDataEndPtr)                                                    //如果指针到处理接收数据的结构体数组尾部了
			U1_Control.URxDataInPtr = &U1_Control.UCB_RxBuffPtrCB[0];                                            //指针归位到处理接收数据的结构体数组的起始位置，也就是0号成员
		U1_Control.URxDataInPtr->StartPtr = &U1_RxBuff[U1_Control.U_RxCounter];                                  //记录下次接收的起始位置
		
		if(U1_RXBUFF_SIZE - U1_Control.U_RxCounter >= U1_DMARX_SIZE){                                            //如果剩余空间大于 DMA接收的单次数据量 进入if
			HAL_UART_Receive_DMA(&U1_Control.UART_Handler, &U1_RxBuff[U1_Control.U_RxCounter], U1_DMARX_SIZE);   //启动DMA接收
            U1_Control.URxDataInPtr->StartPtr = &U1_RxBuff[U1_Control.U_RxCounter];                              //记录下次接收的起始位置	
		}else{                                                                                                   //如果剩余空间不够单次接收量 进入else
			U1_Control.U_RxCounter = 0;                                                                          //U1_Control.U_RxCounter 清零
			HAL_UART_Receive_DMA(&U1_Control.UART_Handler, U1_RxBuff, U1_DMARX_SIZE);                            //启动DMA接收
			U1_Control.URxDataInPtr->StartPtr = U1_RxBuff;                                                       //记录下次接收的起始位置
		}
	}
}
/*-------------------------------------------------*/
/*函数名：串口1接收DMA通道1中断处理函数            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&U1_Control.hdma_rx);
}

/*-------------------------------------------------*/
/*函数名：DMA通道2中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SPI1_Handler.hdmarx);
}

/*-------------------------------------------------*/
/*函数名：DMA通道3中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SPI1_Handler.hdmatx);
}
/*-------------------------------------------------*/
/*函数名：定时器1中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM1_UP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);    
}

/*-------------------------------------------------*/
/*函数名：定时器4中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM2_IRQHandler(void)
{

	HAL_TIM_IRQHandler(&htim2);

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
///*-------------------------------------------------*/
///*函数名：可挂起的系统服务处理函数                 */
///*参  数：无                                       */
///*返回值：无                                       */
///*-------------------------------------------------*/
//void PendSV_Handler(void)
//{
//	
//}
///*-------------------------------------------------*/
///*函数名：SysTic系统嘀嗒定时器处理函数             */
///*参  数：无                                       */
///*返回值：无                                       */
///*-------------------------------------------------*/
//void SysTick_Handler(void)
//{  
//	HAL_IncTick();	
//}
