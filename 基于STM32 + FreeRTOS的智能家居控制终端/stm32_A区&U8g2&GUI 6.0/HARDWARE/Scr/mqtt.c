/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f1xx_hal.h"    //包含需要的头文件
#include "main.h"             //包含需要的头文件
#include "mqtt.h"             //包含需要的头文件
#include "utils_hmac.h"       //包含需要的头文件
#include "delay.h"            //包含需要的头文件
#include "usart.h"            //包含需要的头文件

MQTT_CB   Aliyun_mqtt;        //创建一个用于连接阿里云mqtt的结构体

/*----------------------------------------------------------*/
/*函数名：云服务器初始化参数，得到客户端ID，用户名和密码    */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void IoT_Parameter_Init(void)
{	
	char temp[64];                                                                					       //计算加密的时候，临时使用的缓冲区
    
	memset(&Aliyun_mqtt,0,MQTT_CB_LEN);                                                                    //连接阿里云mqtt的结构体数据全部清零
	sprintf(Aliyun_mqtt.ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);                       //构建客户端ID，并存入缓冲区
	sprintf(Aliyun_mqtt.Username,"%s&%s",DEVICENAME,PRODUCTKEY);                                           //构建用户名，并存入缓冲区	
	memset(temp,0,64);                                                                                     //临时缓冲区全部清零
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);                   //构建加密时的明文   
	utils_hmac_sha1(temp,strlen(temp),Aliyun_mqtt.Passward,(char *)DEVICESECRE,DEVICESECRET_LEN);          //以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
	sprintf(Aliyun_mqtt.ServerIP,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",PRODUCTKEY);                    //构建服务器域名
	Aliyun_mqtt.ServerPort = 1883;                                                                         //服务器端口号1883
    sprintf(Aliyun_mqtt.Stopic_Buff[0],"/sys/%s/%s/thing/service/property/set",PRODUCTKEY,DEVICENAME);     //构建第一个需要订阅的Topic，接受服务器下发数据
	sprintf(Aliyun_mqtt.Stopic_Buff[1],"/ota/device/upgrade/%s/%s",PRODUCTKEY,DEVICENAME);                 //构建第一个需要订阅的Topic，接受OTA升级通知
	sprintf(Aliyun_mqtt.Stopic_Buff[2],"/sys/%s/%s/thing/file/download_reply",PRODUCTKEY,DEVICENAME);      //构建第一个需要订阅的Topic，接受下载固件
}
/*----------------------------------------------------------*/
/*函数名：MQTT CONNECT报文 鉴权连接                         */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp;              //计算报文剩余长度时，使用的临时变量              
	int Remaining_len;     //保存报文剩余长度字节 
	
	Aliyun_mqtt.MessageID = 0;  //报文标识符清零，CONNECT报文虽然不需要添加报文标识符，但是CONNECT报文是第一个发送的报文，在此清零报文标识符，为后续报文做准备
	Aliyun_mqtt.Fixed_len = 1;                                                                                                        //CONNECT报文，固定报头长度暂定为1
	Aliyun_mqtt.Variable_len = 10;                                                                                                    //CONNECT报文，可变报头长度=10
	Aliyun_mqtt.Payload_len = 2 + strlen(Aliyun_mqtt.ClientID) + 2 + strlen(Aliyun_mqtt.Username) + 2 + strlen(Aliyun_mqtt.Passward); //CONNECT报文，计算负载长度      
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;                                                               //剩余长度=可变报头长度+负载长度
	
	Aliyun_mqtt.Pack_buff[0]=0x10;                                   //CONNECT报文 固定报头第1个字节 ：0x10	
	do{                                                              //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;                                    //剩余长度取余128
		Remaining_len = Remaining_len/128;                           //剩余长度取整128
		if(Remaining_len>0) temp |= 0x80;                            //如果Remaining_len大于等于128了 按协议要求位7置位                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;         //剩余长度字节记录一个数据
		Aliyun_mqtt.Fixed_len++;	                                 //固定报头总长度+1    
	}while(Remaining_len>0);                                         //如果Remaining_len>0的话，再次进入循环
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0]=0x00;             //CONNECT报文，可变报头第1个字节 ：固定0x00	            
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1]=0x04;             //CONNECT报文，可变报头第2个字节 ：固定0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2]=0x4D;	         //CONNECT报文，可变报头第3个字节 ：固定0x4D
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3]=0x51;	         //CONNECT报文，可变报头第4个字节 ：固定0x51
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4]=0x54;	         //CONNECT报文，可变报头第5个字节 ：固定0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+5]=0x54;	         //CONNECT报文，可变报头第6个字节 ：固定0x54
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+6]=0x04;	         //CONNECT报文，可变报头第7个字节 ：固定0x04
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+7]=0xC2;	         //CONNECT报文，可变报头第8个字节 ：使能用户名和密码校验，不使用遗嘱功能，不保留会话功能
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+8]=0x00; 	         //CONNECT报文，可变报头第9个字节 ：保活时间高字节 0x00
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+9]=0x64;	         //CONNECT报文，可变报头第10个字节：保活时间高字节 0x64   最终值=100s
	
	/*     CLIENT_ID      */
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+10] = strlen(Aliyun_mqtt.ClientID)/256;                			  		                                           //客户端ID长度高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+11] = strlen(Aliyun_mqtt.ClientID)%256;               			  		                                           //客户端ID长度低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12],Aliyun_mqtt.ClientID,strlen(Aliyun_mqtt.ClientID));                                                    //复制过来客户端ID字串	
	/*     用户名        */ 
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+12+strlen(Aliyun_mqtt.ClientID)] = strlen(Aliyun_mqtt.Username)/256; 	                                               //用户名长度高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+13+strlen(Aliyun_mqtt.ClientID)] = strlen(Aliyun_mqtt.Username)%256; 		                                           //用户名长度低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(Aliyun_mqtt.ClientID)],Aliyun_mqtt.Username,strlen(Aliyun_mqtt.Username));                           //复制过来用户名字串	
	/*      密码        */
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+14+strlen(Aliyun_mqtt.ClientID)+strlen(Aliyun_mqtt.Username)] = strlen(Aliyun_mqtt.Passward)/256;	                        //密码长度高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+15+strlen(Aliyun_mqtt.ClientID)+strlen(Aliyun_mqtt.Username)] = strlen(Aliyun_mqtt.Passward)%256;	                        //密码长度低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+16+strlen(Aliyun_mqtt.ClientID)+strlen(Aliyun_mqtt.Username)],Aliyun_mqtt.Passward,strlen(Aliyun_mqtt.Passward));   //复制过来密码字串
   
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：MQTT DISCONNECT报文 断开连接                      */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_DISCONNECT(void)
{
	Aliyun_mqtt.Pack_buff[0]=0xE0;              //第1个字节 ：固定0xE0                      
	Aliyun_mqtt.Pack_buff[1]=0x00;              //第2个字节 ：固定0x00 
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, 2);   //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：MQTT SUBSCRIBE报文 订阅Topic                      */
/*参  数：topicbuff：订阅topic报文的缓冲区                  */
/*参  数：topicnum：订阅几个topic报文                       */
/*参  数：Qs：订阅等级                                      */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char topicbuff[TOPIC_NUM][TOPIC_SIZE], int topicnum, unsigned char Qs)
{	
	int i;                            //用于for循环
	int temp;                         //计算数据时，使用的临时变量              
	int Remaining_len;                //保存报文剩余长度字节 
	
	Aliyun_mqtt.Fixed_len = 1;        //SUBSCRIBE报文，固定报头长度暂定为1
	Aliyun_mqtt.Variable_len = 2;     //SUBSCRIBE报文，可变报头长度=2     2字节报文标识符
	Aliyun_mqtt.Payload_len = 0;      //SUBSCRIBE报文，负载数据长度暂定为0
	
	for(i=0;i<topicnum;i++)                                             //循环统计topic字符串长度，用来统计负载数据长度
		Aliyun_mqtt.Payload_len += strlen(topicbuff[i]);                //每次累加1个topic长度
	Aliyun_mqtt.Payload_len += 3*topicnum;                              //负载长度不仅包含topic字符串长度，还有等级0的标识字节，一个topic一个，以及2个字节的字符串长度标识字节，一个topic三个，所以再加上3*S_TOPIC_NUM    
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //计算剩余长度=可变报头长度+负载长度
	
	Aliyun_mqtt.Pack_buff[0]=0x82;                                      //SUBSCRIBE报文 固定报头第1个字节 ：0x82	
	do{                                                                 //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;                                       //剩余长度取余128
		Remaining_len = Remaining_len/128;                              //剩余长度取整128
		if(Remaining_len>0) temp |= 0x80;                               //如果Remaining_len大于等于128了 按协议要求位7置位                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //剩余长度字节记录一个数据
		Aliyun_mqtt.Fixed_len++;	                                    //固定报头总长度+1    
	}while(Remaining_len>0);                                            //如果Remaining_len>0的话，再次进入循环
		
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = Aliyun_mqtt.MessageID/256;       //报文标识符高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = Aliyun_mqtt.MessageID%256;		  //报文标识符低字节
	Aliyun_mqtt.MessageID++;                                                          //每用一次加1
	
	temp = 0;
	for(i=0;i<topicnum;i++){                                                                               //循环复制负载topic数据		
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+temp] = strlen(topicbuff[i])/256;                    //topic字符串 长度高字节 标识字节
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3+temp] = strlen(topicbuff[i])%256;		               //topic字符串 长度低字节 标识字节
		memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+temp],topicbuff[i],strlen(topicbuff[i]));    //复制topic字串		
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topicbuff[i])+temp] = Qs;                     //订阅等级0	
		temp += strlen(topicbuff[i]) + 3;                                                                  //len等于本次循环中添加的数据量 等于 topic字串本身长度 + 2个字节长度标识 + 1个字节订阅等级
	}
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：MQTT UNSUBSCRIBE报文 取消订阅Topic                */
/*参  数：topicbuff：取消订阅topic名称缓冲区                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_UNSubscribe(char *topicbuff)
{	
	int temp;                         //计算数据时，使用的临时变量              
	int Remaining_len;                //保存报文剩余长度字节 
	
	Aliyun_mqtt.Fixed_len = 1;                            //UNSUBSCRIBE报文，固定报头长度暂定为1
	Aliyun_mqtt.Variable_len = 2;                         //UNSUBSCRIBE报文，可变报头长度=2     2字节报文标识符
	Aliyun_mqtt.Payload_len = strlen(topicbuff) + 2;      //UNSUBSCRIBE报文，负载数据长度 = topic名称长度 + 2字节长度标识
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //计算剩余长度=可变报头长度+负载长度
	
	Aliyun_mqtt.Pack_buff[0]=0xA0;                                      //UNSUBSCRIBE报文 固定报头第1个字节 ：0xA0
	do{                                                                 //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;                                       //剩余长度取余128
		Remaining_len = Remaining_len/128;                              //剩余长度取整128
		if(Remaining_len>0) temp |= 0x80;                               //如果Remaining_len大于等于128了 按协议要求位7置位                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //剩余长度字节记录一个数据
		Aliyun_mqtt.Fixed_len++;	                                    //固定报头总长度+1    
	}while(Remaining_len>0);                                            //如果Remaining_len>0的话，再次进入循环
		
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0] = Aliyun_mqtt.MessageID/256;       //报文标识符高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1] = Aliyun_mqtt.MessageID%256;		  //报文标识符低字节
	Aliyun_mqtt.MessageID++;                                                          //每用一次加1
	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2] = strlen(topicbuff)/256;                 //topic字符串 长度高字节 标识字节
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3] = strlen(topicbuff)%256;		            //topic字符串 长度低字节 标识字节
    memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4],topicbuff,strlen(topicbuff));    //复制topic字串		
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //加入发送数据缓冲区	
}
/*----------------------------------------------------------*/
/*函数名：MQTT PING报文 保活心跳包                          */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	Aliyun_mqtt.Pack_buff[0]=0xC0;              //第1个字节 ：固定0xC0                      
	Aliyun_mqtt.Pack_buff[1]=0x00;              //第2个字节 ：固定0x00 
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, 2);   //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：MQTT PUBLISH报文 等级0 发布数据                   */
/*参  数：topic_name：发布数据的topic名称                   */
/*参  数：data：数据                                        */
/*参  数：data_len：数据长度                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
		
	Aliyun_mqtt.Fixed_len = 1;                            //PUBLISH等级0报文，固定报头长度暂定为1
	Aliyun_mqtt.Variable_len = 2 + strlen(topic);         //PUBLISH等级0报文，可变报头长度=2字节(topic长度)标识字节+ topic字符串的长度
	Aliyun_mqtt.Payload_len = data_len;                   //PUBLISH等级0报文，负载数据长度 = data_len
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //计算剩余长度=可变报头长度+负载长度	
			
	Aliyun_mqtt.Pack_buff[0]=0x30;                                      //PUBLISH等级0报文 固定报头第1个字节 ：0x0x30
	do{                                                                 //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;                                       //剩余长度取余128
		Remaining_len = Remaining_len/128;                              //剩余长度取整128
		if(Remaining_len>0) temp |= 0x80;                               //如果Remaining_len大于等于128了 按协议要求位7置位                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //剩余长度字节记录一个数据
		Aliyun_mqtt.Fixed_len++;	                                    //固定报头总长度+1    
	}while(Remaining_len>0);                                            //如果Remaining_len>0的话，再次进入循环
			             
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0]=strlen(topic)/256;                      //可变报头第1个字节     ：topic长度高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1]=strlen(topic)%256;		               //可变报头第2个字节     ：topic长度低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));           //可变报头第3个字节开始 ：拷贝topic字符串	
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)],data,data_len);   //有效负荷：拷贝data数据
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：MQTT PUBLISH报文 等级1 发布数据                   */
/*参  数：topic_name：发布数据的topic名称                   */
/*参  数：data：数据                                        */
/*参  数：data_len：数据长度                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs1(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
		
	Aliyun_mqtt.Fixed_len = 1;                            //PUBLISH等级1报文，固定报头长度暂定为1
	Aliyun_mqtt.Variable_len = 2 + 2 + strlen(topic);      //PUBLISH等级1报文，可变报头长度=2字节标识符 + 2字节(topic长度)标识字节 + topic字符串的长度
	Aliyun_mqtt.Payload_len = data_len;                   //PUBLISH等级1报文，负载数据长度 = data_len
	Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len; //计算剩余长度=可变报头长度+负载长度	
			
	Aliyun_mqtt.Pack_buff[0]=0x32;                                      //PUBLISH等级1报文 固定报头第1个字节 ：0x0x32
	do{                                                                 //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;                                       //剩余长度取余128
		Remaining_len = Remaining_len/128;                              //剩余长度取整128
		if(Remaining_len>0) temp |= 0x80;                               //如果Remaining_len大于等于128了 按协议要求位7置位                   				     
		Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = temp;            //剩余长度字节记录一个数据
		Aliyun_mqtt.Fixed_len++;	                                    //固定报头总长度+1    
	}while(Remaining_len>0);                                            //如果Remaining_len>0的话，再次进入循环
			             	
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+0]=strlen(topic)/256;                                    //可变报头第1个字节     ：topic长度高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+1]=strlen(topic)%256;		                             //可变报头第2个字节     ：topic长度低字节
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2],topic,strlen(topic));                         //可变报头第3个字节开始 ：拷贝topic字符串
   	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+2+strlen(topic)] = Aliyun_mqtt.MessageID/256;            //报文标识符高字节
	Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+3+strlen(topic)] = Aliyun_mqtt.MessageID%256;		     //报文标识符低字节
	Aliyun_mqtt.MessageID++;                                                                             //每用一次加1	
	memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len+4+strlen(topic)],data,data_len);                 //有效负荷：拷贝data数据
	
	TxDataBuf_Deal(Aliyun_mqtt.Pack_buff, Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len); //加入发送数据缓冲区
}
/*----------------------------------------------------------*/
/*函数名：处理服务器发来的等级0的推送数据                   */
/*参  数：redata：接收的数据                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdata_Qs0(unsigned char *redata, int data_len)
{
	int  i;                 //用于for循环
	int  topic_len;         //定义一个变量，存放数据中topic字符串长度 + 2字节topic字符串长度标识字节的计算值   
	char topic_buff[128];   //存放topic信息
	int  Remaining_len;     //保存报文剩余长度
	char Remaining_size;    //保存报文剩余长度占用几个字节

	
	
	for(i=1;i<5;i++){                    //循环查看报文剩余长度占用几个字节 最多4个字节
		if((redata[i]&0x80)==0){         //位7不是1的话，说明到了报文剩余长度最后的1个字节
			Remaining_size = i;          //记录i，就是报文剩余长度占用的字节数量
			break;                       //跳出for
		}
	}
	Remaining_len = 0;                   //剩余长度清零
	for(i=Remaining_size;i>0;i--){       //报文剩余长度占用几个字节，就循环计次计算长度
		Remaining_len += (redata[i]&0x7f)*powdata(128,i-1);  //计算
	}
    topic_len = redata[Remaining_size+1]*256 + redata[Remaining_size+2] + 2;                            //topic字符串长度 + 2字节topic字符串长度标识字节
	memset(topic_buff,0,128);                                                                           //清空缓冲区
	memcpy(topic_buff,&redata[Remaining_size+3],topic_len-2);                                           //提取topic信息
	memset(Aliyun_mqtt.cmdbuff,0,350);                                                               	//清空缓冲区
	memcpy(Aliyun_mqtt.cmdbuff,&redata[1+Remaining_size+topic_len],Remaining_len - topic_len);	     	//拷贝命令数据部分到 Aliyun_mqtt.cmdbuff 缓冲区供后续处理	
}
/*----------------------------------------------------------*/
/*函数名：向发送缓冲区添加数据                              */
/*参  数：databuff：数据                                    */
/*参  数：datalen：数据长度                                 */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *databuff, int datalen)
{
	if(U2_TXBUFF_SIZE - U2_Control.U_TxCounter >= datalen){                        //计算发送缓冲区内剩余的空间，还够不够存放本次的数据，够的话进入if
		U2_Control.UTxDataInPtr->StartPtr = &U2_TxBuff[U2_Control.U_TxCounter];	   //记录本次存放数据的起始位置
	}else{                                                                         //反之，不够存放本次的数据，进入else
		U2_Control.U_TxCounter = 0;                                                //发送缓冲区，存放位置变量清零
		U2_Control.UTxDataInPtr->StartPtr = U2_TxBuff;                             //本次存放数据的起始位置，设置为发送缓冲区开始位置
			
	}
	memcpy(U2_Control.UTxDataInPtr->StartPtr,databuff,datalen);	                   //拷贝数据            
    U2_Control.U_TxCounter += datalen;	                                           //发送缓冲区，存放位置累加本次存放的数据量
	U2_Control.UTxDataInPtr->EndPtr = &U2_TxBuff[U2_Control.U_TxCounter];          //记录发送缓冲区本次存放的结束位置		
				
	U2_Control.UTxDataInPtr++;                                                    //数据IN指针下移
	if(U2_Control.UTxDataInPtr==U2_Control.UTxDataEndPtr)                         //如果指针到处理接收数据的结构体数组尾部了
		U2_Control.UTxDataInPtr = &U2_Control.UCB_TxBuffPtrCB[0];                 //指针归位到处理接收数据的结构体数组的起始位置，也就是0号成员
}
/*----------------------------------------------------------*/
/*函数名：x的y次幂                                          */
/*参  数：x                                                 */
/*参  数：y                                                 */
/*返回值：无                                                */
/*----------------------------------------------------------*/
int powdata(int x, int y)
{
	int data,temp,i;
	
	data = 0;
	if(y==0){
		data = 1;
	}else if(y==1){
		data = x;
	}else{
		temp = x;		
		for(i=0;i<y-1;i++){
			data = temp*x;
			temp = data;
		}	
	}	
	return data;
}
