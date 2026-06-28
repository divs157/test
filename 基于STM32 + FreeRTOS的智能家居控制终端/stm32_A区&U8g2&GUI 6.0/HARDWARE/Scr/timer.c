/*             实现定时器功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "timer.h"          //包含需要的头文件
#include "usart.h"          //包含需要的头文件

TIM_HandleTypeDef htim1;    //定时器4句柄 

/*-------------------------------------------------*/
/*函数名：定时器1初始化                            */
/*参  数：arr：自动重装值                          */
/*参  数：psc：时钟预分频数                        */
/*参  数：Repe：重复计数器值                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM1_Init(uint16_t arr, uint16_t psc,uint8_t rep)
{
	htim1.Instance = TIM1;                                         //设置定时器1
	htim1.Init.Prescaler = psc-1;                                  //设置定时器预分频数
	htim1.Init.Period = arr-1;                                     //设置自动重装载值
	htim1.Init.RepetitionCounter = rep;		//重复计数值（高级定时器才用的到）
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim1);                                     //设置
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);                     //清除中断标志
	HAL_TIM_Base_Start_IT(&htim1);                                 //使能定时器1和定时器1更新中断：TIM_IT_UPDATE
}
/*-------------------------------------------------*/
/*函数名：定时器底层驱动，开启时钟，设置中断优先级 */
/*参  数：htim：定时器句柄                         */
/*返回值：无                                       */
/*说  明：此函数会被HAL_TIM_Base_Init()函数调用    */
/*-------------------------------------------------*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM1){	                      //如果是配置定时器1，进入if		
		__HAL_RCC_TIM1_CLK_ENABLE();                  //使能TIM1时钟
		HAL_NVIC_SetPriority(TIM1_UP_IRQn,5,5);       //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);             //开启ITM1中断   
	}
}

