
#include "semaphore.h"
/**
 * 有n个进程将字符逐个读入到一个容量为80的缓冲区中(n>1)，当缓冲区满后，由输出进程Q负责一次性取走这80个字符。
 * 这种过程循环往复，请用信号量和P、V操作写出n个读入进程(P1， P2，…Pn)和输出进程Q能正确工作的动作序列
 * 
*/

#define capacity 80
semaphore full(0), empty(capacity), mutex(1);
int index = 0, cnt = 0;

char *buffer = new char[capacity];

//消费者
void process_Q()
{
    while (1)
    {
        P(&full);
        P(&mutex);
        for (int i = 0; i < capacity; ++i)
        {
            buffer[i] = '\0';
            V(&empty);
        }
        V(&mutex);
    }
}

//生产者
void process_P()
{
    while (1)
    {
        P(&empty);
        P(&mutex);
        buffer[(index++) % capacity] = 'c';
        ++cnt;
        if (cnt == capacity)
        {
            V(&mutex);
            V(&full);
        }
        else
        {
            V(&mutex);
        }
    }
}