/*-------------------------------------------------*/
/*                                                 */
/*              使用RTC实时时钟的头文件            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __RTC_H
#define __RTC_H 

#include "u8g2.h"

typedef struct{       
	RTC_HandleTypeDef hrtc;     //设置RTC的句柄       
	RTC_DateTypeDef sdate;      //记录日期结构体
	RTC_TimeTypeDef tdate;      //记录时间结构体
	uint8_t Seconds;            //临时记录秒数据
}RTC_CB;                        //RTC的结构体

typedef enum {
    DATE_COMPONENT_YEAR,
    DATE_COMPONENT_MONTH,
    DATE_COMPONENT_DAY,
    DATE_COMPONENT_HOUR,
    DATE_COMPONENT_MINUTE,
    DATE_COMPONENT_SECOND
} DateComponent;

extern RTC_CB MyRtc;            //外部变量声明， 定义一个RTC结构体

void RTC_Init(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);   //函数声明，初始化RTC

char* Get_Week(uint8_t WeekDay);

void Set_Time(uint8_t i,u8g2_t *u8g2);

#endif




