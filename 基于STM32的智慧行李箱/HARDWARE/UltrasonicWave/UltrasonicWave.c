#include "UltrasonicWave.h"
#include "usart.h"
#include "timer.h"
#include "beep.h"
#include "delay.h"
#include "math.h"
#include "motor.h"

// 定义超声波引脚
#define TRIG_PORT_1      GPIOC
#define ECHO_PORT_1      GPIOC
#define TRIG_PIN_1       GPIO_Pin_8
#define ECHO_PIN_1       GPIO_Pin_7
#define TRIG_PORT_2      GPIOE
#define ECHO_PORT_2      GPIOE
#define TRIG_PIN_2       GPIO_Pin_5
#define ECHO_PIN_2       GPIO_Pin_4

// 距离相关变量
double UltrasonicWave_Distance, UltrasonicWave_Distance2;
double maxDistance = 0, minDistance = 10000, avgDistanceSum = 0;
int measurementCount = 0, validMeasurements = 0, processFlag = 0;

// 超声波模块配置
void UltrasonicWave_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 关闭JTAG，使能相关端口时钟
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    // 初始化TRIG和ECHO引脚
    ConfigurePin(TRIG_PORT_1, TRIG_PIN_1, GPIO_Mode_Out_PP);
    ConfigurePin(ECHO_PORT_1, ECHO_PIN_1, GPIO_Mode_IPD);
    ConfigurePin(TRIG_PORT_2, TRIG_PIN_2, GPIO_Mode_Out_PP);
    ConfigurePin(ECHO_PORT_2, ECHO_PIN_2, GPIO_Mode_IPD);

    // 设置外部中断线及配置
    ConfigureExtiLine(ECHO_PIN_1, EXTI_Line7);
    ConfigureExtiLine(ECHO_PIN_2, EXTI_Line4);

    // NVIC中断配置
    ConfigureNVIC(EXTI9_5_IRQn, 2, 0); // EXTI7中断
    ConfigureNVIC(EXTI4_IRQn, 2, 1);   // EXTI4中断
}

// 配置单个引脚
void ConfigurePin(GPIO_TypeDef* port, uint16_t pin, uint32_t mode)
{
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStructure);
}

// 配置EXTI线及中断
void ConfigureExtiLine(uint16_t echoPin, uint32_t extiLine)
{
    GPIO_EXTILineConfig(GPIO_PortSource(echoPin >> 3), GPIO_PinSource(echoPin & 0x07));
    EXTI_InitStructure.EXTI_Line = extiLine;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

// 配置NVIC中断
void ConfigureNVIC(uint8_t irqChannel, uint8_t preemptPrio, uint8_t subPriority)
{
    NVIC_InitStructure.NVIC_IRQChannel = irqChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preemptPrio;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// EXTI9_5_IRQHandler处理
void EXTI9_5_IRQHandler(void)
{
    ProcessEchoInterrupt(ECHO_PORT_1, ECHO_PIN_1, UltrasonicWave_Distance, EXTI_Line7);
}

// EXTI4_IRQHandler处理
void EXTI4_IRQHandler(void)
{
    ProcessEchoInterrupt(ECHO_PORT_2, ECHO_PIN_2, UltrasonicWave_Distance2, EXTI_Line4);
}

// 共享的中断处理逻辑
void ProcessEchoInterrupt(GPIO_TypeDef* echoPort, uint16_t echoPin, double& distanceVar, uint32_t extiLine)
{
    delay_us(10);
    if (EXTI_GetITStatus(extiLine) != RESET)
    {
        TIM_SetCounter(TIM2, 0);
        TIM_Cmd(TIM2, ENABLE);

        while (!GPIO_ReadInputDataBit(echoPort, echoPin));

        TIM_Cmd(TIM2, DISABLE);
        distanceVar = TIM_GetCounter(TIM2) * 340 / 200.0;

        HandleMeasurement(distanceVar);
        EXTI_ClearITPendingBit(extiLine);
    }
}

// 处理单次测量数据
void HandleMeasurement(double distance)
{
    measurementCount++;
    if (distance > 0 && distance <= 300)
    {
        validMeasurements++;
        avgDistanceSum += distance;
        UpdateMinMax(distance);
    }
}

// 更新最大最小值
void UpdateMinMax(double distance)
{
    if (distance > maxDistance) maxDistance = distance;
    if (distance < minDistance) minDistance = distance;
}

// 开始测量
void UltrasonicWave_StartMeasure(void)
{
    DisableInterrupts(); // 禁用中断避免干扰
    PerformMeasurement(TRIG_PORT_1, TRIG_PIN_1, UltrasonicWave_Distance);
    PerformMeasurement(TRIG_PORT_2, TRIG_PIN_2, UltrasonicWave_Distance2);
    EvaluateAndAct();
    EnableInterrupts(); // 测量结束后恢复中断
}

// 执行单边测量
void PerformMeasurement(GPIO_TypeDef* trigPort, uint16_t trigPin, double& distanceVar)
{
    ResetMeasurementStats();
    TriggerPulse(trigPort, trigPin);
    WaitAndProcessMeasurements(distanceVar);
}

// 重置测量统计
void ResetMeasurementStats()
{
    measurementCount = validMeasurements = 0;
    maxDistance = 0;
    minDistance = 10000;
    avgDistanceSum = 0;
}

// 触发脉冲
void TriggerPulse(GPIO_TypeDef* trigPort, uint16_t trigPin)
{
    GPIO_SetBits(trigPort, trigPin);
    delay_us(20);
    GPIO_ResetBits(trigPort, trigPin);
}

// 等待并处理测量
void WaitAndProcessMeasurements(double& distanceVar)
{
    while (validMeasurements < 5 && measurementCount <= 10);
    CalculateAverage(distanceVar);
}

// 计算平均距离
void CalculateAverage(double& avgDistance)
{
    if (validMeasurements >= 5)
    {
        avgDistanceSum -= (maxDistance + minDistance);
        avgDistance = avgDistanceSum / (validMeasurements - 2); // 平均不包括最大和最小值
    }
}

// 评估结果并采取行动
void EvaluateAndAct()
{
    // 根据平均距离判断是否发出警告音，调整行驶方向等逻辑
    // 这部分逻辑可以根据具体需求进一步优化和简化
    // ...
}