#include "zf_common_headfile.h"
#ifndef _RAMP_H_
#define _RAMP_H_
#define RAMP_DIS 1000
enum ramp_type_e
{
    RAMP_NONE,  // 非坡道模式
    RAMP_FOUND, // 坡道找到
    RAMP_IN,    // 坡道进入
    RAMP_OUT,   // 准备出坡道
    RAMP_END,   // 坡道结束
    RAMP_NUM,   //
};

extern enum ramp_type_e ramp_type;

void Ramp_Check(void);
void Ramp_Run(void);

#endif