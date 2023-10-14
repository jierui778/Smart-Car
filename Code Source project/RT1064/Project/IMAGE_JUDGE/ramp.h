#include "zf_common_headfile.h"
#ifndef _RAMP_H_
#define _RAMP_H_

#define RAMP_ENCODER_THRES 1000 // 坡道编码器积分阈值

//面向赛道编程

//枚举ramp的状态
enum ramp_type_e
{
    RAMP_FOUND,//发现坡道
    RAMP_NONE,//非坡道模式
    RAMP_BEGIN,//坡道开始
    RAMP_IN,//开始进入
    RAMP_RUNNING,//坡道内
    RAMP_OUT,//坡道出口
    RAMP_END,//坡道结束
    RAMP_NUM,//坡道数量
};

extern enum ramp_type_e ramp_type;//

extern int Ramp_Distance;//记录坡道距离


void Ramp_Check(void);//检查坡道
void Ramp_Run(void);//进入坡道
void Ramp_Out(void);//离开坡道


#endif