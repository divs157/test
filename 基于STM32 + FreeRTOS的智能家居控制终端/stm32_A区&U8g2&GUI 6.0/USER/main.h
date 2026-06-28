/*-----------------------------------------------------*/
/*                                                     */
/*                程序中各种配置信息头文件             */
/*                                                     */
/*-----------------------------------------------------*/


#include "delay.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "led.h"              //包含需要的头文件
#include "iic.h"              //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "AHT10.h"            //包含需要的头文件
#include "bh1750.h"           //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件
#include "spi.h"              //包含需要的头文件
#include "wifi.h"	          //包含需要的头文件
#include "timer.h"            //包含需要的头文件
#include "adc.h"              //包含需要的头文件
#include "FreeRTOS.h"		  //包含需要的头文件
#include "task.h"			  //包含需要的头文件
#include "cmsis_os.h"		  //包含需要的头文件
#include "queue.h"
#include "semphr.h"
#include "u8g2.h"			  //包含需要的头文件
#include "rtc.h"
#include "Message.h"
#include "oled.h"
#include "iot.h"              //包含需要的头文件
#include <stdio.h>
#include "Key.h"
#include "GUI.h"
#include "s12sd.h"
#include "rgb_led.h"

#ifndef __MAIN_H
#define __MAIN_H

/*---------------------------------------------------------------*/
/*-----------EEPROM内保存的阿里云证书信息有效长度----------------*/
/*---------------------------------------------------------------*/
#define VALID_ALICER_DATA      0xDD                                            //表示三元组+固件版本有效
#define PRODUCTKEY_LEN         11                                              //三元组 ProductKey     字符串长度        
#define DEVICENAME_LEN         32                                              //三元组 DeviceName     字符串长度        
#define DEVICESECRET_LEN       32                                              //三元组 DeviceSecret   字符串长度                
#define VERSION_LEN            23                                              //A区应用程序固件版本号字符串长度

//---------------------------------------------------------------//
//------------------启动信息字节，表示含义-----------------------//
//---------------------------------------------------------------//
#define BOOT_STA_O             0x5AA5C33C                                      //表示需要OTA更新应用程序
#define BOOT_STANDBY_KEY       0x2CC24FF4                                      //表示需要进入低功耗,按键唤醒
#define BOOT_STANDBY_RTC       0x3DD36BB6                                      //表示需要进入低功耗,RTC周期唤醒

/*---------------------------------------------------------------*/
/*--------------EEPROM内保存阿里云证书信息的结构体---------------*/
/*---------------------------------------------------------------*/
typedef struct{
	uint8_t  valid_cer;                                                         //保存有效标志变量 如果等于 VALID_CER_DATA 说明信息有效 不等于的话 需要重新设置
	uint8_t  ProductKeyBuff[PRODUCTKEY_LEN+1];                                  //保存ProductKey的缓冲区
	uint8_t  DeviceNameBuff[DEVICENAME_LEN+1];                                  //保存DeviceName的缓冲区
	uint8_t  DeviceSecretBuff[DEVICESECRET_LEN+1];                              //保存DeviceSecret的缓冲区
	uint8_t  Version_ABuff[VERSION_LEN+1];                                      //保存应用A区程序固件版本的缓冲区
	uint32_t OTA_firelen;                                                       //OTA固件大小
	uint32_t OTA_flag;                                                          //是否需要OTA的标识
}AliyunInfo_CB;   
#define ALIINFO_STRUCT_LEN        sizeof(AliyunInfo_CB)                         //EEPROM内保存阿里云证书信息的结构体 长度 
/*---------------------------------------------------------------*/
/*-------------------用于各种系统参数的结构体--------------------*/
/*---------------------------------------------------------------*/
typedef struct{
	int32_t SystemTimer;                                                        //用于全局统计的计时变量  
	int32_t ConnectTimer;                                                       //用于记录当次连接上服务器后的时间 
	int32_t ConnectTimeouter;                                                   //用于记录连接服务器超时时间	
	int32_t PingTimer;                                                          //用于记录发送PING数据包的计时器 
	int32_t PingTimeouter;                                                      //用于记录PING检查事件的计时器 
	uint32_t SysEventFlag;                                                      //发生各种事件的标志变量	
}Sys_CB;
#define SYS_STRUCT_LEN         sizeof(Sys_CB)                                   //用于各种系统参数的 Sys_CB结构体 长度 
	
