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
    float Output, MaxIntegral_Err, MaxOutput; // 位置式PID必须要限幅
} sPosiPID_Info;                              // 位置式PID结构体



extern sPosiPID_Info  Servo_PIDInfo;
extern sIncrPID_Info  Motor_PIDInfo[2];

// extern sIncrPID_Info IncrPID_Info[2];
// extern sPosiPID_Info PosiPID_Info[2];
void PID_Init(void);
void PID_Init_New(void);// 初始化PID控制器
void IncrPID(sIncrPID_Info *IncrPID, sMotor_Info *MotorInfo);
// extern void IncrPID(sIncrPID_Info *IncrPID, sMotor_Info *MotorInfo); // 增量式PID
// extern void PosiPID(sPosiPID_Info *PosiPID, sServo_Info *ServoInfo); // 位置式PID
extern void PosiPID(sPosiPID_Info *PosiPID, float *test);
void Servo_PidSetSpeed(float Target_Angle);

typedef struct
{
	float target_val;//目标值
	float actual_val;//实际值
	float err;//当前偏差
	float err_last;//上次的偏差
	float err_sum;//误差累计值
	float Kp,Ki,Kd;//比例，积分，微分稀释
	
} tPid;
float P_realize(tPid * pid,float actual_val);
void PID_Init(void);
float PI_realize(tPid * pid,float actual_val);
float PID_realize(tPid * pid,float actual_val);
#endif