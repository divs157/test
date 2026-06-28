/*-------------------------------------------------*/
/*                                                 */
/*          使用SysTick实现延时功能的头文件        */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __DELAY_H
#define __DELAY_H 

#include <stdint.h>

void Delay_Us(int us);       //函数声明，延时毫秒函数
void Delay_Ms(int ms);       //函数声明，延时微秒函数
uint64_t system_get_ns(void);


#endif





























