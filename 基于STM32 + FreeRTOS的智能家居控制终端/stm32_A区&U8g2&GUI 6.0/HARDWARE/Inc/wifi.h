/*-------------------------------------------------*/
/*                                                 */
/*              操作Wifi功能的头文件               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart.h"	
#include "main.h"

#define RESET_IO(x)    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (GPIO_PinState)x)  //PA1控制WiFi的复位

#define WiFi_printf       u2_printf                 //串口2控制 WiFi
#define WiFi_RxCounter    U2_Control.U_RxCounter    //串口2控制 WiFi
#define WiFi_RX_BUF       U2_RxBuff                 //串口2控制 WiFi
#define WiFi_RXBUFF_SIZE  U2_RXBUFF_SIZE            //串口2控制 WiFi

#define SSID   "YU"                      //路由器SSID名称
#define PASS   "123456789"                           //路由器密码

extern uint8_t wifi_mode;                           //联网模式 0：SSID和密码写在程序里   1：Smartconfig方式用APP发送

void WiFi_ResetIO_Init(void);
uint8_t WiFi_SendCmd(uint8_t *cmd, int16_t timeout);
uint8_t WiFi_JoinAP(int16_t timeout);
uint8_t WiFi_Connect_Server(int16_t timeout);
uint8_t WiFi_Smartconfig(int16_t timeout);
uint8_t WiFi_WaitAP(int16_t timeout);
char WiFi_WaitPassword(int timeout);
uint8_t WiFi_Smartconfig(int16_t timeout);
uint8_t WiFi_Connect_IoTServer(void);
void SysInit(void);                               
void U2PassiveEvent(uint8_t *, uint16_t);
void ActiveEvent(void);                        
void WiFi_PropertyVersion(void);
void WiFi_PropertyPost(char *);
void WiFi_OTADownload(int, int);

void Set_Wifi(u8g2_t *u8g2,uint8_t *wifi_mode);
	
#endif


