#include "semaphore.h"

/*
理发店理有一位理发师、一把理发椅和n把供等候理发的顾客坐的椅子
如果没有顾客，理发师便在理发椅上睡觉
一个顾客到来时，它必须叫醒理发师
如果理发师正在理发时又有顾客来到，则如果有空椅子可坐，就坐下来等待，否则就离开
使用PV操作求解该问题

理解：其实理发师是消费者, 顾客都是生产者
*/
#define N 10
extern void baber_cut();
extern void get_haircut();
int waiting_cnt = 0, chair = N;
semaphore barber = semaphore(0), customer = semaphore(0), mutex = semaphore(1);

//理发师进程
void process_baber()
{
    while (1)
    {
        P(&customer); //确保有人在等,没有人那么等待
        P(&mutex);
        --waiting_cnt;
        V(&barber);
        V(&mutex);
        baber_cut(); //开始理发
    }
}

void process_customer()
{
    while (1)
    {
        //满了
        P(&mutex);
        if (waiting_cnt == chair)
        {
            V(&mutex);
        }
        else
        {
            ++waiting_cnt;
            V(&customer); //唤醒理发师
            V(&mutex);
            P(&barber);    //如果理发师忙, 就继续等
            get_haircut(); //开始进行理发
        }
    }
}