#include "i2c.h"				//包含需要的头文件
#include "stm32f1xx_hal.h"		//包含需要的头文件
#include "usart.h"				//包含需要的头文件
#include "u8g2.h"			  //包含需要的头文件
#include "oled.h"				//包含需要的头文件
#include "delay.h"            //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "rtc.h"

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			/* HAL initialization contains all what we need so we can skip this part. */
			I2C_Init();               //初始化硬件I2C接口
		break;

		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
			vTaskDelay(arg_int);

		break;
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
			Delay_Us(10);

		break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
			__NOP();

		break;
		
		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

void u8g2_Init(u8g2_t *u8g2)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8g2_gpio_and_delay_stm32);	

	u8g2_InitDisplay(u8g2);		// send init sequence to the display, display is in sleep mode after this,
	u8g2_SetPowerSave(u8g2, 0); // wake up display
	u8g2_ClearDisplay(u8g2);
}

/*
动效 - 逐字符显示字符串
参数：   x/y - 打印位置左下角坐标
        *text - 要打印的字符
        space - 字符间隔
        delay - 打印时间间隔
tip:需要在函数外部设置字体
*/
void PrintStr_CharByChar(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t* text,uint8_t space, uint16_t delayMS)
{
  uint8_t text_len;
 
  text_len = strlen((char*)text);
  for(uint8_t i = 0; i < text_len; i++){
    u8g2_DrawGlyph(u8g2, x + space * i, y, text[i]);
    u8g2_SendBuffer(u8g2);
    Delay_Ms(delayMS); //这个延时要短一些才有流畅的转换效果
  }
}

 
void LinearClearScreen(u8g2_t *u8g2, LinearClearDirction direction, uint8_t speed, uint16_t delayMS)
{
  int screenWidth = u8g2_GetDisplayWidth(u8g2);
  int screenHeight = u8g2_GetDisplayHeight(u8g2);
  int x, y;
  
  u8g2_SetDrawColor(u8g2, 0); //设置绘图颜色为黑色
  
  switch (direction) {
    case CLEAR_LEFT:
      for (x = 0; x < screenWidth; x += speed) {
        u8g2_DrawBox(u8g2, 0, 0, x, screenHeight);
        u8g2_SendBuffer(u8g2);
        vTaskDelay(delayMS);
      }
    break;
      
    case CLEAR_RIGHT:
      for (x = screenWidth - 1; x >= 0; x -= speed) {
        u8g2_DrawBox(u8g2, x, 0, screenWidth, screenHeight);
        u8g2_SendBuffer(u8g2);
        vTaskDelay(delayMS);
      }
    break;
 
    case CLEAR_UP:
      for (y = 0; y < screenHeight; y += speed) {
        u8g2_DrawBox(u8g2, 0, 0, screenWidth, y);
        u8g2_SendBuffer(u8g2);
        vTaskDelay(delayMS);
    }
    break;
 
    case CLEAR_DOWN:
      for (y = screenHeight - 1; y >= 0; y -= speed) {
        u8g2_DrawBox(u8g2, 0, y, screenWidth, screenHeight);
        u8g2_SendBuffer(u8g2);
        vTaskDelay(delayMS);
      }
      break;
  }
  u8g2_ClearBuffer(u8g2);
  u8g2_SendBuffer(u8g2);
  // 恢复正常绘图模式
  u8g2_SetDrawColor(u8g2, 1);
}

typedef void (*DrawFuncCallback)(u8g2_t *u8g2, uint8_t x, uint8_t y, void* params); //带传参的句柄
 
//画空心矩形
void EasingDrawBox(u8g2_t *u8g2, uint8_t x, uint8_t y, void *params)
{
  uint8_t* box_params = (uint8_t*)params;
  uint8_t width = box_params[0];
  uint8_t height = box_params[1];
  
  u8g2_DrawRFrame(u8g2, x, y, width, height,(float)0.8);
}
 
void BlinkDraw(u8g2_t *u8g2, DrawFuncCallback drawFunc, uint8_t x, uint8_t y, void *params,uint16_t delayMS,uint8_t t) 
{
	if(t==0)
	{
		// 反色绘制
		u8g2_SetDrawColor(u8g2, 2); // 设置为反色模式
		drawFunc(u8g2, x, y, params);
		u8g2_SendBuffer(u8g2);
		// 等待
		vTaskDelay(delayMS);
		// 再次反色绘制（恢复原状态）
		drawFunc(u8g2, x, y, params);
		u8g2_SendBuffer(u8g2);
		// 等待
		//vTaskDelay(delayMS);
		Delay_Ms(delayMS);    
	}
	else
	{
		drawFunc(u8g2, x, y, params);
	}
}