/*---------------------------------------------------------------*/
/*-----------------------系统事件发生标志定义--------------------*/
/*---------------------------------------------------------------*/
#define CONNECT_START         0x80000000          						        //表示启动连接服务器事件发生
#define CONNECT_EVENT         0x40000000          						        //表示连接上服务器事件发生
#define CONNECT_PACK          0x20000000          						        //表示MQTT CONNECT报文成功事件发生
#define PING_SENT             0x10000000          						        //表示PING保活包发送事件发生
#define PING_CHECK            0x08000000          						        //表示PING检查事件发生
#define OTA_EVENT             0x04000000          						        //表示OTA事件发生

/*---------------------------------------------------------------*/
/*-------------------------  分  割  线  ------------------------*/
/*--------------  模板程序中 分割线以上部分 不修改 --------------*/
/*-------  模板程序中 分割线以下部分 根据不同的功能修改 ---------*/
/*---------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*----------------------自定义事件发生标志定义-------------------*/
/*---------------------------------------------------------------*/
//根据功能自行添加，注意不要和系统事件发生标志定义的数值一样
//需要多少，自行添加
#define UP_TEMP_EVENT         0x00000001          //表示该上传温度事件发生
#define UP_HUMI_EVENT         0x00000002          //表示该上传湿度事件发生
#define UP_LIGHT_EVENT        0x00000004          //表示该上传光照度事件发生
#define UP_ADC_EVENT          0x00000008          //表示该上传ADC事件发生

/*---------------------------------------------------------------*/
/*-----------------------功能属性标识符宏定义--------------------*/
/*---------------------------------------------------------------*/
//所有程序中需要用到的功能属性的标识符在此定义
//标识符名称必须和服务器后台设置的完全一样，大小写也必须一样
#define ATTRIBUTE1            "PowerSwitch_1"       //功能属性1标识符
#define ATTRIBUTE2            "PowerSwitch_2"       //功能属性2标识符
#define ATTRIBUTE3            "temperature"         //功能属性3标识符
#define ATTRIBUTE4            "humidity"            //功能属性4标识符
#define ATTRIBUTE5            "lightlux"            //功能属性5标识符
#define ATTRIBUTE6            "ADC_CH1"             //功能属性6标识符
#define ATTRIBUTE7            "ADC_CH2"             //功能属性7标识符
#define ATTRIBUTE8            "ADC_CH3"             //功能属性8标识符
#define ATTRIBUTE9            "powerstate"             //功能属性9标识符
#define ATTRIBUTE10            "HSVColor"             //功能属性10标识符
#define ATTRIBUTE11            "brightness"             //功能属性11标识符

//... ... 需要多少，自行添加

//---------------------事件报警标识符宏定义--------------------//
//所有程序中需要用到的事件报警的标识符在此定义
//标识符名称必须和服务器后台设置的完全一样，大小写也必须一样
#define EVENT1                "事件1标识符"     //事件1标识符
#define EVENT2                "事件2标识符"     //事件2标识符
//... ... 需要多少，自行添加

//---------功能属性参数结构体，定义各种变量用于保存属性值---------//              
typedef struct{
//... ... 需要多少，自行添加
	uint8_t Switch1Sta;					         //用于记录开关1状态的变量											
	uint8_t Switch2Sta;					         //用于记录开关2状态的变量
	int32_t TempTimer;                           //用于上传温度计时的变量
	int32_t HumiTimer;                           //用于上传湿度计时的变量
	int32_t LightTimer;                          //用于上传光照度计时的变量
	int32_t ADCTimer;                            //用于上传ADC计时的变量
	double tempdata;	                         //用于记录温度的变量
	double humidata;	                         //用于记录湿度的变量	
	double lightata;	                         //用于记录光照度的变量	
	double adcdata[3];	                         //用于记录3个ADC通道值的数组
	int32_t  RGB_sta;                            //灯开关状态：0 关闭状态  1 开启状态
	int32_t  RGBLed_ColorData;                   //用于保存RGB的分量值
	int32_t  Brightness;                         //亮度
	int32_t  Saturation;                         //饱和度
	int32_t  Value;                              //明度
	int32_t  Hue;                                //色调	

}Attribute_CB;   
#define Attribute_STRUCT_LEN         sizeof(Attribute_CB)   //功能属性参数结构体 长度 
	
//------------各种外部变量声明，便于其他源文件调用变量-----------//
extern Sys_CB  SysCB;                //外部变量声明，用于各种系统参数的结构体
extern Attribute_CB AttributeCB;     //外部变量声明，用于保存各种属性功能的结构体
extern AliyunInfo_CB     AliInfoCB;  //外部变量声明，EEPROM内保存的阿里云证书信息结构体
extern u8g2_t u8g2;


/* 导出函数原型 ---------------------------------------------*/
void Error_Handler(void);


#endif
