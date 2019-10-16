#include "myMQTT.h"
#include "myWifi.h"
/**
 * �任��128���ƴ洢
 * ���ؽ���ָ��
 * ʣ�೤�ȵ��ڿɱ䱨ͷ�ĳ��ȣ�10�ֽڣ�������Ч�غɵĳ���
 */
u8* IntTo128(u32 num ,u8* array)
{
    if (num <= 0xfffffff)           //���ܳ����������
    {
        while(1)
        {
            *array = num & 0x7f;
            if(num > *array) *array |= 0x80;
            else return ++ array;   //ƫ�Ƶ���һ��λ�÷���
            num >>= 7;
            array ++;       
        }

    }else return 0;
}


/***
 * ��128���Ƶ���ת����int
 * numbyte��¼ռ�õ��ֽ���
 */
u32 Change128ToInt(u8* dat,u8* numbyte)
{
	u32 num = 0;
	u8 i = 0;
	*numbyte = 0;
	while(i<4)
	{
		(*numbyte) ++;
		num = num + ((*dat &0x7f)<<(i*7));
		if(*dat & 0x80)
		{
			dat ++;
			i ++;
		}else return num;
	}
	return num;
}
/***
 * ��̬�����ڴ棬����������ڴ�ȫ������
 */

void* Clearmalloc(u32 num)
{
	u8 *p = (u8*)my_malloc(num);
	u32 i = 0;
	for(i = 0;i<num;i++) p[i] = 0;
	return (void*)p;

}
/***
 * �ڴ濽��
 * ����pd���������һ��ָ��
 */
//u8* mymemcpy(u8* pd ,u8* ps ,u32 len)
//{
//	u32 i = 0;
//	u32 temp = len/sizeof(unsigned int);
//	for(i=0;i<temp;i++) ((u32*)pd)[i] = ((u32*)ps)[i];
//	i *= sizeof(unsigned int);
//	for(;i<len;i++) pd[i] = ps[i];
//	return &pd[len];
//}


/**
 * �������һ��numλ��ClientID
 */
u8* CreatClientID(u8 num)
{
	u8* id = (u8*)mymalloc(num+1);
	u8 i = num;
	for(;num>0;num--)
		id[i-num] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"[(myrandom()&0x1f)];

	return id;
}
/***
 * ����һ���»Ự
 */
SessionStr* myMQTT_CreatNewSessionStr(const char* name,const char* password,const char* dns,u16 port)
{
	SessionStr* ss = (SessionStr*)mymalloc(sizeof(SessionStr));
	ss->wwwDNS = (u8*)dns;
	ss->port = port;
	ss->usrName = (u8*)name;
	ss->passWord = (u8*)password;
	ss->keepAlive = KEEPALIVE_SEC;
	ss->clientId = "LED0|securemode=3,signmethod=hmacsha1|";//CreatClientID(32);
	ss->connectFlags = CONNECT_FLAG_PARA;					//�û�������+����Ự
	ss->protocolLevel = MQTT_Protocol_Level;

	return ss;
}

//����ַ������ֽڸ���
u32 GetStringByteNum(const char* str)
{
	const char* p = str;
	u32 i = 0;
	while(*p!=0)
	{
		i++;
		p++;
	}
	return i;
}
//ת�����ֶΣ����ַ������ֽڵ���ʽ���浽�����У�������ǰ����ӳ���
//
u8* Str2ByteSector(u8* str , u8* pdat)
{
	u32 len = GetStringByteNum(str);
	pdat[0] = len >> 8;
	pdat[1] = (u8)len;
	return mymemcpy(&pdat[2],str,len);
}


/**
 * �������ӱ���CONNECT
 */
ControlStr* myMQTT_MallocCONNECTMessage(SessionStr* ss)
{
	ControlStr* mControlStr = mymalloc(sizeof(ControlStr));
	mControlStr->fixHeader.control = CONNECT;
	mControlStr->variableHeader.pdat =  (u8*)mymalloc(10);
	mControlStr->variableHeader.length = 10;
	mControlStr->variableHeader.pdat[0] = 0;
	mControlStr->variableHeader.pdat[1] = 0x04;
	mControlStr->variableHeader.pdat[2] = 'M';
	mControlStr->variableHeader.pdat[3] = 'Q';
	mControlStr->variableHeader.pdat[4] = 'T';
	mControlStr->variableHeader.pdat[5] = 'T';
	mControlStr->variableHeader.pdat[6] = ss->protocolLevel;
	mControlStr->variableHeader.pdat[7] = ss->connectFlags;
	mControlStr->variableHeader.pdat[8] = ss->keepAlive >> 8;
	mControlStr->variableHeader.pdat[9] = (u8)(ss->keepAlive);

	mControlStr->payload.length = GetStringByteNum(ss->clientId) + GetStringByteNum(ss->usrName) + GetStringByteNum(ss->passWord) +6;
	mControlStr->payload.pdat =  (u8*)mymalloc(mControlStr->payload.length);
	Str2ByteSector(ss->clientId ,mControlStr->payload.pdat);
	u8* p = mControlStr->payload.pdat;
	p = p + 2 + GetStringByteNum(ss->clientId);
	Str2ByteSector(ss->usrName ,p);
	p = p + 2 + GetStringByteNum(ss->usrName);
	Str2ByteSector(ss->passWord ,p);

	IntTo128(mControlStr->payload.length + mControlStr->variableHeader.length,mControlStr->fixHeader.leftNum);
	return mControlStr;

}


/**
 * �ͷſ��Ʊ��Ľṹ���ڴ�
 */

void Free_ControlMessage(ControlStr* message)
{
	//�ͷſɱ䱨ͷ�ṹ��
	myfree(message->variableHeader.pdat);
	//�ͷ���Ч�غɽṹ��
	myfree(message->payload.pdat);
	//�ͷſ��Ʊ��Ľṹ��
	myfree(message);
}
/***
 * ��֯���Ϳ��Ʊ�������
 * �������ݱ���ͷ
 */

DataMessageStr*  myMQTT_GetControlMessage(ControlStr* message)
{
	DataMessageStr* dataMessage = mymalloc(sizeof(DataMessageStr));
	u8 i = 0;
	u8 lenbyte = 0; //��Ҫ���ֽ���
	u32 len = Change128ToInt(message->fixHeader.leftNum,&lenbyte);//���ʣ�೤��,��Ҫ���ֽ���
	dataMessage->length = len + lenbyte + 1;
	dataMessage->pdat = mymalloc(dataMessage->length);
	u8* pdat = dataMessage->pdat;
	*pdat = message->fixHeader.control;	//�����ļ�����

	while(i<4)							//����ʣ�೤��
	{
		pdat ++;
		*pdat = message->fixHeader.leftNum[i];
		if(*pdat & 0x80) i++;
		else break;
	}
	pdat ++;
	//�����ɱ䱨ͷ
	mymemcpy(pdat,message->variableHeader.pdat,message->variableHeader.length);
	pdat += message->variableHeader.length;
	//������Ч�غ�
	mymemcpy(pdat,message->payload.pdat,message->payload.length);
	Free_ControlMessage(message);		//�ͷſ��Ʊ��Ľṹ���ڴ�

	return dataMessage;
}

// �Ͽ�����
void myMQTT_Disconnect()
{
	u8 discmd[2] = {0xe0,00};
	espconn_send(&ST_NetCon,discmd,2);
}

// ��������PING��
void myMQTT_Ping()
{
	u8 pincmd[2] = {0xc0,00};
	espconn_send(&ST_NetCon,pincmd,2);
}

// �������ⱨ��
void myMQTT_Subscrible()
{

}
