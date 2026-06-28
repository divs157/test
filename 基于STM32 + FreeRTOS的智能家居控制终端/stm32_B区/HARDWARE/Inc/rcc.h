/*-------------------------------------------------*/
/*                                                 */
/*             实现rcc功能的头文件               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __RCC_H
#define __RCC_H

#define M24C02_WADDR   0xA0                               //写24c02的地址
#define M24C02_RADDR   0xA1                               //读24c02的地址

void RccClock_HSE_Init(void);


#endif

