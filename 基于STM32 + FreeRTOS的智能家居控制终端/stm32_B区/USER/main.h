/*-----------------------------------------------------*/
/*                                                     */
/*                 各种配置、信息头文件                */
/*                                                     */
/*-----------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H

//--------------------------------------------------------------//
//--------------本地更新固件 Xmodem 协议 相关内容---------------//
//--------------------------------------------------------------//
#define XMODEM_PACK_SIZE       133                                        //Xmodem 协议 一包数据中  所有数据的大小 
#define XMODEM_PACKDATA_SIZE   128                                        //Xmodem 协议 一包数据中  用户数据的大小
#define XMODEM_SHO             0x01                                       //Xmodem 协议 SHO值   数据头
#define XMODEM_EOT             0x04                                       //Xmodem 协议 EOT值   发送结束
#define XMODEM_ACK             0x06                                       //Xmodem 协议 ACK值   应答
#define XMODEM_NAK             0x15                                       //Xmodem 协议 ACK值   非应答
#define XMODEM_CAN             0x18                                       //Xmodem 协议 CAN值   停止传输

//---------------------------------------------------------------//
//---------------BootLoader、应用A区 Flash分区-------------------//
//---------------------------------------------------------------//
#define FLASH_BASE_ADDR        0x08000000 	                                                   //STM32 FLASH的起始地址
#define FLASH_SECTO_NUM        128                                                              //STM32 FLASH所有扇区数
#define FLASH_SECTOR_SIZE      1024                                                            //STM32 FLASH扇区大小
#define FLASH_BOOT_SECTO_NUM   20                                                              //BootLoader占用扇区的个数
#define FLASH_A_SECTO_NUM      FLASH_SECTO_NUM - FLASH_BOOT_SECTO_NUM                          //应用程序A区占用扇区的个数
#define FLASH_A_ADDR		   FLASH_BASE_ADDR + FLASH_BOOT_SECTO_NUM * FLASH_SECTOR_SIZE      //应用程序A区起始位置
#define FLASH_A_START_SECTOR  (FLASH_A_ADDR - FLASH_BASE_ADDR)/FLASH_SECTOR_SIZE               //应用程序A区起始扇区

//---------------------------------------------------------------//
//------------------启动信息字节，表示含义-----------------------//
//---------------------------------------------------------------//
#define BOOT_STA_O             0x5AA5C33C                                //表示需要OTA更新应用程序
#define BOOT_STANDBY_KEY       0x2CC2                                    //表示需要进入低功耗,按键唤醒

//---------------------------------------------------------------//
//-------------------------事件发生标志定义----------------------//
//---------------------------------------------------------------//
#define A_XMODEM        0x00000001                                      //A区下载（Xmodem）事件
#define ALIYUN_CER      0x00000002                                      //设置阿里云三元组证书事件
#define OTA_MODEM       0x00000004                                      //A区下载（OTA）事件

/*---------------------------------------------------------------*/
/*-----------EEPROM内保存的阿里云证书信息有效长度----------------*/
/*---------------------------------------------------------------*/
#define VALID_ALICER_DATA      0xDD                                            //表示三元组+固件版本有效
#define PRODUCTKEY_LEN         11                                              //三元组 ProductKey     字符串长度        
#define DEVICENAME_LEN         32                                              //三元组 DeviceName     字符串长度        
#define DEVICESECRET_LEN       32                                              //三元组 DeviceSecret   字符串长度                
#define VERSION_LEN            23                                              //A区应用程序固件版本号字符串长度

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

//---------------------------------------------------------------//
//----------------结构体，本地/OTA更新固件时的变量---------------//
//---------------------------------------------------------------//
typedef struct{   
   	uint8_t     UpdateBuff[FLASH_SECTOR_SIZE];      //保存Xmodem/OTA更新时，固件数据的缓冲区
	uint16_t    UpdateCRC16_res;                    //保存计算xmodem协议CRC16的结果
	uint16_t    XmodemPackNO;                       //保存接收的xmodem数据包编号	
}Update_CB;      

//---------------------------------------------------------------//
//-------------各种外部变量声明，便于其他源文件调用变量----------//
//---------------------------------------------------------------//
extern Update_CB      Update;                      //外部变量声明，本地/OTA更新固件时结构体变量
extern uint32_t BootEventFlag;                     //外部变量声明，发生各种事件的标志变量
extern AliyunInfo_CB   AliInfoCB;                  //外部变量声明，EEPROM内保存的所有信息结构体变量 
 

void Error_Handler(void);

#endif











