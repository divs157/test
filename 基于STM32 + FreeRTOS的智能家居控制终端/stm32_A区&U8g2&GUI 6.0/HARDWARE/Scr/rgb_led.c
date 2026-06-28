/*-------------------------------------------------*/
/*                                                 */
/*            实现RGB彩灯功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"  //包含需要的头文件
#include "rgb_led.h"        //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化RGB彩灯控制IO                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void RGB_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStruct;                   //定义一个设置GPIO的变量
		
	RGB_GROUP_CLK_ENABLE;                               //RGB 对应的IO分组 时钟使能
	GPIO_InitStruct.Pin = RGB_PIN;                      //设置RGB对应的IO口
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;         //推免输出
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 //上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;       //高速
	HAL_GPIO_Init(RGB_GROUP, &GPIO_InitStruct);         //设置	
	
    RGB_L;                                              //拉低对应IO  
}
/*------------------------------------------------*/
/*函数名：设置RGB彩灯数据                         */
/*参  数：RGBdata :RGB三色值                      */
/*参  数：num :级联的彩灯个数                     */
/*返回值：无                                      */
/*------------------------------------------------*/
                                                      //定义一个变量，把RGB的值转换成GRB

void RGB_SetData(uint32_t RGBdata, uint8_t num)
{ 
	int32_t GRBdata;   
	int8_t i;                                                                //定义一个变量，用于for循环
	
	GRBdata = ((RGBdata<<8)&0x00FF0000)|((RGBdata>>8)&0x0000FF00)|(RGBdata&0x000000FF);  //把RGB的值转换成GRB
	while(num--){                                                                        //级联了几个RGB，就循环几次
		for(i=23;i>=0;i--){                                                              //循环24次发送3字节数据 共计24个二进制位
			if(GRBdata&(1<<i)){                                                          //从最高位开始一位一位的发送，如果是二进制位1，进入if
				RGB_BIT1;                                                                //RGB彩灯发送二进制位1
			}else RGB_BIT0;                                                              //反之如果是是二进制位0，进入else，RGB彩灯发送二进制位0
		}
	}
	RGB_L;                                                                               //发送完24位数据后，拉低对应IO
}
/*-------------------------------------------------*/
/*函数名：关闭RGB彩灯                              */
/*参  数：num :级联的彩灯个数                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void RGB_Close(uint8_t num)
{ 
	int8_t i;                   //定义一个变量，用于for循环
	
	while(num--){               //级联了几个RGB，就循环几次
		for(i=23;i>=0;i--){     //循环24次发送二进制位0，关闭彩灯
			RGB_BIT0;           //RGB彩灯发送二进制位0
		}
	}
	RGB_L;                      //发送完24位数据后，拉低对应IO
}
/*-------------------------------------------------*/
/*函数名：计算RGB分量结合亮度值的数据              */
/*参  数：data ：RGB值                             */
/*参  数：v    ：亮度                              */
/*返回值：RGB的最终值                              */
/*-------------------------------------------------*/
uint32_t RBG_SetColoerData(int32_t data, int32_t v)
{				
	return (((((data&0xFF0000)>>16))*v/100)<<16)|(((((data&0x00FF00)>>8))*v/100)<<8)|(((((data&0x0000FF)>>0))*v/100));
}

/*-------------------------------------------------*/
/*函数名：计算RGB分量数据                           */
/*参  数：data ：保存RGB值的缓冲区                  */
/*参  数：h    ：色度                              */
/*参  数：s    ：饱和度                             */
/*参  数：v    ：明度                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
uint32_t RBG_GetColoerData(int32_t h, int32_t s, int32_t v)
{
	int32_t R,G,B;                //定义3个变量，保存RGB的3色分量值
	
	if((h>=0)&&(h<60)){                                    
		R = 255;
		G = h*255/60;
		B = 0;
	}else if((h>=60)&&(h<120)){
		R = 255-(h-60)*255/60;
		G = 255;
		B = 0;
	}else if((h>=120)&&(h<180)){
		R = 0;
		G = 255;
		B = (h-120)*255/60;	
	}else if((h>=180)&&(h<240)){
		R = 0;
		G = 255-(h-180)*255/60;
		B = 255;		
	}else if((h>=240)&&(h<300)){
		R = (h-240)*255/60;
		G = 0;
		B = 255;		
	}else if((h>=300)&&(h<360)){
		R = 255;
		G = 0;
		B = 255-(h-300)*255/60;
	}

	R = R*s/100+255*(100-s)/100;
	G = G*s/100+255*(100-s)/100;
	B = B*s/100+255*(100-s)/100;
	
	return (((R*v/100)<<16)|((G*v/100)<<8)|(B*v/100));
}


/*-------------------------------------------------*/
/* 函数名：RGB_SetRGB                                    */
/* 参数   ：R       ：红色分量值                            */
/* 参数   ：G       ：绿色分量值                            */
/* 参数   ：B       ：蓝色分量值                            */
/* 参数   ：brightness：亮度值，范围通常是0-255             */
/* 参数   ：num     ：需要设置的LED数量                      */
/* 返回值：无                                          */
/*-------------------------------------------------*/
//void RGB_SetRGB(uint8_t R, uint8_t G, uint8_t B, uint8_t brightness, uint8_t num) {
//    int8_t i;  // 定义一个变量，用于for循环

