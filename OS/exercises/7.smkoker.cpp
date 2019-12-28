#include "semaphore.h"
/**
 * 一个经典同步问题：吸烟者问题(patil，1971)。
 * 三个吸烟者在一个房间内，还有一个香烟供应者。
 * 为了制造并抽掉香烟，每个吸烟者需要三样东西：烟草、纸和火柴，供应者有丰富货物提供。
 * 三个吸烟者中，第一个有自己的烟草，第二个有自己的纸和第三个有自己的火柴。
 * 供应者随机地将两样东西放在桌子上，允许一个吸烟者进行对健康不利的吸烟。
 * 当吸烟者完成吸烟后唤醒供应者，供应者再把两样东西放在桌子上，唤醒另一个吸烟者。试用信号量和P、V操作求解该问题
 * 
*/
extern int random(int);
semaphore S[3] = {semaphore(0), semaphore(0), semaphore(0)};
semaphore mutex(1);

void process_provider()
{
    while (1)
    {
        int i = random(3) % 3;
        int j = random(3) % 3;
        P(&mutex);
        //put i and j on the table
        int offset = 3 - (i + j) % 3;
        V(S + offset);
    }
}

void process_smoker(int i)
{
    while (1)
    {
        P(&S[i % 3]);
        //begin to smoke
        V(&mutex);
    }
}