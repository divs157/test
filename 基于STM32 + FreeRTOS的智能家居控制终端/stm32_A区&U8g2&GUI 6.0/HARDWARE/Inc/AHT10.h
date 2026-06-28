/*-------------------------------------------------*/
/*                                                 */
/*         实现AHT10功能的头文件       				*/
/*                                                 */
/*-------------------------------------------------*/

#ifndef __AHT10_H
#define __AHT10_H


#define AHT10_ADDRESS 0x70
#define AHT10_WRITE 0x70
#define AHT10_READ 0x71


void AHT10_Init(void);
uint8_t AHT10_Check(void);
void AHT10_Reset(void);
uint8_t AHT10_Read_Data(double *temperature,double *humidity);



#endif
