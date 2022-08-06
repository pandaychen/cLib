#ifndef _STORMLI_HASH_TABLE_H_
#define _STORMLI_HASH_TABLE_H_

//http://www.cppblog.com/lmlf001/archive/2014/10/13/31858.html#208558

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>

template <typename valueType>
class HashTable
{
    enum KindOfNode
    {
        Active,
        Empty,
        Deleted
    }; //节点状态，活跃/空/已删除
public:
    int find(unsigned long _key);                            //查找_key是否在表中，若在，返回0；否则返回-1；找到时，m,n为节点的位置
    int insert(unsigned long _key, const valueType &_value); //插入节点，若该节点已在表中，返回1；若插入成功，返回0，表满，返回-1
    int remove(unsigned long _key);                          //删除节点 若节点不存在，返回-1，成功时返回0
public:
    double getFullRate() const;                                      //返回Hash表空间利用率
    bool isFull() const { return currentSize == maxSize + maxList; } //表是否为满
    void clear();                                                    //清除表内容
    void setList(int len = 5)
    {
        if (listLen == -1)
            listLen = len;
    } // default 5,the better one
    double getOverflowRate() const { return double(maxList) / currentSize; }

public:
    HashTable(unsigned long maxline, unsigned short line = 5); //构造函数line default 5,a better range
    ~HashTable();

private:
    struct HashNode //哈希节点
    {
        unsigned long key; // name
        valueType value;   // value
        KindOfNode state;  //节点状态
        HashNode() : state(Empty) {}
        ~HashNode() {}
    };

    struct list_node
    { //链表节点 存储溢出数据
        unsigned long key;
        valueType value;
        list_node *next;
        list_node() : next(NULL) {}
    };

    unsigned long currentSize; //当前数据量
    unsigned long maxSize;     //最大数据量
    unsigned long maxLine;     //每行最大值
    unsigned short lines;      //行数
    HashNode **ht;             //哈希阵列
    unsigned long *modTable;   //每行的模数
    unsigned long m, n;        //查找时存储节点位置，否则做为临时变量使用
    HashNode *mem;             //哈希表的实际存储空间
    list_node **Overflow_list; //溢出链
    unsigned long maxList;     // list current size
    int listLen;               // list length  default -1---no limit

private:
    void getMode();        //生成模数
    void clear_Overflow(); //清除溢出链数据
};

template <typename valueType>
HashTable<valueType>::HashTable(unsigned long maxline, unsigned short line)
{
    maxLine = maxline;
    lines = line;
    currentSize = 0;
    ht = new HashNode *[lines];
    assert(ht != NULL);

    mem = new HashNode[maxLine * lines]; //实际空间
    assert(mem != NULL);
    int i;
    for (i = 0; i < lines; i++) //分配空间到哈希阵列
        ht[i] = mem + maxLine * i;
    /*
    for(i=0;i<lines;i++){                //申请存储空间
        ht[i]=new HashNode[maxLine];
        if(ht[i]==NULL){                //申请失败，释放已申请空间，退出
            for(j=i-1;i>=0;j++)
                delete []ht[j];
            delete []ht;
            exit(-1);
        }
    }
    */

    modTable = new unsigned long[lines + 1]; // modTable[lines]  Overlist
    assert(modTable != NULL);
    getMode();
    /*-----------not must need--------------------------------*/
    long temp = modTable[0];
    for (i = 0; i < lines + 1; i++)
        modTable[i] = modTable[i + 1];
    modTable[lines] = temp;
    /*------------------------------------------------------*/
    maxSize = 0;
    for (i = 0; i < lines; i++)
        maxSize += modTable[i]; //最大值

    Overflow_list = new list_node *[modTable[lines]]; // Overflow head list ,all NULL pointer
    assert(Overflow_list != NULL);
    listLen = -1;
    maxList = 0;
}

template <typename valueType>
void HashTable<valueType>::getMode()
{ //采用 6n+1 6n-1 素数集中原理
    if (maxLine < 5)
    {
        this->~HashTable();
        exit(-1);
    }

    unsigned long t, m, n, p;
    int i, j, a, b, k;
    int z = 0;

    for (t = maxLine / 6; t >= 0, z <= lines; t--)
    {
        i = 1;
        j = 1;
        k = t % 10;
        m = 6 * t; /**i,j的值 是是否进行验证的标志也是对应的6t-1和6t+1的素性标志**/
        if (((k - 4) == 0) || ((k - 9) == 0) || ((m + 1) % 3 == 0))
            j = 0; /*此处是简单验证6*t-1,6*t+1 是不是素数，借以提高素数纯度**/
        if (((k - 6) == 0) || ((m - 1) % 3 == 0))
            i = 0; /***先通过初步判断去除末尾是5，及被3整除的数***/
        for (p = 1; p * 6 <= sqrt(m + 1) + 2; p++)
        {
            n = p * 6; /**将6*p-1和6*p+1看作伪素数来试除*****/
            k = p % 10;
            a = 1;
            b = 1; /**同样此处a,b的值也是用来判断除数是否为素数提高除数的素数纯度**/
            if (((k - 4) == 0) || ((k - 9) == 0))
                a = 0;
            if (((k - 6) == 0))
                b = 0;
            if (i)
            { /*如果i非零就对m-1即所谓6*t-1进行验证，当然还要看除数n+1,n-1,素性纯度*/
                if (a)
                {
                    if ((m - 1) % (n + 1) == 0)
                        i = 0;
                } /***一旦被整除就说明不是素数故素性为零即将i 赋值为零***/
                if (b)
                {
                    if ((m - 1) % (n - 1) == 0)
                        i = 0;
                }
            }
            if (j)
            { /**如果j非零就对m+1即所谓6*t+1进行验证，当然还要看除数n+1,n-1,素性纯度*/
                if (a)
                {
                    if ((m + 1) % (n + 1) == 0)
                        j = 0;
                } /***一旦被整除就说明不是素数故素性为零即将j 赋值为零***/
                if (b)
                {
                    if ((m + 1) % (n - 1) == 0)
                        j = 0;
                }
            }
            if ((i + j) == 0)
                break; /**如果已经知道6*t-1,6*t+1都不是素数了那就结束试除循环***/
        }
        if (j)
        {
            modTable[z++] = m + 1;
            if (z > lines)
                return;
        }
        if (i)
        {
            modTable[z++] = m - 1;
            if (z > lines)
                return;
        }
    }
}

