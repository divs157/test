/*-------------------------------------------------*/
/*                                                 */
/*              实现FLASH功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "stmflash.h"         //包含需要的头文件
#include "boot.h"             //包含需要的头文件
#include "usart.h"            //包含需要的头文件

Flash_CB stm32FlashCB;        //操作stm32 flash的结构体
LoadApp loadapp;              //定义一个函数类型的参数,用于跳转到应用程序区

/*-------------------------------------------------*/
/*函数名：从指定地址开始写入指定长度的32Bit数据    */
/*参  数：WriteAddr：写入地址                      */
/*参  数：pBuffer：写入的数据                      */
/*参  数：NumToWrite：写入的数据长度               */
/*返回值：0 正确 非零 错误                         */
/*-------------------------------------------------*/
uint8_t Stm32Flash_Write(uint32_t WriteAddr, uint64_t *pBuffer,uint32_t NumToWrite)	
{ 
    HAL_FLASH_Unlock();                                                                             //解锁
	while(NumToWrite){                                                                              //循环写入数据
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WriteAddr, *pBuffer) != HAL_OK){        //一次写入8字节数据，并判断是否写入正确，如果错误进入if
			HAL_FLASH_Lock();                                                                       //上锁
			return 1;	                                                                            //返回1
		}
		WriteAddr += 8;                                                                             //写入正确的话，地址下移8个字节                        
		pBuffer++;                                                                                  //数据下移8个字节，接着往后写入数据 
		NumToWrite -=8;                                                                             //待写入数据量-8字节                   		
	} 
	HAL_FLASH_Lock();                                                                               //上锁
	return 0;                                                                                       //正确，返回0
}
/*-------------------------------------------------*/
/*函数名：擦除扇区                                 */
/*参  数：StartSector：起始扇区                    */
/*参  数：num：擦除几个扇区                        */
/*返回值：0 正确 非零 错误                         */
/*-------------------------------------------------*/
uint8_t Stm32Flash_Erase(uint16_t StartSector, uint16_t num, uint32_t *PageError)   	
{
	
    stm32FlashCB.EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;               //扇区擦除
	stm32FlashCB.EraseInitStruct.PageAddress = StartSector * FLASH_SECTOR_SIZE + FLASH_BASE_ADDR;     //开始擦除的扇区地址
	stm32FlashCB.EraseInitStruct.NbPages     = num;                                 //擦除几个扇区	
	HAL_FLASH_Unlock();                                                             //解锁	
	if(HAL_FLASHEx_Erase(&stm32FlashCB.EraseInitStruct, PageError) != HAL_OK){      //擦除扇区，如果出错进入if
		HAL_FLASH_Lock();                                                           //上锁
		u1_printf("擦除A区Flash失败\r\n");                                          //串口1输出信息	
		return 1;                                                                   //返回1
	}	
	HAL_FLASH_Lock();                                                               //上锁
	u1_printf("擦除A区Flash成功\r\n");                                              //串口1输出信息	
	return 0;                                                                       //返回0
}
/*-------------------------------------------------*/
/*函数名：设置栈顶地址                             */
/*参  数：addr:栈顶地址                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
__asm void MSR_MSP(uint32_t addr) 
{
	MSR MSP, r0 			
	BX r14
}
/*-------------------------------------------------*/
/*函数名：跳转到应用程序段                         */
/*参  数：appxaddr:用户代码起始地址                */
/*返回值：0:正确 1：错误                           */
/*-------------------------------------------------*/
uint8_t Load_APP(uint32_t appxaddr)
{
	//if(((*(volatile uint32_t*)appxaddr)&0x2FFE0000)==0x20000000){	    //检查栈顶地址是否合法.
	if((*(volatile uint32_t*)appxaddr)>=0x20000000&&(*(volatile uint32_t*)appxaddr)<=0x20004FFF)
	{
     	loadapp=(LoadApp)*(volatile uint32_t*)(appxaddr+4);		        //用户代码区第二个字为程序开始地址(复位地址)	
		MSR_MSP(*(volatile uint32_t*)appxaddr);					        //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		u1_printf("准备跳转到A区...\r\n");                 				//串口1输出信息
		BootLoader_Clear();                                            //跳转清理               				//串口1输出信息
		loadapp();									                    //跳转应用程序区
	}else{
		u1_printf("跳转到A区失败，进入BootLoader\r\n");                 //串口1输出信息
		U1Rx_Buff_Init();                                               //初始化串口1处理接收数据的各个变量
		BootLoader_Info();                                              //输出BootLoader命令列表信息	
		return 1;                                                       //错误，返回1
	}
	return 0;                                                           //正确返回0
}
