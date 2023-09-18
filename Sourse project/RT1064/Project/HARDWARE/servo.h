#include "zf_common_headfile.h"
#ifndef _SERVO_H_
#define _SERVO_H_

#define SERVO_CH (PWM4_MODULE2_CHA_C30)
#define SERVO_FRE 50//舵机频率:50Hz


void Servo_Init();
void Servo_SetPwm(int32 pwm);//设置舵机的PWM值
void Servo_SetAngle(float angle);//设置舵机的角度值

#endif //