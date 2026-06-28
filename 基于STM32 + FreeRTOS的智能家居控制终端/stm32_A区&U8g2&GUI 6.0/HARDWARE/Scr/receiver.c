/* 包含必要的头文件 */
#include "receiver.h"
#include "u8g2.h"
#include "delay.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "Message.h"
#include "Key.h"
#include "main.h"

/* 用于存储红外接收器中断时间的数组 */
static uint64_t g_IRReceiverIRQ_Timers[68];
static int g_IRReceiverIRQ_Cnt = 0;

/* 辅助函数 */

/**********************************************************************
 * 函数名称： IRReceiver_Init
 * 功能描述： 红外接收器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 **********************************************************************/
void IRReceiver_Init(void)
{
    /* PA10被配置为双边沿触发, 并使能了中断 */
	GPIO_InitTypeDef GPIO_InitStruct;                 //定义一个设置GPIO的变量
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			          //使能GPIOA时钟


    /*Configure GPIO pin : PA15 */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	
	

}





extern QueueHandle_t g_xQueueReceiver;

/**********************************************************************
 * 函数名称： IRReceiver_IRQTimes_Parse
 * 功能描述： 解析中断回调函数里记录的时间序列，得到的device和key放入环形缓冲区
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1) - 失败
 **********************************************************************/
static int IRReceiver_IRQTimes_Parse(void)
{
    uint64_t time;
    int i;
    int m, n;
    unsigned char datas[4];
    unsigned char data = 0;
    int bits = 0;
    int byte = 0;
	
	struct Receiver receiver_data;

    /* 1. 判断前导码 : 9ms的低脉冲, 4.5ms高脉冲 */
    time = g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0];
    if (time < 8000000 || time > 10000000)
    {
        return -1;
    }

    time = g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1];
    if (time < 3500000 || time > 55000000)
    {
        return -1;
    }

    /* 2. 解析数据 */
    for (i = 0; i < 32; i++)
    {
        m = 3 + i*2;
        n = m+1;
        time = g_IRReceiverIRQ_Timers[n] - g_IRReceiverIRQ_Timers[m];
        data <<= 1;
        bits++;
        if (time > 1000000)
        {
            /* 得到了数据1 */
            data |= 1;
        }

        if (bits == 8)
        {
            datas[byte] = data;
            byte++;
            data = 0;
            bits = 0;
        }
    }

    /* 判断数据正确性 */
    datas[1] = ~datas[1];
    datas[3] = ~datas[3];
    
    if ((datas[0] != datas[1]) || (datas[2] != datas[3]))
    {
        g_IRReceiverIRQ_Cnt = 0;
        return -1;
    }

//    PutKeyToBuf(datas[0]);
//    PutKeyToBuf(datas[2]);
	
	//写队列
	receiver_data.Receiver_dev = datas[0];
	receiver_data.Receiver_val = datas[2];
	
	xQueueSendFromISR(g_xQueueReceiver,&receiver_data,NULL);
	
    return 0;
}

/**********************************************************************
 * 函数名称： isRepeatedKey
 * 功能描述： 解析中断回调函数里记录的时间序列，判断是否重复码
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1 - 是, (0) - 不是
 **********************************************************************/
static int isRepeatedKey(void)
{
    uint64_t time;

    /* 1. 判断重复码 : 9ms的低脉冲, 2.25ms高脉冲 */
    time = g_IRReceiverIRQ_Timers[1] - g_IRReceiverIRQ_Timers[0];
    if (time < 8000000 || time > 10000000)
    {
        return 0;
    }

    time = g_IRReceiverIRQ_Timers[2] - g_IRReceiverIRQ_Timers[1];
    if (time < 2000000 || time > 2500000)
    {
        return 0;
    }   

    return 1;
}

/**********************************************************************
 * 函数名称： IRReceiver_IRQ_Callback
 * 功能描述： 红外接收器的中断回调函数，记录中断时刻
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 **********************************************************************/
void IRReceiver_IRQ_Callback(void)
{
    uint64_t time;
    static uint64_t pre_time = 0;
        
    /* 1. 记录中断发生的时间 */    
    time = system_get_ns();
    
    /* 一次按键的最长数据 = 引导码 + 32个数据"1" = 9+4.5+2.25*32 = 85.5ms
     * 如果当前中断的时间, 距离上次中断的时刻超过这个时间, 以前的数据就抛弃
     */
    if (time - pre_time > 100000000) 
    {
        g_IRReceiverIRQ_Cnt = 0;
    }
    pre_time = time;
    
    g_IRReceiverIRQ_Timers[g_IRReceiverIRQ_Cnt] = time;

    /* 2. 计数中断次数 */
    g_IRReceiverIRQ_Cnt++;

    /* 3. 次数达标时, 解析数据, 放入buffer */
    if (g_IRReceiverIRQ_Cnt == 4)
    {
        /* 是否重复码 */
        if (isRepeatedKey())
        {
            /* device: 0, val: 0, 表示重复码 */
            g_IRReceiverIRQ_Cnt = 0;
        }
    }
    if (g_IRReceiverIRQ_Cnt == 68)
    {
        IRReceiver_IRQTimes_Parse();
        g_IRReceiverIRQ_Cnt = 0;
    }
}

/**********************************************************************
 * 函数名称： IRReceiver_CodeToString
 * 功能描述： 把接收到的按键码转换为按键名字
 * 输入参数： code - 按键值
 * 输出参数： 无
 * 返 回 值： NULL - 无法识别的按键值; 其他值 - 按键名字
 **********************************************************************/
const char *IRReceiver_CodeToString(uint8_t code)
{
    const uint8_t codes[]= {0xa2, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, \
                            0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, \
                            0x5a, 0x42, 0x4a, 0x52, 0x00};
    const char *names[]= {"Power", "Menu", "Test", "+", "Return", "Left", "Play", "Right", \
                            "0", "-", "C", "1", "2", "3", "4", "5", \
                            "6", "7", "8", "9", "Repeat"};
    int i;
    
    for (i = 0; i < sizeof(codes)/sizeof(codes[0]); i++)
    {
        if (code == codes[i])
        {
            return names[i];
        }
    }
    return "Error";
}

/**********************************************************************
 * 函数名称： HAL_GPIO_EXTI_Callback
 * 功能描述： 外部中断的中断回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
        case GPIO_PIN_15:
        {
            IRReceiver_IRQ_Callback();
            break;
        }
        default:
        {
            break;
        }
    }
}

/**********************************************************************
 * 函数名称： IRReceiver_Test
 * 功能描述： 红外接收器测试程序
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 **********************************************************************/
void IRReceiver_Test(void)
{

    
}

