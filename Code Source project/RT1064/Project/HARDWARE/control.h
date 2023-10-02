#include "zf_common_headfile.h"
#ifndef _CONTROL_H
#define _CONTROL_H

#define SPEED_MAX 3
#define SPEED_MIN 0.5


#define PI 3.1415926//圆周率
#define WHEEL_R 33 // 轮子半径：33mm
#define WHEEL_C 2*PI*WHEEL_R // 轮子周长

#define ENCODER_COUNT (2300) // 轮子转一圈，对应计数器的计数值（得手动测）

enum motor_mode
{
    MOTOR_NORMAL,MOTORSOFT,MODE_STOP
};




void TrackLine_Control(float current_err);//巡线控制
void Motor_Control();
// void Direction_Control();
void Speed_Control(float left_speed,float right_speed);//速度控制
void Angle_Control(float current_err);//舵机角度控制
void SpeedUp_Control(void);//加速控制
void SpeedDown_Control(void);//减速控制

#endif
