#include "zf_common_headfile.h"
#ifndef _CONTROL_H
#define _CONTROL_H

#define SPEED_MAX 3
#define SPEED_MIN 0.5

enum motor_mode
{
    MOTOR_NORMAL,MOTORSOFT,MODE_STOP
};
void Motor_Control();
// void Direction_Control();
void Speed_Control(float left_speed,float right_speed);//速度控制
void Angle_Control(float current_err);//舵机角度控制
void SpeedUp_Control(void);//加速控制
void SpeedDown_Control(void);//减速控制

#endif
