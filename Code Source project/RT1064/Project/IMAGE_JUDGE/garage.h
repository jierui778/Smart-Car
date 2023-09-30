#include "zf_common_headfile.h"
#ifndef _GARAGE_H_
#define _GARAGE_H_

enum garage_type_e
{
    GARAGE_NONE = 0, // 非车库模式
    GARAGE_OUT_LEFT,//左出库
    GARAGE_OUT_RIGHT, // 右出库
    GARAGE_FOUND_LEFT,
    GARAGE_FOUND_RIGHT, // 发现车库，即斑马线+单侧L角点(未使用)
    GARAGE_IN_LEFT,
    GARAGE_IN_RIGHT, // 进库，发现车库后判断第几次，从而决定是否进库
    GARAGE_PASS_LEFT,
    GARAGE_PASS_RIGHT, // 不进库，发现车库后判断第几次，从而决定是否进库
    GARAGE_STOP,       // 进库完毕，停车
    GARAGE_NUM,
};



extern enum garage_type_e garage_type;//枚举类型全局定义




void Garage_out(void);
void Garage_in(void);
void Garage_stop(void);
void Garage_pass(void);
void Garage_Check(void);//车库检测函数


#endif