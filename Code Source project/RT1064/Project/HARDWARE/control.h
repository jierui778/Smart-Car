#include "zf_common_headfile.h"
#ifndef _CONTROL_H
#define _CONTROL_H

#define CONTROL_MODE 0 // 控制模式选择，0为转向控制，1为转向与差速控制

#define MINMAX(input, low, upper) MIN(MAX(input, low), upper) // 限幅度函数

#define SPEED_MAX 3
#define SPEED_MIN 0.5

#define Pi 3.1415926            // 圆周率
#define WHEEL_R 0.033           // 轮子半径：33mm=0.033m
#define WHEEL_C 2 * PI *WHEEL_R // 轮子周长

#define WHEEL_COUNT (2300)   // 轮子转一圈，对应编码器的计数值（得手动测）
#define ENCODER_COUNT (1024) // 编码器转一圈，对应的编码器计数值（1024线编码器标定值）

#define track 15.26f      // 车身长度
#define length 20.0f      // 轴距
#define wheelbase 1.27f   // 轮宽
#define k 0.5f            // 调节系数k    //先调这个
#define drift_factor 0.1f // 漂移系数     //后调这个，如果飘得离谱，就改小一点\

typedef struct
{
    float TargetSpeed;//目标速度
    float Speed;//当前速度
} sMotor_Info;


typedef struct
{
    float TargetAngle;//目标角度
    float Angle;//当前角度
    float Output;//输出
    int Max_Output;//最大输出
} sServo_Info;

extern sMotor_Info Motor_Info[2];
extern sServo_Info ServoInfo;


// typedef struct
//{
//     float TargetAngle;//目标角度
//     float Angle;//当前角度
//     float Output;//输出
//     int Max_Output;//最大输出
// } sServo_Info;

// extern sMotor_Info IncrPID_Info[2];
// extern sServo_Info PosiPID_Info[2];//

enum motor_mode
{
    MOTOR_NORMAL,
    MOTORSOFT,
    MODE_STOP,
};

void TrackLine_Control(float current_err);               // 巡线控制
void Speed_Control(float left_speed, float right_speed); // 速度控制
void SpeedUp_Control(void);                              // 加速控制
void SpeedDown_Control(void);                            // 减速控制
void ackermann(float err, float v,sMotor_Info *Motor_Info_L,sMotor_Info *Motor_Info_R);
// 四轮车，后期考虑将误差比例分配舵机与电机进行调节//阿克曼结构

#endif
