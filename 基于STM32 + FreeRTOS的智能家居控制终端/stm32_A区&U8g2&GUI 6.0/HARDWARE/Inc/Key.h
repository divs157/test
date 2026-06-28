#ifndef __KEY_H
#define __KEY_H


#include "Message.h"



extern TIM_HandleTypeDef tim4;

extern  uint16_t counter_1,Sta_Flag_data;

void TIM4_Init(uint32_t ARR,uint32_t PSC);

extern int16_t Key_Get(void);
extern int16_t Encoder_Get(void);


#endif

