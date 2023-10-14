#include "zf_common_headfile.h"
#ifndef _PID_H
#define _PID_H

//PID参数结构体
typedef struct
{
    float kp;
    float ki;
    float kd;
} PID;


//无/微分先行，微分低通滤波，积分限幅，梯形积分，变积分
enum{
    PID_NONE,

}PID_set_e;


extern PID TraceTurnParam;//转向环PID参数
extern PID SpeedParam;//速度环PID参数
//位置式PID控制器
extern int PositionPID(float Deviation, PID pid);
//增量式PID控制器
extern int IncrementPID(float Deviation, PID pid);
extern int IncrementPID2(float Deviation, PID pid);
void PID_Init(void);


#endif