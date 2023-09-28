#ifndef _PID_H
#define _PID_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MINMAX(input, low, upper) MIN(MAX(input, low), upper)//限幅度函数
#define Servo_Mid 1200//机械中值（待测量）
#define Servo_Max 1250//舵机转向的最大值
#define Servo_Min 1150//舵机转向的最小值
typedef struct
{
    float kp;
    float ki;
    float kd;
} PID;

// 预采用位置式PID控制舵机，采用增量式PID控制电机
// pid控制器
//外部变量声明
extern PID TraceTurnPdate;
extern PID SpeedPdate;

//位置式PID控制器
extern int PositionPID(float deviation, PID pid);
extern int PositionPID2(float deviation, PID pid);

//增量式PID控制器
extern int IncrementalPID(float deviation, PID pid);
extern int IncrementalPID2(float deviation, PID pid);
extern int PositionPID_Servo(float deviation, PID pid);
#endif