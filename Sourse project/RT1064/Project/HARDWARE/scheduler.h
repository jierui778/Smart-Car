#include "zf_common_headfile.h"

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static uint8 Scheduler_Counter;//调度器计数器.时基为xxx
volatile static uint8 Counter_Flag;//调度器计时标志位

//任务
typedef struct
{
    void (*task)(void);//任务执行的函数指针
    uint32 period;//任务执行的周期，具体时间取决于时基
    uint32 delay;//任务延时
    uint8 flag; // 任务是否执行
} Task_t;
// your code goes here




void Schedule_Setup(void);
void Schedule_Run(void);
void pit_handler(void);
#endif // SCHEDULER_H
