/*-------------------------------------------------*/
/*                                                 */
/*              实现FLASH功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __STMFLASH_H__
#define __STMFLASH_H__

typedef  void (*LoadApp)(void);	                              //定义一个函数类型的参数.

typedef struct{       
	FLASH_EraseInitTypeDef EraseInitStruct;                   //擦除FLASH扇区配置句柄	
	uint32_t PageError;                                       //保存操作错误返回值	
}Flash_CB;                                                    //操作stm32 flash的结构体

extern Flash_CB stm32FlashCB;                                 //外部变量声明，操作stm32 flash的结构体

uint8_t Stm32Flash_Write(uint32_t, uint64_t *,uint32_t);      //函数声明，写Flash扇区
uint8_t Stm32Flash_Erase(uint16_t, uint16_t, uint32_t *);	  //函数声明，擦除Flash扇区
void MSR_MSP(uint32_t);	   	                                  //函数声明，设置栈顶地址
uint8_t Load_APP(uint32_t);	                                  //函数声明，跳转到应用程序段

#endif

















