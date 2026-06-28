#include "stm32f1xx_hal.h"
#include "Key.h"
#include "usart.h"
#include "main.h"
#include "Encoder.h"
#include "Message.h"
#include "GUI.h"

uint8_t Flag = 0;

TIM_HandleTypeDef tim4;

TIM_IC_InitTypeDef tim4_IC_3;
TIM_IC_InitTypeDef tim4_IC_4;

uint16_t counter,Sta_Flag;
uint16_t counter_data,Sta_Flag_data;




QueueHandle_t g_xQueueMenu;	
QueueHandle_t g_xQueueReceiver;	



void TIM4_Init(uint32_t ARR,uint32_t PSC)
{
	tim4.Instance = TIM4;
	tim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	tim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim4.Init.CounterMode = TIM_COUNTERMODE_UP;		//向上计数模式
	tim4.Init.Period = ARR-1;
	tim4.Init.Prescaler = PSC-1;
	HAL_TIM_IC_Init(&tim4);		//配置输入捕获
	
	__HAL_TIM_CLEAR_IT(&tim4,TIM_IT_UPDATE);        //清除定时器中断标志位
	
	//通道3
	tim4_IC_3.ICFilter = 0x0F;	//滤波
	tim4_IC_3.ICPolarity = TIM_ICPOLARITY_FALLING;			//下降沿
	tim4_IC_3.ICPrescaler = TIM_ICPSC_DIV1;
	tim4_IC_3.ICSelection = TIM_ICSELECTION_DIRECTTI;		//直连
	HAL_TIM_IC_ConfigChannel(&tim4, &tim4_IC_3, TIM_CHANNEL_3);

	//通道4
	tim4_IC_4.ICFilter = 0x0F;	//滤波
	tim4_IC_4.ICPolarity = TIM_ICPOLARITY_RISING;			//上升沿
	tim4_IC_4.ICPrescaler = TIM_ICPSC_DIV1;
	tim4_IC_4.ICSelection = TIM_ICSELECTION_INDIRECTTI;		//交叉
	HAL_TIM_IC_ConfigChannel(&tim4, &tim4_IC_4, TIM_CHANNEL_4);

	//Start只开启了捕获中断
    HAL_TIM_IC_Start_IT(&tim4,TIM_CHANNEL_3);       //开启通道3 先不开启通道4 检测到按键按下再开启
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_InitType;
	if(htim->Instance == TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		GPIO_InitType.Mode = GPIO_MODE_AF_INPUT;
		GPIO_InitType.Pin = GPIO_PIN_8;
		GPIO_InitType.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitType);

		//开启中断
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
        HAL_NVIC_SetPriority(TIM4_IRQn,5,0);
        HAL_NVIC_EnableIRQ(TIM4_IRQn);
	}
}

int16_t num = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	struct Key key_data;
	if(htim->Instance == TIM3)
	{
		__HAL_TIM_CLEAR_FLAG(&tim3,TIM_FLAG_CC1);	//再清除
        if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&tim3))			//!判断DIR位计数方向
        {
            // Temp = ((int16_t)HAL_TIM_ReadCapturedValue(&tim1,TIM_CHANNEL_1)) / 2;
//			u1_printf("左转\r\n");
			
			
			key_data.Key_Data = CM_ROATATING;
			key_data.Key_Param = EK_LEFT;
			xQueueSendFromISR(g_xQueueMenu,&key_data,NULL);
			
			
//			msg.message = CM_ROATATING;
//			msg.param = EK_LEFT;
//			MSG_SendMessage(&msg);	//	发送旋转编码器向左旋转信息

        }
        else
        {
            // Temp = ((int16_t)HAL_TIM_ReadCapturedValue(&tim1,TIM_CHANNEL_1) + 1) / 2;
//			u1_printf("右转\r\n");
			
			key_data.Key_Data = CM_ROATATING;
			key_data.Key_Param = EK_RIGHT;	
			xQueueSendFromISR(g_xQueueMenu,&key_data,NULL);

			
//			msg.message = CM_ROATATING;
//			msg.param = EK_RIGHT;
//			MSG_SendMessage(&msg);	//	发送旋转编码器向右旋转信息

        }
	}
