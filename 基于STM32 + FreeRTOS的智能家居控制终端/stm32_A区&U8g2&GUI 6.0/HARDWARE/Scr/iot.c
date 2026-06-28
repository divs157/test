/*-------------------------------------------------*/
/*                                                 */
/*         IOT物联网综合的源文件       				*/
/*                                                 */
/*-------------------------------------------------*/
#include "stm32f1xx_hal.h"  
#include "main.h"            //包含需要的头文件
#include "iot.h"              //包含需要的头文件

extern TaskHandle_t xConWifiTaskHandle;
extern TaskHandle_t xShowTimeTaskHandle;

void IotTask(void *params)
{
	uint16_t i;

	
	while(1){                                                                                                //主循环		
		/*-----------------------------------------------------------------------------------------*/  
		/*                             处 理 串 口 2 接 收 到 的 数 据                             */
		/*-----------------------------------------------------------------------------------------*/
		if(U2_Control.URxDataOutPtr != U2_Control.URxDataInPtr){                                             //如果串口2的 URxDataOutPtr指针 和 URxDataInPtr指针不相等，则if成立，说明串口2有数据接收到了                     					   						
			U2_Control.U_Rxdatalen = U2_Control.URxDataOutPtr->EndPtr - U2_Control.URxDataOutPtr->StartPtr;  //计算本次接收的数据量				
			u1_printf("串口2本次接收%d字节报文数据：\r\n",U2_Control.U_Rxdatalen);		                     //串口输出提示信息
			for(i=0;i<U2_Control.U_Rxdatalen;i++)
				u1_printf("%02x ",U2_Control.URxDataOutPtr->StartPtr[i]);		                             //串口输出提示信息
            u1_printf("\r\n\r\n");		                                                                     //串口输出提示信息					
			U2PassiveEvent(U2_Control.URxDataOutPtr->StartPtr,U2_Control.U_Rxdatalen);                       //处理串口2被动事件数据					
			U2_Control.URxDataOutPtr ++;                                                                     //串口2的 URxDataOutPtr指针 下移
			if(U2_Control.URxDataOutPtr==U2_Control.URxDataEndPtr)                                           //如果到达处理接收数据的结构体数组尾部了
				U2_Control.URxDataOutPtr = &U2_Control.UCB_RxBuffPtrCB[0];                                   //串口2的 URxDataOutPtr指针 回到处理接收数据的结构体数组开头  
		}
		/*-----------------------------------------------------------------------------------------*/  
		/*                              处 理 串 口 2 发 送 的 数 据                               */
		/*-----------------------------------------------------------------------------------------*/
		if(U2_Control.UTxDataOutPtr != U2_Control.UTxDataInPtr){                                             //如果串口2的 UTxDataOutPtr指针 和 UTxDataInPtr指针不相等，则if成立，说明串口2有数据发送到了                     					   						
			U2_Control.U_Txdatalen = U2_Control.UTxDataOutPtr->EndPtr - U2_Control.UTxDataOutPtr->StartPtr;  //计算本次发送的数据量				
			u1_printf("串口2本次发送报文数据：\r\n");		                                                 //串口输出提示信息
			for(i=0;i<U2_Control.U_Txdatalen;i++)
				u1_printf("%02x ",U2_Control.UTxDataOutPtr->StartPtr[i]);		                             //串口输出提示信息
			u1_printf("\r\n\r\n");		                                                                     //串口输出提示信息			
			u2_TxData(U2_Control.UTxDataOutPtr->StartPtr,U2_Control.U_Txdatalen);	                         //发送数据					
			U2_Control.UTxDataOutPtr ++;                                                                     //串口2的 UTxDataOutPtr指针 下移
			if(U2_Control.UTxDataOutPtr==U2_Control.UTxDataEndPtr)                                           //如果到达处理发送数据的结构体数组尾部了
				U2_Control.UTxDataOutPtr = &U2_Control.UCB_TxBuffPtrCB[0];                                   //串口2的 UTxDataOutPtr指针 回到处理发送数据的结构体数组开头  
		}
		/*----------------------------------------------------------------------------------------*/
		/*                                     主 动 事 件                                        */
		/*----------------------------------------------------------------------------------------*/	
		ActiveEvent();
        /*----------------------------------------------------------------------------------------*/
		/*                                        延时                                            */
		/*----------------------------------------------------------------------------------------*/
		vTaskDelay(10);		
	}
}


