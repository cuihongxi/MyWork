/**
 * 2019��10��17��00:36:46
 * �ַ���������غ���
 */

#ifndef	__mystring_h
#define	__mystring_h
#include "uhead.h"


u8* IntTo128(u32 num ,u8* array);							// �任��128���ƴ洢
u32 Change128ToInt(u8* dat,u8* numbyte); 					// ��128���Ƶ���ת����int
u8* mymemcpy(void* pdat ,const void* psor ,u32 len);				// �ڴ濽��
void Str_Insert(char* sor,char* str,u32 position);			// ��sor�ַ�����position���ַ��������str
void GetNetIPAddr(char* str);								// ȥ���˿ںŽ��ַ��������ַ,Ҫ��֤���ϸ�� ip:port ��ʽ���ַ���
u16 GetNetPort(char* str);									// ��ȡ�˿ں�,Ҫ��֤���ϸ�� ip:port ��ʽ���ַ���
bool CampareCMD(char* str,char* cmd);						// �ײ�ƥ���ַ���,ȥ��ƥ����ײ��ͻس����У������µ��ַ���
void ChangeHex2Str(u8* hexbuf,u32 len);						// ��16����ת�����ַ���,���浽hexbuf�У�hexbuf �Ĵ�С������len*2+1
u32 GetStringByteNum(const char* str);						// ����ַ������ֽڸ���
bool CampareString(char* str1,char* str2);					// �Ƚ������ַ����Ƿ����
char* GetSubAndPub(char* str);								// ���ַ�����ֳ������publish,�ڡ�{��λ�ò���0������˫0�ַ���str,���صڶ����ַ�����ʼ��λ��

#endif

