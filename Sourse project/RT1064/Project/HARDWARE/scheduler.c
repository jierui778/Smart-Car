#include "scheduler.h"


//#define CR_BEGIN
//{
//    uint8 interrupt_state = interrupt_global_disable();

//#define CR_END

//    interrupt_global_enable(interrupt_state);
//}//确保在部分代码块中禁用中断，以确保代码块的原子性

// 根据数组长度，判断线程数量
#define TASK_LIST_END \
    {                 \
        NULL, 0, 0, 0    \
    }

//任务执行函数
static void Scheduler1(void)//task1
{

}

static void Scheduler2(void)//task2
{
}

static void Scheduler3(void)//task3
{
}


//任务列表
static Task_t Task_List[] =
    {
        {Scheduler1, 0, 0, 0},
        {Scheduler2, 0, 0, 0},
        {Scheduler3, 0, 0, 0},
//创建
};


#define TASK_NUM (sizeof(Task_List) / sizeof(Task_t))


/**
 * @brief 任务框架的时基计数标志位
 *
 */
void Scheduler_Tick(void)
{
    Counter_Flag = TRUE;
}
/**
 * @brief 每次中断后调用，判断线程是否应该执行
 *
 */
static void Scheduler_UpdateFlag(void)
{
    for (uint8 index = 0; index < TASK_NUM; index++)
    {
        if (Task_List[index].period != 0)
        {
            if (Scheduler_Counter % Task_List[index].period == 0)
            {
                Task_List[index].flag = TRUE;
            }
        }
    }
}




/**
 * @brief 循环判断执行线程
 *
 */
void Scheduler_Run(void)
{
//    CR_BEGIN//确保在部分代码块中禁用中断，以确保代码块的原子性
    if(Counter_Flag)
    {
        Counter_Flag = FALSE;//清除标志位
        ++Scheduler_Counter;//计数器加一
        Scheduler_UpdateFlag();//判断线程是否应该执行
    }


//    CR_END//确保在部分代码块中开启中断，以确保代码块的原子性
    for (uint8 index = 0;index<TASK_NUM;index++)//循环判断所有线程是否应该执行
    {
        if(Task_List[index].flag == TRUE)//判断线程是否应该执行
        {
            if(Task_List[index].delay == 0)//判断线程是否延时
            {
                Task_List[index].flag = FALSE;//清除标志位
                Task_List[index].task();//执行线程
            }
            else
            {
                Task_List[index].delay--;//线程延时减一
            }
        }////////////////////////////////待修改
        Task_List[index].task();
    }
}


