/*-------------------------------------------------*/
/*                                                 */
/*              实现BOOT功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "boot.h"             //包含需要的头文件
#include "delay.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件
#include "timer.h"            //包含需要的头文件
#include "stmflash.h"         //包含需要的头文件
#include "rtc.h"              //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：BootLoader分支走向                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BootLoader_Branch(void)
{
	if(BootLoader_Enter(20)==0){                                                                      //20*100ms内，没有输入小写字母w，进入if
	    if(AliInfoCB.OTA_flag==BOOT_STA_O){                                                           //判断是否需要OTA升级					
			u1_printf("OTA升级\r\n");                                                                 //串口1输出信
			u1_printf("固件版本：%s\r\n",AliInfoCB.Version_ABuff);                                    //串口1输出信息
			u1_printf("固件大小：%d\r\n",AliInfoCB.OTA_firelen);                                      //串口1输出信息
			if(Stm32Flash_Erase(FLASH_A_START_SECTOR,FLASH_A_SECTO_NUM,&stm32FlashCB.PageError)==0){  //擦除A区Flash命令 执行擦除 如果成功 进入if						
				BootEventFlag |= OTA_MODEM;                                                           //A区下载（OTA）事件发生  				
			}			
		}else if(HAL_RTCEx_BKUPRead(&MyRtc,RTC_BKP_DR1)==BOOT_STANDBY_KEY){                           //读取后备寄存器，如果等于BOOT_STANDBY_KEY，进入低功耗,按键唤醒								
			u1_printf("进入低功耗,按键唤醒\r\n");                                                     //串口输出提示信息
			HAL_RTCEx_BKUPWrite(&MyRtc,RTC_BKP_DR1,0x0);                                              //后备寄存器1,写入
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);                                                 //设置PA0用于唤醒
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                                                        //清除PWR_FLAG_WU标志
			HAL_PWR_EnterSTANDBYMode();                                                               //进入待机模式						
		}else Load_APP(FLASH_A_ADDR);                                                                 //反之，如果不是BOOT_STA_O，进入else，跳转A区			
	}else{                                                                                            //反之，20*100ms内，输入小写字母w，进入else
		U1Rx_Buff_Init();                                                                             //初始化串口1处理接收数据的各个变量
		BootLoader_Info();                                                                            //输出BootLoader命令列表信息	
	}	
}
/*-------------------------------------------------*/
/*函数名：是否进入BootLoader                       */
/*参  数：counter:倒计时次数                       */
/*返回值：1：进入BootLoader   0：不进入BootLoader  */
/*-------------------------------------------------*/
uint8_t BootLoader_Enter(uint8_t counter)
{	
	u1_printf("\r\n2秒内，输入‘w’进入bootloader\r\n");              //串口1输出信息	
	while(counter--){			                                    //while循环等待倒计时到0
		if(U1_RxBuff[0] == 'w')                                     //如果接收到的第一个数据是小写字母w，将会进入BootLoader
			return 1;                                               //直接返回1，不等倒计时了，表示进入BootLoader
		HAL_Delay(100);                                              //每次循环延时100ms
	}	 
    return 0;                                                       //返回0，表示不进入BootLoader
}
/*-------------------------------------------------*/
/*函数名：BootLoader输出信息                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BootLoader_Info(void)
{	                          	
	u1_printf("[1]A区擦除\r\n");                        //串口1输出信息                                 
	u1_printf("[2]A区下载（Xmodem）\r\n");              //串口1输出信息
	u1_printf("[3]设置阿里云三元组证书信息\r\n");       //串口1输出信息
	u1_printf("[4]查询阿里云三元组证书信息\r\n\r\n");   //串口1输出信息
}	
/*-------------------------------------------------*/
/*函数名：处理BootLoader事件                       */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BootLoader_Event(uint8_t *data, uint16_t datalen)
{		      
	/*------------------------------------------------------------------------------------------------*/
	/*                                                                                                */
	/*                                          无事件发生                                            */
	/*                                                                                                */
	/*------------------------------------------------------------------------------------------------*/
	if(BootEventFlag==0){                                                                             //没有事件发生，进入if
		/*--------------------------------------*/
		/*         收到命令：'1'A区擦除         */
		/*--------------------------------------*/
		if((datalen==1)&&(data[0]=='1')){                                                             //2个成立条件  (1)1字节长度   (2)收到字符1
			u1_printf("准备擦除A区Flash\r\n");                                                        //串口1输出信息		
			Stm32Flash_Erase(FLASH_A_START_SECTOR,FLASH_A_SECTO_NUM,&stm32FlashCB.PageError);         //接收到擦除A区Flash命令 执行擦除							
			BootLoader_Info();                                                                        //输出BootLoader命令列表信息
		}	
		/*--------------------------------------*/
		/*    收到命令：'2'A区下载（Xmodem）    */
		/*--------------------------------------*/	
		else if((datalen==1)&&(data[0]=='2')){                                                        //2个成立条件  (1)1字节长度   (2)收到字符2
			u1_printf("准备通过Xmodem下载A区程序\r\n");                                               //串口1输出信息	
			__HAL_TIM_SET_COUNTER(&htim1,0);                                                          //清零定时器4计数器
			__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);                                                //清除中断标志
			HAL_TIM_Base_Start_IT(&htim1);                                                            //开定时器4，周期3S  用于定时发送C
			if(Stm32Flash_Erase(FLASH_A_START_SECTOR,FLASH_A_SECTO_NUM,&stm32FlashCB.PageError)==0){  //先擦除A区Flash，成功进入if
				u1_printf("请发送Xmodem bin 固件\r\n");                                               //串口1输出信息						
				BootEventFlag |= A_XMODEM ;                                                           //A区下载（Xmodem）事件发生 
				Update.XmodemPackNO = 0;                                                              //Xmodem当前接收的数据包编号清零
			}else{                                                                                    //如果擦除A区Flash 失败 进入else     
				BootLoader_Info();                                                                    //输出BootLoader命令列表信息
			}			
		}
		/*--------------------------------------*/
		/* 收到命令：'3'设置阿里云三元组证书信息*/
		/*--------------------------------------*/	
		else if((datalen==1)&&(data[0]=='3')){                                                        //2个成立条件  (1)1字节长度   (2)收到字符4
			BootEventFlag |= ALIYUN_CER;                                                              //ALIYUN_CER置位：表示设置阿里云三元组证书事件发生
			u1_printf("三元组证书信息重新设置\r\n");                                                  //串口1输出  提示信息
			u1_printf("输入三元组证书信息\r\n");                                                      //串口1输出信息
			u1_printf("格式如下：\r\n");                                                              //串口1输出信息
			u1_printf("/ProductKey/DeviceName/DeviceSecret/固件版本号/\r\n");                         //串口1输出信息
		}
		/*--------------------------------------*/
		/* 收到命令：'4'查询阿里云三元组证书信息*/
		/*--------------------------------------*/	
		else if((datalen==1)&&(data[0]=='4')){                                                        //2个成立条件  (1)1字节长度   (2)收到字符3			
			EEprom_ReadInfo();                                                                        //EEprom读取阿里云证书信息
			Printf_AliCer();                                                                          //串口输出阿里云证书信息
			BootLoader_Info();                                                                        //输出BootLoader命令列表信息
		}
	}	
	/*------------------------------------------------------------------------------------------------*/
	/*                                                                                                */
	/*                                    处理A区下载（Xmodem）事件                                   */
	/*                                                                                                */
	/*------------------------------------------------------------------------------------------------*/	
	else if(BootEventFlag&A_XMODEM){                                                                                          //如果A区下载（Xmodem）事件发生，进入if		
		/*--------------------------------------*/
		/*              收到数据包了            */
		/*--------------------------------------*/
		if((datalen==XMODEM_PACK_SIZE)&&(data[0]==XMODEM_SHO)){                                                              //2个成立条件  （1）XMODEM_PACK_SIZE字节数据长度   （2）第1个字节是SHO    				
			__HAL_TIM_DISABLE(&htim1);                                                                                       //关闭定时器1
			memcpy(&Update.UpdateBuff[(Update.XmodemPackNO%(FLASH_SECTOR_SIZE/XMODEM_PACKDATA_SIZE))*XMODEM_PACKDATA_SIZE],&data[3],XMODEM_PACKDATA_SIZE);//拷贝固件数据到缓冲区
			Update.XmodemPackNO++;                                                                                           //数据包编号+1		
			if((Update.XmodemPackNO%(FLASH_SECTOR_SIZE/XMODEM_PACKDATA_SIZE)) == 0){			                             //接收够1个扇区的数据，就写入flash															 
				Stm32Flash_Write(FLASH_A_ADDR + ((Update.XmodemPackNO/(FLASH_SECTOR_SIZE/XMODEM_PACKDATA_SIZE))-1)*FLASH_SECTOR_SIZE,(uint64_t *)Update.UpdateBuff,FLASH_SECTOR_SIZE);// 写入FLASH     			
			}
			u1_TxHex(XMODEM_ACK);                                                                                            //发送一字节XMODEM_ACK 准备接收下一个数据包														
		}	
		/*--------------------------------------*/
		/*         收到发送结束标志了           */
		/*--------------------------------------*/		
		else if((datalen==1)&&(data[0]==XMODEM_EOT)){                                                                        //2个成立条件  (1)1字节长度   (2)值等于XMODEM_EOT 发送完毕了
			u1_TxHex(XMODEM_ACK);                                                                                            //发送一字节XMODEM_ACK 结束传输			
			if((Update.XmodemPackNO%(FLASH_SECTOR_SIZE/XMODEM_PACKDATA_SIZE)) != 0)	                                         //接收过程中 每接收2k数据 就写一扇区到Flash 接收完毕后 如果还有剩余 把剩下的写入Flash
				Stm32Flash_Write(FLASH_A_ADDR + (Update.XmodemPackNO/(FLASH_SECTOR_SIZE/XMODEM_PACKDATA_SIZE))*FLASH_SECTOR_SIZE,(uint64_t *)Update.UpdateBuff,FLASH_SECTOR_SIZE);// 写入FLASH 
			BootEventFlag &=~ A_XMODEM;                                                                                      //清除A区下载（Xmodem）事件
			NVIC_SystemReset();                                                                                              //重启	
		} 	
		/*--------------------------------------*/
		/*         收到发送中止标志了           */
		/*--------------------------------------*/
		else if(data[0]==XMODEM_CAN){                                                                                        //1个成立条件  (1)接收的数据是XMODEM_CAN 停止传输
			__HAL_TIM_DISABLE(&htim1);                                                                                      //关闭定时器4
			u1_TxHex(XMODEM_EOT);                                                                                            //发送一字节XMODEM_EOT 结束传输
			BootEventFlag &=~ A_XMODEM;                                                                                      //清除A区下载（Xmodem）事件
			HAL_Delay(500);                                                                                                   //延迟
			BootLoader_Info();                                                                                               //输出BootLoader命令列表信息
		}
	}
	/*------------------------------------------------------------------------------------------------*/
	/*                                                                                                */
	/*                                  处理设置阿里云三元组证书事件                                  */
	/*                                                                                                */
	/*------------------------------------------------------------------------------------------------*/	
	else if(BootEventFlag&ALIYUN_CER){                                                //设置阿里云三元组证书事件发生，进入if
		if(BootLoader_AliCerInspect(data)==0){                                        //校验证书数据，根据不同的结果分别处理				
			u1_printf("阿里云证书信息设置完毕\r\n");                                  //串口1输出信息	
			AliInfoCB.valid_cer = VALID_ALICER_DATA;                                  //设置证书信息有效
			EEprom_WriteData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                        //保存信息
		    Printf_AliCer();                                                          //串口输出阿里云证书信息 
		    BootEventFlag &=~ ALIYUN_CER;                                             //清除事件标志
			BootLoader_Info();                                                        //输出BootLoader命令列表信息
		}else u1_printf("证书信息输入错误，重新输入\r\n");                            //串口1输出信息	
	}
}
/*-------------------------------------------------*/
/*函数名：主动事件                                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void ActiveEvent(void)
{	
    uint16_t i;	
	/*------------------------------------------------------------------------------------------------*/
	/*                                                                                                */
	/*                                           处理OTA事件                                          */
	/*                                                                                                */
	/*------------------------------------------------------------------------------------------------*/	
	if(BootEventFlag&OTA_MODEM){                                                                                         //OTA事件发生，进入if
		for(i=0;i<AliInfoCB.OTA_firelen/FLASH_SECTOR_SIZE;i++){        		                                             //一次读出FLASH_SECTOR_SIZE大小数据
		    u1_printf("读取%d~%d字节固件，写入Flash，请耐心等待... ...\r\n",i*FLASH_SECTOR_SIZE,(i+1)*FLASH_SECTOR_SIZE-1);//串口1输出信息
			W25Qxx_Read(Update.UpdateBuff,i*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);                                       //读FLASH_SECTOR_SIZE大小数据
			Stm32Flash_Write(FLASH_A_ADDR + i*FLASH_SECTOR_SIZE,(uint64_t *)Update.UpdateBuff,FLASH_SECTOR_SIZE);        //写入flash			
		}
		if((AliInfoCB.OTA_firelen%FLASH_SECTOR_SIZE)!=0){                                                                //是否还有不满FLASH_SECTOR_SIZE整数倍的数据
			u1_printf("读取%d~%d字节固件，写入Flash，请耐心等待... ...\r\n",i*FLASH_SECTOR_SIZE,AliInfoCB.OTA_firelen);//串口1输出信息
			W25Qxx_Read(Update.UpdateBuff,(AliInfoCB.OTA_firelen/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE,AliInfoCB.OTA_firelen%FLASH_SECTOR_SIZE);  //读出数据			
			Stm32Flash_Write(FLASH_A_ADDR + i*FLASH_SECTOR_SIZE,(uint64_t *)Update.UpdateBuff,FLASH_SECTOR_SIZE);        //写入flash
		} 
		u1_printf("OTA更新完毕，重启\r\n");                                                                              //串口1输出信息
		AliInfoCB.OTA_flag = 0;                                                                                          //取消OTA升级标志
		EEprom_WriteData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                                                               //保存信息到EEprom
		NVIC_SystemReset();                                                                                              //重启	
	}   	
}
/*-------------------------------------------------*/
/*函数名：BootLoader跳转到应用区前的清理工作       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BootLoader_Clear(void)
{
	HAL_UART_DeInit(&U1_Control.UART_Handler);      //串口1恢复默认值
	HAL_TIM_Base_DeInit(&htim1);                    //定时器1恢复默认值
	HAL_RTC_DeInit(&MyRtc);                         //RTC恢复默认值
}
/*-------------------------------------------------*/
/*函数名：串口输出阿里云证书信息                   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Printf_AliCer(void)
{			
	u1_printf("--------------------------阿里云设备证书信息-------------------------\r\n");          //串口1输出信息
	u1_printf("ProductKey：%s\r\n",AliInfoCB.ProductKeyBuff);                                        //串口1输出信息
	u1_printf("DeviceName：%s\r\n",AliInfoCB.DeviceNameBuff);                                        //串口1输出信息
	u1_printf("DeviceSecret：%s\r\n",AliInfoCB.DeviceSecretBuff);                                    //串口1输出信息
	u1_printf("固件版本：%s\r\n",AliInfoCB.Version_ABuff);                                           //串口1输出信息
	u1_printf("---------------------------------------------------------------------\r\n\r\n");      //串口1输出信息	
}
/*-------------------------------------------------*/
/*函数名：检测接收到的阿里云证书信息               */
/*参  数：无                                       */
/*返回值：非零：错误 0：正确                       */
/*-------------------------------------------------*/
uint8_t BootLoader_AliCerInspect(uint8_t *data)
{
	if(sscanf((char *)data,"/%[^/]/%[^/]/%[^/]/%[^/]/",AliInfoCB.ProductKeyBuff,AliInfoCB.DeviceNameBuff,AliInfoCB.DeviceSecretBuff,AliInfoCB.Version_ABuff)==4){     //提取信息，正确进入if
		if(strlen((char *)AliInfoCB.ProductKeyBuff)!=PRODUCTKEY_LEN) return 1;           //提取 PRODUCTKEY    数据后，如果长度错误 返回1
		if(strlen((char *)AliInfoCB.DeviceNameBuff)>DEVICENAME_LEN) return 2;            //提取 DEVICENAME    数据后，如果长度错误 返回2
		if(strlen((char *)AliInfoCB.DeviceSecretBuff)!=DEVICESECRET_LEN) return 3;       //提取 DEVICESECRET  数据后，如果长度错误 返回3
	    if(strlen((char *)AliInfoCB.Version_ABuff)!=VERSION_LEN) return 4;               //提取 Version_ABuff 数据后，如果长度错误 返回4			
	}else return 5;                                                                      //输入的格式错误  返回5
	
	return 0;                                                                            //正确，返回0
}
