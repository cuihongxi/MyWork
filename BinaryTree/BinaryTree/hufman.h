/**
 * 2020��2��4��16:33:43
 * �������㷨 
 * ���裺
 * ��    ����һ��ȫ�ֱ������� u32 array[256] = {0},�����鱣��Ҫѹ���ļ���Ȩ�ء�
 * ��    �������� SortData(u32* array, u8* file,u32 length) ���ļ�file���ꡣ����array������Ҫѹ���ļ��ĸ����ֽ�Ȩ�ء�
 * ��    ���� powStr* BulidHufmanTree(u32* array) ����һ�Ź��������������������Ȩ�ؽڵ�ָ��
            
 */

#ifndef __HUFMAN_H
#define __HUFMAN_H

#include "uhead.h"
#include "BinaryTree.h"

// Ȩ�ؽڵ�
typedef struct{
    btreeStr*  bs;
	u8	power;           // Ȩ�أ� ������ֽ�Ϊ��λ�����ݽ���ѹ����ÿ��Ҷ�ӽڵ������Ȩ��Ϊunsigned char����
}powStr;

//Ҷ�ӽڵ�
typedef struct{
    powStr pow;        
    u8  value;          // �ڵ��ֵ
    u8* code;           // �����ַ���
}leafStr;

typedef struct 
{
    u8 power;
    u8 value;
}valStr;





void SortData(u32* array, u8* file,u32 length); // ��ȡ�ļ�file,u32 array[256] ����ֵ��¼Ȩ��power���±��¼value
powStr* BulidHufmanTree(u32* array);            // ������    

#endif // !__HUFMAN_H