template <typename valueType>
HashTable<valueType>::~HashTable()
{
    delete[] mem; //释放空间
    delete[] ht;
    delete[] modTable;

    clear_Overflow();
    delete[] Overflow_list;
    /*
    for(int i=0;i<lines;i++)
        delete []ht[i];
    delete []ht;
    delete []modTable;
    */
}

template <typename valueType>
int HashTable<valueType>::find(unsigned long _key)
{
    int i;
    for (i = 0; i < lines; i++)
    {
        m = i;
        n = (_key + maxLine) % modTable[m];
        if (ht[m][n].key == _key && ht[m][n].state == Active)
            return 0;
    }
    if (listLen == 0)
        return -1;
    m = lines;
    n = (_key + maxLine) % modTable[m];
    list_node *pre = Overflow_list[n];
    while (pre != NULL)
    {
        if (pre->key == _key)
            return 0;
        pre = pre->next;
    }
    return -1;
}

template <typename valueType>
int HashTable<valueType>::insert(unsigned long _key, const valueType &_value)
{
    if (find(_key) == 0)
        return 1; //已存在

    int i, len = 0;
    for (i = 0; i < lines; i++)
    {
        m = i;
        n = (_key + maxLine) % modTable[m];
        if (ht[m][n].state != Active)
        { //阵列中找到空位,插入数据
            ht[m][n].key = _key;
            ht[m][n].value = _value;
            ht[m][n].state = Active;
            currentSize++;
            return 0;
        }
    }
    if (listLen == 0)
        return -1;

    //阵列已满,插入溢出链表
    m = lines;
    n = (_key + maxLine) % modTable[m];
    list_node *pre = Overflow_list[n];

    list_node *now = new list_node;
    now->key = _key;
    now->value = _value;

    if (pre == NULL)
    {
        Overflow_list[n] = now;
    }
    else
    {
        len = 1;
        while (pre->next != NULL)
        {
            pre = pre->next;
            len++;
        }
        if (listLen > 0 && len >= listLen)
            return -1; // full  ;if listLen<0  no limit ,continue the left operation
        pre->next = now;
    }
    currentSize++;
    maxList++;

    return 0;
}

template <typename valueType>
int HashTable<valueType>::remove(unsigned long _key)
{
    if (find(_key) != 0)
        return -1;
    if (m < lines)
    {                             //节点在阵列中
        ht[m][n].state = Deleted; //节点状态置为Deleted
        currentSize--;
    }
    else
    { //节点在溢出链中
        list_node *pre = Overflow_list[n], *now;
        if (pre->key == _key)
        {
            Overflow_list[n] = pre->next;
            delete pre;
        }
        else
        {
            while (pre->next->key != _key && pre->next != NULL)
                pre = pre->next;
            now = pre->next;
            pre->next = now->next;
            delete now;
        }
        currentSize--;
        maxList--;
    }
    return 0;
}

template <typename valueType>
double HashTable<valueType>::getFullRate() const
{
    return double(currentSize) / (maxSize + maxList); //空间使用率
}

template <typename valueType>
void HashTable<valueType>::clear()
{
    int i, j;
    for (i = 0; i < lines; i++) //把所有节点状态置为Empty
        for (j = 0; j < modTable[i]; j++)
            ht[i][j].state = Empty;
    if (listLen != 0)
        clear_Overflow();
    maxList = 0;
    currentSize = 0;
}

template <typename valueType>
void HashTable<valueType>::clear_Overflow()
{
    list_node *pre = NULL, *now = NULL;
    for (int i = 0; i < modTable[lines]; i++)
    {
        pre = Overflow_list[i];
        if (pre == NULL)
            continue;
        while (pre->next != NULL)
        {
            now = pre->next;
            pre->next = now->next;
            delete now;
        }
        delete pre;
        Overflow_list[i] = NULL;
    }
}

#endif