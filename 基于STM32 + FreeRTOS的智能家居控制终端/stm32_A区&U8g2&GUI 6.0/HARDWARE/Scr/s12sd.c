/*-------------------------------------------------*/
/*                                                 */
/*              使用RTC实时时钟的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "u8g2.h"
#include "main.h"
#include "s12sd.h"


uint32_t  sum;
uint32_t  UV_Dat;
uint32_t average;

void s12sd_Read_Data(void)
{
	sum = 0;
	for (int i = 0; i < 1024; i++) {
        UV_Dat = Get_Adc(ADC_CHANNEL_4); // 读取ADC值
        sum += UV_Dat; // 将ADC值加到总和中
    }
	UV_Dat=Get_Adc(ADC_CHANNEL_4)*4980.0/4095.0;
	
	average = sum / 1024;
}

uint8_t uv;

uint8_t UV_Index(uint32_t D_Index)
{
	if(D_Index < 50)
	{  //查表，把得到的光电流值转换成紫外线等级
		uv = 0;
	}
	  else if(D_Index < 227)
	{
		uv = 1;
	 }
	  else if(D_Index < 318)
	{
		uv = 2;
    }
	  else if(D_Index < 408)
	{
		uv = 3;
	}
	  else if(D_Index < 503)
	{
		uv = 4;
	}
	  else if(D_Index < 606)
	{
		uv = 5;
  }
	  else if(D_Index < 696)
	{
		uv = 6;
	}
	  else if(D_Index < 795)
	{
		uv = 7;
	}
	  else if(D_Index < 881)
	{
		uv = 8;
	}
	  else if(D_Index < 976)
	{
		uv = 9;
	}
	  else if(D_Index < 1079)
	{
		uv = 10;
	}
	  else
	{
		uv = 11;
	}
	return uv;
}



