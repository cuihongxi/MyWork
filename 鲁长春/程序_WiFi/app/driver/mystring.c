#include "mystring.h"

/**
 * �任��128���ƴ洢
 * ���ؽ���ָ��
 * ʣ�೤�ȵ��ڿɱ䱨ͷ�ĳ��ȣ�10�ֽڣ�������Ч�غɵĳ���
 */
u8*  IntTo128(u32 num ,u8* array)
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
u32  Change128ToInt(u8* dat,u8* numbyte)
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
u8*  mymemcpy(void* pdat ,const void* psor,u32 len)
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
void  Str_Insert(char* sor,char* str,u32 position)
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
bool  CampareCMD(char* str,char* cmd)
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
void  ChangeHex2Str(u8* hexbuf,u32 len)
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
u32  GetStringByteNum(const char* str)
{
//	const char* p = str;
//	u32 i = 0;
//	while(*p!=0)
//	{
//		i++;
//		p++;
//	}
//	return i;
	return strlen(str);

}
