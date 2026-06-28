/*-------------------------------------------------*/
/*                                                 */
/*               操作Wifi功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "wifi.h"	          //包含需要的头文件
#include "delay.h"	          //包含需要的头文件
#include "usart.h"	          //包含需要的头文件
#include "led.h"              //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件
#include "24c02.h"            //包含需要的头文件
#include "w25qxx.h"           //包含需要的头文件
#include "bh1750.h"           //包含需要的头文件
#include "adc.h"              //包含需要的头文件


uint8_t wifi_mode = 1;        //联网模式 0：SSID和密码写在程序里   1：Smartconfig方式用APP发送

extern TaskHandle_t xShowTimeTaskHandle;
extern TaskHandle_t xConWifiTaskHandle;

extern void IotTask(void *params);

extern void Upload_Data(void);
	
/*-------------------------------------------------*/
/*函数名：初始化WiFi的复位IO                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();                //使能端口A时钟	
	GPIO_Initure.Pin=GPIO_PIN_1;                 //设置PA1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;       //推免输出
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;      //低速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);          //设置
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);                                 //复位IO拉高电平
}
/*-------------------------------------------------*/
/*函数名：WiFi发送设置指令                         */
/*参  数：cmd：指令                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t WiFi_SendCmd(uint8_t *cmd, int16_t timeout)
{
	U2Rx_Buff_Init();                                   //初始化串口2处理接收数据的各个变量
	WiFi_printf("%s\r\n",cmd);                          //发送指令
	while(timeout--){                                   //等待超时时间到0
		vTaskDelay(100);                                  //延时100ms
		if(strstr((char *)WiFi_RX_BUF,"OK"))            //如果接收到OK表示指令成功
			break;       						        //主动跳出while循环
		u1_printf("%d ",timeout);                       //串口输出现在的超时时间
	}
	u1_printf("\r\n");                                  //串口输出信息
	if(timeout<=0)return 1;                             //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else return 0;		         				        //反之，表示正确，说明收到OK，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：WiFi加入路由器指令                       */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t WiFi_JoinAP(int16_t timeout)
{		
	U2Rx_Buff_Init();                                           //初始化串口2处理接收数据的各个变量
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS);    	//发送指令	
	while(timeout--){                                           //等待超时时间到0
		vTaskDelay(1000);                                         //延时1s
		if(strstr((char *)WiFi_RX_BUF,"WIFI GOT IP\r\n\r\nOK")) //如果接收到WIFI GOT IP表示成功
			break;       						    			//主动跳出while循环
		u1_printf("%d ",timeout);                   			//串口输出现在的超时时间
	}
	u1_printf("\r\n");                              			//串口输出信息
	if(timeout<=0)return 1;                         			//如果timeout<=0，说明超时时间到了，也没能收到WIFI GOT IP，返回1
	return 0;                                       			//正确，返回0
}
/*-------------------------------------------------*/
/*函数名：连接TCP服务器，并进入透传模式            */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
uint8_t WiFi_Connect_Server(int16_t timeout)
{	
	U2Rx_Buff_Init();                                           //初始化串口2处理接收数据的各个变量
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",Aliyun_mqtt.ServerIP,Aliyun_mqtt.ServerPort);//发送连接服务器指令
	while(timeout--){                                      	 	//等待超时与否
		vTaskDelay(100);                                     	 	//延时100ms	
		if(strstr((char *)WiFi_RX_BUF ,"CONNECT"))          	//如果接受到CONNECT表示连接成功
			break;                                          	//跳出while循环
		if(strstr((char *)WiFi_RX_BUF ,"CLOSED"))           	//如果接受到CLOSED表示服务器未开启
			return 1;                                       	//服务器未开启返回1
		if(strstr((char *)WiFi_RX_BUF ,"ALREADY CONNECTED"))	//如果接受到ALREADY CONNECTED已经建立连接
			return 2;                                       	//已经建立连接返回2
		u1_printf("%d ",timeout);                           	//串口输出现在的超时时间  
	}
	u1_printf("\r\n");                                      	//串口输出信息
	if(timeout<=0)return 3;                                 	//超时错误，返回3
	else                                                    	//连接成功，准备进入透传
	{
		u1_printf("连接服务器成功，准备进入透传\r\n");      	//串口显示信息
		U2Rx_Buff_Init();                                       //初始化串口2处理接收数据的各个变量
		WiFi_printf("AT+CIPSEND\r\n");                      	//发送进入透传指令
		while(timeout--){                                   	//等待超时与否
			vTaskDelay(100);                                 	 	//延时100ms	
			if(strstr((char *)WiFi_RX_BUF,"\r\nOK\r\n\r\n>"))   //如果成立表示进入透传成功
				break;                          				//跳出while循环
			u1_printf("%d ",timeout);           				//串口输出现在的超时时间  
		}
		if(timeout<=0)return 4;                 				//透传超时错误，返回4	
	}
	return 0;	                                				//成功返回0	
}
/*-------------------------------------------------*/
/*函数名：等待加入路由器                           */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t WiFi_WaitAP(int16_t timeout)
{		
	while(timeout--){                               		//等待超时时间到0
		vTaskDelay(1000);                            		 	//延时1s
		if(strstr((char *)WiFi_RX_BUF,"WIFI GOT IP"))       //如果接收到WIFI GOT IP表示成功
			break;       						    		//主动跳出while循环
		u1_printf("%d ",timeout);                   		//串口输出现在的超时时间
	}
	u1_printf("\r\n");                              		//串口输出信息
	if(timeout<=0)return 1;                         		//如果timeout<=0，说明超时时间到了，也没能收到WIFI GOT IP，返回1
	return 0;                                       		//正确，返回0
}
/*-------------------------------------------------*/
/*函数名：WiFi等待获取密码                         */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_WaitPassword(int timeout)
{
	
	U2Rx_Buff_Init();                           //初始化串口2处理接收数据的各个变量                           
	while(timeout--){                           //等待超时时间到0
		vTaskDelay(1000);                         //延时1s
		if(strstr((char *)WiFi_RX_BUF,"connected")) //如果串口接受到connected表示成功
			break;                              //跳出while循环  
		u1_printf("%d ",timeout);               //串口输出现在的超时时间  
	}	
	u1_printf("\r\n");                          //串口输出信息
	if(timeout<=0)return 1;                     //超时错误，返回1
	return 0;                                   //正确返回0
}

