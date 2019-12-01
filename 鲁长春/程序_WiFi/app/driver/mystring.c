#include "mystring.h"

/**
 * �任��128���ƴ洢
 * ���ؽ���ָ��
 * ʣ�೤�ȵ��ڿɱ䱨ͷ�ĳ��ȣ�10�ֽڣ�������Ч�غɵĳ���
 */
u8* ICACHE_FLASH_ATTR IntTo128(u32 num ,u8* array)
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
u32 ICACHE_FLASH_ATTR Change128ToInt(u8* dat,u8* numbyte)
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
 * �ڴ濽��
 * ����pd���������һ��ָ��
 */
u8* ICACHE_FLASH_ATTR mymemcpy(void* pdat ,const void* psor,u32 len)
{
	u32 i = 0;
	u32 temp = len/sizeof(u32);
	u8* pd = (u8*)pdat;
	u8* ps = (u8*)psor;
	for(i=0;i<temp;i++) ((u32*)pd)[i] = ((u32*)ps)[i];
	i *= sizeof(u32);
	for(;i<len;i++) pd[i] = ps[i];
	return &pd[len];
}


/**
 * �������һ��numλ��ClientID
 */
//u8* CreatClientID(u8 num)
//{
//	u8* id = (u8*)mymalloc(num+1);
//	u8 i = num;
//	for(;num>0;num--)
//		id[i-num] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"[(myrandom()&0x1f)];
//
//	return id;
//}

// ��sor�ַ�����position���ַ��������str
// ����ֻ��256���ֽ�
void ICACHE_FLASH_ATTR Str_Insert(char* sor,char* str,u32 position)
{

	char* back = malloc(strlen(sor) - position + 1);	// ���뻺��
	u8 i = 0;
	for(i=0;sor[position+i] != 0;i++) back[i] = sor[position+i];	// �������������Ϣ
	for(i = 0;str[i]!=0;i++)sor[position+i] = str[i];				// ����
	u8 j = i;
	for(;back[i-j] != 0;i++)sor[position+i] = back[i-j];
	free(back);
}

//�ײ�ƥ���ַ���,ȥ��ƥ����ײ��ͻس����У������µ��ַ���
bool ICACHE_FLASH_ATTR CampareCMD(char* str,char* cmd)
{
	u8 i = 0;
	u8 j = 0;
	for(i=0;cmd[i]!=0;i++) if( (cmd[i])^(str[i])) return false;
	for(j=0;str[i]!=0 && str[i] != '\r' && str[i] != '\n';i++)
	{
		str[j] = str[i];
		j++;
	}
	str[j] = 0;
	return true;
}


// ��ȡ�˿ں�,Ҫ��֤���ϸ�� ip:port ��ʽ���ַ���
u16 ICACHE_FLASH_ATTR GetNetPort(char* str)
{
	u16 port = 0;
	while(*str != ':')str ++;
	str ++;
	while(*str>='0' && *str <= '9' && *str != 0)
	{
		port = port*10 + (*str - '0');
		str ++;
	}
	return port;
}
// ȥ���˿ںŽ��ַ��������ַ,Ҫ��֤���ϸ�� ip:port ��ʽ���ַ���
void ICACHE_FLASH_ATTR GetNetIPAddr(char* str)
{
	while(*str != ':') str ++;
	*str = 0;
}

// ��16����ת�����ַ���,���浽hexbuf�У�hexbuf �Ĵ�С������len*2+1
void ICACHE_FLASH_ATTR ChangeHex2Str(u8* hexbuf,u32 len)
{
	u8* buff  = (u8*)malloc(len*2);
	u32 i;
	for(i=0;i<len;i++)
	{
		buff[i*2] = "0123456789ABCDEF"[hexbuf[i] >> 4];
		buff[i*2 + 1] = "0123456789ABCDEF"[hexbuf[i] & 0x0f];
	}

	for(i=0;i<len*2;i++) hexbuf[i] = buff[i];
	hexbuf[i] = 0;
	free(buff);
}

//����ַ������ֽڸ���
u32 ICACHE_FLASH_ATTR GetStringByteNum(const char* str)
{
/*	const char* p = str;
	u32 i = 0;
	while(*p!=0)
	{
		i++;
		p++;
	}
	return i;*/
	return strlen(str);

}

//�Ƚ������ַ����Ƿ����
bool ICACHE_FLASH_ATTR CampareString(char* str1,char* str2)
{
	u16 i = 0;
	//debug("str1: %s\r\n str2:%s\r\n",str1,str2);
	for(i=0;str1[i]!=0;i++)
	{
		if(str1[i]^str2[i])return false;
	}
	if(str2[i] == 0) return true;
	return false;
}

// ������������ֵ
#define	EXCHANGE(a,b)	do{(a)^=(b);(b)^=(a);(a)^=(b);}while(0);

// ���ַ�����ֳ������publish,�ڡ�{��λ�ò���0������˫0�ַ���str,���صڶ����ַ�����ʼ��λ��
char* ICACHE_FLASH_ATTR GetSubAndPub(char* str)
{
	u8 i = 0;
	u8 j = 0;
	char* p = 0;
	while(str[i]!=0 && str[i]!='{') i++;
	if(str[i] == 0) return 0;
	p = &str[i+1];
	while(str[i]!=0)
	{
		EXCHANGE(str[i],j);
		i++;
	}
	EXCHANGE(str[i],j);
	i++;
	str[i] = 0;
	return p;
}


