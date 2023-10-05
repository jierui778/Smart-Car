#include "zf_common_headfile.h"
#ifndef _PID_H
#define _PID_H

//#define MIN(a, b) ((a) < (b) ? (a) : (b))
//#define MAX(a, b) ((a) > (b) ? (a) : (b))
// #define MINMAX(input, low, upper) MIN(MAX(input, low), upper)//限幅度函数
// #define Servo_Mid 1200//机械中值（待测量）
// #define Servo_Max 1250//舵机转向的最大值
// #define Servo_Min 1150//舵机转向的最小值
//PID参数结构体
typedef struct
{
    float kp;
    float ki;
    float kd;
} PID;
// typedef struct
// {
// 	float target_val;//目标值
// 	float actual_val;//实际值
// 	float err;//当前偏差
// 	float err_last;//上次偏差
// 	float err_sum;//误差累计值
// 	float Kp,Ki,Kd;//比例，积分，微分系数

// } tPid;
// 预采用位置式PID控制舵机，采用增量式PID控制电机
// pid控制器
//外部变量声明
extern PID TraceTurnParam;//转向环PID参数
extern PID SpeedParam;//速度环PID参数
//位置式PID控制器
extern int PositionPID(float Deviation, PID pid);
//增量式PID控制器
extern int IncrementPID(float Deviation, PID pid);
extern int IncrementPID2(float Deviation, PID pid);
void PID_Init(void);
#endif