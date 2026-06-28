#ifndef __ENCODER_H
#define __ENCODER_H

extern TIM_HandleTypeDef tim3;

extern TIM_Encoder_InitTypeDef tim3_Encoder;		//?定时器1的编码器控制结构体

void TIM3_Init(uint32_t ARR,uint32_t PSC);

#endif
