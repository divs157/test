/*-------------------------------------------------*/
/*                                                 */
/*            实现定时器功能的头文件                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef _TIMER_H
#define _TIMER_H

extern TIM_HandleTypeDef htim1;                   //外部变量声明，定时器4句柄

void TIM1_Init(uint16_t, uint16_t,uint8_t);               //函数声明，定时器4初始化

#endif
