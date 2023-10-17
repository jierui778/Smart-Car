#include "zf_common_headfile.h"
#ifndef _CIRCLE_H_
#define _CIRCLE_H_




enum circle_type_e {
    CIRCLE_NONE = 0, // 非圆环模式
    CIRCLE_LEFT_FOUND,CIRCLE_RIGHT_FOUND, // 左右圆环开始,即单侧发现L角点，另一侧为长直道
    CIRCLE_RIGHT_IN,CIRCLE_LEFT_IN, // 进圆环
    CIRCLE_LEFT_OUT,CIRCLE_RIGHT_OUT, // 出圆环
    CIRCLE_LEFT_END,CIRCLE_RIGHT_END,
    CIRCLE_STOP, // 停车
    CIRCLE_NUM,
};

extern enum circle_type_e circle_type;//枚举类型全局定义

//进圆环必定丢线，考虑进圆环仅巡单边线
#endif