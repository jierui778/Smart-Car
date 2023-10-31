#include "zf_common_headfile.h"
#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#define CIRCLE_DIS 1000

enum circle_type_e
{
    CIRCLE_NONE, // 非圆环模式
    CIRCLE_LEFT_FOUND,
    CIRCLE_RIGHT_FOUND, // 左右圆环开始,即单侧发现L角点，另一侧为长直道
    CIRCLE_RIGHT_IN,
    CIRCLE_RIGHT_IN1,
    CIRCLE_LEFT_IN, // 进圆环
    // CIRCLE_LEFT_IN1,
    CIRCLE_LEFT_RUN,
    CIRCLE_RIGHT_RUN, // 圆环内巡线
    CIRCLE_LEFT_OUT,
    CIRCLE_RIGHT_OUT, // 出圆环
    CIRCLE_LEFT_END,
    CIRCLE_RIGHT_END,
    CIRCLE_NUM,
};

/*状态1到状态2:左边近拐点坐标小于90即可 (或记录上次的左下拐点的y坐标,然后判断两次y坐标相差是否超过10)
状态2到状态3 : 左边近拐点坐标小于一个值就进入状态3*/
extern enum circle_type_e circle_type; // 枚举类型全局定义
extern int cross_state;
// 进圆环必定丢线，考虑进圆环仅巡单边线
void Circle_Check(void);
void Circle_Run(void);
void Circle_Run_Left_Found(void);

#endif