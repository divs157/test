/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H
         
#define  MQTT_TxData(x)       u2_TxData(x)           //串口2控制数据
#define  PRODUCTKEY   AliInfoCB.ProductKeyBuff       //产品KEY
#define  DEVICENAME   AliInfoCB.DeviceNameBuff       //设备名  
#define  DEVICESECRE  AliInfoCB.DeviceSecretBuff     //设备秘钥   
#define  TOPIC_NUM    3                              //需要订阅的最大Topic数量
#define  TOPIC_SIZE   64                             //存放Topic字符串名称缓冲区长度

typedef struct{
	char ClientID[64];                         //存放客户端ID的缓冲区
	char Username[64];                         //存放用户名的缓冲区
	char Passward[64];                         //存放密码的缓冲区
	char ServerIP[64];                         //存放服务器IP或是域名
	int  ServerPort;                           //存放服务器的端口号
	unsigned char Pack_buff[256];              //发送报文的缓冲区
	int  MessageID;                            //记录报文标识符
	int  Fixed_len;                       	   //固定报头长度
	int  Variable_len;                         //可变报头长度
	int  Payload_len;                          //有效负荷长度
	char Stopic_Buff[TOPIC_NUM][TOPIC_SIZE];   //包含的是订阅的主题列表
	char cmdbuff[400];                         //保存推送的数据中的命令数据部分
	int  streamId;                             //OTA升级时，保存streamId数据
	int  streamFileId;                         //OTA升级时，保存streamFileId
	int  streamSize;                           //OTA升级时，固件总的大小
    char OTA_Versionbuff[64];                  //OTA升级时，保存新的版本号
	int  OTA_timers;                           //OTA升级时，每次下载2048，总共需要下载多少次
	int  OTA_num;                              //OTA升级时，保存当前下载到第几次2k数据了
}MQTT_CB;  
#define MQTT_CB_LEN         sizeof(MQTT_CB)    //结构体长度 

extern MQTT_CB   Aliyun_mqtt;  //外部变量声明，用于连接阿里云mqtt的结构体

extern char Ping_flag;         //外部变量声明，ping报文状态      0：正常状态，等待计时时间到，发送Ping报文
                               //外部变量声明，ping报文状态      1：Ping报文已发送，当收到 服务器回复报文的后 将1置为0
extern char Connect_flag;      //外部变量声明，同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
extern char ReConnect_flag;    //外部变量声明，重连服务器状态    0：连接还存在  1：连接断开，重连
extern char ConnectPack_flag;  //外部变量声明，CONNECT报文状态   1：CONNECT报文成功
extern char SubcribePack_flag; //外部变量声明，订阅报文状态      1：订阅报文成功

void IoT_Parameter_Init(void);                                                    //函数声明，云服务器初始化参数，得到客户端ID，用户名和密码
void MQTT_ConectPack(void);                                                       //函数声明，MQTT CONNECT报文       鉴权连接
void MQTT_DISCONNECT(void);                                                       //函数声明，MQTT DISCONNECT报文    断开连接
void MQTT_Subscribe(char topicbuff[TOPIC_NUM][TOPIC_SIZE],int, unsigned char);    //函数声明，MQTT SUBSCRIBE报文     订阅Topic
void MQTT_UNSubscribe(char *);                                                    //函数声明，MQTT UNSUBSCRIBE报文   取消订阅Topic
void MQTT_PingREQ(void);                                                          //函数声明，MQTT PING报文          保活心跳包
void MQTT_PublishQs0(char *,char *,int);                                          //函数声明，MQTT PUBLISH报文 等级0 发布数据
void MQTT_PublishQs1(char *,char *,int);                                          //函数声明，MQTT PUBLISH报文 等级1 发布数据
void MQTT_DealPushdata_Qs0(unsigned char *,int);                                  //函数声明，处理服务器发来的等级0的推送数据
void TxDataBuf_Deal(unsigned char *, int);                                        //函数声明，向发送缓冲区添加数据
int powdata(int , int);
#endif
