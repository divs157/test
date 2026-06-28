/*-------------------------------------------------*/
/*                                                 */
/*               实现ADC功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "adc.h"              //包含需要的头文件

ADC_HandleTypeDef hadc1;      //ADC1 配置句柄

/*-------------------------------------------------*/
/*函数名：初始化ADC1                               */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void ADC1_Init(void)
{
	hadc1.Instance = ADC1;                                                  //配置ADC1
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;                             //右对齐
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;                             //禁用排序器，就用1个通道
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;                       //软件触发转换开始
	HAL_ADC_Init(&hadc1);                                                   //开始配置
}
/*-------------------------------------------------*/
/*函数名：ADC1的底层初始化                         */
/*参  数：hadc：ADC1配置句柄                       */
/*返回值：无                                       */
/*说  明：此函数会被HAL_ADC_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef  PeriphClkInit;
	
	if(hadc->Instance==ADC1){                                  //如果是配置ADC1
		__HAL_RCC_GPIOA_CLK_ENABLE();                          //开启端口A的时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();                          //开启端口B的时钟
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;//设置ADC时钟
		PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;   //PCLK2 6分频 72/6=12 不能超过14
		HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);             //设置
		__HAL_RCC_ADC1_CLK_ENABLE();                  //开启ADC的时钟
		
		GPIO_InitStruct.Pin = GPIO_PIN_4;             //设置PA4
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;      //模拟输入模式
		GPIO_InitStruct.Pull = GPIO_NOPULL;           //无上下拉
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);       //配置
		
		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;//设置PB0 1
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;      //模拟输入模式
		GPIO_InitStruct.Pull = GPIO_NOPULL;           //无上下拉
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);       //配置
	}
}
/*-------------------------------------------------*/
/*函数名：读取指定通道的结果                       */
/*参  数：ch：通道                                 */
/*返回值：ADC结果                                  */
/*-------------------------------------------------*/
uint16_t Get_Adc(uint32_t ch)   
{  
	ADC_ChannelConfTypeDef sConfig = {0};
	
	sConfig.Channel = ch;                               //设置通道
	sConfig.Rank = ADC_REGULAR_RANK_1;                  //设置为BANK1
	sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;   //选用转换周期配置
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);            //配置   	
	HAL_ADC_Start(&hadc1);                              //开启ADC1	
    HAL_ADC_PollForConversion(&hadc1,100);              //轮询等待转换结束
 
	return (uint16_t)HAL_ADC_GetValue(&hadc1);	        //返回转换结果
}
