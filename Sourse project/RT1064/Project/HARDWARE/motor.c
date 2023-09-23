#include "motor.h"
//DRV驱动方案
//PWM频率一般选择13-17kHZ，这里我们选着10kHZ



/**
 * @brief 电机引脚初始化
 *
 */
void Motor_Init(void)
{

    gpio_init(MOTOR_DIRL, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(MOTOR_DIRR, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    
    pwm_init(MOTOR_PWMR, 10000, 0);
    pwm_init(MOTOR_PWML, 10000, 0);

    // pwm_set_duty(MOTOR_PWML, 0);
    // pwm_set_duty(MOTOR_PWMR, 0); // 两电机先不动
}

/**
 * @brief 设置左电机的速度
 *
 * @param Speed 速度值，范围-1000~1000
 */
void Motor_SetL(int16 Speed)
{
    if (Speed > 0)
    {
        gpio_set_level(MOTOR_DIRL, 0);
        pwm_set_duty(MOTOR_PWML, Speed); //
    }
    else
    {
        gpio_set_level(MOTOR_DIRL, 1);
        pwm_set_duty(MOTOR_PWML, -Speed);
    }
}
/**
 * @brief 设置右电机的速度
 *
 * @param Speed 速度值，范围-1000~1000
 */
void Motor_SetR(int16 Speed)
{
    if (Speed > 0)
    {
        gpio_set_level(MOTOR_DIRR, 0);
        pwm_set_duty(MOTOR_PWMR, Speed);
    }
    else
    {
        gpio_set_level(MOTOR_DIRR, 1);
        pwm_set_duty(MOTOR_PWMR, -Speed);
    }
}