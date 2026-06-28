/*-------------------------------------------------*/
/*                                                 */
/*            实现各个串口功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "usart.h"            //包含需要的头文件

UCB_CB U1_Control;            //控制串口1的结构体
UCB_CB U2_Control;            //控制串口2的结构体

uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];         //定义一个数组缓冲区，串口1发送缓冲区 

uint8_t  U2_TxBuff[U2_TXBUFF_SIZE];         //定义一个数组缓冲区，串口2发送缓冲区 
uint8_t  U2_RxBuff[U2_RXBUFF_SIZE];         //定义一个数组缓冲区，串口2发接收冲区 

/*-------------------------------------------------*/
/*函数名：初始化串口1                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U1_Init(uint32_t bound)
{	
	U1_Control.UART_Handler.Instance=USART1;					        //串口1
	U1_Control.UART_Handler.Init.BaudRate=bound;				        //设置波特率
	U1_Control.UART_Handler.Init.WordLength=UART_WORDLENGTH_8B;         //设置字长为8位数据格式
	U1_Control.UART_Handler.Init.StopBits=UART_STOPBITS_1;	            //设置一个停止位
	U1_Control.UART_Handler.Init.Parity=UART_PARITY_NONE;		        //设置无奇偶校验位
	U1_Control.UART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;         //设置无硬件流控
	U1_Control.UART_Handler.Init.Mode=UART_MODE_TX;		                //设置发送模式
	HAL_UART_Init(&U1_Control.UART_Handler);					        //设置串口1
}
/*-------------------------------------------------*/
/*函数名：初始化串口2                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U2_Init(uint32_t bound)
{	
	U2_Control.UART_Handler.Instance=USART2;					        //串口2
	U2_Control.UART_Handler.Init.BaudRate=bound;				        //设置波特率
	U2_Control.UART_Handler.Init.WordLength=UART_WORDLENGTH_8B;         //设置字长为8位数据格式
	U2_Control.UART_Handler.Init.StopBits=UART_STOPBITS_1;	            //设置一个停止位
	U2_Control.UART_Handler.Init.Parity=UART_PARITY_NONE;		        //设置无奇偶校验位
	U2_Control.UART_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;         //设置无硬件流控
	U2_Control.UART_Handler.Init.Mode=UART_MODE_TX_RX;		            //设置收发模式
	HAL_UART_Init(&U2_Control.UART_Handler);					        //设置串口2  
	__HAL_UART_CLEAR_IDLEFLAG(&U2_Control.UART_Handler);                //清除空闲中断标志 
	__HAL_UART_ENABLE_IT(&U2_Control.UART_Handler,UART_IT_IDLE);        //开启空闲中断	
	U2Rx_Buff_Init();                                                   //初始化串口2处理接收数据的各个变量
}
/*-------------------------------------------------*/
/*函数名：串口1的底层初始化                        */
/*参  数：huart：串口配置句柄                      */
/*返回值：无                                       */
/*说  明：此函数会被HAL_UART_Init()回调调用        */
/*-------------------------------------------------*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef GPIO_Initure;                 //GPIO端口设置变量
	
	if(huart->Instance==USART1){                   //如果配置的是串口1，则进入改if分支，进行串口1的底层初始化
		__HAL_RCC_GPIOA_CLK_ENABLE();			   //使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			   //使能串口1时钟
		__HAL_RCC_DMA1_CLK_ENABLE();               //使能DMA1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_9;             //准备设置PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;       //复用功能
		GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);       //设置PA9 10
		
		GPIO_Initure.Pin = GPIO_PIN_10;            //准备设置PA10
		GPIO_Initure.Mode = GPIO_MODE_INPUT;       //复用功能
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);       //设置PA9 10      
	}	
	if(huart->Instance==USART2){                   //如果配置的是串口2，则进入改if分支，进行串口1的底层初始化
		__HAL_RCC_GPIOA_CLK_ENABLE();			   //使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			   //使能串口1时钟
		__HAL_RCC_DMA1_CLK_ENABLE();               //使能DMA1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_2;             //准备设置PA2
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;       //复用功能
		GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //高速
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);       //设置PA2
		
		GPIO_Initure.Pin = GPIO_PIN_3;             //准备设置PA3
		GPIO_Initure.Mode = GPIO_MODE_INPUT;       //复用功能
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);       //设置PA3
		
		HAL_NVIC_SetPriority(USART2_IRQn,0,3);	   //设置串口2中断的抢占优先级为0，子优先级是3
		HAL_NVIC_EnableIRQ(USART2_IRQn);           //使能串口2的中断
		
		U2_Control.hdma_rx.Instance                 = DMA1_Channel6;               //DMA1通道6
		U2_Control.hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;        //外设到存储区
		U2_Control.hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;            //外设不递增
		U2_Control.hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;             //存储区递增
		U2_Control.hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;         //按字节存取
		U2_Control.hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;         //按字节存取
		U2_Control.hdma_rx.Init.Mode                = DMA_NORMAL;                  //正常模式
		U2_Control.hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;           //高优先级
		HAL_DMA_Init(&U2_Control.hdma_rx);                                         //配置

		__HAL_LINKDMA(huart, hdmarx, U2_Control.hdma_rx);                          //和串口2DMA连接

		HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 5);                            //配置DMA1通道6的中断，优先级
		HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);                                    //使能DMA1通道6的中断            
	}
}
/*-------------------------------------------------*/
/*函数名：串口1 printf函数                         */
/*参  数：fmt,...  格式化输出字符串和参数          */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u1_printf(char* fmt,...) 
{  
	unsigned int i;    	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)U1_TxBuff,fmt,ap);
	va_end(ap);
	
	U1_Control.U_Txdatalen = strlen((const char*)U1_TxBuff);	                 //计算串口1需要发送的数据量
	while(__HAL_UART_GET_FLAG(&U1_Control.UART_Handler,UART_FLAG_TC)!=SET);	     //等待串口1发送空闲，然后才能发送数据	
	for(i = 0;i < U1_Control.U_Txdatalen;i ++){				                     //利用for循环，一个字节，一个字节的发送
		U1_Control.UART_Handler.Instance->DR = U1_TxBuff[i];                     //把需要发送的字节，填充到串口1，启动发送
		while(__HAL_UART_GET_FLAG(&U1_Control.UART_Handler,UART_FLAG_TC)!=SET);  //等待本次字节发送结束，才可以发送下一个字节	
	}	
}
/*-------------------------------------------------*/
/*函数名：串口2 printf函数                         */
/*参  数：fmt,...  格式化输出字符串和参数          */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u2_printf(char* fmt,...) 
{  
	unsigned int i;    	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)U2_TxBuff,fmt,ap);
	va_end(ap);
	
	U2_Control.U_Txdatalen = strlen((const char*)U2_TxBuff);	                 //计算串口2需要发送的数据量
	while(__HAL_UART_GET_FLAG(&U2_Control.UART_Handler,UART_FLAG_TC)!=SET);	     //等待串口2发送空闲，然后才能发送数据	
	for(i = 0;i < U2_Control.U_Txdatalen;i ++){				                     //利用for循环，一个字节，一个字节的发送
		U2_Control.UART_Handler.Instance->DR = U2_TxBuff[i];                     //把需要发送的字节，填充到串口2，启动发送
		while(__HAL_UART_GET_FLAG(&U2_Control.UART_Handler,UART_FLAG_TC)!=SET);  //等待本次字节发送结束，才可以发送下一个字节	
	}	
}
/*----------------------------------------------------------*/
/*函数名：初始化处理接收数据的结构体数组                    */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void U2Rx_Buff_Init(void)
{	
	U2_Control.UTxDataInPtr  = &U2_Control.UCB_TxBuffPtrCB[0];           //指向标记发送数据起始位置，截止位置的结构体的数组的插入数据的位置的指针，在最开始的时候指向该数组的0号成员
	U2_Control.UTxDataOutPtr =  U2_Control.UTxDataInPtr;                 //指向标记发送数据起始位置，截止位置的结构体的数组的提取数据的位置的指针，在最开始的时候指向该数组的0号成员
    U2_Control.UTxDataEndPtr = &U2_Control.UCB_TxBuffPtrCB[TNUM-1];      //指向标记发送数据起始位置，截止位置的结构体的数组的结尾数据的位置的指针，在最开始的时候指向该数组的最后一个成员
	U2_Control.UTxDataInPtr->StartPtr = U2_TxBuff;                       //发送起始位置
	U2_Control.UTxDataInPtr->EndPtr = U2_TxBuff;                         //发送起始位置
	U2_Control.U_TxCounter = 0;                                          //串口2累计发送位置变量，设置为0
	
	U2_Control.URxDataInPtr  = &U2_Control.UCB_RxBuffPtrCB[0];           //指向标记接收数据起始位置，截止位置的结构体的数组的插入数据的位置的指针，在最开始的时候指向该数组的0号成员
	U2_Control.URxDataOutPtr =  U2_Control.URxDataInPtr;                 //指向标记接收数据起始位置，截止位置的结构体的数组的提取数据的位置的指针，在最开始的时候指向该数组的0号成员
    U2_Control.URxDataEndPtr = &U2_Control.UCB_RxBuffPtrCB[RNUM-1];      //指向标记接收数据起始位置，截止位置的结构体的数组的结尾数据的位置的指针，在最开始的时候指向该数组的最后一个成员
	U2_Control.URxDataInPtr->StartPtr = U2_RxBuff;                       //接收起始位置
	U2_Control.URxDataInPtr->EndPtr = U2_RxBuff;                         //接收起始位置
	U2_Control.U_RxCounter = 0;                                          //串口2累计接收位置变量，设置为0
	memset((char *)U2_RxBuff,0,U2_RXBUFF_SIZE);                          //清空串口2接收缓冲区 
	HAL_UART_DMAStop(&U2_Control.UART_Handler);		                           //停止DMA
	HAL_UART_Receive_DMA(&U2_Control.UART_Handler, U2_RxBuff, U2_DMARX_SIZE);  //启动DMA接收
}
/*-------------------------------------------------*/
/*函数名：串口2发送缓冲区中的数据                  */
/*参  数：data：数据                               */
/*参  数：len：数据量                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u2_TxData(uint8_t *data, uint16_t len)
{  
	uint16_t i;                                                                  //定义一个变量，用于for循环
	
	for(i=0;i<len;i++){	                                                         //循环一个一个字节的发送
		while(__HAL_UART_GET_FLAG(&U2_Control.UART_Handler,UART_FLAG_TC)!=SET);	 //等待串口2发送空闲，然后才能发送数据	
		U2_Control.UART_Handler.Instance->DR = data[i];                          //把需要发送的字节，填充到串口2，启动发送
	}
	while(__HAL_UART_GET_FLAG(&U2_Control.UART_Handler,UART_FLAG_TC)!=SET);      //等待最后一个字节发送结束	
}
