#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "stm32f1xx_hal.h"    //包含需要的头文件





/*结构体定义*********************/

//	消息结构体，用于存储每一个消息
typedef struct TagMSG{
	uint8_t message;			//	消息
	uint8_t param;				//	消息参数
	uint8_t count;				//	消息重复计数，每个新消息为1，最大值为255
}MSG;

/*********************结构体定义*/





/*枚举*********************/

//	消息枚举
enum {
	CM_Init=0,			//	消息队列初始化
	CM_KEY,				//	按键按下

	CM_ROATATING,		//	旋转编码器旋转

};
//	消息参数列表
enum PARAM
{
	VK_UNPRESS=0,	//	没有按键按下
	VK_CLICK,		//	单击
	VK_DOUBLE,		//	双击
	VK_LONG,		//	长按
	
	EK_LEFT,		//	旋转编码器向左旋转
	EK_RIGHT,		//	旋转编码器向右旋转
};

/*********************枚举*/

typedef struct Key{
	uint8_t Key_Data;
	uint8_t Key_Param;
}Key;


typedef struct Receiver{
	uint8_t Receiver_dev;
	uint8_t Receiver_val;
}Receiver;

/*宏定义*********************/



/*********************宏定义*/





/*函数声明*********************/

void MSG_Init(void);
uint8_t MSG_SendMessage(MSG* M);
uint8_t MSG_GetMessage(MSG* M);
void MSG_Task(void *params);
//	void MSG_PostMessage(MSG* M);

/*********************函数声明*/





#endif
