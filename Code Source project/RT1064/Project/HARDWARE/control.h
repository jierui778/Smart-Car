#include "zf_common_headfile.h"
#ifndef _CONTROL_H
#define _CONTROL_H

#define Max_Speed 3
#define Min_Speed 0.5
void Motor_Control();
void Direction_Control();
void Control_Setspeed(float left_speed,float right_speed);
void Control_SpeedUp(void);
void Control_SpeedDown(void);

#endif
