#include "hufman.h"
#include "SingleList.h"

u8 tabhufmanstaticbuff = 0;             // 一个已经定义好的全局变量，不能修改

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

// 字符串拷贝之后在末尾添加字符
void StrCopyInsertChar(u8* sorce,u8* obj,const char dat)
{
    u8 i = 0;
    for(;sorce[i];i++)
    {
        obj[i] = sorce[i];
    }
    
    obj[i] = dat;  
    i++;
    obj[i] = 0; 
}

// 获得字符串长度,不包含休止符
u32 StrGetLength(u8* str)
{
    u32 i = 0;
    while(str[i]) i++;
    return i;
}

/**
 * 新建一个hufman树叶子节点
 */
leafStr* NewHufmanNode(u8  value,u32 power)
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
    leafStr* min = NewHufmanNode(0,0xffffffff); // 创建一个叶子节点保存数据，需要free
    u32 i = 0;
    for(i=0;i<length;i++)
    {
        if(array[i] && (array[i] < min->pow.power)){
            min->pow.power = array[i];
            min->value = i;
        } 
    }
    // 返回记录，并将最小值的数组0xff
    array[min->value] = 0xffffffff;
    return min;
}

 // 用一个链表保存对非0xffffffff数组的从小到大的排序
 // u32 array[256]
 SingleList* SaveSinglist(u32* array)
 {
     SingleList* bk;

    SingleList* list = NewSingleList();
    leafStr* val = GetMinData(array,256);
    SingleList_InsertEnd(list,val);
    while(val->pow.power != 0xffffffff)
    {
         val = GetMinData(array,256);
         if(val->pow.power != 0xffffffff) SingleList_InsertEnd(list,val);
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
        debug("[%X]:    %d  ",SingeListGetnode(leafStr,bk)->value,SingeListGetnode(leafStr,bk)->pow.power);
    }
    debug("\r\n");
    return list;
 }

/**
 *  创建hufman树
 * 
 */
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
        SingleList_DeleteNode(minList,right);       // 从排序链表中摘下     
        SingleList_Iterator(&bk); 
        left = SingeListGetnode(powStr,bk);
        while(((SingleListNodeStr*)bk)->next != 0 && (left->power == right->power))
        {
            SingleList_Iterator(&bk); 
            left = SingeListGetnode(powStr,bk);
        }
        SingleList_DeleteNode(minList,left); // 从排序链表中摘下 
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

/**
 *  释放hufman树叶子节点 
 */
u8 FreeHufmanTree(powStr* hufmanTree)
{
    if (hufmanTree)
    {
        if(hufmanTree->bs->left == 0 && hufmanTree->bs->right == 0)
        {
            FreePowerTreeNode(hufmanTree);
            return 0;
        }else
        {
            if(hufmanTree->bs->left)
            {
                 if(((powStr*)(hufmanTree->bs->left))->bs->left == 0 && ((powStr*)(hufmanTree->bs->left))->bs->right == 0)
                 {
                    FreePowerTreeNode(hufmanTree->bs->left);
                    hufmanTree->bs->left = 0;                    
                 }
            }

            if(hufmanTree->bs->right)
            {
                if(((powStr*)(hufmanTree->bs->right))->bs->left == 0 && ((powStr*)(hufmanTree->bs->right))->bs->right == 0) 
                {         
                    FreePowerTreeNode(hufmanTree->bs->right);
                    hufmanTree->bs->right = 0;
                }                  
            }
            
            if(hufmanTree->bs->left)
            {
                FreeHufmanTree(hufmanTree->bs->left);
            }
            if(hufmanTree->bs->right)
            {
                FreeHufmanTree(hufmanTree->bs->right);
            }
            return 1;
        }
    } 
    return 0;   
}
/**
 *  释放hufman树，及叶子节点 
 */
