#include "schedule.h"
#include "encoder.h"
#include "image.h"

#define TASK_LIST_END \
    {                 \
        NULL, 0, 0, 0 \
    } // 任务列表结束标志

// 任务执行函数
static void Schedule1(void) // task1:更新编码器数据//周期为10ms
{

    test();
}

static void Schedule2(void) // task2:
{
}

static void Schedule3(void) // task3
{
}

// 任务列表
static Task_t Task_List[] =
    {
        {Schedule1, 1, 0, 0}, // 任务1，周期为10ms
        {Schedule2, 0, 0, 0},
        {Schedule3, 0, 0, 0},
        // 创建
};

#define TASK_NUM (sizeof(Task_List) / sizeof(Task_t)) // 任务数量

/**
 * @brief 任务框架的时基计数标志位(放于中断进行更新)
 *
 */
void Schedule_Tick(void)
{
    Counter_Flag = TRUE;
}

/**
 * @brief 每次中断后调用，判断线程是否应该执行
 *
 */
static void Schedule_UpdateFlag(void)
{
    for (uint8 index = 0; index < TASK_NUM; index++)
    {
        if (Task_List[index].period != 0)
        {
            if (Schedule_Counter % Task_List[index].period == 0)
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
void Schedule_Run(void)
{
    //    CR_BEGIN//确保在部分代码块中禁用中断，以确保代码块的原子性
    if (Counter_Flag)
    {
        Counter_Flag = FALSE;  // 清除标志位
        ++Schedule_Counter;    // 计数器加一
        Schedule_UpdateFlag(); // 判断线程是否应该执行
    }

    //    CR_END//确保在部分代码块中开启中断，以确保代码块的原子性
    for (uint8 index = 0; index < TASK_NUM; index++) // 循环判断所有线程是否应该执行
    {
        if (Task_List[index].flag == TRUE) // 判断线程是否应该执行
        {
            if (Task_List[index].delay == 0) // 判断线程是否延时
            {
                Task_List[index].flag = FALSE; // 清除标志位
                Task_List[index].task();       // 执行线程
            }
            else
            {
                Task_List[index].delay--; // 线程延时减一
            }
        } ////////////////////////////////待修改
          //        Task_List[index].task();
    }
}
/**
 * @brief 中断更新任务调度框架的时基计数标志位，以及判断线程是否应该执行
 *
 */
void pit_handler(void)
{
    Schedule_Tick();
    gpio_toggle_level(B9); // test
    Encoder_Update(&Encoder_L_Data, &Encoder_R_Data, &Encoder_L_Dis, &Encoder_R_Dis);
}
// //void pit_handler(void)
// //{
// //    gpio_toggle_level(B9);                                                    // 触发 PIT 中断后翻转 LED 状态
// //}