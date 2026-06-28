/*-------------------------------------------------*/
/*                                                 */
/*         读写W25Qxx存储器芯片功能的源文件       */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "w25qxx.h"          //包含需要的头文件
#include "spi.h"              //包含需要的头文件
#include "usart.h"            //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化W25Qxx                            */
/*参  数：无                                       */
/*返回值：0：正确                                  */
/*-------------------------------------------------*/
uint8_t W25Qxx_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();                             //使能端口B时钟
	
	GPIO_Initure.Pin=GPIO_PIN_12;                              //设置PD2
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;                    //推免输出
//    GPIO_Initure.Pull=GPIO_PULLDOWN;                          //下拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;                   //低速模式
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);                       //设置	
    CS_DISENABLE;                                             //CS无效信号	

	SPI1_Init();                                              //初始化SPI1接口
	if(W25Qxx_ReadID()==W25QxxID){                            //读取ID，正确进入if
		W25Qxx_Write_Enable();                                //写使能
	}else return 1;                                           //读取ID错误，返回1
	
	return 0;                                                 //正确 返回0
}
/*-------------------------------------------------*/
/*函数名：W25Qxx擦写使能                          */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25Qxx_Write_Enable(void)   
{
	W25Qxx_Wait_Busy();                     //等待空闲
	CS_ENABLE;                               //CS有效信号
    SPI1_ReadWriteByte(W25Qxx_WriteEnable); //发送写使能  
	CS_DISENABLE;                            //CS无效信号	    	      
}
/*-------------------------------------------------*/
/*函数名：W25Qxx擦写禁止                          */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25QXX_Write_Disable(void)
{
	W25Qxx_Wait_Busy();                    //等待空闲
	CS_ENABLE;                              //CS有效信号
    SPI1_ReadWriteByte(W25Qxx_WriteDisable);  //发送写禁止 
	CS_DISENABLE;                           //CS无效信号
}
/*-------------------------------------------------*/
/*函数名：读取W25Qxx芯片ID                        */
/*参  数：无                                       */
/*返回值：0XEF17 是W25Qxx的ID                     */
/*-------------------------------------------------*/
uint16_t W25Qxx_ReadID(void)
{  
	uint8_t wdata[6] = {0x90,0x00,0x00,0x00,0x00,0x00};             //读取ID指令数据
	uint8_t rdata[6];                                               //接收数据缓冲区
	
	W25Qxx_Wait_Busy();                                             //等待空闲
	CS_ENABLE;                                                      //CS有效信号				    
	HAL_SPI_TransmitReceive(&SPI1_Handler,wdata,rdata,6,1000);      //发送并接收数据
	CS_DISENABLE;                                                   //CS无效信号				    
	return (rdata[4]*256 + rdata[5]);                               //返回ID                           
}  
/*-------------------------------------------------*/
/*函数名：W25Qxx等待空闲                          */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25Qxx_Wait_Busy(void)   
{   
	uint8_t res;
	
	do{
		CS_ENABLE;                                  //CS有效信号				    
		SPI1_ReadWriteByte(W25Qxx_ReadStatusReg1); //发送读取状态寄存器1命令	    
		res = SPI1_ReadWriteByte(0xff); 	        //读取状态寄存器1    
		CS_DISENABLE;                               //CS无效信号
	}while((res&0x01)==0x01);                       //等待BUSY位清空，表示空闲
} 
/*-------------------------------------------------*/
/*函数名：W25Qxx擦除一个扇区（4KB）               */
/*参  数：NBsector:第几个扇区 （0~2047）           */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25Qxx_Erase_Sector(uint16_t NBsector)   
{  
    uint8_t wdata[4];                                     //指令数据缓冲区
	
	wdata[0] = W25Qxx_SectorErase;                       //扇区擦除指令 
	wdata[1] = (NBsector*4096)>>16;                       //A23~A16地址位    
	wdata[2] = (NBsector*4096)>>8;                        //A15~A8地址位   
	wdata[3] = (NBsector*4096)>>0;                        //A7~A0地址位
	
	W25Qxx_Write_Enable();                               //写使能   
	W25Qxx_Wait_Busy();                                  //等待空闲
	CS_ENABLE;                                            //CS有效信号	  
	HAL_SPI_Transmit(&SPI1_Handler,wdata,4,1000);         //发送数据    
	CS_DISENABLE;                                         //CS无效信号	
	W25Qxx_Wait_Busy();                                  //等待擦除完成	
}
/*-------------------------------------------------*/
/*函数名：W25Qxx擦除一个块（64KB）                */
/*参  数：NBsector:第几个块 （0~127）              */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25Qxx_Erase_Block(uint8_t NBBlock)   
{  
    uint8_t wdata[4];                                     //指令数据缓冲区
	
	wdata[0] = W25Qxx_BlockErase;                        //块擦除指令 
	wdata[1] = (NBBlock*16*4096)>>16;                     //A23~A16地址位    
	wdata[2] = (NBBlock*16*4096)>>8;                      //A15~A8地址位   
	wdata[3] = (NBBlock*16*4096)>>0;                      //A7~A0地址位
	
	W25Qxx_Write_Enable();                               //写使能   
	W25Qxx_Wait_Busy();                                  //等待空闲
	CS_ENABLE;                                            //CS有效信号	  
    HAL_SPI_Transmit(&SPI1_Handler,wdata,4,1000);         //发送数据    
	CS_DISENABLE;                                         //CS无效信号
    W25Qxx_Wait_Busy();                                  //等待擦除完成	
}
/*-------------------------------------------------*/
/*函数名：W25Qxx写入一页数据（256字节）           */
/*参  数：wdata:待写入数据                         */
/*参  数：NBPage:第几个页（0~32767）               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25Qxx_Write_Page(uint8_t* wbuff, uint16_t NBPage)
{   	
	uint8_t wdata[4];                                        //指令数据缓冲区
	
	wdata[0] = W25Qxx_PageProgram;                          //页（256B）写入指令
	wdata[1] = (NBPage*256)>>16;                             //A23~A16地址位    
	wdata[2] = (NBPage*256)>>8;                              //A15~A8地址位   
	wdata[3] = (NBPage*256)>>0;                              //A7~A0地址位
   
	W25Qxx_Write_Enable();                                  //写使能
	W25Qxx_Wait_Busy();                                     //等待空闲
	CS_ENABLE;                                               //CS有效信号	  
    HAL_SPI_Transmit(&SPI1_Handler,wdata,4,2000);            //发送数据
	HAL_SPI_Transmit(&SPI1_Handler,wbuff,256,20000);          //发送数据       
	CS_DISENABLE;                                            //CS无效信号	     	         
} 
/*-------------------------------------------------*/
/*函数名：W25Qxx读取数据                          */
/*参  数：rdata:存放数据缓冲区                     */
/*参  数：rAddr:读取的开始地址                     */
/*参  数：len:读取多少字节数据                     */
/*返回值：无                                       */
/*-------------------------------------------------*/
void W25Qxx_Read(uint8_t* rdata,uint32_t rAddr,uint32_t len)   
{ 
    uint8_t wdata[4];                                     //指令数据缓冲区
	
	wdata[0] = W25Qxx_ReadData;                          //读数据指令
	wdata[1] = rAddr>>16;                                 //A23~A16地址位    
	wdata[2] = rAddr>>8;                                  //A15~A8地址位   
	wdata[3] = rAddr>>0;                                  //A7~A0地址位
	
	W25Qxx_Wait_Busy();                                  //等待空闲
	CS_ENABLE;                                            //CS有效信号	  
    HAL_SPI_Transmit(&SPI1_Handler,wdata,4,1000);         //发送数据
	HAL_SPI_Receive(&SPI1_Handler,rdata,len,20000);       //接收数据            
	CS_DISENABLE;                                         //CS无效信号	     	      
}
