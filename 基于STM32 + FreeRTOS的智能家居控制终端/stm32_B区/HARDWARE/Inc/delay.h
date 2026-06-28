/*-------------------------------------------------*/
/*                                                 */
/*          使用SysTick实现延时功能的头文件        */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __DELAY_H
#define __DELAY_H 

void Delay_CLock_Init(void);   //函数声明，初始化延迟计数器 配置时钟函数
void Delay_Us(int us);       //函数声明，延时毫秒函数
void Delay_Ms(int Ms);       //函数声明，延时微秒函数
void RccClock_HSE_Init(void);

#endif





























