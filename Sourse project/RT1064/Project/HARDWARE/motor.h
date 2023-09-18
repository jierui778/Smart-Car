#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_headfile.h"


#define PWM_1 PWM2_MODULE3_CHA_D2
#define PWM_2 PWM2_MODULE3_CHB_D3
#define DIR_1 D0
#define DIR_2 D1

void Motor_Init(void);
void Motor_COntrol();

#endif