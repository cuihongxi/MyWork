#include "hufman.h"
#include "SingleList.h"

u8 tabhufmanstaticbuff = 0;             // һ���Ѿ�����õ�ȫ�ֱ����������޸�

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

// �ַ�������֮����ĩβ����ַ�
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

// ����ַ�������,��������ֹ��
u32 StrGetLength(u8* str)
{
    u32 i = 0;
    while(str[i]) i++;
    return i;
}

/**
 * �½�һ��hufman��Ҷ�ӽڵ�
 */
leafStr* NewHufmanNode(u8  value,u32 power)
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
    leafStr* min = NewHufmanNode(0,0xffffffff); // ����һ��Ҷ�ӽڵ㱣�����ݣ���Ҫfree
    u32 i = 0;
    for(i=0;i<length;i++)
    {
        if(array[i] && (array[i] < min->pow.power)){
            min->pow.power = array[i];
            min->value = i;
        } 
    }
    // ���ؼ�¼��������Сֵ������0xff
    array[min->value] = 0xffffffff;
    return min;
}

 // ��һ��������Է�0xffffffff����Ĵ�С���������
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
    debug("Ȩ�������С�����˳��\r\n");
    while(SingleList_Iterator(&bk))
    {
        debug("[%X]:    %d  ",SingeListGetnode(leafStr,bk)->value,SingeListGetnode(leafStr,bk)->pow.power);
    }
    debug("\r\n");
    return list;
 }

/**
 *  ����hufman��
 * 
 */
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
        SingleList_DeleteNode(minList,right);       // ������������ժ��     
        SingleList_Iterator(&bk); 
        left = SingeListGetnode(powStr,bk);
        while(((SingleListNodeStr*)bk)->next != 0 && (left->power == right->power))
        {
            SingleList_Iterator(&bk); 
            left = SingeListGetnode(powStr,bk);
        }
        SingleList_DeleteNode(minList,left); // ������������ժ�� 
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

/**
 *  �ͷ�hufman��Ҷ�ӽڵ� 
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
 *  �ͷ�hufman������Ҷ�ӽڵ� 
 */
void Free_HufmanTree(powStr* hufmanTree)
{
    while(FreeHufmanTree(hufmanTree));
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

// һ���Զ��꣬������������
powStr* BulidHufmanTreeOneTime(u8* file,u32 length)
{
    u32 array[256] = {0};
    SortData(array,file,length);    
    return BulidHufmanTree(array);
}


//һ���Զ����ַ���str������ӳ���
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
    _TabHufmanCreat(map->hafmanTree,map); // ������������ӳ���
    return map;
 }



// ���ֽ��в��롮0����1��
void CopressFile_AddBit(u8* buf,u32 buf_byte,u8 buf_bit,const char dat)
{
    u8 d = dat - 0x30; 
    if(d) buf[buf_byte] |= (u8)(0x80 >> buf_bit);
    else buf[buf_byte] &= (u8)(~(0x80 >> buf_bit));
}

// ����mapѹ��,map��֪�ļ�����
void HufmanCompressFile_map(mapTabStr* map,u8* file,u8* hufmanfile)
{
    u32 datbuf_byte = 0;        // ��¼ƫ�Ƶ�datbuf�ĸ��ֽ�
    u8  datbuf_bit = 0;         // ��¼ƫ�Ƶ�datbuf�ֽ��е��ĸ�λ
    u32 i = 0;
    for(i= 0;i<map->length;i++)    
    {   
        u32 ip = 0;
        u8* pstr = map->tab[file[i]];       // ��ø��ַ���ѹ����
        for(;pstr[ip];ip++)                 // ������ѹ���룬ͬʱ��ӵ�datbuf
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
  *  ������ѹ��
  *  ѹ��������ݱ�����datbuf�У����ػ�������Ϣmap
  */

mapTabStr* HufmanCompressFile(u8* file,u32 length,u8* hufmanfile)
{
    mapTabStr* map =  BulidHufmanTabForStr(file,length);
    u32 i = 0;
    u32 datbuf_byte = 0;        // ��¼ƫ�Ƶ�datbuf�ĸ��ֽ�
    u8  datbuf_bit = 0;         // ��¼ƫ�Ƶ�datbuf�ֽ��е��ĸ�λ
    map->length = length;

    // for(i= 0;i<length;i++)    
    // {   
    //     u32 ip = 0;
    //     u8* pstr = map->tab[file[i]];   // ��ø��ַ���ѹ����
    //     for(;pstr[ip];ip++)                 // ������ѹ���룬ͬʱ��ӵ�datbuf
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

// ��ѹ������ԭ�����ֽ�
u8 UncopressByte( mapTabStr* map,u8* hufmanfile,u32* datbuf_byte,u8* datbuf_bit)
{
    SingleList* list = NewSingleList();
    SingleList* bk = list;
    u32 i = 0;
    u32 j = 0;

    for(i=0;i<256;i++)
    {
        if(map->tab[i] == 0) continue;
        if(((map->tab[i])[0] - 0x30) == ((hufmanfile[*datbuf_byte]  >> (7-*datbuf_bit)) & 0x01))   // ��ƥ�����ӵ�������
            SingleList_InsertEnd(list,&(map->tab[i]));
    }
    //�������м���Ѱ�ҷ���������    
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
                //ɾ������
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
 *  ���������� 
 */
void HufmanUncompressFile(u8* hufmanfile,mapTabStr* map,u8* datbuf)
{
    u32 i = 0;
    u32 datbuf_byte = 0;        // ��¼ƫ�Ƶ�datbuf�ĸ��ֽ�
    u8  datbuf_bit = 0;         // ��¼ƫ�Ƶ�datbuf�ֽ��е��ĸ�λ
    for(i=0;i<map->length;i++)
    {
        datbuf[i] = UncopressByte(map,hufmanfile,&datbuf_byte,&datbuf_bit);
    }
}

/********************************************Windows C�����ļ�����***************************************************/
mapTabStr* HufmanCompress_CFile(u8* filename,u8* hufmanfile)      // ѹ��
{
    mapTabStr* map;
    u32 i = 0;
    u32 array[1024] = {0};
    u32 datbuf_byte = 0;        // ��¼ƫ�Ƶ�datbuf�ĸ��ֽ�
    u8  datbuf_bit = 0;         // ��¼ƫ�Ƶ�datbuf�ֽ��е��ĸ�λ
    FILE *fhufman;              // ѹ���ļ��ľ��
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
        filedat = fgetc(fd);        // ��һ���ֽ�
        if(filedat == EOF) break;   // -1ʱ�ļ�����
        else
        {
            array[(u8)filedat] ++; 
            map->length ++;
        }  
    }
    fclose(fd);
  //  debug("Read file finish\r\n");
    

    map->hafmanTree = BulidHufmanTree(array);
    _TabHufmanCreat(map->hafmanTree,map); // ������������ӳ���
  //  HufmanCompressFile_map( map,file,hufmanfile);
    fd = fopen(filename,"rb");
    fhufman =fopen(hufmanfile,"wb");
    for(i= 0;i<map->length;i++)    
    {   
        u32 ip = 0;
        u8* pstr = map->tab[fgetc(fd)];       // ��ø��ַ���ѹ����
        for(;pstr[ip];ip++)                 // ������ѹ���룬ͬʱ��ӵ�datbuf
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
                fwrite (array,4096,1,fhufman); // һ����д��
              }
        }
    }
    if(datbuf_bit) datbuf_byte ++;
    fwrite ((u8*)array,1,datbuf_byte,fhufman);      // ʣ��д��
    fclose(fd);
    fclose(fhufman);

    return map;    
}