//    // 根据LED数量进行循环
//    while (num--) {
//        // 发送红色分量
//        for (i = 7; i >= 0; i--) {  // 循环8次发送红色分量的8个二进制位
//            uint8_t bit = (B * brightness) >> i;
//            if (bit & 1) {  // 如果是二进制位1，进入if
//                RGB_BIT1;  // RGB彩灯发送二进制位1
//            } else {
//                RGB_BIT0;  // 反之如果是二进制位0，进入else，RGB彩灯发送二进制位0
//            }
//        }

//        // 发送绿色分量
//        for (i = 7; i >= 0; i--) {  // 循环8次发送绿色分量的8个二进制位
//            uint8_t bit = (R * brightness) >> i;
//            if (bit & 1) {  // 如果是二进制位1，进入if
//                RGB_BIT1;  // RGB彩灯发送二进制位1
//            } else {
//                RGB_BIT0;  // 反之如果是二进制位0，进入else，RGB彩灯发送二进制位0
//            }
//        }

//        // 发送蓝色分量
//        for (i = 7; i >= 0; i--) {  // 循环8次发送蓝色分量的8个二进制位
//            uint8_t bit = (B * brightness) >> i;
//            if (bit & 1) {  // 如果是二进制位1，进入if
//                RGB_BIT1;  // RGB彩灯发送二进制位1
//            } else {
//                RGB_BIT0;  // 反之如果是二进制位0，进入else，RGB彩灯发送二进制位0
//            }
//        }
//    }

//    RGB_L;  // 发送完所有数据后，拉低对应IO
//}

/*-------------------------------------------------*/
/* 函数名：RGB_SetRGB                                    */
/* 参数   ：R       ：红色分量值                            */
/* 参数   ：G       ：绿色分量值                            */
/* 参数   ：B       ：蓝色分量值                            */
/* 参数   ：v       ：亮度值                                */
/* 参数   ：num     ：需要设置的LED数量                      */
/* 返回值：无                                          */
/*-------------------------------------------------*/
void RGB_SetRGB(uint8_t R, uint8_t G, uint8_t B, uint8_t v, uint8_t num)
{ 
    uint32_t RGBdata;   
    int32_t GRBdata;   
    int8_t i;                                                                //定义一个变量，用于for循环

    // 将R、G、B合并成RGBdata
    RGBdata = ((uint32_t)R << 16) | ((uint32_t)G << 8) | B;

    // 计算结合亮度值后的RGB数据
    uint8_t adjustedR = (R * v) / 100;
    uint8_t adjustedG = (G * v) / 100;
    uint8_t adjustedB = (B * v) / 100;

    // 确保调整后的RGB值不超过255
    adjustedR = adjustedR > 255 ? 255 : adjustedR;
    adjustedG = adjustedG > 255 ? 255 : adjustedG;
    adjustedB = adjustedB > 255 ? 255 : adjustedB;

    // 把调整亮度后的RGB的值转换成GRB
    GRBdata = ((((uint32_t)adjustedR << 16) | ((uint32_t)adjustedG << 8) | adjustedB) << 8) & 0x00FF0000 |
              ((((uint32_t)adjustedR << 16) | ((uint32_t)adjustedG << 8) | adjustedB) >> 8) & 0x0000FF00 |
              (((uint32_t)adjustedR << 16) | ((uint32_t)adjustedG << 8) | adjustedB) & 0x000000FF;

    while(num--){                                                                        //级联了几个RGB，就循环几次
        for(i=23;i>=0;i--){                                                              //循环24次发送3字节数据 共计24个二进制位
            if(GRBdata & (1 << i)){                                                    //从最高位开始一位一位的发送，如果是二进制位1，进入if
                RGB_BIT1;                                                                //RGB彩灯发送二进制位1
            } else {                                                                      //反之如果是是二进制位0，进入else，RGB彩灯发送二进制位0
                RGB_BIT0;
            }
        }
    }
    RGB_L;                                                                               //发送完24位数据后，拉低对应IO
}
