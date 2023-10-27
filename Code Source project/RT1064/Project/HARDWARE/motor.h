#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_headfile.h"


#define MOTOR_PWML PWM2_MODULE3_CHA_D2//定义左电机PWM引脚D2
#define MOTOR_PWMR PWM2_MODULE3_CHB_D3//定义右电机PWM引脚D3
#define MOTOR_DIRL D0//定义左电机方向引脚D0
#define MOTOR_DIRR D1//定义右电机方向引脚D


#define PWM_MAX 7000//对输出PWM进行限幅



extern uint32 speed;

void Motor_Init(void);//电机引脚初始化
void Motor_SetPwmL(int16 Speed);//设置左电机的PWM
void Motor_SetPwmR(int16 Speed);//设置右电机的PWM
void Motor_SetPwmAll(int16 left_speed,int16 right_speed);//设置左右电机的速度
#endif