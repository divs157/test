/*-------------------------------------------------*/
/*                                                 */
/*              使用RTC实时时钟的源文件            */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "rtc.h"            //包含需要的头文件

RTC_HandleTypeDef MyRtc;    //定义一个RTC结构体
 
/*-------------------------------------------------*/
/*函数名：初始化RTC                                */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void RTC_Init(void)
{
	MyRtc.Instance = RTC;      //RTC
	HAL_RTC_Init(&MyRtc);      //设置RTC
	
}
/*-------------------------------------------------*/
/*函数名：RTC的底层初始化                          */
/*参  数：hrtc：RTC配置句柄                        */
/*返回值：无                                       */
/*说  明：此函数会被HAL_RTC_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	if(hrtc->Instance==RTC){             //如果是RTC
		__HAL_RCC_BKP_CLK_ENABLE();      //开启后备区的时钟
		__HAL_RCC_PWR_CLK_ENABLE();      //使能PWR时钟
	}
}
/*-------------------------------------------------*/
/*函数名：RTC的底层恢复默认                        */
/*参  数：hrtc：RTC配置句柄                        */
/*返回值：无                                       */
/*说  明：此函数会被HAL_RTC_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
	if(hrtc->Instance==RTC){                //如果是RTC
		__HAL_RCC_RTC_DISABLE();            //关闭RTC的时钟
    	__HAL_RCC_BKP_CLK_DISABLE();        //关闭后备区的时钟
		__HAL_RCC_PWR_CLK_DISABLE();        //关闭PWR时钟
	}
}
