#include "servo.h"
#include "zf_common_headfile.h"
#include "pid.h"
#include "control.h"

/**
 * @brief 设置舵机角度
 *
 * @param Angle 角度值，范围-90~90
 */
void Servo_SetAngle(float Angle) // 具体换算关系看舵机位置调整
{
    float pwm;
    if(Angle>SERVO_ANGLE_MAX)
    {
        Angle=SERVO_ANGLE_MAX;
    }
    if(Angle<-SERVO_ANGLE_MAX)
    {
        Angle=-SERVO_ANGLE_MAX;
    }
    
        
    pwm = (float)(PWM_DUTY_MAX / (1000.0 / (float)SERVO_FRE) * (0.5 + (float)(Angle-ERROR) / 90.0));
    pwm_set_duty(SERVO_CH, MINMAX((uint32)pwm,0,PWM_DUTY_MAX));

    // FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, pwm);
}

/**
 * @brief 初始化舵机
 *
 */
void Servo_Init()
{
    pwm_init(SERVO_CH, SERVO_FRE, 0);
}

