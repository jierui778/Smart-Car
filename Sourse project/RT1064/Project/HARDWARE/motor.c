#include "motor.h"

/*
简介：电机初始化函数。电机方向引脚的初始化，pwm通道的初始化，只需两路PWM即可驱动电机
*/
void Motor_Init(void)
{
    pwm_init(PWM_1, 10000, 0);
    pwm_init(PWM_2, 10000, 0);
    //pwm通道初始化，初始频率10000，占空比0
    gpio_init(DIR_1, GPO, 0, GPIO_PIN_CONFIG); 		//单片机端口D0 初始化DIR_1			GPIO
	gpio_init(DIR_2, GPO, 0, GPIO_PIN_CONFIG); 		//单片机端口D1 初始化DIR_2			GPIO
    //电机方向IO口初始化

    pwm_duty(PWM_1,0);
    pwm_duty(PWM_2,0);//两电机先不动
}



/**
 * @brief 设置左电机的速度
 *
 * @param Speed 速度值，范围-1000~1000
 */
void Motor_SetL(int16 Speed)
{
    if(Speed>0)
    {
        gpio_set(DIR_1, 0);
        pwm_duty(PWM_1,Speed);
    }
    else
    {
        gpio_set(DIR_1, 1);
        pwm_duty(PWM_1,-Speed);
    }
}
/**
 * @brief 设置右电机的速度
 *
 * @param Speed 速度值，范围-1000~1000
 */
 */
void Motor_SetR(int16 Speed)
{
    if(Speed>0)
    {
        gpio_set(DIR_1, 0);
        pwm_duty(PWM_1,Speed);
    }
    else
    {
        gpio_set(DIR_1, 1);
        pwm_duty(PWM_1,-Speed);
    }
}