#include "stm32f1xx_hal.h"
#include "Encoder.h"
#include "usart.h"
#include "main.h"

TIM_HandleTypeDef tim3;

TIM_Encoder_InitTypeDef tim3_Encoder;		//?定时器1的编码器控制结构体

void TIM3_Init(uint32_t ARR,uint32_t PSC)
{
	tim3.Instance = TIM3;
	tim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;		//设置自动重装载值
	tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
	tim3.Init.CounterMode = TIM_COUNTERMODE_UP;				//向上计数
	tim3.Init.Period = ARR;					//自动重装值		捕获模式给最大值FFFF
	tim3.Init.Prescaler = PSC;				//预分频器值

	tim3_Encoder.EncoderMode = TIM_ENCODERMODE_TI2;		//?模式		12捕获 12参考
	tim3_Encoder.IC1Filter = 0x8;		//?滤波
	tim3_Encoder.IC1Polarity = TIM_ENCODERINPUTPOLARITY_RISING;		//?输入极性  在哪个边沿上产生标志位的置位
	tim3_Encoder.IC1Prescaler = TIM_ICPSC_DIV1;
	tim3_Encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;		//?一一对应
	
	HAL_TIM_Encoder_Init(&tim3,&tim3_Encoder);		//!初始化都会产生一次更新事件
	__HAL_TIM_CLEAR_FLAG(&tim3,TIM_FLAG_UPDATE);		//!手动清除初始化后的更新中断
	HAL_TIM_Encoder_Start_IT(&tim3,TIM_CHANNEL_3);

}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_InitType;
	if(htim->Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_AFIO_CLK_ENABLE();
		__HAL_AFIO_REMAP_TIM3_PARTIAL();
		__HAL_AFIO_REMAP_SWJ_NONJTRST();

		GPIO_InitType.Mode = GPIO_MODE_AF_INPUT;
		GPIO_InitType.Pin = GPIO_PIN_4 | GPIO_PIN_5;
		GPIO_InitType.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB,&GPIO_InitType);
		
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
		HAL_NVIC_SetPriority(TIM3_IRQn,5,0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);

	}
}
