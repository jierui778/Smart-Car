#include "scheduler.h"
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


#define TASK_NUM (sizeof(Task_list) / sizeof(Task_t))
//根据数组长度，判断线程数量
#define TASK_LIST_END \
    {                 \
        NULL, 0, 0    \
    }


/**
 * @brief 任务框架的时基
 *
 */
void Scheduler_Tick(void)
{

}



/**
 * @brief 循环判断执行线程
 *
 */
void Scheduler_Run(void)
{
    uint8 index;
    for (index = 0;index<TASK_NUM;index++)//循环判断所有线程是否应该执行
    {
        Task_List[index].task();
    }
}



