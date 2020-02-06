#include "hufman.h"
#include "SingleList.h"


/**
 * 新建一个hufman树叶子节点
 */
leafStr* NewHufmanNode(u8  value,u8	power)
{
    leafStr* ls = (leafStr*)malloc(sizeof(leafStr));
    ls->pow.power = power;
    ls->pow.bs = NewBinaryTreeNode();                       // 新建一个二叉树叶子节点,free时需要释放
    ls->value = value;
    return ls;
}

/**
 *  释放hufman叶子节点 
 */
void FreeHufmanNode(leafStr* ls)
{
    free(ls->pow.bs);
    free(ls);
}


 /**
  * 新建一个权重节点 , 按照左大右小的顺序，将两个权重节点添加到该节点下
  */

powStr* PowerTreeAddLR(powStr* left,powStr* right)
{
    powStr* ps = (powStr*)malloc(sizeof(powStr));
    ps->power = left->power + right->power;
    ps->bs = NewBinaryTreeNode();                       // 新建一个二叉树叶子节点,free时需要释放
    ps->bs->left = left;
    ps->bs->right = right;
    return ps;
}

/**
 *  释放权重节点 
 */
void FreePowerTreeNode(powStr* ps)
{
    free(ps->bs);
    free(ps);
}

// 数组值记录权重power，下标记录value
// u32 array[256] = {0};
void SortData(u32* array, u8* file,u32 length)
{
    u32 i = 0;
    for(i=0;i<length;i++) array[file[i]] ++; 
}   

// 排序返回数组最小值，并将该位置0xff
leafStr* GetMinData(u32* array,u32 length)
{
    leafStr* min = NewHufmanNode(0,0xff); // 创建一个叶子节点保存数据，需要free
    u32 i = 0;
    for(i=0;i<length;i++)
    {
        if(array[i] && array[i] < min->pow.power){
            min->pow.power = array[i];
            min->value = i;
        } 
    }
    // 返回记录，并将最小值的数组0xff
    array[min->value] = 0xff;
    return min;
}

 // 用一个链表保存对非0xff数组的从小到大的排序
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
    debug("权重链表从小到大的顺序：\r\n");
    while(SingleList_Iterator(&bk))
    {
        debug("  [%d] = %d,",SingeListGetnode(leafStr,bk)->value,SingeListGetnode(leafStr,bk)->pow.power);
    }
    debug("\r\n");
    return list;
 }

 // 挑选出链表的最小两个节点，组成一个二叉树
 // 将这两个节点从链表中摘下，并用形成的二叉树节点在链表中重新排序
 powStr* BulidHufmanTree(u32* array)
 {
      SingleList* bk;
      powStr* right;                                // 右子树
      powStr* left;                                 // 左子树
      powStr* newpowNode;                           // 新权重节点
      SingleList* minList = SaveSinglist(array);    // 将数组的权重从小到大的顺序保存到链表中
    
      bk = minList; 
      while(((SingleListNodeStr*)SingleList_Iterator(&bk))->next)
      {
        right = SingeListGetnode(powStr,bk);        // 获得一个节点，并从链表中摘下来，作为右子树
        SingleList_DeleteNodeNoFree(minList,right); // 从排序链表中摘下     

        SingleList_Iterator(&bk); 
        left = SingeListGetnode(powStr,bk);
        while(((SingleListNodeStr*)bk)->next != 0 && (left->power == right->power))
        {
            SingleList_Iterator(&bk); 
            left = SingeListGetnode(powStr,bk);
        }
        SingleList_DeleteNodeNoFree(minList,left); // 从排序链表中摘下 
        newpowNode = PowerTreeAddLR(left,right);   // 建立新权重点

        // 安照权值从小到大的顺序将新节点插入到排序链表中
        bk = minList;
        if(bk->next == 0)                           // 霍夫曼树创建完成
        {
            free(minList);                          // 删除排序链表
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
        if(bk == 0) SingleList_InsertEnd(minList,newpowNode); // 新节点权值大于所有的节点权值
        bk = minList; 
      }
	  return newpowNode;
 }

// 以字符串形式保存哈夫曼映射表
void TabHufmanSave(leafStr* leaf,mapTabStr* map)
{

}

// 字符串拷贝
void Strcopy(u8* sorce,u8* obj)
{
    u8 i = 0;
    for(;sorce[i];i++)
    {
        obj[i] = sorce[i];
    }
    obj[i] = 0;
}

// 往字符串末尾插入一个ascii码
void Strinsert(u8* str,const char dat)
{
    u8 i = 0;
    for(;str[i];i++);
    str[i] = dat;
    i++;
    str[i] = 0;
}

// 获得字符串长度,不包含休止符
u32 StrGetLength(u8* str)
{
    u32 i = 0;
    while(str[i]) i++;
    return i;
}
 /**
  *     解析哈夫曼树，先序遍历保存映射表
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