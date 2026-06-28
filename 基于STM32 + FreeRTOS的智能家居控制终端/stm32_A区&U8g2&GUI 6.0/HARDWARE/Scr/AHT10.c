/*-------------------------------------------------*/
/*                                                 */
/*         实现AHT10功能的源文件       				*/
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  
#include "delay.h"            //包含需要的头文件
#include "AHT10.h"              //包含需要的头文件
#include "iic.h"

void AHT10_Init(void)
{
	IIC_Init();
	Delay_Ms(50);//延时50ms让传感器稳定
	IIC_Start();
	IIC_Send_Byte(AHT10_ADDRESS);  //获取状态
	//初始化校准
	IIC_Send_Byte(0xe1);	
	IIC_Send_Byte(0x08);
	IIC_Send_Byte(0x00);
	IIC_Stop();	
	Delay_Ms(50);//延时50ms让传感器稳定
}
 
/**
brief 检查AHT10是否存在
param NONE
return 0存在  1不存在
*/
uint8_t AHT10_Check(void)
{
	uint8_t ack=0;
	IIC_Start();
	IIC_Send_Byte(AHT10_ADDRESS);
	ack=IIC_Wait_Ack();
	IIC_Stop();	
	return ack;
}
 
/**
brief AHT10软复位
param NONE
return NONE
*/
void AHT10_Reset(void)
{
	IIC_Start();
	IIC_Send_Byte(AHT10_WRITE);
	IIC_Wait_Ack();
	IIC_Send_Byte(0xba);
	IIC_Wait_Ack();
	IIC_Stop();	
}
 
/**
brief 检查AHT10读温湿度数据
param *temperature：需要读出的温度数据，float指针类型,精度范围+-0.3C
param *humidity：需要读出的湿度数据，uint8_t指针类型,精度范围+-2RH
return 0 读数据正常 1读数据失败
*/
uint8_t AHT10_Read_Data(double *temperature,double *humidity)
{
	uint8_t ack;
	uint32_t SRH=0,ST=0;
	uint8_t databuff[6];
	IIC_Start();
	IIC_Send_Byte(AHT10_WRITE);
	IIC_Wait_Ack();
	IIC_Send_Byte(0xac);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x33);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
	IIC_Stop();	  
	Delay_Ms(80);//延时一会等待数据读出
	IIC_Start();
	IIC_Send_Byte(AHT10_READ);
	IIC_Wait_Ack();
	ack=IIC_Read_Byte(1);
	if((ack&0x40)==0)
	{
		databuff[0]=IIC_Read_Byte(1);
		databuff[1]=IIC_Read_Byte(1);
		databuff[2]=IIC_Read_Byte(1);
		databuff[3]=IIC_Read_Byte(1);
		databuff[4]=IIC_Read_Byte(0);
		IIC_Stop();
		SRH=(databuff[0]<<12)+(databuff[1]<<4)+(databuff[2]>>4);
		ST=((databuff[2]&0X0f)<<16)+(databuff[3]<<8)+(databuff[4]);
		*humidity=(double)(SRH*100.0/1024/1024+0.5);
		*temperature=((double)(ST*2000.0/1024/1024+0.5))/10.0-50;
		return 0;
	}
	IIC_Stop();	
	return 1;
}





