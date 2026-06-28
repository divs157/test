/*-------------------------------------------------*/
/*                                                 */
/*          使用SysTick实现延时功能的源文件        */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "delay.h"          //包含需要的头文件

void Delay_Us(int us)
{
#if 0    
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;

    ticks = us * reload / (1000);  /* 假设reload对应1ms */
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
#else
    extern TIM_HandleTypeDef        htim2;
    TIM_HandleTypeDef *hHalTim = &htim2;

    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = __HAL_TIM_GET_AUTORELOAD(hHalTim);

    ticks = us * reload / (1000);  /* 假设reload对应1ms */
    told = __HAL_TIM_GET_COUNTER(hHalTim);
    while (1)
    {
        tnow = __HAL_TIM_GET_COUNTER(hHalTim);
        if (tnow != told)
        {
            if (tnow > told)
            {
                tcnt += tnow - told;
            }
            else
            {
                tcnt += reload - told + tnow;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
#endif        
}

void Delay_Ms(int ms)
{
    for (int i = 0; i < ms; i++)
        Delay_Us(1000);
}