void Free_HufmanTree(powStr* hufmanTree)
{
    while(FreeHufmanTree(hufmanTree));
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
            debug("[%X]:    %s\r\n",((leafStr*)hufmanTree)->value,str);
        }else
        {
            u8* str0 = (u8*)malloc(StrGetLength(str) + 2);
            if(hufmanTree->bs->left)
            {
                StrCopyInsertChar(str,str0,'0');
                TabHufmanCreat(hufmanTree->bs->left,map,str0);
            }
            if(hufmanTree->bs->right)
            {
                StrCopyInsertChar(str,str0,'1');
                TabHufmanCreat(hufmanTree->bs->right,map,str0);
            }
            free(str0);          
        }
    }
}

// 一次性读完，建立哈夫曼树
powStr* BulidHufmanTreeOneTime(u8* file,u32 length)
{
    u32 array[256] = {0};
    SortData(array,file,length);    
    return BulidHufmanTree(array);
}


//一次性读完字符串str，建立映射表
mapTabStr* BulidHufmanTabForStr(u8* str,u32 length) 
 {
    mapTabStr* map;
    u32 i = 0;
    u32 array[256] = {0};
    SortData(array,str,length);
    map = (mapTabStr*)malloc(sizeof(mapTabStr));
    for(i=0;i<256;i++)
    {
        map->tab[i] = 0;
    }
    map->hafmanTree = BulidHufmanTree(array);
    _TabHufmanCreat(map->hafmanTree,map); // 解析树，保存映射表
    return map;
 }



// 在字节中插入‘0’或‘1’
void CopressFile_AddBit(u8* buf,u32 buf_byte,u8 buf_bit,const char dat)
{
    u8 d = dat - 0x30; 
    if(d) buf[buf_byte] |= (u8)(0x80 >> buf_bit);
    else buf[buf_byte] &= (u8)(~(0x80 >> buf_bit));
}

// 依据map压缩,map已知文件长度
void HufmanCompressFile_map(mapTabStr* map,u8* file,u8* hufmanfile)
{
    u32 datbuf_byte = 0;        // 记录偏移到datbuf哪个字节
    u8  datbuf_bit = 0;         // 记录偏移到datbuf字节中的哪个位
    u32 i = 0;
    for(i= 0;i<map->length;i++)    
    {   
        u32 ip = 0;
        u8* pstr = map->tab[file[i]];       // 获得该字符的压缩码
        for(;pstr[ip];ip++)                 // 遍历该压缩码，同时添加到datbuf
        {
              CopressFile_AddBit(hufmanfile,datbuf_byte,datbuf_bit,pstr[ip]);
              datbuf_bit ++;
              if(datbuf_bit > 7)
              {
                  datbuf_bit = 0;
                  datbuf_byte ++;
              }
        }
    }
}
 /**
  *  霍夫曼压缩
  *  压缩后的数据保存在datbuf中，返回霍夫曼信息map
  */

mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* hufmanfile)
{
    mapTabStr* map =  BulidHufmanTabForStr(file,length);
    u32 i = 0;
    u32 datbuf_byte = 0;        // 记录偏移到datbuf哪个字节
    u8  datbuf_bit = 0;         // 记录偏移到datbuf字节中的哪个位
    map->length = length;

    // for(i= 0;i<length;i++)    
    // {   
    //     u32 ip = 0;
    //     u8* pstr = map->tab[file[i]];   // 获得该字符的压缩码
    //     for(;pstr[ip];ip++)                 // 遍历该压缩码，同时添加到datbuf
    //     {
    //           CopressFile_AddBit(hufmanfile,datbuf_byte,datbuf_bit,pstr[ip]);
    //           datbuf_bit ++;
    //           if(datbuf_bit > 7)
    //           {
    //               datbuf_bit = 0;
    //               datbuf_byte ++;
    //           }
    //     }
    // }
    HufmanCompressFile_map( map,file,hufmanfile);
    return map;
}

