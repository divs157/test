/*-------------------------------------------------*/
/*                                                 */
/*             实现各个串口功能的头文件            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __USART_H
#define __USART_H

#include "stdio.h"            //包含需要的头文件
#include "stdarg.h"		      //包含需要的头文件 
#include "string.h"           //包含需要的头文件

#define NUM  10                                 //标记接收数据起始位置，截止位置的结构体数组成员数量
#define U1_TXBUFF_SIZE     256                  //定义串口1 发送缓冲区大小
#define U1_RXBUFF_SIZE     1024*2               //定义串口1 接收缓冲区大小
#define U1_DMARX_SIZE      512                 //定义串口1 DMA单次接收的最大数据量
#define U1_COPYBUFF_SIZE   U1_DMARX_SIZE        //定义处理串口1接收数据的缓冲区副本大小

typedef struct{       
	uint8_t *StartPtr;                          //标记本次接收数据的起始位置
	uint8_t *EndPtr;                            //标记本次接收数据的截止位置
}UCB_RxBuffPtrCB;                               //标记接收数据起始位置，截止位置的结构体

typedef struct{       
	uint16_t U_RxCounter;                       //定义一个变量，记录串口接收的数据在缓冲区数组内的位置	
	uint16_t U_Rxdatalen;                       //定义一个变量，保存当次串口接收到的数据的大小  
	uint16_t U_Txdatalen;                       //定义一个变量，保存当次串口需要发送的数据的大小  
	UCB_RxBuffPtrCB  UCB_RxBuffPtrCB[NUM];      //定义一个标记接收数据起始位置，截止位置的结构体的数组
	UCB_RxBuffPtrCB *URxDataInPtr;              //指向标记接收数据起始位置，截止位置的结构体的数组的插入数据的位置
	UCB_RxBuffPtrCB *URxDataOutPtr;             //指向标记接收数据起始位置，截止位置的结构体的数组的提取数据的位置
	UCB_RxBuffPtrCB *URxDataEndPtr;             //指向标记接收数据起始位置，截止位置的结构体的数组的最后一个数组成员的位置		
	UART_HandleTypeDef UART_Handler;            //串口设置句柄	
	DMA_HandleTypeDef hdma_rx;                  //配置接收 DMA的句柄
}UCB_CB;                                        //串口1控制结构体

extern UCB_CB U1_Control;                       //声明外部变量，控制串口1的结构体
extern uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];      //声明外部变量，定义一个数组缓冲区，串口1发送缓冲区 
extern uint8_t  U1_RxBuff[U1_RXBUFF_SIZE];      //声明外部变量，定义一个数组缓冲区，串口1发接收冲区 
extern uint8_t  U1Copy_RxBuff[U1_COPYBUFF_SIZE];//声明外部变量，定义一个数组缓冲区，串口1处理接收数据的副本缓冲区

void U1_Init(uint32_t);                                          //函数声明，初始化串口1
void u1_printf(char* ,...);                                      //函数声明，串口1 printf函数
void U1Rx_Buff_Init(void);                                       //函数声明，初始化处理接收数据的结构体数组
void u1_TxHex(uint8_t);                                          //函数声明，串口1发送一个16进制数据

#endif


