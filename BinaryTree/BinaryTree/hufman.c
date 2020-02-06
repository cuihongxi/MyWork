#include "hufman.h"
#include "SingleList.h"


/**
 * �½�һ��hufman��Ҷ�ӽڵ�
 */
leafStr* NewHufmanNode(u8  value,u8	power)
{
    leafStr* ls = (leafStr*)malloc(sizeof(leafStr));
    ls->pow.power = power;
    ls->pow.bs = NewBinaryTreeNode();                       // �½�һ��������Ҷ�ӽڵ�,freeʱ��Ҫ�ͷ�
    ls->value = value;
    return ls;
}

/**
 *  �ͷ�hufmanҶ�ӽڵ� 
 */
void FreeHufmanNode(leafStr* ls)
{
    free(ls->pow.bs);
    free(ls);
}


 /**
  * �½�һ��Ȩ�ؽڵ� , ���������С��˳�򣬽�����Ȩ�ؽڵ���ӵ��ýڵ���
  */

powStr* PowerTreeAddLR(powStr* left,powStr* right)
{
    powStr* ps = (powStr*)malloc(sizeof(powStr));
    ps->power = left->power + right->power;
    ps->bs = NewBinaryTreeNode();                       // �½�һ��������Ҷ�ӽڵ�,freeʱ��Ҫ�ͷ�
    ps->bs->left = left;
    ps->bs->right = right;
    return ps;
}

/**
 *  �ͷ�Ȩ�ؽڵ� 
 */
void FreePowerTreeNode(powStr* ps)
{
    free(ps->bs);
    free(ps);
}

// ����ֵ��¼Ȩ��power���±��¼value
// u32 array[256] = {0};
void SortData(u32* array, u8* file,u32 length)
{
    u32 i = 0;
    for(i=0;i<length;i++) array[file[i]] ++; 
}   

// ���򷵻�������Сֵ��������λ��0xff
leafStr* GetMinData(u32* array,u32 length)
{
    leafStr* min = NewHufmanNode(0,0xff); // ����һ��Ҷ�ӽڵ㱣�����ݣ���Ҫfree
    u32 i = 0;
    for(i=0;i<length;i++)
    {
        if(array[i] && array[i] < min->pow.power){
            min->pow.power = array[i];
            min->value = i;
        } 
    }
    // ���ؼ�¼��������Сֵ������0xff
    array[min->value] = 0xff;
    return min;
}

 // ��һ��������Է�0xff����Ĵ�С���������
 // u32 array[256]
 SingleList* SaveSinglist(u32* array)
 {
     SingleList* bk;

    SingleList* list = NewSingleList();
    leafStr* val = GetMinData(array,256);
    SingleList_InsertEnd(list,val);
    while(val->pow.power != 0xff)
    {
         val = GetMinData(array,256);
         if(val->pow.power != 0xff) SingleList_InsertEnd(list,val);
         else
         {
             FreeHufmanNode(val);
             break;
         } 
    }

    bk = list;
    debug("Ȩ�������С�����˳��\r\n");
    while(SingleList_Iterator(&bk))
    {
        debug("  [%d] = %d,",SingeListGetnode(leafStr,bk)->value,SingeListGetnode(leafStr,bk)->pow.power);
    }
    debug("\r\n");
    return list;
 }

 // ��ѡ���������С�����ڵ㣬���һ��������
 // ���������ڵ��������ժ�£������γɵĶ������ڵ�����������������
 powStr* BulidHufmanTree(u32* array)
 {
      SingleList* bk;
      powStr* right;                                // ������
      powStr* left;                                 // ������
      powStr* newpowNode;                           // ��Ȩ�ؽڵ�
      SingleList* minList = SaveSinglist(array);    // �������Ȩ�ش�С�����˳�򱣴浽������
    
      bk = minList; 
      while(((SingleListNodeStr*)SingleList_Iterator(&bk))->next)
      {
        right = SingeListGetnode(powStr,bk);        // ���һ���ڵ㣬����������ժ��������Ϊ������
        SingleList_DeleteNodeNoFree(minList,right); // ������������ժ��     

        SingleList_Iterator(&bk); 
        left = SingeListGetnode(powStr,bk);
        while(((SingleListNodeStr*)bk)->next != 0 && (left->power == right->power))
        {
            SingleList_Iterator(&bk); 
            left = SingeListGetnode(powStr,bk);
        }
        SingleList_DeleteNodeNoFree(minList,left); // ������������ժ�� 
        newpowNode = PowerTreeAddLR(left,right);   // ������Ȩ�ص�

        // ����Ȩֵ��С�����˳���½ڵ���뵽����������
        bk = minList;
        if(bk->next == 0)                           // ���������������
        {
            free(minList);                          // ɾ����������
            return newpowNode;
        } 
        while(SingleList_Iterator(&bk))
        {
            if(newpowNode->power < SingeListGetnode(powStr,bk)->power)
            {
                SingleList_InsertBefore(minList,bk,newpowNode);
                break;
            }    
        }
        if(bk == 0) SingleList_InsertEnd(minList,newpowNode); // �½ڵ�Ȩֵ�������еĽڵ�Ȩֵ
        bk = minList; 
      }
	  return newpowNode;
 }

// ���ַ�����ʽ���������ӳ���
void TabHufmanSave(leafStr* leaf,mapTabStr* map)
{

}

// �ַ�������
void Strcopy(u8* sorce,u8* obj)
{
    u8 i = 0;
    for(;sorce[i];i++)
    {
        obj[i] = sorce[i];
    }
    obj[i] = 0;
}

// ���ַ���ĩβ����һ��ascii��
void Strinsert(u8* str,const char dat)
{
    u8 i = 0;
    for(;str[i];i++);
    str[i] = dat;
    i++;
    str[i] = 0;
}

// ����ַ�������,��������ֹ��
u32 StrGetLength(u8* str)
{
    u32 i = 0;
    while(str[i]) i++;
    return i;
}
 /**
  *     �������������������������ӳ���
  **/
void TabHufmanCreat(powStr* hufmanTree,mapTabStr* map,u8* str)
{
    if (hufmanTree)
    {
        if(hufmanTree->bs->left == 0 && hufmanTree->bs->right == 0)
        {          
            u8* strend =  (u8*)malloc(StrGetLength(str) + 1);
            Strcopy(str,strend);
            map->tab[((leafStr*)hufmanTree)->value] = strend;
            debug("%s\r\n",str);
        }else
        {
            //u8 str0[40] = {0};
            u8* str0 = (u8*)malloc(StrGetLength(str) + 2);
            if(hufmanTree->bs->left)
            {
                Strcopy(str,str0);
                Strinsert(str0,'0');
                TabHufmanCreat(hufmanTree->bs->left,map,str0);
            }
            if(hufmanTree->bs->right)
            {
                Strcopy(str,str0);
                Strinsert(str0,'1');
                TabHufmanCreat(hufmanTree->bs->right,map,str0);
            }
            free(str0);          
        }
    }
}