/*-------------------------------------------------*/
/*                                                 */
/*              实现BOOT功能的头文件               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __BOOT_H
#define __BOOT_H

void BootLoader_Branch(void);                 //函数声明，BootLoader分支走向
uint8_t BootLoader_Enter(uint8_t);            //函数声明，是否进入BootLoader
void BootLoader_Info(void);                   //函数声明，BootLoader输出信息 
void BootLoader_Event(uint8_t *, uint16_t);   //函数声明，处理BootLoader事件
void BootLoader_Clear(void);                  //函数声明，BootLoader跳转到应用区前的清理工作
void Printf_AliCer(void);                     //函数声明，串口输出阿里云证书信息
uint8_t BootLoader_AliCerInspect(uint8_t *);  //函数声明，检测接收到的阿里云证书信息
void ActiveEvent(void);                       //函数声明，主动事件

#endif


