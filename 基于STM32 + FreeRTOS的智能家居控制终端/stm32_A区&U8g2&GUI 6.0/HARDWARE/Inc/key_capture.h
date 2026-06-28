/*-------------------------------------------------*/
/*                                                 */
/*            实现按键捕获功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/
#ifndef __KEY_CAPTURE_H
#define __KEY_CAPTURE_H	


/*--------------------------------------------------------------------------*/
/*                      控制按键的IO引脚 和 时钟使能                        */
/*                        可以根据需要修改和增减                            */
/*--------------------------------------------------------------------------*/
#define KEY1_GROUP               GPIOA                                    //按键 对用的IO分组
#define KEY1_PIN                 GPIO_PIN_0                               //按键 对用的IO
#define KEY1_GROUP_CLK_ENABLE    __HAL_RCC_GPIOA_CLK_ENABLE();            //按键 对用的IO分组 时钟使能
#define KEY1_MODE                GPIO_MODE_AF_INPUT                       //按键 对用的IO模式
#define KEY1_PULL                GPIO_PULLDOWN                            //按键 对用的IO上下拉模式
#define KEY1_PRESS_STA           1                                        //按键 按下时对应的电平
#define KEY1_PRESS_NO            1                                        //按键 编号
/*--------------------------------------------------------------------------*/

#define KEY1_IN_STA              HAL_GPIO_ReadPin(KEY1_GROUP, KEY1_PIN)   //读取按键IO电平状态,可以判断按键是否按下

extern TIM_HandleTypeDef Timer2;                                          //外部变量声明，定义一个配置定时器的句柄 

void KEY_Capture_Init(void);                                              //函数声明，按键捕获初始化程序，判断长按短按

#endif
