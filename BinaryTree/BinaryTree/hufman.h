/**
 * 2020��2��4��16:33:43
 * �������㷨ѹ������ѹ������
 * ��ע��   ����ʹ���˶�̬�ڴ����͵���������ݽṹ����Ҫ��Ӧ���ļ�
 * ѹ�����裺
 * ��    ����һ��������ӳ��������    mapTabStr* map;
 *              ע���ñ���������ӳ���ϵ������������ַ����ѹ���ļ���С������������ѹ��֮��ͽ�ѹʱû�б����ã�����Ȼ��������
 *                ������������Ե��� Free_HufmanTree(powStr* hufmanTree);�ͷſռ�
 * ��    ����ѹ���ļ�������  mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* hufmanfile);
 *              ע����file�ļ�����ѹ����ѹ��֮��ı��뱣���� hufmanfile �С�ͬʱ����ӳ��������map���ա�
 *              ���ӣ�map = HufmanCompressFile("111111222233344556",19,hufmanfile); 
 * ��ѹ���裺
 * ��    ���ý�ѹ�������� void HufmanUncompressFile(u8* hufmanfile,mapTabStr* map,u8* datbuf);
 *              ע��ʹ����Ӧ�Ĺ�����ӳ���map ����ѹѹ���ļ�hufmanfile����ѹ��֮������ݱ��浽datbuf��
 *              ���ӣ�HufmanUncompressFile(hufmanfile,map,encodefile);
 * ˼·���̣�
 *  * ѹ�����裺
 * ��    ����һ��ȫ�ֱ������� u32 array[256] = {0},�����鱣��Ҫѹ���ļ���Ȩ�ء�
 * ��    �������� SortData(u32* array, u8* file,u32 length) ���ļ�file���ꡣ����array������Ҫѹ���ļ��ĸ����ֽ�Ȩ�ء�
 * ��    ���� powStr* BulidHufmanTree(u32* array) ����һ�Ź��������������������Ȩ�ؽڵ�ָ��
 * ��    ������������ӳ��� ��
 *          ������  mapTabStr map; ����_TabHufmanCreat(hufmanTree,map)����н������ú�������һ��ȫ�ֱ���tabhufmanstaticbuff

 *          
 */

#ifndef __HUFMAN_H
#define __HUFMAN_H

#include "uhead.h"
#include "BinaryTree.h"

// Ȩ�ؽڵ�
typedef struct{
    btreeStr*  bs;
	u32	power;           // Ȩ�أ� ������ֽ�Ϊ��λ�����ݽ���ѹ��
}powStr;

//Ҷ�ӽڵ�
typedef struct{
    powStr pow;        
    u8  value;          // �ڵ��ֵ
}leafStr;

//������ӳ���
typedef struct 
{
    u8* tab[256];       // �����ַ�����ַ 
    powStr* hafmanTree; // ��������
    u32 length;         // ��ѹ���ļ��ĳ��ȣ�byteΪ��λ
}mapTabStr;

void TabHufmanCreat(powStr* hufmanTree,mapTabStr* map,u8* str); // ������������ӳ���
extern u8 tabhufmanstaticbuff ;
/* �û����� */
void SortData(u32* array, u8* file,u32 length);         // ��ȡ�ļ�file,u32 array[256] ����ֵ��¼Ȩ��power���±��¼value
powStr* BulidHufmanTree(u32* array);                    // ������                
void Free_HufmanTree(powStr* hufmanTree);               // �ͷŹ��������ռ�

powStr* BulidHufmanTreeOneTime(u8* file,u32 length);    // һ���Զ��꣬������������

#define _TabHufmanCreat(hufmanTree,map)     TabHufmanCreat(hufmanTree,map,&tabhufmanstaticbuff)     // // ������������ӳ���

mapTabStr* BulidHufmanTabForStr(u8* str,u32 length) ;              //һ���Զ����ַ���str������ӳ���

mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* hufmanfile);      // ѹ��
void HufmanUncompressFile(u8* hufmanfile,mapTabStr* map,u8* datbuf);// ��ѹ


/********************************************Windows C�����ļ�����***************************************************/
mapTabStr* HufmanCompress_CFile(u8* filename,u8* hufmanfile);      // ѹ��


u32 StrGetLength(u8* str);

#endif // !__HUFMAN_H
