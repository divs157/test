/*-------------------------------------------------*/
/*                                                 */
/*            实现RGB彩灯功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __RGB_LED_H
#define __RGB_LED_H

/*--------------------------------------------------------------------------*/
/*                      控制RGB彩灯的IO引脚 和 时钟使能                     */
/*--------------------------------------------------------------------------*/
#define RGB_GROUP             GPIOA                                               //RGB彩灯 对应的IO分组
#define RGB_PIN               GPIO_PIN_0                                         //RGB彩灯 对应的IO
#define RGB_GROUP_CLK_ENABLE  __HAL_RCC_GPIOA_CLK_ENABLE();                       //RGB彩灯 对应的IO分组 时钟使能
/*--------------------------------------------------------------------------*/

//RGB彩灯发送二进制位0
#define RGB_BIT0  HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_SET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);    
//RGB彩灯发送二进制位1
#define RGB_BIT1  HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_SET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_SET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_SET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);

#define RGB_H   HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_SET);       //拉高对应IO
#define RGB_L   HAL_GPIO_WritePin(RGB_GROUP, RGB_PIN, GPIO_PIN_RESET);     //拉低对应IO


void RGB_Init(void);                                                       //函数声明，初始化RGB彩灯控制IO
void RGB_SetData(uint32_t, uint8_t);                                       //函数声明，设置RGB彩灯数据
void RGB_Close(uint8_t);                                                   //函数声明，关闭RGB彩灯
uint32_t RBG_SetColoerData(int32_t, int32_t);  								//函数声明，计算RGB分量结合亮度值的数据 
uint32_t RBG_GetColoerData(int32_t , int32_t , int32_t);                   //函数声明，计算RGB分量数据//函数声明，计算RGB分量结合亮度值的数据 
void RGB_SetRGB(uint8_t R, uint8_t G, uint8_t B, uint8_t v, uint8_t num);
	
#endif
