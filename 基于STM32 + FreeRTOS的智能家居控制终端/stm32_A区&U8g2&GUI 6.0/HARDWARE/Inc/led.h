/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的头文件                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

/*--------------------------------------------------------------------------*/
/*                      控制LED的IO引脚 和 时钟使能                         */
/*                        可以根据需要修改和增减                            */
/*--------------------------------------------------------------------------*/
#define LED1_GROUP             GPIOA                                               //对应的IO分组
#define LED1_PIN               GPIO_PIN_11                                         //对应的IO

#define LED2_GROUP             GPIOA                                               //对应的IO分组
#define LED2_PIN               GPIO_PIN_12                                         //对应的IO                                       
/*--------------------------------------------------------------------------*/

#define LED1_TOGGLE    HAL_GPIO_TogglePin(LED1_GROUP,  LED1_PIN)                   //对应IO电平翻转，LED亮灭状态改变
#define LED2_TOGGLE    HAL_GPIO_TogglePin(LED2_GROUP,  LED2_PIN)                   //对应IO电平翻转，LED亮灭状态改变

#define LED1_IN_STA    HAL_GPIO_ReadPin(LED1_GROUP,  LED1_PIN)                     //读取LED当前电平状态
#define LED2_IN_STA    HAL_GPIO_ReadPin(LED2_GROUP,  LED2_PIN)                     //读取LED当前电平状态

#define LED1_ON        HAL_GPIO_WritePin(LED1_GROUP, LED1_PIN, GPIO_PIN_RESET)     //拉低对应IO，点亮LED
#define LED1_OFF       HAL_GPIO_WritePin(LED1_GROUP, LED1_PIN, GPIO_PIN_SET)       //拉高对应IO，熄灭LED

#define LED2_ON        HAL_GPIO_WritePin(LED2_GROUP, LED2_PIN, GPIO_PIN_RESET)     //拉低对应IO，点亮LED
#define LED2_OFF       HAL_GPIO_WritePin(LED2_GROUP, LED2_PIN, GPIO_PIN_SET)       //拉高对应IO，熄灭LED

#define LED_AON       {LED1_ON;LED2_ON;}                                           //全部LED点亮
#define LED_AOFF      {LED1_OFF;LED2_OFF;}                                         //全部LED熄灭
#define LED_ATOGGLE   {LED1_TOGGLE;LED2_TOGGLE;}                                   //全部LED亮灭状态改变

void LED_Init(void);                                                               //函数声明，初始化LED函数

#endif
