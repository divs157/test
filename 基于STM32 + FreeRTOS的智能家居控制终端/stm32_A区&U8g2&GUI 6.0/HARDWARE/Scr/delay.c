/*-------------------------------------------------*/
/*                                                 */
/*          使用SysTick实现延时功能的源文件        */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "delay.h"          //包含需要的头文件

/**********************************************************************
 * 函数名称： udelay
 * 功能描述： us级别的延时函数
 * 输入参数： us - 延时多少us
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
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


/**********************************************************************
 * 函数名称： mdelay
 * 功能描述： ms级别的延时函数
 * 输入参数： ms - 延时多少ms
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void Delay_Ms(int ms)
{
    for (int i = 0; i < ms; i++)
        Delay_Us(1000);
}

/**********************************************************************
 * 函数名称： system_get_ns
 * 功能描述： 获得系统时间(单位ns)
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 系统时间(单位ns)
 ***********************************************************************/
uint64_t system_get_ns(void)
{
    //extern uint32_t HAL_GetTick(void);
    extern TIM_HandleTypeDef        htim2;
    TIM_HandleTypeDef *hHalTim = &htim2;
    
    uint64_t ns = HAL_GetTick();
    uint64_t cnt;
    uint64_t reload;

    cnt = __HAL_TIM_GET_COUNTER(hHalTim);
    reload = __HAL_TIM_GET_AUTORELOAD(hHalTim);

    ns *= 1000000;
    ns += cnt * 1000000 / reload;
    return ns;
}

unsigned long getRunTimeCounterValue(void)
{
	return system_get_ns() / 1000;
}
