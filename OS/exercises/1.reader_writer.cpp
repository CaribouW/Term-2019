#include "semaphore.h"

//读者写者问题
#define WriterFirst
//#define balance
//1 读者优先
semaphore rwmutex = semaphore(1);
semaphore r_counter_mutex = semaphore(1);
static int reader_count = 0;
#ifdef balance
semaphore queue = semaphore(1);
#endif

#ifndef WriterFirst
void process_reader()
{
    while (1)
    {
#ifdef balance
        P(&queue);
#endif
        P(&r_counter_mutex);
        if (0 == reader_count)
            P(&rwmutex);
        ++reader_count;
        V(&r_counter_mutex);
#ifdef balance
        V(&queue);
#endif
        //Do read
        P(&r_counter_mutex);
        --reader_count;
        if (0 == reader_count)
            V(&rwmutex);
        V(&r_counter_mutex);
    }
}
#endif

#ifndef WriterFirst
void process_writer()
{
    while (1)
    {
#ifdef balance
        P(&queue);
#endif
        P(&rwmutex);
#ifdef balance
        V(&queue);
#endif
        //do write
        V(&rwmutex);
    }
}
#endif

//写者优先
//queue是原来公平竞争的queue互斥量
//现在让写者不受queue的影响
#ifdef WriterFirst
semaphore queue = semaphore(1);
semaphore w_cnt_mutex = semaphore(1);
int writer_cnt = 0;

void process_reader()
{
    while (1)
    {
        P(&queue);              //读者需要等待的队列, 必须在写者个数是0才可以进入
        P(&r_counter_mutex);
        if (0 == reader_count)
            P(&rwmutex);
        ++reader_count;
        V(&r_counter_mutex);
        V(&queue);

        //read

        P(&r_counter_mutex);
        --reader_count;
        if (0 == reader_count)
            V(&rwmutex);
        V(&r_counter_mutex);
        V(&queue);
    }
}

void process_writer(){
    while(1){
        P(&w_cnt_mutex);
        if(0 == writer_cnt) P(&queue);
        ++writer_cnt;
        P(&rwmutex);
        V(&w_cnt_mutex);

        //write

        P(&w_cnt_mutex);
        --writer_cnt;
        if(0 == writer_cnt) V(&queue);
        V(&rwmutex);
        V(&w_cnt_mutex);
    }
}

#endif
