#include <stdio.h>
#include <stdlib.h>

typedef int SLDataType;

typedef struct SeqList
{
    SLDataType* arr;
    int size;
    unsigned int capacity;  // 使用unsigned int来存储容量
} SL;

static void SeqListCheckCapacity(SL* psl)
{
    unsigned long long newcapacity;  // 使用unsigned long long来计算新容量

    // 当前容量已经非常大，直接退出
    if (psl->capacity > (INT_MAX / 2))
    {
        printf("容量溢出，当前容量：%u\n", psl->capacity);
        exit(-1);
    }

    newcapacity = (unsigned long long)psl->capacity * 2;

    // 如果 newcapacity 大于 INT_MAX，则将其设置为 INT_MAX
    if (newcapacity > (unsigned long long)INT_MAX)
    {
        newcapacity = INT_MAX;
    }

    SLDataType* tmp = (SLDataType*)realloc(psl->arr, newcapacity * sizeof(SLDataType));

    // 如果realloc失败，尝试减小容量
    while (tmp == NULL && newcapacity > 0)
    {
        newcapacity = newcapacity / 2;
        tmp = (SLDataType*)realloc(psl->arr, newcapacity * sizeof(SLDataType));
    }

    if (tmp == NULL)
    {
        printf("realloc失败，当前容量：%u，尝试分配容量：%llu\n", psl->capacity, newcapacity);
        exit(-1);
    }

    psl->arr = tmp;

    // 确保 newcapacity 不为0再更新容量
    if (newcapacity != 0)
    {
        psl->capacity = (unsigned int)newcapacity;
    }
    else
    {
        printf("新容量为0，无法更新容量\n");
        exit(-1);
    }
}

int main()
{
    SL sl;
    sl.capacity = 1073741824;  // 设置当前容量为 1073741824

    // 测试 SeqListCheckCapacity
    SeqListCheckCapacity(&sl);
    printf("新容量：%u\n", sl.capacity);  // 应该输出 2147483647

    return 0;
}
