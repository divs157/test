#include "stm32f10x.h" // 包含STM32F10x系列微控制器的头文件

#include "led.h" // 包含LED控制相关的函数声明
#include "delay.h" // 包含延时函数的头文件
#include "sys.h" // 包含系统配置相关的函数声明
#include "key.h" // 包含按键控制相关的函数声明
#include "beep.h" // 包含蜂鸣器控制相关的函数声明
#include "usart.h" // 包含串口通信相关的函数声明
#include "exti.h" // 包含外部中断控制相关的函数声明
#include "timer.h" // 包含定时器相关的函数声明
#include "UltrasonicWave.h" // 包含超声波测距模块相关的函数声明

int main(void)
{
    // SystemInit(); // 系统初始化，被注释掉，如果需要使用默认的系统初始化可以取消注释

    delay_init(72); // 初始化延时函数，参数72代表系统时钟频率为72MHz
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC中断优先级分组，设置为分组2
    uart_init(9600); // 初始化串口通信，波特率为9600
    LED_Init(); // 初始化LED端口
    BEEP_Init(); // 初始化蜂鸣器
    KEY_Init(); // 初始化按键端口
    Timerx_Init(5000, 7199); // 初始化定时器，设置定时频率为10Khz，定时周期为500ms
    UltrasonicWave_Configuration(); // 初始化超声波测距模块

    TIM3_PWM_Init(899, 0); // 初始化定时器3的PWM功能，参数899和0代表具体的PWM配置值
    TIM4_PWM_Init(899, 0); // 初始化定时器4的PWM功能，参数899和0代表具体的PWM配置值

    while(1) // 主循环
    {
        UltrasonicWave_StartMeasure(); // 开始超声波测距，发送一个大于10us的脉冲并测量返回的高电平时间
        // 主循环中可以添加其他任务或处理逻辑
    }
}