extern TaskHandle_t xMyIotTaskHandle;
/*-------------------------------------------------*/
/*函数名：WiFi 配网                                */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t WiFi_Smartconfig(int16_t timeout)
{	
	vTaskSuspend(xMyIotTaskHandle);	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
	vTaskDelay(500);                                     //延时
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);                                       //复位IO拉低电平                                     //复位IO拉高电平	
	vTaskDelay(2000);                                    //延时

	u1_printf("准备开启Smartconfig\r\n");              //串口提示数据
	if(WiFi_SendCmd("AT+CWSTARTSMART",50)){            //开启Smartconfig，100ms超时单位，总计5s超时时间
		u1_printf("开启Smartconfig失败\r\n");//返回非0值，进入if，串口提示数据
		return 1;                                      //返回4
	}else u1_printf("开启Smartconfig成功\r\n");        //串口提示数据

	u1_printf("请使用微信Airkiss传输密码\r\n");        //串口提示数据
	if(WiFi_WaitPassword(60)){                         //传输密码，1s超时单位，总计60s超时时间
		u1_printf("传输密码失败\r\n");       //返回非0值，进入if，串口提示数据
		NVIC_SystemReset();                                          //重启
		return 2;                                      //返回5
	}else u1_printf("传输密码成功\r\n");               //串口提示数据

	u1_printf("准备关闭Smartconfig\r\n");              //串口提示数据
	if(WiFi_SendCmd("AT+CWSTOPSMART",50)){             //关闭Smartconfig，100ms超时单位，总计5s超时时间
		u1_printf("关闭Smartconfig失败\r\n");//返回非0值，进入if，串口提示数据
		NVIC_SystemReset();                                          //重启
		return 3;                                      //返回6
	}else u1_printf("关闭Smartconfig成功\r\n");        //串口提示数据
	
	u1_printf("准备设置自动连接\r\n");                 //串口提示数据
	if(WiFi_SendCmd("AT+CWAUTOCONN=1",50)){            //设置自动连接，100ms超时单位，总计5s超时时间
		u1_printf("设置自动连接失败\r\n");   //返回非0值，进入if，串口提示数据
		NVIC_SystemReset();                                          //重启
		return 4;                                      //返回3
	}else u1_printf("设置自动连接成功\r\n");           //串口提示数据	
	
	return 0;                                          //正确返回0
}

extern TaskHandle_t xMyIotTaskHandle;

