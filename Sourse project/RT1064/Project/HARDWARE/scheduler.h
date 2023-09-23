#include "zf_common_headfile.h"
#ifndef _SCHEDULER_H
#define _SCHEDULER_H


//任务
typedef struct
{
    void (*task)(void);//任务执行的函数指针
    uint32 period;//任务执行的周期，具体时间取决于时基
    uint32 delay;//任务延时
    uint32 run;//任务是否执行
} Task_t;
// your code goes here




void Schedule_Setup(void);
void Schedule_Run(void);
#endif // SCHEDULER_H
