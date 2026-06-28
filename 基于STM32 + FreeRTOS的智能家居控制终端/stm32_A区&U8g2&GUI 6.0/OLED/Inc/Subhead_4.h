#ifndef __SUBHEAD_4_H
#define __SUBHEAD_4_H

#include "menu.h"

extern Menu Subhead_4;

void Subhead_4_Init(void);

extern uint16_t Read_Switch_Status(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin);

#endif