/*-------------------------------------------------*/
/*函数名：WiFi连接服务器                           */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t WiFi_Connect_IoTServer(void)
{		
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
	vTaskDelay(1000);                                     //延时
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);                                       //复位IO拉低电平                                     //复位IO拉高电平	
	vTaskDelay(1000);                                     //延时
	U2_Init(115200);                                   //初始化串口2
	
	u1_printf("准备设置STA模式\r\n");                  //串口提示数据
	if(WiFi_SendCmd("AT+CWMODE=1",10)){                //设置STA模式，100ms超时单位，总计5s超时时间
		u1_printf("设置STA模式失败\r\n");    //返回非0值，进入if，串口提示数据
		NVIC_SystemReset();                //重启
		return 2;                                      //返回2
	}else u1_printf("设置STA模式成功\r\n");            //串口提示数据
	
	if(wifi_mode==0){                                      //如果联网模式=0：SSID和密码写在程序里 
		u1_printf("准备取消自动连接\r\n");                 //串口提示数据
		if(WiFi_SendCmd("AT+CWAUTOCONN=0",10)){            //取消自动连接，100ms超时单位，总计5s超时时间
			u1_printf("取消自动连接失败\r\n");   //返回非0值，进入if，串口提示数据
			return 3;                                      //返回3
		}else u1_printf("取消自动连接成功\r\n");           //串口提示数据
				
		u1_printf("准备连接路由器\r\n");                   //串口提示数据	
		if(WiFi_JoinAP(30)){                               //连接路由器,1s超时单位，总计30s超时时间
			u1_printf("连接路由器失败\r\n");     //返回非0值，进入if，串口提示数据
			return 4;                                      //返回4	
		}else u1_printf("连接路由器成功\r\n");             //串口提示数据			
	}else{                                                 //如果联网模式=1：Smartconfig方式,用APP发送                                             
		u1_printf("等待连接路由器\r\n");                   //串口提示数据	
		if(WiFi_WaitAP(60)){                               //等待连接路由器,1s超时单位，总计10s超时时间
			u1_printf("连接路由器失败\r\n");     //返回非0值，进入if，串口提示数据
			return 5;                                      //返回5	
		}else u1_printf("连接路由器成功\r\n");             //串口提示数据					

	}	
	u1_printf("准备设置透传\r\n");                     //串口提示数据
	if(WiFi_SendCmd("AT+CIPMODE=1",50)){               //设置透传，100ms超时单位，总计5s超时时间
		u1_printf("设置透传失败\r\n");       //返回非0值，进入if，串口提示数据
		return 6;                                      //返回6
	}else u1_printf("设置透传成功\r\n");               //串口提示数据
	
	u1_printf("准备关闭多路连接\r\n");                 //串口提示数据
	if(WiFi_SendCmd("AT+CIPMUX=0",50)){                //关闭多路连接，100ms超时单位，总计5s超时时间
		u1_printf("关闭多路连接失败\r\n");   //返回非0值，进入if，串口提示数据
		return 7;                                      //返回7
	}else u1_printf("关闭多路连接成功\r\n");           //串口提示数据
	
	u1_printf("准备连接服务器\r\n");                   //串口提示数据
	if(WiFi_Connect_Server(100)){                      //连接服务器，100ms超时单位，总计10s超时时间
		u1_printf("连接服务器失败\r\n");     //返回非0值，进入if，串口提示数据
		return 8;                                      //返回8
	}else u1_printf("连接服务器成功\r\n");             //串口提示数据
	vTaskResume(xMyIotTaskHandle);
	U2Rx_Buff_Init();                            	   //初始化串口2处理接收数据的各个变量
	SysCB.SysEventFlag |= CONNECT_EVENT;        	   //表示连接上服务器事件发生	
	SysCB.ConnectTimer = 0;                      	   //用于记录当次连接上服务器后的时间 计数器清零
	SysCB.PingTimer  = 0;                       	   //用于记录发送PING数据包的 计数器清零
	AttributeCB.TempTimer = 0;                   	   //用于记录温度上传 计数器清零
	AttributeCB.HumiTimer = 0;                   	   //用于记录湿度上传 计数器清零
	AttributeCB.LightTimer = 0;                   	   //用于记录光强度上传 计数器清零
	AttributeCB.ADCTimer = 0;                   	   //用于记录ADC上传 计数器清零
	MQTT_ConectPack();                           	   //构建Conect报文，加入发送缓冲区，准备发送
	vTaskDelete(xConWifiTaskHandle);
	return 0;                                          //正确返回0	
}
/*-------------------------------------------------*/
/*函数名：用于各种系统参数的结构体初始化           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysInit(void)
{			 
	memset(&SysCB,0,SYS_STRUCT_LEN);                //用于各种系统参数的结构体,全部清零
}
/*-------------------------------------------------*/
/*函数名：串口2被动事件                            */
/*参  数：data ：数据                              */
/*参  数：datalen ：数据长度                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void U2PassiveEvent(uint8_t *data, uint16_t datalen)
{	
	uint16_t i;                        //用于for循环
	uint16_t tempsize,tempoffset;      //用于保存，下载量和下载偏移的计算结果
	int  firminfo_len;                 //存放下载的固件描述信息占用的长度 
	char firminfo_buff[128];           //存放下载的固件描述信息缓冲区
	int  bOffset,bSize;                //当次下载的OTA固件的偏移量和固件
	char tempbuff[128];	
	/*----------------------------------------------*/
	/*               串口2被动事件                  */
	/*                通用数据处理                  */
	/*         基本每个程序都需要处理的数据         */
	/*----------------------------------------------*/
	
	/*----------------------------------------------*/
	/*               处理CONNACK报文                */
	/*----------------------------------------------*/	
	if((data[0]==0x20)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0x20 且 当前是连接上服务器的，进入if
		u1_printf("接收到CONNACK报文\r\n");                                          //串口输出信息
		switch(data[3]){	 		                                                 //接收数据的第4个字节，表示CONNECT报文是否成功
			case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
			            SysCB.SysEventFlag |= CONNECT_PACK;        	                 //表示MQTT CONNECT报文成功事件发生
						MQTT_Subscribe(Aliyun_mqtt.Stopic_Buff,TOPIC_NUM,1);         //构建Subscribe报文，加入发送缓冲区，准备发送
						break;                                                       //跳出分支case 0x00                                              
			case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
			            NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x01   
			case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x02 
			case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x03
			case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x04
			case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case 0x05 		
			default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
						NVIC_SystemReset();                                          //重启		
						break;                                                       //跳出分支case default 								
		}		
	}
	/*----------------------------------------------*/
	/*                处理SUBACK报文                */
	/*----------------------------------------------*/
	if((data[0]==0x90)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0x90 且 当前是连接上服务器的，进入if
		u1_printf("接收到SUBACK报文\r\n");                                           //串口输出信息
        for(i=0;i<datalen-4;i++){                                                    //循环查询订阅结果
			switch(data[4+i]){		                                                 //从第5个字节，是订阅结果数据，每个topic有一个结果字节			
				case 0x00 :
				case 0x01 : u1_printf("第%d个Topic订阅成功\r\n",i+1);                //串口输出信息					           
							break;                                                   //跳出分支                                             
				default   : u1_printf("第%d个Topic订阅失败，准备重启\r\n");          //串口输出信息 
							NVIC_SystemReset();                                      //重启		
							break;                                                   //跳出分支 								
			}
		}		
	}
	/*----------------------------------------------*/
	/*              处理PINGRESP报文                */
	/*----------------------------------------------*/
    if((data[0]==0xD0)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0xD0 且 当前是连接上服务器的，进入if
		u1_printf("接收到PINGRESP报文\r\n");                                         //串口输出信息
		SysCB.SysEventFlag &=~ PING_SENT;                                            //清除PING保活包发送事件发生标志
		if(SysCB.SysEventFlag&PING_CHECK){                                           //判断事件是否发生  
			SysCB.SysEventFlag &=~PING_CHECK;                                        //清除事件标志
			SysCB.PingTimeouter = 0;                                                 //PING检查事件计数器清零
        }			
	}
	/*----------------------------------------------*/
	/*              处理PUBACK报文                  */
	/*----------------------------------------------*/
	if((data[0]==0x40)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0x40 且 当前是连接上服务器的，进入if
		u1_printf("接收到PUBACK报文\r\n");                                           //串口输出信息
        u1_printf("等级1 标识符%d的报文，发送成功\r\n",data[2]*256+data[3]);         //串口输出信息 
		if((SysCB.SysEventFlag&OTA_EVENT)){                                          //如果if成立，表示OTA事件发生了，此时收到的就是同步OTA版本号的回复报文
			SysCB.SysEventFlag &=~OTA_EVENT;                                         //清除事件标志
			u1_printf("准备重启由BootLoader更新固件\r\n");                           //串口输出信息
			NVIC_SystemReset();                                                      //重启，更新flsah									
		}
	}
	/*----------------------------------------------*/
	/*        处理服务器推送来的等级0报文           */
	/*----------------------------------------------*/
	if((data[0]==0x30)&&(SysCB.SysEventFlag&CONNECT_EVENT)){	                     //如果接收数据的第1个字节是0x30 且 当前是连接上服务器的，进入if
		u1_printf("接收到服务器推送的等级0报文\r\n");                                //串口输出信息
        MQTT_DealPushdata_Qs0(data,datalen);	                                     //处理等级0的推送数据，提取命令数据
        u1_printf("%s\r\n",Aliyun_mqtt.cmdbuff);                                     //串口1输出数据
		if(strstr(Aliyun_mqtt.cmdbuff,ATTRIBUTE1)){	                                 //如果搜索到功能属性ATTRIBUTE1 对应的命令			
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":0")){                   //如果搜索到"PowerSwitch_1":0 说明服务器下发关闭开关命令				
				AttributeCB.Switch1Sta = 0;	                                         //记录开关状态
				u1_printf("LED1熄灭\r\n");                                           //串口输出信息
				LED1_OFF;                                                            //LED1熄灭++
				WiFi_PropertyPost("{\"params\":{\"PowerSwitch_1\":0}}");             //向服务器上传数据
			}
			else if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_1\":1")){              //如果搜索到"PowerSwitch_1":1 说明服务器下发打开开关命令
				AttributeCB.Switch1Sta = 1;	                                         //记录开关状态
				u1_printf("LED1点亮\r\n");                                           //串口输出信息
				LED1_ON;                                                             //LED1点亮+
				WiFi_PropertyPost("{\"params\":{\"PowerSwitch_1\":1}}");             //向服务器上传数据
			}			
		}
		if(strstr(Aliyun_mqtt.cmdbuff,ATTRIBUTE2)){	                                //如果搜索到功能属性ATTRIBUTE2 对应的命令			
			if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":0")){                  //如果搜索到"PowerSwitch_2":0 说明服务器下发关闭开关命令				
				AttributeCB.Switch2Sta = 0;	                                        //记录开关状态
				u1_printf("LED2熄灭\r\n");                                          //串口输出信息
				LED2_OFF;                                                           //LED2熄灭
				WiFi_PropertyPost("{\"params\":{\"PowerSwitch_2\":0}}");            //向服务器上传数据
			}
			else if(strstr(Aliyun_mqtt.cmdbuff,"\"PowerSwitch_2\":1")){             //如果搜索到"PowerSwitch_2":1 说明服务器下发打开开关命令
				AttributeCB.Switch2Sta = 1;	                                        //记录开关状态
				u1_printf("LED2点亮\r\n");                                          //串口输出信息
				LED2_ON;                                                            //LED2点亮
				WiFi_PropertyPost("{\"params\":{\"PowerSwitch_2\":1}}");            //向服务器上传数据
			}			
		}
		if(strstr(Aliyun_mqtt.cmdbuff,ATTRIBUTE9)){	                                 //如果搜索到功能属性ATTRIBUTE1 对应的命令			
			if(sscanf(Aliyun_mqtt.cmdbuff,"{\"method\":\"thing.service.property.set\",\"id\":\"%*d\",\"params\":{\"powerstate\":%d},\"version\":\"1.0.0\"}",&AttributeCB.RGB_sta)==1){
				u1_printf("开关状态：%d\r\n",AttributeCB.RGB_sta);                   //串口提示数据
				if(AttributeCB.RGB_sta==1){
					RGB_SetData(RBG_SetColoerData(RBG_GetColoerData(AttributeCB.Hue,AttributeCB.Saturation,AttributeCB.Value),AttributeCB.Brightness),4); //设置彩灯的值				
					WiFi_PropertyPost("{\"params\":{\"powerstate\":1}}");            //向服务器上传数据
				}else{
					RGB_Close(4);                                                    //关闭RGB
					WiFi_PropertyPost("{\"params\":{\"powerstate\":0}}");            //向服务器上传数据
				}
			}			
		}
		else if(strstr(Aliyun_mqtt.cmdbuff,ATTRIBUTE10)){	                        //如果搜索到功能属性ATTRIBUTE1 对应的命令			
			if(sscanf(Aliyun_mqtt.cmdbuff,"{\"method\":\"thing.service.property.set\",\"id\":\"%*d\",\"params\":{\"HSVColor\":{\"Saturation\":%d,\"Value\":%d,\"Hue\":%d}},\"version\":\"1.0.0\"}",&AttributeCB.Saturation,&AttributeCB.Value,&AttributeCB.Hue)==3){ //格式化搜索，保存数据信息
				u1_printf("饱和度：%d\r\n",AttributeCB.Saturation);                  //串口提示数据
				u1_printf("明  度：%d\r\n",AttributeCB.Value);                       //串口提示数据
				u1_printf("色  调：%d\r\n",AttributeCB.Hue);                         //串口提示数据
				if(AttributeCB.RGB_sta==1)
					RGB_SetData(RBG_SetColoerData(RBG_GetColoerData(AttributeCB.Hue,AttributeCB.Saturation,AttributeCB.Value),AttributeCB.Brightness),4); //设置彩灯的值			
			}
			memset(tempbuff,0,128);
			sprintf(tempbuff,"{\"params\":{\"HSVColor\":{\"Saturation\":%d,\"Value\":%d,\"Hue\":%d}}}",AttributeCB.Saturation,AttributeCB.Value,AttributeCB.Hue);
			WiFi_PropertyPost(tempbuff);                                             //向服务器上传数据
		}
		else if(strstr(Aliyun_mqtt.cmdbuff,ATTRIBUTE11)){	                         //如果搜索到功能属性ATTRIBUTE1 对应的命令			
			if(sscanf(Aliyun_mqtt.cmdbuff,"{\"method\":\"thing.service.property.set\",\"id\":\"%*d\",\"params\":{\"brightness\":%d},\"version\":\"1.0.0\"}",&AttributeCB.Brightness)==1){
				u1_printf("亮  度：%d\r\n",AttributeCB.Brightness);                  //串口提示数据	 
				if(AttributeCB.RGB_sta==1)
					RGB_SetData(RBG_SetColoerData(RBG_GetColoerData(AttributeCB.Hue,AttributeCB.Saturation,AttributeCB.Value),AttributeCB.Brightness),4); //设置彩灯的值
			}
			memset(tempbuff,0,128);
			sprintf(tempbuff,"{\"params\":{\"brightness\":%d}}",AttributeCB.Brightness);
			WiFi_PropertyPost(tempbuff);                                             //向服务器上传数据
		}
		if(strstr(Aliyun_mqtt.cmdbuff,"streamId")){	                                //如果搜索到streamId 说明是OTA升级命令		
			//提取3个关键数据
			//待OTA升级的固件编号
			//待OTA升级的固件文件数
			//待OTA升级的版本号

			if(sscanf(Aliyun_mqtt.cmdbuff,"{\"code\":\"1000\",\"data\":{\"size\":%d,\"streamId\":%d,\"sign\":\"%*32s\",\"dProtocol\":\"mqtt\",\"version\":\"%23s\",\"signMethod\":\"Md5\",\"streamFileId\":%d,\"md5\":\"%*32s\"},\"id\":%*d,\"message\":\"success\"}",&Aliyun_mqtt.streamSize,&Aliyun_mqtt.streamId,Aliyun_mqtt.OTA_Versionbuff,&Aliyun_mqtt.streamFileId)==4){
				u1_printf("OTA升级的固件编号：%d\r\n",Aliyun_mqtt.streamId);         //串口输出信息
				u1_printf("OTA升级的固件文件数：%d\r\n",Aliyun_mqtt.streamFileId);   //串口输出信息
				u1_printf("OTA升级的固件总大小：%d\r\n",Aliyun_mqtt.streamSize);     //串口输出信息
				u1_printf("OTA升级的版本号：%s\r\n",Aliyun_mqtt.OTA_Versionbuff);    //串口输出信息
				SysCB.SysEventFlag |= OTA_EVENT;                                     //OTA事件发生
				W25Qxx_Erase_Block(0);                                               //擦除第0块 共计64K空间
				if((Aliyun_mqtt.streamSize%256)!=0)                                  //判断固件是不是正好是256的整数倍，不是进入if
					Aliyun_mqtt.OTA_timers = Aliyun_mqtt.streamSize/256 + 1;         //计算下载次数
				else                                                                 //反之，进入else
					Aliyun_mqtt.OTA_timers = Aliyun_mqtt.streamSize/256;             //计算下载次数
				u1_printf("每次下载256字节数据，共计需要下载：%d次\r\n",Aliyun_mqtt.OTA_timers);    //串口输出信息
				Aliyun_mqtt.OTA_num = 1;                                                //Aliyun_mqtt.OTA_num等于1，开始第一次下载
				if(((Aliyun_mqtt.streamSize-((Aliyun_mqtt.OTA_num - 1)*256))/256)>0)    //是不是够256，是的话进入if
					tempsize = 256;                                                     //下载量就是256
				else                                                                    //不足256的话进入else
					tempsize = Aliyun_mqtt.streamSize-((Aliyun_mqtt.OTA_num - 1)*256);  //计算下载量
				tempoffset = ((Aliyun_mqtt.OTA_num - 1)*256);                           //计算当次下载的偏移量		
				u1_printf("准备OTA下载固件第%d次\r\n",Aliyun_mqtt.OTA_num);		        //串口输出提示信息
				WiFi_OTADownload(tempsize,tempoffset);                                  //发送报文，下载固件
			}else u1_printf("提取下载固件描述信息数据失败\r\n");		                //串口输出提示信息	 				
		}
		if(strstr(&Aliyun_mqtt.cmdbuff[2],"fileLength")){	                            //如果搜索到fileLength 说明下载的固件数据	
			u1_printf("接收到OTA下载固件\r\n");		                                                     	    //串口输出提示信息
			firminfo_len = Aliyun_mqtt.cmdbuff[0] *256 + Aliyun_mqtt.cmdbuff[1] + 2;                            //下载的固件描述信息占用的长度 
			memset(firminfo_buff,0,128);                                                                     	//清空缓冲区
			memcpy(firminfo_buff,&Aliyun_mqtt.cmdbuff[2],firminfo_len - 2);	     	                            //拷贝固件描述信息
			if(sscanf(firminfo_buff,"{\"code\":200,\"data\":{\"bOffset\":%d,\"fileLength\":%*d,\"bSize\":%d},\"id\":\"%*d\",\"message\":\"success\"}",&bOffset,&bSize)==2){ //提取数据
				u1_printf("本次下载范围：%d ~ %d\r\n",bOffset,bOffset+bSize);		                            //串口输出提示信息	
			}else u1_printf("提取下载固件描述信息数据失败\r\n");		                                        //串口输出提示信息	   
			W25Qxx_Write_Page((uint8_t *)&Aliyun_mqtt.cmdbuff[firminfo_len],(bOffset/256));                     //写入外部flash		
			Aliyun_mqtt.OTA_num++;                                                      //Aliyun_mqtt.OTA_num下载次数增加1
			if(Aliyun_mqtt.OTA_num<=Aliyun_mqtt.OTA_timers){                            //还没有下载完毕，进入if
				if(((Aliyun_mqtt.streamSize-((Aliyun_mqtt.OTA_num - 1)*256))/256)>0)    //是不是够256，是的话进入if
					tempsize = 256;                                                     //下载量就是256
				else                                                                    //不足256的话进入else
					tempsize = Aliyun_mqtt.streamSize-((Aliyun_mqtt.OTA_num - 1)*256);  //计算下载量
				tempoffset = ((Aliyun_mqtt.OTA_num - 1)*256);                           //计算当次下载的偏移量		
				u1_printf("准备OTA下载固件第%d次\r\n",Aliyun_mqtt.OTA_num);		        //串口输出提示信息
				WiFi_OTADownload(tempsize,tempoffset);                                  //发送报文，下载固件
			}else{         
				u1_printf("OTA固件下载完毕\r\n");	                                    //串口输出提示信息   
				memset(AliInfoCB.Version_ABuff,0,VERSION_LEN+1);                        //清除固件版本号缓冲区
				memcpy(AliInfoCB.Version_ABuff,Aliyun_mqtt.OTA_Versionbuff,strlen(Aliyun_mqtt.OTA_Versionbuff));  //拷贝新的版本号
				AliInfoCB.OTA_firelen = Aliyun_mqtt.streamSize;                         //记录固件的大小
				AliInfoCB.OTA_flag = BOOT_STA_O;                                        //标记需要OTA
				EEprom_WriteData(0,&AliInfoCB,ALIINFO_STRUCT_LEN);                      //保存信息到EEprom
				WiFi_PropertyVersion();                                                 //同步固件信息										
			}
			
		}		
	}
}
/*-------------------------------------------------*/
/*函数名：主动事件                                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void ActiveEvent(void)
{		
	char tempdatabuff[256];  //用于临时构建数据的缓冲区

	/*---------------------------------------------*/
	/*             自定义主动事件                  */
	/*---------------------------------------------*/   	
	if((SysCB.SysEventFlag&UP_TEMP_EVENT)&&(!(SysCB.SysEventFlag&OTA_EVENT))){                      //判断UP_TEMP_EVENT置位：表示该上传温度数据了  且OTA事件未发生
		SysCB.SysEventFlag &=~UP_TEMP_EVENT;                                          			    //清除UP_TEMP_EVENT的置位,清除上传温度数据事件		
		AHT10_Read_Data(&AttributeCB.tempdata,&AttributeCB.humidata);                               //读取温湿度值
		u1_printf("温度：%.2f °C\r\n",AttributeCB.tempdata);                          			    //串口输出数据
		memset(tempdatabuff,0,256);                                                       			//清空临时缓冲区	
		sprintf(tempdatabuff,"{\"params\":{\"%s\":%.2f}}",ATTRIBUTE3,AttributeCB.tempdata);         //构建数据				
		WiFi_PropertyPost(tempdatabuff);                                                            //向服务器上传数据
	} 
	else if((SysCB.SysEventFlag&UP_HUMI_EVENT)&&(!(SysCB.SysEventFlag&OTA_EVENT))){                  //判断如UP_HUMI_EVENT置位：表示该上传湿度数据了 且OTA事件未发生
		SysCB.SysEventFlag &=~UP_HUMI_EVENT;                                          			     //清除UP_HUMI_EVENT的置位,清除上传湿度数据事件		
		AHT10_Read_Data(&AttributeCB.tempdata,&AttributeCB.humidata);                                //读取温湿度值
		u1_printf("湿度：%.2f %%\r\n",AttributeCB.humidata);                           			     //串口输出数据
		memset(tempdatabuff,0,256);                                                       			 //清空临时缓冲区	
		sprintf(tempdatabuff,"{\"params\":{\"%s\":%.2f}}",ATTRIBUTE4,AttributeCB.humidata);          //构建数据		
		WiFi_PropertyPost(tempdatabuff);                                                             //向服务器上传数据
	} 	
	else if((SysCB.SysEventFlag&UP_LIGHT_EVENT)&&(!(SysCB.SysEventFlag&OTA_EVENT))){  //判断如UP_LIGHT_EVENT置位：表示该上传光照度数据了 且OTA事件未发生
		SysCB.SysEventFlag &=~UP_LIGHT_EVENT;                                         //清除UP_LIGHT_EVENT的置位,清除上传光照度数据事件
		Start_BH1750();	                                                              //采集转换
		vTaskDelay(200);                                  							  //延时200ms，等待转换完毕
		AttributeCB.lightata = Read_BH1750()/1.2;                           	  	  //读取并计算数值
		u1_printf("光照度：%.2f lx\r\n",AttributeCB.lightata);   				      //串口输出	
		memset(tempdatabuff,0,256);                                                       			 //清空临时缓冲区	
		sprintf(tempdatabuff,"{\"params\":{\"%s\":%.2f}}",ATTRIBUTE5,AttributeCB.lightata);          //构建数据		
		WiFi_PropertyPost(tempdatabuff);                                                             //向服务器上传数据
	} 
	else if((SysCB.SysEventFlag&UP_ADC_EVENT)&&(!(SysCB.SysEventFlag&OTA_EVENT))){    //判断如UP_ADC_EVENT置位：表示该上传ADC数据了 且OTA事件未发生
		SysCB.SysEventFlag &=~UP_ADC_EVENT;                                           //清除UP_ADC_EVENT的置位,清除上传ADC数据事件
		AttributeCB.adcdata[0] = (double)(Get_Adc(ADC_CHANNEL_4)*(3.3/4096.0));       //获取数据
		AttributeCB.adcdata[1] = (double)(Get_Adc(ADC_CHANNEL_8)*(3.3/4096.0));       //获取数据
		AttributeCB.adcdata[2] = (double)(Get_Adc(ADC_CHANNEL_9)*(3.3/4096.0));       //获取数据
		u1_printf("ADC_CH4:%.2f V\r\n",AttributeCB.adcdata[0]);                       //串口输出
		u1_printf("ADC_CH8:%.2f V\r\n",AttributeCB.adcdata[1]);                       //串口输出
		u1_printf("ADC_CH9:%.2f V\r\n",AttributeCB.adcdata[2]);                       //串口输出		
		memset(tempdatabuff,0,256);                                                       			 //清空临时缓冲区	
		sprintf(tempdatabuff,"{\"params\":{\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f}}",ATTRIBUTE6,AttributeCB.adcdata[0],ATTRIBUTE7,AttributeCB.adcdata[1],ATTRIBUTE8,AttributeCB.adcdata[2]);          //构建数据		
		WiFi_PropertyPost(tempdatabuff);                                                             //向服务器上传数据
	}   	
}
/*-------------------------------------------------*/
/*函数名：向服务器上传版本号                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void WiFi_PropertyVersion(void)
{	
	char topicdatabuff[64];        //用于构建发送topic的缓冲区
    char tempdatabuff[64];         //用于临时构建数据的缓冲区
	
	memset(topicdatabuff,0,64);                                                          //清空临时缓冲区	
	sprintf(topicdatabuff,"/ota/device/inform/%s/%s",PRODUCTKEY,DEVICENAME);             //构建发送topic					
	memset(tempdatabuff,0,64);                                                           //清空临时缓冲区	
	sprintf(tempdatabuff,"{\"id\": 1,\"params\":{\"version\":\"%s\"}}",AliInfoCB.Version_ABuff);   //构建数据	
	MQTT_PublishQs1(topicdatabuff,tempdatabuff,strlen(tempdatabuff));                    //等级1的PUBLISH报文，加入发送缓冲区   	
}
/*-------------------------------------------------*/
/*函数名：向服务器上传数据                         */
/*参  数：postdata ：上传的数据                    */
/*返回值：无                                       */
/*-------------------------------------------------*/
void WiFi_PropertyPost(char * postdata)
{	
	char topicdatabuff[64];       //用于构建发送topic的缓冲区

	memset(topicdatabuff,0,64);                                                          //清空临时缓冲区	
	sprintf(topicdatabuff,"/sys/%s/%s/thing/event/property/post",PRODUCTKEY,DEVICENAME); //构建发送topic					
	MQTT_PublishQs0(topicdatabuff,postdata,strlen(postdata));                            //等级1的PUBLISH报文，加入发送缓冲区   	
}
/*-------------------------------------------------*/
/*函数名：OTA下载固件                              */
/*参  数：size ：下载多少数据                      */
/*参  数：offset ：下载偏移量                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void WiFi_OTADownload(int size, int offset)
{	
	char topicdatabuff[64];       //用于构建发送topic的缓冲区
    char tempdatabuff[128];        //用于临时构建数据的缓冲区
	
	memset(topicdatabuff,0,64);                                                         //清空临时缓冲区	
	sprintf(topicdatabuff,"/sys/%s/%s/thing/file/download",PRODUCTKEY,DEVICENAME);      //构建发送topic					
	memset(tempdatabuff,0,128);                                                         //清空临时缓冲区	
	u1_printf("OTA升级的固件编号：%d\r\n",Aliyun_mqtt.streamId);         //串口输出信息
	sprintf(tempdatabuff,"{\"id\": \"1\",\"params\": {\"fileInfo\":{\"streamId\":%d,\"fileId\":%d},\"fileBlock\":{\"size\":%d,\"offset\":%d}}}",Aliyun_mqtt.streamId,Aliyun_mqtt.streamFileId,size,offset);   //构建数据	
	MQTT_PublishQs0(topicdatabuff,tempdatabuff,strlen(tempdatabuff));                   //等级1的PUBLISH报文，加入发送缓冲区   	
}


		

