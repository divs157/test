#include "motor.h"

/**
 * 使能电机1正转，使用指定的PWM占空比控制速度。
 * 
 * @param pwm 指定的PWM占空比值，用于控制电机速度。
 */
void motor1_go(u16 pwm) {
    TIM_SetCompare2(TIM3, pwm);  // 设置定时器3的比较值2，用于PWM输出
    GPIO_SetBits(GPIOA, GPIO_Pin_5);  // 将GPIOA的第5位设置为高电平，正转
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);  // 将GPIOA的第4位设置为低电平
}

/**
 * 使能电机1反转，使用指定的PWM占空比控制速度。
 * 
 * @param pwm 指定的PWM占空比值，用于控制电机速度。
 */
void motor1_back(u16 pwm) {
    TIM_SetCompare2(TIM3, pwm);  // 设置定时器3的比较值2
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);  // 将GPIOA的第5位设置为低电平，反转
    GPIO_SetBits(GPIOA, GPIO_Pin_4);  // 将GPIOA的第4位设置为高电平
}

/**
 * 停止电机1的转动。
 */
void motor1_stop(void) {
    TIM_SetCompare2(TIM3, 0);  // 将定时器3的比较值2设置为0，停止PWM输出
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);  // 将GPIOA的第4位和第5位都设置为低电平，停止电机
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

// 电机2的控制函数与电机1类似，但是使用的是定时器4和GPIOB的引脚。

/**
 * 使能电机2正转，使用指定的PWM占空比控制速度。
 * 
 * @param pwm 指定的PWM占空比值。
 */
void motor2_go(u16 pwm) {
    TIM_SetCompare2(TIM4, pwm);
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
}

/**
 * 使能电机2反转，使用指定的PWM占空比控制速度。
 * 
 * @param pwm 指定的PWM占空比值。
 */
void motor2_back(u16 pwm) {
    TIM_SetCompare2(TIM4, pwm);
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
}

/**
 * 停止电机2的转动。
 */
void motor2_stop(void) {
    TIM_SetCompare2(TIM4, 0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}

/**
 * 控制机器人向左转。
 */
void left(void) {
    motor1_go(300);  // 电机1正转，速度300
    motor2_go(400);  // 电机2正转，速度400
    delay_ms(300);   // 延迟300毫秒
}

/**
 * 控制机器人向右转。
 */
void right(void) {
    motor2_go(300);  // 电机2正转，速度300
    motor1_go(400);  // 电机1正转，速度400
    delay_ms(300);   // 延迟300毫秒
}

/**
 * 控制机器人直行。
 */
void go(void) {
    motor1_go(300);  // 电机1正转，速度300
    motor2_go(300);  // 电机2正转，速度300
    delay_ms(300);   // 延迟300毫秒
}

/**
 * 停止机器人的运动。
 */
void stop(void) {
    motor1_stop();  // 停止电机1
    motor2_stop();  // 停止电机2
    delay_ms(100);  // 延迟100毫秒
}