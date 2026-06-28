/*-------------------------------------------------*/
/*                                                 */
/*          实现各个SPI接口功能的源文件            */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "spi.h"              //包含需要的头文件

SPI_HandleTypeDef SPI1_Handler;  //SPI1句柄

DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

/*-------------------------------------------------*/
/*函数名：初始化SPI1接口                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SPI1_Init(void)
{
    SPI1_Handler.Instance=SPI1;                                  //SPI1
    SPI1_Handler.Init.Mode=SPI_MODE_MASTER;                      //设置SPI工作模式，设置为主模式
    SPI1_Handler.Init.Direction=SPI_DIRECTION_2LINES;            //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI1_Handler.Init.DataSize=SPI_DATASIZE_8BIT;                //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI1_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;             //串行同步时钟的空闲状态为高电平
    SPI1_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;                  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI1_Handler.Init.NSS=SPI_NSS_SOFT;                          //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI1_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2; //定义波特率预分频的值:波特率预分频值为2
    SPI1_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;                 //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI1_Handler.Init.TIMode=SPI_TIMODE_DISABLE;                 //关闭TI模式
    SPI1_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE; //关闭硬件CRC校验
    SPI1_Handler.Init.CRCPolynomial=7;                           //CRC值计算的多项式
    HAL_SPI_Init(&SPI1_Handler);                                 //初始化
	__HAL_SPI_ENABLE(&SPI1_Handler);                             //使能SPI1
	SPI1_ReadWriteByte(0xFF);
}

/*-------------------------------------------------*/
/*函数名：SPI接口的底层初始化                      */
/*参  数：hspi：SPI接口配置句柄                    */
/*返回值：无                                       */
/*说  明：此函数会被HAL_SPI_Init()回调调用         */
/*-------------------------------------------------*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
	if(hspi->Instance==SPI1){     
		__HAL_RCC_GPIOA_CLK_ENABLE();                        //开启端口A时钟  
		__HAL_RCC_SPI1_CLK_ENABLE();                         //开启SPI1接口时钟
//		__HAL_RCC_DMA1_CLK_ENABLE();
		
		GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;   //配置PA5 6 7
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;                   //复用模式
		GPIO_Initure.Pull=GPIO_PULLUP;                       //上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;             //HIGH模式
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);                  //配置	
		
//		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
//        HAL_NVIC_SetPriority(SPI1_IRQn,3,0);
//        HAL_NVIC_EnableIRQ(SPI1_IRQn);
//		
//		hdma_spi1_tx.Instance = DMA1_Channel3;
//        hdma_spi1_tx.Init.Mode = DMA_NORMAL;
//        hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//        hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//        hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
//        hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//        hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//        hdma_spi1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
//        __HAL_LINKDMA(hspi,hdmatx,hdma_spi1_tx);
//        HAL_DMA_Init(&hdma_spi1_tx);

//        HAL_NVIC_SetPriority(DMA1_Channel3_IRQn,3,0);
//        HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

//        hdma_spi1_rx.Instance = DMA1_Channel2;
//        hdma_spi1_rx.Init.Mode = DMA_NORMAL;
//        hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//        hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//        hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
//        hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//        hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//        hdma_spi1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
//        __HAL_LINKDMA(hspi,hdmarx,hdma_spi1_rx);
//        HAL_DMA_Init(&hdma_spi1_rx);

//        HAL_NVIC_SetPriority(DMA1_Channel2_IRQn,3,0);
//        HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	}
}
/*-------------------------------------------------*/
/*函数名：SPI接口读写数据                          */
/*参  数：TxData：发送的数据                       */
/*返回值：读取的数据                               */
/*-------------------------------------------------*/
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;                                                    //定义一个变量保存接收到的数据
    
	HAL_SPI_TransmitReceive(&SPI1_Handler,&TxData,&Rxdata,1, 1000);    //发送并接收一字节数据     
 	return Rxdata;          		                                   //返回收到的数据		
}


