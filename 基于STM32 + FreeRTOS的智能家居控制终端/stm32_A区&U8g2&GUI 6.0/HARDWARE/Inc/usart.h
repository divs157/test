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

#define RNUM  10                                //标记接收数据起始位置，截止位置的结构体数组成员数量
#define TNUM  10                                //标记发送数据起始位置，截止位置的结构体数组成员数量

#define U1_TXBUFF_SIZE     128                  //定义串口1 发送缓冲区大小

#define U2_TXBUFF_SIZE     1024                 //定义串口2 发送缓冲区大小
#define U2_RXBUFF_SIZE     1024*3               //定义串口2 接收缓冲区大小
#define U2_DMARX_SIZE      1024                 //定义串口2 DMA单次接收的最大数据量

typedef struct{       
	uint8_t *StartPtr;                          //标记本次接收数据的起始位置
	uint8_t *EndPtr;                            //标记本次接收数据的截止位置
}UCB_RxBuffPtrCB;                               //标记接收数据起始位置，截止位置的结构体

typedef struct{       
	uint8_t *StartPtr;                          //标记本次发送数据的起始位置
	uint8_t *EndPtr;                            //标记本次发送数据的截止位置
}UCB_TxBuffPtrCB;                               //标记发送数据起始位置，截止位置的结构体

typedef struct{       
	uint8_t U_RxCompleted;                      //定义一个变量, 0：表示当次接收还未完成 1：表示接收完成 
	int16_t U_RxCounter;                        //定义一个变量，记录串口接收的数据在缓冲区数组内的位置	
	int16_t U_TxCounter;                        //定义一个变量，记录串口发送的数据在缓冲区数组内的位置
	int16_t U_Rxdatalen;                        //定义一个变量，保存当次串口接收到的数据的大小  
	int16_t U_Txdatalen;                        //定义一个变量，保存当次串口需要发送的数据的大小  
	UCB_RxBuffPtrCB  UCB_RxBuffPtrCB[RNUM];     //定义一个标记接收数据起始位置，截止位置的结构体的数组
	UCB_RxBuffPtrCB  UCB_TxBuffPtrCB[TNUM];     //定义一个标记发送数据起始位置，截止位置的结构体的数组
	UCB_RxBuffPtrCB *URxDataInPtr;              //指向标记接收数据起始位置，截止位置的结构体的数组的插入数据的位置
	UCB_RxBuffPtrCB *URxDataOutPtr;             //指向标记接收数据起始位置，截止位置的结构体的数组的提取数据的位置
	UCB_RxBuffPtrCB *URxDataEndPtr;             //指向标记接收数据起始位置，截止位置的结构体的数组的最后一个数组成员的位置		
	UCB_RxBuffPtrCB *UTxDataInPtr;              //指向标记发送数据起始位置，截止位置的结构体的数组的插入数据的位置
	UCB_RxBuffPtrCB *UTxDataOutPtr;             //指向标记发送数据起始位置，截止位置的结构体的数组的提取数据的位置
	UCB_RxBuffPtrCB *UTxDataEndPtr;             //指向标记发送数据起始位置，截止位置的结构体的数组的最后一个数组成员的位置	
	UART_HandleTypeDef UART_Handler;            //串口设置句柄	
	DMA_HandleTypeDef hdma_rx;                  //配置接收 DMA的句柄
}UCB_CB;                                        //串口1控制结构体

extern UCB_CB U1_Control;                       //声明外部变量，控制串口1的结构体
extern uint8_t  U1_TxBuff[U1_TXBUFF_SIZE];      //声明外部变量，定义一个数组缓冲区，串口1发送缓冲区 

extern UCB_CB U2_Control;                       //声明外部变量，控制串口2的结构体
extern uint8_t  U2_TxBuff[U2_TXBUFF_SIZE];      //声明外部变量，定义一个数组缓冲区，串口2发送缓冲区 
extern uint8_t  U2_RxBuff[U2_RXBUFF_SIZE];      //声明外部变量，定义一个数组缓冲区，串口2发接收冲区 

void U1_Init(uint32_t);                                          //函数声明，初始化串口1
void U2_Init(uint32_t);                                          //函数声明，初始化串口2
void u1_printf(char* ,...);                                      //函数声明，串口1 printf函数
void u2_printf(char* ,...);                                      //函数声明，串口2 printf函数
void U2Rx_Buff_Init(void);                                       //函数声明，初始化处理接收数据的结构体数组
void u2_TxData(uint8_t *, uint16_t);		                     //函数声明，串口2发送缓冲区中的数据
#endif


