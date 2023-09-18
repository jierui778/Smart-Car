#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_headfile.h"


#define MOTOR_PWML PWM2_MODULE3_CHA_D2
#define MOTOR_PWMR PWM2_MODULE3_CHB_D3
#define MOTOR_DIRL D0
#define MOTOR_DIRR D1

void Motor_Init(void);
//void Motor_Control();
void Motor_SetL(int16 Speed);
void Motor_SetR(int16 Speed);

#endif