// 解压，返回原来的字节
u8 UncopressByte( mapTabStr* map,u8* hufmanfile,u32* datbuf_byte,u8* datbuf_bit)
{
    SingleList* list = NewSingleList();
    SingleList* bk = list;
    u32 i = 0;
    u32 j = 0;

    for(i=0;i<256;i++)
    {
        if(map->tab[i] == 0) continue;
        if(((map->tab[i])[0] - 0x30) == ((hufmanfile[*datbuf_byte]  >> (7-*datbuf_bit)) & 0x01))   // 将匹配的添加到链表中
            SingleList_InsertEnd(list,&(map->tab[i]));
    }
    //在链表中继续寻找符合条件的    
    while(1)
    {
        j ++;
        (*datbuf_bit) ++;
        if(*datbuf_bit > 7)
        {
            *datbuf_bit = 0;
            (*datbuf_byte) ++;
        }        
        while (SingleList_Iterator(&bk))
        {
            if((*SingeListGetnode(u8*,bk))[j] == 0)
            {
               // debug("zhao dao le !  code= %d\r\n",(u8)((u32)SingeListGetnode(u8*,bk) - (u32)map->tab)/sizeof(u8*));
                //删掉链表
                FreeSingList(list);
                return (u8)(((u32)SingeListGetnode(u8*,bk) - (u32)map->tab)/sizeof(u8*));    
            }
            if(((hufmanfile[*datbuf_byte]  >> (7-*datbuf_bit)) & 0x01) != ((*SingeListGetnode(u8*,bk))[j] - 0x30))
            {
                SingleList_DeleteNode(list,SingeListGetnode(u8*,bk));
            }

        }
        bk = list;
        
    }

    return 0;
}
/**
 *  霍夫曼解码 
 */
void HufmanUncompressFile(u8* hufmanfile,mapTabStr* map,u8* datbuf)
{
    u32 i = 0;
    u32 datbuf_byte = 0;        // 记录偏移到datbuf哪个字节
    u8  datbuf_bit = 0;         // 记录偏移到datbuf字节中的哪个位
    for(i=0;i<map->length;i++)
    {
        datbuf[i] = UncopressByte(map,hufmanfile,&datbuf_byte,&datbuf_bit);
    }
}

/********************************************Windows C语言文件操作***************************************************/
mapTabStr* HufmanCompress_CFile(u8* filename,u8* hufmanfile)      // 压缩
{
    mapTabStr* map;
    u32 i = 0;
    u32 array[1024] = {0};
    u32 datbuf_byte = 0;        // 记录偏移到datbuf哪个字节
    u8  datbuf_bit = 0;         // 记录偏移到datbuf字节中的哪个位
    FILE *fhufman;              // 压缩文件的句柄
    u8* hufbuf = 0;
    FILE * fd = fopen(filename,"rb");
    int filedat = 0;
    map = (mapTabStr*)malloc(sizeof(mapTabStr));
    map->length = 0;
    for(i=0;i<256;i++)
    {
        map->tab[i] = 0;
    }
    while(1)
    {
        filedat = fgetc(fd);        // 读一个字节
        if(filedat == EOF) break;   // -1时文件结束
        else
        {
            array[(u8)filedat] ++; 
            map->length ++;
        }  
    }
    fclose(fd);
  //  debug("Read file finish\r\n");
    

    map->hafmanTree = BulidHufmanTree(array);
    _TabHufmanCreat(map->hafmanTree,map); // 解析树，保存映射表
  //  HufmanCompressFile_map( map,file,hufmanfile);
    fd = fopen(filename,"rb");
    fhufman =fopen(hufmanfile,"wb");
    for(i= 0;i<map->length;i++)    
    {   
        u32 ip = 0;
        u8* pstr = map->tab[fgetc(fd)];       // 获得该字符的压缩码
        for(;pstr[ip];ip++)                 // 遍历该压缩码，同时添加到datbuf
        {
              CopressFile_AddBit((u8*)array,datbuf_byte,datbuf_bit,pstr[ip]);
              datbuf_bit ++;
              if(datbuf_bit > 7)
              {
                  datbuf_bit = 0;
                  datbuf_byte ++;
              }
              if(datbuf_byte >= 4096)
              {
                datbuf_byte = 0;
                fwrite (array,4096,1,fhufman); // 一次性写入
              }
        }
    }
    if(datbuf_bit) datbuf_byte ++;
    fwrite ((u8*)array,1,datbuf_byte,fhufman);      // 剩余写入
    fclose(fd);
    fclose(fhufman);

    return map;    
}