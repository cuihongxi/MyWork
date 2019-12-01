/***
 *  2019年7月20日13:21:34
 *  MQTT协议
 *  崔洪玺
 *  2019年11月30日21:56:51 （完结）
 *  该文件不对外开放，只在myMQTT_APP文件中被引用
 */
#ifndef __myMQTT_H
#define __myMQTT_H

/**
 * 移植相关
 */
#include "uhead.h"
#include "mem.h"
#include "SingleList.h"

#define	 	my_malloc 	os_zalloc		//动态内存申请
#define	 	myfree	    os_free			//释放
#define		myrandom	os_random		//随机数生成
#define	 	mymalloc	Clearmalloc		//动态申请并清零内存
/*
 *
 *
    固定报头：报文类型 + 剩余长度
    可变报头：协议名长度2字节 + 协议名 + 协议级别 + 连接标志 + 和保持连接
    有效载荷：必须按这个顺序出现：客户端标识符，遗嘱主题，遗嘱消息，用户名，密码
 */

/**
 * MQTT控制报文的类型，4~7位 | 0~3位
 */
typedef enum{
    Reserved = 0,   			//保留                                            					有效载荷           	报文标识符字段
    CONNECT = 1<<4,        		//客户端请求连接服务端                        				 需要               	不需要
    CONNACK = 2<<4,        		//服务端到客户端,连接报文确认                  			不需要              	不需要
    PUBLISH = 3<<4,        		//两个方向都允许,发布消息                       			可选             	需要（如果QoS > 0）
    PUBACK = 4<<4,         		//两个方向都允许,QOS1消息发布收到确认       		不需要              	需要
    PUBREC = 5<<4,         		//两个方向都允许,发布收到(保证交付第一步)        不需要              	需要
    PUBREL = (6<<4)|0x02,  		//两个方向都允许,发布释放(保证交付第2步)         不需要             	 需要
    PUBCOMP = 7<<4,        		//两个方向都允许,QOS2消息发布完成(保证交付第3步)  不需要             	 需要
    SUBSCRIBE = (8<<4)|0x02,	//客户端到服务端,客户端订阅请求                  		 	 需要               	需要
    SUBACK = 9<<4,         		//服务端到客户端,订阅请求报文确认                			 需要               	 需要
    UNSUBSCRIBE = (10<<4)|0x02, //客户端到服务端,客户端取消订阅请求          			需要                	需要
    UNSUBACK = 11<<4,       	//服务端到客户端,取消订阅报文确认                			不需要             	 需要
    PINGREQ = 12<<4,        	//客户端到服务端,心跳请求                       			不需要              	不需要
    PINGRESP = 13<<4,       	//服务端到客户端,心跳响应                       			不需要              	不需要
    DISCONNECT = 14<<4,     	//客户端到服务端,客户端断开连接                  			不需要              	不需要
    Reserved15 = 15<<4,     	//保留

}myMQTT_ControlType;

typedef	u16	IDTYPE;				// 报文标识符的类型

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
// 连接返回码
#define		CONNACK_OK						0	// 连接OK
#define		CONNACK_ERROR_PL				1	// 不支持的协议版本
#define		CONNACK_ERROR_CLIENTID			2	// 不合格的客户端标识符
#define		CONNACK_ERROR_SERVICE			3	// 服务器不可用
#define		CONNACK_ERROR_USERNAME			4	// 用户名或密码错误
#define		CONNACK_ERROR_AUTH				5	// 客户端未被授权


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
	u8* pdat;				//指向可变报头数据
	u32 length;				//可变报头长度
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
	FixedHeaderStr 		fixHeader;				// 固定报头：报文类型 + 剩余长度
	VariableHeaderStr 	variableHeader;			// 可变报头
	PayloadStr 			payload;				// 有效载荷

}ControlStr;

/***
 * 数据组织后的结构体
 */

typedef struct{
	u8* pdat;			//指向有效载荷数据
	u32 length;			//有效长度
}DataMessageStr;

typedef enum{
	 PUBLISH_QoS0 = 0,        	//发布消息控制报文,报文的服务质量等级标志
	 PUBLISH_QoS2 = 0X04,       //发布消息控制报文,报文的服务质量等级标志,QoS0最多分发一次,QoS1至少发布一次
	 PUBLISH_QoS1 = 0X02,       //发布消息控制报文,报文的服务质量等级标志
}enumQos;

typedef struct{
	 u8* 		sub;		// 字符串
	 enumQos 	reqQos;		// 服务质量要求
}subStr;				// 订阅主题结构体

typedef enum{
	dup_no = 0,
	dup_yes = 0x08,
}enumdup;				// 是否重发标志

typedef struct{
	 u8* 		sub;		// 主题字符串
	 enumQos 	reqQos;		// 服务质量要求
	 u8* 		pub;		// 字符串
	 bool		retain;		// publish报文中是否保存信息
	 enumdup	dup;		// 是否重发标志
}pubStr;				// publish结构体

