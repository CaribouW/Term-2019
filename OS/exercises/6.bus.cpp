#include "semaphore.h"
/**
 * 汽车司机与售票员之间必须协同工作，一方面只有售票员把车门关好了司机才能开车，
 * 因此，售票员关好门应通知司机开车，然后售票员进行售票。
 * 另一方面，只有当汽车已经停下，售票员才能开门上下客，故司机停车后应该通知售票员。
 * 假定某辆公共汽车上有一名司机与两名售票员，汽车当前正在始发站停车上客，试用信号量与P、V操作写出他们的同步算法 
*/

semaphore stop_1(1) , stop_2(0) , start_1(0) , start_2(0);

void process_driver(){
    while(1){
        P(&start_1);
        P(&start_2);
        //开车
        //停车
        V(&stop_1);
        V(&stop_2);
    }
}

void process_seller_1(){
    while(1){
        //开车门
        //上乘客
        V(&start_1);
        //关车门
        P(&stop_1);
    }
}


void process_seller_2(){
    while(1){
        //开车门
        //上乘客
        V(&start_2);
        //关车门
        P(&stop_2);
    }
}