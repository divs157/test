/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的源文件                */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "led.h"            //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化LED函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStruct;                 //定义一个设置GPIO的变量
		
	__HAL_RCC_GPIOA_CLK_ENABLE();			          //使能GPIOB时钟

	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;  //设置PA11 12
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;       //推免输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;      //低速
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);           //设置
	
	LED_AOFF;                                         //LED全部熄灭
}

