#include "servo.h"

/**
 * @brief 设置舵机角度
 *
 * @param angle 角度值，范围-90~90
 */
void Servo_SetAngle(int16 angle)//具体换算关系看舵机位置调整
{
    int pwm = (int)((angle/180.0 + 1) * 500);  //90度占空为1.5ms，0度占空1ms，180度占空为2ms
    // FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, pwm);
}




/**
 * @brief 初始化舵机
 *
 */
void Servo_Init()
{
    //放置舵机io口初始化代码
}