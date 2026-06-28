/*-------------------------------------------------*/
/*                                                 */
/*             实现BH1750功能的头文件              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __BH1750_H
#define __BH1750_H

#define	  SlaveAddress   0x46

void Write_BH1750(unsigned char REG_Address);   //函数声明，写BH1750
uint16_t Read_BH1750(void);                     //函数声明，读BH1750
void Start_BH1750(void);                        //函数声明，开始采集数据

#endif
