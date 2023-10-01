#include "zf_common_headfile.h"
#ifndef _SERVO_H_
#define _SERVO_H_

#define SERVO_CH (PWM4_MODULE2_CHA_C30)
#define SERVO_FRE 50//舵机频率:50Hz
#define ERROR 6//机械零点值误差，视具体调节修改

void Servo_Init();

void Servo_SetAngle(float Angle);//设置舵机的角度值，舵机角度范围-90~90

#endif