typedef struct{
	 void*  mes;		// 信息头地址
	 IDTYPE id;			// 报文标识符

}idNodeStr;				// 报文标识符链结构体

typedef	u8	ERROTYPE;

/****错误编码****/
#define	ERRO_OK			0		// 无错误
#define	ERRO_QoS2		0x04	// 暂时不对QoS2进行处理
#define	ERRO_MALLOC		0x08	// 动态内存申请失败


/**
 * 会话结构体
 */
typedef struct _SessionStr{
	struct espconn* 		espconn;			// 网络连接结构体
	char* 					url;				// 网址
	u16 					port;				// 端口号
	u8  					protocolLevel ;		// 协议级别
	char*  					usrName;
	char*  					passWord;
	u8  					connectFlags;		// 连接标志
	u16 					keepAlivetime;		// 保存连接，秒
	char*  					clientId;			// 客户端标识符 , 客户端ID
	myMQTT_ControlType 		messageType;		// 报文类型
	SingleList*				subList;			// 订阅主题链表，已成功订阅的主题添加在这个链表下
	SingleList*				idList;				// 报文标识符链表,idNodeStr添加在这个链表下
	IDTYPE					id;					// 报文标识符，每发送一次报文则自增一次
	ERROTYPE				erro;				// 保存错误代码

	void(*Connect)(struct _SessionStr*)		;			// 连接报文
	void(*Disconnect)(struct _SessionStr*)	;			// 断开连接报文
	void(*Ping)(struct _SessionStr*)		;			// PING报文
	void(*Subscribe)(struct _SessionStr*, char*, enumQos);	// 订阅主题
	void(*Unsub)(struct _SessionStr*,char* sub)		;	// 取消订阅
	void(*Puback)(struct _SessionStr*)		;			// 发布确认
	void(*KeepAlive)(struct _SessionStr*)					;			// Ping包
	void(*DisConnect)(struct _SessionStr*)					;			// 与服务器断开连接
	void(*FixVariableHeader)(ControlStr*,struct _SessionStr*);			// 填充报文可变报头
	void(*FixPayload)(ControlStr*,struct _SessionStr*);					// 填充报文有效载荷
	void(*ServerCB)(struct _SessionStr* ss,char * pdata, unsigned short len);	// 服务器回复的回调函数
	void(*Publish)(struct _SessionStr* ss,char* sub,char* pubdata,enumQos reqQos,bool retain,enumdup dup);// 发布消息

}SessionStr;




/**
 * 发布消息 PUBLISH:
 *
 *
 */
//Flag Bits，0~3位





/*用户默认参数*/
#define	CONNECT_FLAG_PARA	(ConnectFlags_UserNameFlag|ConnectFlags_PassWordFlag|ConnectFlags_CleanSession)	//连接标志参数默认值
#define	KEEPALIVE_SEC					60			// 秒，保持连接时长
#define MQTT_Protocol_Level             0x04        // 协议级别



/**
 * 函数
 */




ControlStr*  	myMQTT_CreatMessage(SessionStr* ss);						// 创建报文
void  			FixConnectVariableHeader(ControlStr* cs,SessionStr* ss);	// 填充CONNECT报文可变报头
void  			FixConnectPayload(ControlStr* cs,SessionStr* ss);			// 填充CONNECT报文有效载荷
void  			FixSubscribeVariableHeader(ControlStr* cs,SessionStr* ss);	// 填充Subscribe报文可变报头
void  			FixSubscribePayload(ControlStr* cs,SessionStr* ss);			// 填充Subscribe报文有效载荷
void  			FixUnSubscribeVariableHeader(ControlStr* cs,SessionStr* ss);// 填充UnSubscribe报文可变报头
void  			FixUnSubscribePayload(ControlStr* cs,SessionStr* ss);		// 填充UnSubscribe报文有效载荷
void  			FixPublishVariableHeader(ControlStr* cs,SessionStr* ss);	// 填充Publish报文可变报头
void  			FixPublishPayload(ControlStr* cs,SessionStr* ss);			// 填充Publish报文有效载荷

void 			myMQTT_Disconnect();										// 断开链接
void 			myMQTT_Ping();												// 心跳包，PING包

ERROTYPE  		myMQTT_SendtoServer(SessionStr* ss);						// 发送MQTT到服务器

void 			FreeSubnodInLink(SessionStr* ss,SingleList* list,void* nod);	//释放链表下的主题节点
void  			FreePubnodInIdLink(SessionStr* ss,void* nod);					//释放id链表下的publish节点
pubStr* 		myMQTT_PublishCB(SessionStr* ss,char * pdata, unsigned short len);	// 接收到服务器的推送,需要释放返回的空间

#endif


