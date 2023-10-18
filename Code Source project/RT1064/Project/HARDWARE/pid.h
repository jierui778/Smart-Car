#include "zf_common_headfile.h"
#include "control.h"
#ifndef _PID_H
#define _PID_H
typedef struct
{
    float Kp, Ki, Kd;
    float Err, LastErr, LastLastErr;
    float Output, MaxOutput;
} sIncrPID_Info; // 增量式PID结构体
typedef struct
{
    float Kp, Ki, Kd;
    float Err, LastErr, Integral_Err;
    float Output, MaxIntegral_Err,MaxOutput;//位置式PID必须要限幅
} sPosiPID_Info; // 位置式PID结构体

extern sIncrPID_Info IncrPID_Info[2];
extern sPosiPID_Info PosiPID_Info[2];
void PID_Init(void);                                                // 初始化PID控制器
extern void IncrPID(sIncrPID_Info *IncrPID, sMotor_Info *MotorInfo); // 增量式PID
extern void PosiPID(sPosiPID_Info *PosiPID, sServo_Info *ServoInfo); // 位置式PID

#endif