extern void ConWifiTask(void *params);
extern TaskHandle_t xConWifiTaskHandle;

void Boot_animation(void)
{	
	xTaskCreate(ConWifiTask, "ConWifiTask", 128, NULL, osPriorityNormal,&xConWifiTaskHandle);
	for(int i=10;i<=80;i=i+2)
	{
		u8g2_ClearBuffer(&u8g2); 
			
		char buff[20];
		sprintf(buff,"%d%%",(int)(i/80.0*100));
		u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(&u8g2,100,49,buff);
		
		u8g2_DrawRBox(&u8g2,16,40,i,10,4);
		u8g2_DrawRFrame(&u8g2,16,40,80,10,4);
		
		u8g2_SendBuffer(&u8g2);
	}
	u8g2_SetFont(&u8g2,u8g2_font_ncenB12_tf);
	PrintStr_CharByChar(&u8g2, 16,32, "STM32_XY", 13, 50);
	u8g2_SendBuffer(&u8g2);
}

char seconds_str[3];                 //C语言，建立字符串变量
char minutes_str[3];
char hours_str[3];
char date_str[3];                 //C语言，建立字符串变量
char month_str[3];
char year_str[4];
char weekDay_str[3];


void Time_animation(u8g2_t *u8g2)
{
	
	u8g2_SetFontPosTop(u8g2); 
	//u8g2_SetFontPosBaseline(u8g2);	
	HAL_RTC_GetDate(&MyRtc.hrtc, &MyRtc.sdate, RTC_FORMAT_BIN);	                                      //读取日期数据	
	HAL_RTC_GetTime(&MyRtc.hrtc, &MyRtc.tdate, RTC_FORMAT_BIN);                                       //读取时间数据	
	
	strcpy(seconds_str, u8x8_u8toa(MyRtc.tdate.Seconds, 2));		// C语言，复制字符串
	strcpy( minutes_str,u8x8_u8toa( MyRtc.tdate.Minutes,2));
	strcpy(hours_str,u8x8_u8toa(MyRtc.tdate.Hours,2));	
	
	strcpy(date_str, u8x8_u8toa(MyRtc.sdate.Date, 2));		// C语言，复制字符串
	strcpy( month_str,u8x8_u8toa( MyRtc.sdate.Month,2));
	//strcpy(year_str,u8x8_u8toa(MyRtc.sdate.Year,2));
	
	sprintf(year_str,"20%s",u8x8_u8toa(MyRtc.sdate.Year,2));                       //构建
	
	strcpy(weekDay_str,u8x8_u8toa(MyRtc.sdate.WeekDay,1));
	do
	{
		u8g2_ClearBuffer(u8g2);
		u8g2_SetFont(u8g2,u8g2_font_logisoso20_tn);   //定义字体
		u8g2_DrawStr(u8g2,15,23,hours_str);               //确定字符的位置，以下均同，略
		u8g2_DrawStr(u8g2,41,23,":");
		u8g2_DrawStr(u8g2,50,23, minutes_str);
		u8g2_DrawStr(u8g2,80,23,":");
		u8g2_DrawStr(u8g2,90,23,seconds_str);
		u8g2_SetFont(u8g2,u8g2_font_t0_13b_tf);   //定义字体
		u8g2_DrawStr(u8g2,15,10,year_str);
		u8g2_DrawStr(u8g2,41,10,".");
		u8g2_DrawStr(u8g2,46,10, month_str);
		u8g2_DrawStr(u8g2,60,10,".");
		u8g2_DrawStr(u8g2,66,10,date_str);
		u8g2_DrawStr(u8g2,82,10,Get_Week(MyRtc.sdate.WeekDay));
	}while(u8g2_NextPage(u8g2));
	if(MyRtc.Seconds != MyRtc.tdate.Seconds){                                                         //临时秒数据内的数据 和 读取的秒数据不一样 说明过去1秒了，进入if
		MyRtc.Seconds = MyRtc.tdate.Seconds; 				
	} 

}





