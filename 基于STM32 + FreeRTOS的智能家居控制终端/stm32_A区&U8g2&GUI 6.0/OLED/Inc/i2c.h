#ifndef __MYI2C_H
#define __MYI2C_H

#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c2;
extern DMA_HandleTypeDef hdma_i2c2_rx;
extern DMA_HandleTypeDef hdma_i2c2_tx;


void I2C_Init(void);


#endif
