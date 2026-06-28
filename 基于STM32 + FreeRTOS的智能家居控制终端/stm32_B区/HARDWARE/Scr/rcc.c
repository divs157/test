#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "rcc.h"
#include "main.h"             //包含需要的头文件

/**
  * @brief 系统时钟配置
  * @retval None
  */
void RccClock_HSE_Init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;   
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		NVIC_SystemReset();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		NVIC_SystemReset();
	}
	
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;                		   			     //配置RTC时钟        
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;                 					 //选择外部32.768K时钟
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)                					 //配置，如果失败进入if，重启
		NVIC_SystemReset();   
}

//void RccClock_HSE_Init(void)
//{
//	RCC_OscInitTypeDef RCC_OscInitStruct;
//    RCC_ClkInitTypeDef RCC_ClkInitStruct;
//	RCC_PeriphCLKInitTypeDef PeriphClkInit;
//		
//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;            //使用外部高速主晶振8M + 外部32.768K RTC晶振
//	RCC_OscInitStruct.HSEState            = RCC_HSE_ON;                                          //开外部高速晶振
//	RCC_OscInitStruct.LSEState            = RCC_LSE_ON;                            			     //开外部32.768K RTC晶振
//	RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;                                          //开启PLL
//	RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;                                   //PLL时钟源选择HSE 外部晶振
//	RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL9;                                        //9倍频 8*9=72
//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)                               			 //配置，如果失败进入if，重启
//		NVIC_SystemReset();                         
//	
//	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
//	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;                                    //SYSCLK时钟源选择PLL   8*12/3=32MHz
//	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                           //AHB  1分频  72/1=72MHz
//	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;                                            //APB1 2分频  72/2=36MHz
//	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;                                            //APB2 1分频  72/1=72MHz
//	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)                      //配置，如果失败进入if，重启
//		NVIC_SystemReset();                                                                      //重启
//	
//	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;                		   			     //配置RTC时钟        
//    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;                 					 //选择外部32.768K时钟
//    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)                					 //配置，如果失败进入if，重启
//		NVIC_SystemReset();
//}
