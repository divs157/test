#ifndef _RECEIVER_H
#define _RECEIVER_H

#include <stdint.h>

#define IR_KEY_POWER  0xa2
#define IR_KEY_MENU   0xe2
#define IR_KEY_TEST   0x22
#define IR_KEY_ADD    0x02
#define IR_KEY_RETURN 0xc2
#define IR_KEY_LEFT   0xe0
#define IR_KEY_PLAY   0xa8
#define IR_KEY_RIGHT  0x90
#define IR_KEY_0      0x68
#define IR_KEY_DEC    0x98
#define IR_KEY_C      0xb0
#define IR_KEY_1      0x30
#define IR_KEY_2      0x18
#define IR_KEY_3      0x7a
#define IR_KEY_4      0x10
#define IR_KEY_5      0x38
#define IR_KEY_6      0x5a
#define IR_KEY_7      0x42
#define IR_KEY_8      0x4a
#define IR_KEY_9      0x52
#define IR_KEY_REPEAT 0x00

/**
 * @brief 初始化红外接收器
 * 
 * 该函数用于初始化红外接收器的硬件设置
 */
void IRReceiver_Init(void);

/**
 * @brief 读取红外接收器数据
 * 
 * @param pDev 用于存储设备ID的指针
 * @param pData 用于存储按键码的指针
 * @return int 0表示成功,-1表示失败(无数据)
 */
int IRReceiver_Read(uint8_t *pDev, uint8_t *pData);

/**
 * @brief 将按键码转换为对应的字符串
 * 
 * @param code 按键码
 * @return const char* 返回按键名称,如果无法识别则返回NULL
 */
const char *IRReceiver_CodeToString(uint8_t code);

/**
 * @brief 红外接收器测试程序
 * 
 * 该函数用于测试红外接收器的功能
 */
void IRReceiver_Test(void);

#endif /* _RECEIVER_H */

