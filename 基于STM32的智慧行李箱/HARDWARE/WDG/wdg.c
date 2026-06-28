#include "stm32f10x.h" // 引入STM32F10x系列微控制器的头文件
#include "wdg.h" // 引入看门狗配置函数的头文件
#include "led.h" // 引入LED控制相关的头文件

// 以下代码是针对Mini STM32开发板的看门狗驱动代码

// 独立看门狗(IWDG)初始化函数
void IWDG_Init(u8 prer, u16 rlr) {
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  // 使能对IWDG_PR和IWDG_RLR寄存器的写操作
    
    IWDG_SetPrescaler(prer);  // 设置IWDG预分频值，例如64
    IWDG_SetReload(rlr);  // 设置IWDG重装载值
    IWDG_ReloadCounter();  // 根据IWDG重装载寄存器的值重装载IWDG计数器
    IWDG_Enable();  // 使能IWDG
}

// 喂独立看门狗函数，防止其触发
void IWDG_Feed(void) {
    IWDG_ReloadCounter();   
}

// 保存窗口看门狗(WWDG)计数器的设置值，默认为最大值0x7F
u8 WWDG_CNT = 0x7F;

// 窗口看门狗(WWDG)初始化函数
void WWDG_Init(u8 tr, u8 wr, u32 fprer) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);  // 使能WWDG时钟

    WWDG_SetPrescaler(fprer);  // 设置WWDG预分频值
    WWDG_SetWindowValue(wr);  // 设置WWDG窗口值
    WWDG_Enable(tr);  // 使能WWDG，并设置计数器值

    WWDG_ClearFlag();  // 清除WWDG标志
    WWDG_NVIC_Init();  // 初始化WWDG NVIC配置
    WWDG_EnableIT();  // 开启WWDG中断
}

// 设置WWDG计数器的值
void WWDG_Set_Counter(u8 cnt) {
    WWDG_Enable(cnt);  // 使能WWDG并设置计数器值
}

// 窗口看门狗中断服务程序初始化
void WWDG_NVIC_Init(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;  // 设置WWDG中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  // 设置抢占优先级为2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // 设置子优先级为3
    NVIC_Init(&NVIC_InitStructure);  // 根据NVIC初始化结构体初始化NVIC
}

// 窗口看门狗中断处理函数
void WWDG_IRQHandler(void) {
    // 更新WWDG计数器
    WWDG_Set_Counter(WWDG_CNT);  // 设置WWDG计数器为预设值，防止复位

    // 清除提前唤醒中断标志位
    WWDG_ClearFlag();

    // 切换LED1的状态，作为中断发生的指示
    LED1 = !LED1;
}