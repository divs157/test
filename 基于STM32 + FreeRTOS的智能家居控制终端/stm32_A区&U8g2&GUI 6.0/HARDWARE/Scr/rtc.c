/*-------------------------------------------------*/
/*                                                 */
/*              使用RTC实时时钟的源文件            */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "rtc.h"            //包含需要的头文件
#include <stdio.h>
#include "u8g2.h"
#include "main.h"
#include <stdbool.h>
#include "main.h"

extern QueueHandle_t g_xQueueMenu;


RTC_CB MyRtc;               //定义一个RTC结构体
 
/*-------------------------------------------------*/
/*函数名：初始化RTC                                */
/*参  数：Year：年                                 */
/*参  数：Month：月                                */
/*参  数：Date：日                                 */
/*参  数：WeekDay：星期                            */
/*参  数：Hours：时                                */
/*参  数：Minutes：分                              */
/*参  数：Seconds：秒                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
void RTC_Init(uint8_t Year, uint8_t Month, uint8_t Date, uint8_t WeekDay, uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
   
	MyRtc.hrtc.Instance = RTC;                             //RTC
	MyRtc.hrtc.Init.AsynchPrediv = 32767;                  //分频器
	HAL_RTC_Init(&MyRtc.hrtc);                             //设置RTC
		
	sDate.Month = Month;                                   //设置年
	sDate.Year = Year;                                     //设置月
	sDate.Date = Date;                                     //设置日
	sDate.WeekDay = WeekDay;                               //设置星期
	HAL_RTC_SetDate(&MyRtc.hrtc, &sDate, RTC_FORMAT_BIN);  //设置日期
	
	sTime.Hours = Hours;                                   //设置时
	sTime.Minutes = Minutes;                               //设置分
	sTime.Seconds = Seconds;                               //设置秒
	HAL_RTC_SetTime(&MyRtc.hrtc, &sTime, RTC_FORMAT_BIN);  //这是时间
	
	MyRtc.Seconds = 0xFF;                                  //临时记录秒时间的变量设置为0xFF
}

/*-------------------------------------------------*/
/*函数名：RTC的底层初始化                          */
/*参  数：hrtc：RTC配置句柄                        */
/*返回值：无                                       */
/*说  明：此函数会被HAL_RTC_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	if(hrtc->Instance==RTC){             //如果是RTC
		__HAL_RCC_RTC_ENABLE();          //开启RTC的时钟
    }
}

char* Get_Week(uint8_t WeekDay)
{
	
	static char weekDay_str[5];
	
	switch(WeekDay)
	{
		case 1:
			sprintf(weekDay_str,"Mon.");                       //构建
		break;	
		case 2:
			sprintf(weekDay_str,"Tues.");                       //构建
		break;	
		case 3:
			sprintf(weekDay_str,"Wed.");                       //构建
		break;
		case 4:
			sprintf(weekDay_str,"Thur.");                       //构建
		break;		
		case 5:
			sprintf(weekDay_str,"Fri.");                       //构建
		break;		
		case 6:
			sprintf(weekDay_str,"Sat.");                       //构建
		break;		
		case 7:
			sprintf(weekDay_str,"Sun.");                       //构建
		break;
	}
	
	return weekDay_str;
}



bool isLeapYear(int year) 
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int getDaysInMonth(int year, int month) 
{
    switch (month) {
        case 2:
            return isLeapYear(year) ? 29 : 28;
        case 4: case 6: case 9: case 11:
            return 30;
        default:
            return 31;
    }
}


void Set_Date(u8g2_t *u8g2, uint8_t datePtr, DateComponent component,int8_t minVal, int8_t maxVal, const char *label)
{
    static char dateStr[3];
    g_xQueueMenu = xQueueCreate(10,sizeof(struct Key));
	struct Key	key_data;
    int8_t dateValue = datePtr;
	
    while(1)
    {
		u8g2_ClearBuffer(u8g2);
		u8g2_DrawRFrame(u8g2, 10, 10, 108, 40, 7);
		u8g2_DrawStr(u8g2, 13, 13, label);
		strcpy(dateStr, u8x8_u8toa(dateValue, 2));
		u8g2_DrawStr(u8g2, 90, 13, dateStr);
		u8g2_DrawRBox(u8g2, 14, 33, 100/maxVal*dateValue, 4, (float)0.5);
		u8g2_SendBuffer(u8g2);
		int year = MyRtc.sdate.Year;
		int month = MyRtc.sdate.Month;
		int day = MyRtc.sdate.Date;
		int hour = MyRtc.tdate.Hours;
		int minute = MyRtc.tdate.Minutes;
		int second = MyRtc.tdate.Seconds;
		if(component == DATE_COMPONENT_DAY)
			maxVal = getDaysInMonth(year, month);
		
		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,0))
		{
			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_CLICK)
			{
				switch (component)
                {
                    case DATE_COMPONENT_YEAR:
						year = dateValue;                        
					break;
                    case DATE_COMPONENT_MONTH:
                        month = dateValue;
                        break;
                    case DATE_COMPONENT_DAY:
                        day = dateValue;
                        break;
                    case DATE_COMPONENT_HOUR:
                        hour = dateValue;
                        break;
                    case DATE_COMPONENT_MINUTE:
                        minute = dateValue;
                        break;
                    case DATE_COMPONENT_SECOND:
                        second = dateValue;
                        break;
				}
				RTC_Init(year, month, day, MyRtc.sdate.WeekDay, hour, minute, second);
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
				return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
			{
				LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
				return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_LONG)
			{
				LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
				return;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_RIGHT)
			{
				dateValue--;
                if(dateValue < minVal)
                {
                    dateValue = minVal;
                }
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
			{
				dateValue++;
                if(dateValue > maxVal)
                {
                    dateValue = maxVal;
                }
				
			}
			
		}
	}
}






