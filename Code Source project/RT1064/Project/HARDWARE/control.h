#include "zf_common_headfile.h"
#ifndef _CONTROL_H
#define _CONTROL_H

#define CONTROL_MODE 0 // 控制模式选择，0为转向控制，1为转向与差速控制

#define MINMAX(input, low, upper) MIN(MAX(input, low), upper) // 限幅度函数

#define SPEED_MAX 3
#define SPEED_MIN 0.5

#define Pi 3.1415926            // 圆周率
#define WHEEL_R 0.033           // 轮子半径：33mm=0.033m
#define WHEEL_C 2 * Pi *WHEEL_R // 轮子周长

#define WHEEL_COUNT (2300)   // 轮子转一圈，对应编码器的计数值（得手动测）
#define ENCODER_COUNT (1024) // 编码器转一圈，对应的编码器计数值（1024线编码器标定值）

enum motor_mode
{
    MOTOR_NORMAL,
    MOTORSOFT,
    MODE_STOP,
};

void Angle_Control(float current_err); // 巡线控制

void Speed_Control(float left_speed, float right_speed); // 速度控制
void SpeedUp_Control(void);                              // 加速控制
void SpeedDown_Control(void);                            // 减速控制

void Car_Stop(void);//

// 四轮车，后期考虑将误差比例分配舵机与电机进行调节

#endif
