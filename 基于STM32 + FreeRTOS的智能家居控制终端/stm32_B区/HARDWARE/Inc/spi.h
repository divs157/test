/*-------------------------------------------------*/
/*                                                 */
/*          实现各个SPI接口功能的头文件            */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __SPI_H
#define __SPI_H

extern SPI_HandleTypeDef SPI1_Handler;  //SPI1句柄
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

void SPI1_Init(void);                          //函数声明，初始化SPI1接口
uint8_t SPI1_ReadWriteByte(uint8_t);           //函数声明，SPI接口读写数据

#endif


