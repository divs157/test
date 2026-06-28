#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "usart.h"

I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

void I2C_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 400000;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    HAL_I2C_Init(&hi2c2);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hi2c->Instance == I2C2)
    {
        __HAL_RCC_I2C2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_DMA1_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
		
		hdma_i2c2_tx.Instance = DMA1_Channel4;
        hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
        hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(hi2c, hdmatx, hdma_i2c2_tx);
        HAL_DMA_Init(&hdma_i2c2_tx);

        HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

        hdma_i2c2_rx.Instance = DMA1_Channel5;
        hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
        hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
        __HAL_LINKDMA(hi2c, hdmarx, hdma_i2c2_rx);
        HAL_DMA_Init(&hdma_i2c2_rx);

        HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    }
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == I2C2)
	{
		while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY);
	}
	
}



