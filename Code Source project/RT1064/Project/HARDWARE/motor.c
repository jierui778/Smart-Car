#include "motor.h"
// DRV驱动方案
// PWM频率一般选择13-17kHZ，这里我们选着10kHZ

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

    pwm_set_duty(MOTOR_PWML, 800);
    pwm_set_duty(MOTOR_PWMR, 800); // 两电机先不动
}

/**
 * @brief 设置左电机的速度
 *
 * @param Speed 速度值，范围-1000~1000
 */
void Motor_SetPwmR(int16 Speed)
{
    if (Speed > 0)
    {
        gpio_set_level(MOTOR_DIRL, 1);
        pwm_set_duty(MOTOR_PWML, Speed); //
    }
    else
    {
        gpio_set_level(MOTOR_DIRL, 0);
        pwm_set_duty(MOTOR_PWML, -Speed);
    }
}
/**
 * @brief 设置右电机的速度
 *
 * @param Speed 速度值，范围-1000~1000
 */
void Motor_SetPwmL(int16 Speed)
{
    if (Speed > 0)
    {
        gpio_set_level(MOTOR_DIRR, 1);
        pwm_set_duty(MOTOR_PWMR, Speed);
    }
    else
    {
        gpio_set_level(MOTOR_DIRR, 0);
        pwm_set_duty(MOTOR_PWMR, -Speed);
    }
}

/**
 * @brief 设置左右电机的速度
 *
 * @param left_speed 左电机速度，范围为-1000到1000
 * @param right_speed 右电机速度，范围为-1000到1000
 */
void Motor_SetPwmAll(int16 left_speed, int16 right_speed)
{
    if (left_speed > 1000)
    {
        left_speed = 1000;
    }
    else if (left_speed < -1000)
    {
        left_speed = -1000;
    }
    if (right_speed > 1000)
    {
        right_speed = 1000;
    }
    else if (right_speed < -1000)
    {
        right_speed = -1000;
    }

    Motor_SetPwmL(left_speed);
    Motor_SetPwmR(right_speed);
}