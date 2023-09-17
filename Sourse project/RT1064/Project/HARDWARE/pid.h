#ifndef _PID_H
#define _PID_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MINMAX(input, low, upper) MIN(MAX(input, low), upper)//限幅度函数

typedef struct
{
    float kp;
    float ki;
    float kd;
} PID;

// 预采用位置式PID控制舵机，采用增量式PID控制电机
// pid控制器
extern int PositionPID(float deviation, PID pid);
extern int PositionPID2(float deviation, PID pid);
// extern int PositionPIDToSpeed(float deviation, PID pid);
// extern int PositionPIDToSpeed2(float deviation, PID pid);
extern int IncrementalPID(float deviation, PID pid);
extern int IncrementalPID2(float deviation, PID pid);

#endif