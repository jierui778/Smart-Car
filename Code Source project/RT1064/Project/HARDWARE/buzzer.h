#include "zf_common_headfile.h"
#ifndef _BUZZER_H
#define _BUZZER_H

#define BEEP (B11)//定义蜂鸣器引脚



void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);

#endif