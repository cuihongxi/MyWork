#include "mystring.h"

/**
 * 变换成128进制存储
 * 返回结束指针
 * 剩余长度等于可变报头的长度（10字节）加上有效载荷的长度
 */
u8*  IntTo128(u32 num ,u8* array)
{
    if (num <= 0xfffffff)           //不能超出最大数量
    {
        while(1)
        {
            *array = num & 0x7f;
            if(num > *array) *array |= 0x80;
            else return ++ array;   //偏移到下一个位置返回
            num >>= 7;
            array ++;
        }

    }else return 0;
}


/***
 * 将128进制的数转换成int
 * numbyte记录占用的字节数
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
 * 内存拷贝
 * 返回pd拷贝完的下一个指针
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
 * 随机生成一个num位的ClientID
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

// 在sor字符串的position个字符串后插入str
// 缓存只有256个字节
void  Str_Insert(char* sor,char* str,u32 position)
{

	char* back = malloc(strlen(sor) - position + 1);	// 申请缓存
	u8 i = 0;
	for(i=0;sor[position+i] != 0;i++) back[i] = sor[position+i];	// 保存插入后面的信息
	for(i = 0;str[i]!=0;i++)sor[position+i] = str[i];				// 插入
	u8 j = i;
	for(;back[i-j] != 0;i++)sor[position+i] = back[i-j];
	free(back);
}

//首部匹配字符串,去掉匹配的首部和回车换行，生成新的字符串
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


// 获取端口号,要保证是严格的 ip:port 格式的字符串
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
// 去掉端口号将字符串变成网址,要保证是严格的 ip:port 格式的字符串
void ICACHE_FLASH_ATTR GetNetIPAddr(char* str)
{
	while(*str != ':') str ++;
	*str = 0;
}

// 将16进制转换成字符串,保存到hexbuf中，hexbuf 的大小至少是len*2+1
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

//获得字符串的字节个数
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
