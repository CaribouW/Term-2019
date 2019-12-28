#include "semaphore.h"
/**
 * 有一个铁笼子，每次只能放入一个动物。猎手向笼中放入老虎，农夫向笼中放入羊；
 * 动物园等待取笼中的老虎，饭店等待取笼中的羊。请用P、V操作原语写出同步执行的程序
*/
semaphore edge(1);
semaphore tiger(0) , sheep(0);

//猎人
void hunter(){
    while (1)
    {           
        P(&edge);
        V(&tiger);
    }
}

//农夫
void farmer(){
    while(1){
        P(&edge);
        V(&sheep);
    }
}

void zoo(){
    while (1)
    {
        P(&tiger);
        V(&edge);
    }
}
void restaurant(){
    while (1)
    {
        P(&sheep);
        V(&edge);
    }
    
}
