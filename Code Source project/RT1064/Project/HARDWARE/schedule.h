#include "zf_common_headfile.h"
#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// #define CR_BEGIN
// {
//     uint8 interrupt_state = interrupt_global_disable();

// #define CR_END

//     interrupt_global_enable(interrupt_state);
// } // 确保在部分代码块中禁用中断，以确保代码块的原子性

static uint8 Schedule_Counter;//调度器计数器.时基为中断周期，以中断初始化周期为准
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


// void Schedule_Setup(void);
void Schedule_Run(void);
void pit_handler(void);
#endif // SCHEDULER_H
