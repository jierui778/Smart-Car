#include "servo.h"
#include "zf_common_headfile.h"
/**
 * @brief 设置舵机角度
 *
 * @param angle 角度值，范围-90~90
 */
void Servo_SetAngle(float angle)//具体换算关系看舵机位置调整
{
    int pwm;
    pwm = (uint32)PWM_DUTY_MAX / (1000.0 / (float)SERVO_FRE) * (0.5 + (angle) / 90.0);
    // FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, pwm);
}



void Servo_SetPwm(int pwm)
{
    pwm_set_duty(SERVO_CH, pwm);
}


/**
 * @brief 初始化舵机
 *
 */
void Servo_Init()
{
    pwm_init(SERVO_CH, SERVO_FRE, 0);
}