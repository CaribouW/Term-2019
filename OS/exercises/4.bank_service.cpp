#include "semaphore.h"

/**
 * 某大型银行办理人民币储蓄业务，由n个储蓄员负责。
 * 每个顾客进入银行后先至取号机取一个号，并且在等待区找到空沙发坐下等着叫号。
 * 取号机给出的号码依次递增，并假定有足够多的空沙发容纳顾客。
 * 当一个储蓄员空闲下来，就叫下一个号。请用信号量和P，V操作正确编写储蓄员进程和顾客进程的程序 
*/
//号 number
#define depo_cnt 10
int id = 0;
semaphore customer(0), depo(depo_cnt);
semaphore mutex(1);

void process_customer(){
    while(1){
        //取号
        id++;
        P(&mutex);      //进入临界区
        //找到沙发等待
        V(&mutex);
        V(&customer);
        P(&depo);   //沙发等待
    }
}

void process_depo(){
    while(1){
        P(&depo);
        P(&mutex);
        //呼唤顾客
        V(&mutex);
        V(&depo);
    }
}