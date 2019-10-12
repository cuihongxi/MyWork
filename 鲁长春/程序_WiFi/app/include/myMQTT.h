/***
 *  2019年7月20日13:21:34
 *  MQTT协议
 *  崔洪玺
 */
#ifndef __myMQTT_H
#define __myMQTT_H

/**
 * 移植相关
 */
#include "uhead.h"
#include "mem.h"

#define	 	my_malloc 	os_zalloc		//动态内存申请
#define	 	myfree	    os_free			//释放
#define		myrandom	os_random		//随机数生成


#define	 	mymalloc	Clearmalloc		//动态申请并清零内存
#define	 	mymemcpy	os_memcpy		//内存拷贝
/*
 *
 *
    固定报头：报文类型 + 剩余长度
    可变报头：协议名长度2字节 + 协议名 + 协议级别 + 连接标志 + 和保持连接
    有效载荷：必须按这个顺序出现：客户端标识符，遗嘱主题，遗嘱消息，用户名，密码
 */
#define MQTT_Protocol_Level             0x04            //协议级别
/**
 * MQTT控制报文的类型，4~7位 | 0~3位
 */
typedef enum{
    Reserved = 0,   //保留                                            有效载荷           报文标识符字段
    CONNECT = 1<<4,        //客户端请求连接服务端                         需要               不需要        
    CONNACK = 2<<4,        //服务端到客户端,连接报文确认                  不需要              不需要
    PUBLISH = 3<<4,        //两个方向都允许,发布消息                       可选             需要（如果QoS > 0）
    PUBACK = 4<<4,         //两个方向都允许,QOS1消息发布收到确认           不需要              需要
    PUBREC = 5<<4,         //两个方向都允许,发布收到(保证交付第一步)        不需要              需要
    PUBREL = (6<<4)|0x02,  //两个方向都允许,发布释放(保证交付第2步)         不需要              需要
    PUBCOMP = 7<<4,        //两个方向都允许,QOS2消息发布完成(保证交付第3步)  不需要              需要
    SUBSCRIBE = (8<<4)|0x02,//客户端到服务端,客户端订阅请求                  需要               需要
    SUBACK = 9<<4,         //服务端到客户端,订阅请求报文确认                 需要                需要
    UNSUBSCRIBE = (10<<4)|0x02, //客户端到服务端,客户端取消订阅请求          需要                需要
    UNSUBACK = 11<<4,       //服务端到客户端,取消订阅报文确认                不需要              需要
    PINGREQ = 12<<4,        //客户端到服务端,心跳请求                       不需要              不需要
    PINGRESP = 13<<4,       //服务端到客户端,心跳响应                       不需要              不需要
    DISCONNECT = 14<<4,     //客户端到服务端,客户端断开连接                  不需要              不需要
    Reserved15 = 15<<4,     //保留

}myMQTT_ControlType;



//连接标志 Connect Flags
#define ConnectFlags_Reserved           0x01   //必须为0
#define ConnectFlags_CleanSession       0x02   //清理会话：1则清理，0不清理
#define ConnectFlags_WillFlag           0x04   //遗嘱标志：1表示，网络连接关闭时，服务端发布遗嘱消息
#define ConnectFlags_WillQoS0           0x08   //遗嘱QoSL：指定发布遗嘱消息时使用的服务质量等级,\
                                                如果遗嘱标志被设置为0，遗嘱QoS也必须设置为0(0x00) 
#define ConnectFlags_WillQoS1           0x10   //遗嘱QoSH：指定发布遗嘱消息时使用的服务质量等级,\
                                                如果遗嘱标志被设置为0，遗嘱QoS也必须设置为0(0x00) 
#define ConnectFlags_WillRetain         0x20   //遗嘱保留:如果遗嘱消息被发布时需要保留.\
                                                如果遗嘱标志被设置为0:遗嘱保留标志也必须设置为0.\
                                                如果遗嘱标志被设置为1：\
                                                如果遗嘱保留被设置为0，服务端必须将遗嘱消息当作非保留消息发布\
                                                如果遗嘱保留被设置为1，服务端必须将遗嘱消息当作保留消息发布

#define ConnectFlags_PassWordFlag       0x40   //密码标志
#define ConnectFlags_UserNameFlag       0x80   //用户名标志

/***
 * 固定报头结构体
 */
typedef struct{
	myMQTT_ControlType control;		//报文类型
	u8 leftNum[4];					//剩余长度，128进制
}FixedHeaderStr;

/***
 * 可变报头结构体
 */
typedef struct{
	u8* pdat;			//指向可变报头数据
	u32 length;			//可变报头长度
}VariableHeaderStr;

/***
 * 有效载荷结构体
 */
typedef struct{
	u8* pdat;			//指向有效载荷数据
	u32 length;			//有效载荷长度
}PayloadStr;

/***
 * 控制报文结构体
 */
typedef struct{
	FixedHeaderStr 		fixHeader;
	VariableHeaderStr 	variableHeader;
	PayloadStr 			payload;

}ControlStr;

/***
 * 数据组织后的结构体
 */

typedef struct{
	u8* pdat;			//指向有效载荷数据
	u32 length;			//有效长度
}DataMessageStr;


/**
 * 会话结构体
 */
typedef struct{
	u8* wwwDNS;				//网址
	u16 port;				//端口号
	u8  protocolLevel ;		//协议级别
	u8* usrName;
	u8* passWord;
	u8  connectFlags;		//连接标志
	u16 keepAlive;			//保存连接，秒
	u8* clientId;			//客户端标识符 “0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ”
	u16 clientIdnum;		//客户端标识符长度

}SessionStr;




/**
 * 发布消息 PUBLISH:
 *
 *
 */
//Flag Bits，0~3位
#define PUBLISH_DUP         0X08        //发布消息控制报文,重复分发标志,0 表示这是客户端或服务端第一次请求发送这个PUBLISH报文
#define PUBLISH_QoS1        0X04        //发布消息控制报文,报文的服务质量等级标志,QoS0最多分发一次,QoS1至少发布一次
#define PUBLISH_QoS0        0X02        //发布消息控制报文,报文的服务质量等级标志
#define	PUBLISH_RETAIN		0x01		//保留标志,1 服务器保留为问候消息
/**
 * 函数
 */
u8* IntTo128(u32 num ,u8* array);   							// 变换成128进制存储

// 创建一个新会话
SessionStr* myMQTT_CreatNewSessionStr(const char* name,const char* password,const char* dns,u16 port);
ControlStr* myMQTT_MallocCONNECTMessage(SessionStr* ss);		// 创建连接报文CONNECT
DataMessageStr*  myMQTT_GetControlMessage(ControlStr* message);	// 组织发送控制报文数据


#endif


