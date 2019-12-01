/**
 * 2019年10月17日00:36:46
 * 字符串处理相关函数
 */

#ifndef	__mystring_h
#define	__mystring_h
#include "uhead.h"


u8* IntTo128(u32 num ,u8* array);							// 变换成128进制存储
u32 Change128ToInt(u8* dat,u8* numbyte); 					// 将128进制的数转换成int
u8* mymemcpy(void* pdat ,const void* psor ,u32 len);				// 内存拷贝
void Str_Insert(char* sor,char* str,u32 position);			// 在sor字符串的position个字符串后插入str
void GetNetIPAddr(char* str);								// 去掉端口号将字符串变成网址,要保证是严格的 ip:port 格式的字符串
u16 GetNetPort(char* str);									// 获取端口号,要保证是严格的 ip:port 格式的字符串
bool CampareCMD(char* str,char* cmd);						// 首部匹配字符串,去掉匹配的首部和回车换行，生成新的字符串
void ChangeHex2Str(u8* hexbuf,u32 len);						// 将16进制转换成字符串,保存到hexbuf中，hexbuf 的大小至少是len*2+1
u32 GetStringByteNum(const char* str);						// 获得字符串的字节个数
bool CampareString(char* str1,char* str2);					// 比较两个字符串是否相等
char* GetSubAndPub(char* str);								// 将字符串拆分成主题和publish,在“{”位置插入0，生成双0字符串str,返回第二个字符串开始的位置

#endif