//	if(htim->Instance == TIM3)
//	{
//		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//		{
//			if(__HAL_TIM_IS_TIM_COUNTING_DOWN(htim))
//			{
//				//num = ((int16_t)HAL_TIM_ReadCapturedValue(&tim3,TIM_CHANNEL_1) - 1) / 4;
//				u1_printf("逆时针转动%d\r\n",num);		//!读取捕获值
//				msg.message = CM_ROATATING;
//				msg.param = EK_LEFT;
//				MSG_SendMessage(&msg);	//	发送旋转编码器向左旋转信息
//			
//			}
//		}
//		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
//		{
//			if(!(__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)))
//			{
//				//num = ((int16_t)HAL_TIM_ReadCapturedValue(&tim3,TIM_CHANNEL_2) + 1) / 4;
//				u1_printf("顺时针转动%d\r\n",num);		//!读取捕获值
//				msg.message = CM_ROATATING;
//				msg.param = EK_RIGHT;
//				MSG_SendMessage(&msg);	//	发送旋转编码器向右旋转信息

//			}
//		}
//	}
	else if(htim->Instance == TIM4)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			__HAL_TIM_SET_COUNTER(htim,0);
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);	//在开中断之前习惯清一下标志位
			__HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);

			 HAL_TIM_IC_Start_IT(htim,TIM_CHANNEL_4);       //检测道通道3按下 开启通道4
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			if(Sta_Flag == 0)
            {
                counter++;
            }
            else
            {
//				Sta_Flag_data = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3) + Sta_Flag * 2500 / 10;

				key_data.Key_Data = CM_KEY;
				key_data.Key_Param = VK_LONG;			
				xQueueSendFromISR(g_xQueueMenu,&key_data,NULL);

//				msg.message = CM_KEY;
//				msg.param = VK_LONG;
//				MSG_SendMessage(&msg);	//	发送按键1松开消息
                u1_printf("按键时长:%d\r\n",HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3) + Sta_Flag * 2500 / 10);
                __HAL_TIM_DISABLE_IT(htim,TIM_IT_UPDATE);
                HAL_TIM_IC_Stop(htim,TIM_CHANNEL_4);
                Sta_Flag = 0;
                counter = 0;
            }
		}
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	struct Key key_data;
	if (htim->Instance == TIM2) 
	{
		HAL_IncTick();
	}
	else if(htim->Instance == TIM4)
	{
		if(counter == 0)
        {
            Sta_Flag++;
        }
        else
        {
            u1_printf("按键次数:%d\r\n",counter);
			if(counter == 1)
			{
				
				key_data.Key_Data = CM_KEY;
				key_data.Key_Param = VK_CLICK;
				xQueueSendFromISR(g_xQueueMenu,&key_data,NULL);
//				u1_printf("按键次数:%d\r\n",counter);
//				msg.message = CM_KEY;
//				msg.param = VK_CLICK;
//				MSG_SendMessage(&msg);	//	发送按键1松开消息

			}
			else if(counter == 2)
			{
				
				key_data.Key_Data = CM_KEY;
				key_data.Key_Param = VK_DOUBLE;
				xQueueSendFromISR(g_xQueueMenu,&key_data,NULL);
//				u1_printf("按键次数:%d\r\n",counter);
//				msg.message = CM_KEY;
//				msg.param = VK_DOUBLE;
//				MSG_SendMessage(&msg);	//	发送按键1松开消息

			}
            __HAL_TIM_DISABLE_IT(htim,TIM_IT_UPDATE);
            HAL_TIM_IC_Stop(htim,TIM_CHANNEL_4);
            counter = 0;
            Sta_Flag = 0;
        }
	}
}

int16_t Encoder_Get(void)
{
	MSG msg;
    if( MSG_GetMessage(&msg) )	//	接收消息
	{
		if(msg.message == CM_ROATATING && msg.param == EK_RIGHT )	//	处理编码器右转
		{
			return 	Turn_right;
		}
		
		else if(msg.message == CM_ROATATING && msg.param == EK_LEFT )	//	处理编码器左转
		{
			return Turn_left;
		}
	}
    return NULL_CLICK;
}

int16_t Key_Get(void)
{
	MSG msg;
    if( MSG_GetMessage(&msg) )	//	接收消息
	{
		if(msg.message == CM_KEY && msg.param == VK_CLICK )	//	处理单击
		{
			u1_printf("按键次数:1\r\n");
			return SINGLE_CLICK;
		}
		else if(msg.message == CM_KEY && msg.param == VK_DOUBLE )	//	处理双击
		{
			u1_printf("按键次数:2\r\n");
			return DOUBLE_CLICK;
			
		}
		else if(msg.message == CM_KEY && msg.param == VK_LONG )	//	处理长按
		{
			u1_printf("长按\r\n");
			return LONG_PRESS;
			
		}
	}
    return NULL_CLICK;
}





