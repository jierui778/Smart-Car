/**
 ******************************************************************************
 * @文件名 pid.c
 * @文件说明：本文件为PID相关函数文件
 ******************************************************************************/
#include "pid.h"
#include "motor.h"
// 速度PI参数
PID SpeedDate =
    {
        -40, // 赋值比例值
        -5,  // 赋值积分值
        0    // 赋值微分值
};
// 转向PD参数
PID TraceTurnPdate =
    {
        2,  // 赋值比例值
        0,  // 赋值积分值
        0.6 // 赋值微分值
};

/*@brief:位置式PID控制器1
 *        [in] deviation:偏差
 * @return:调节电机的PWM
 */
int PositionPID(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
    int Pwm;
    static float Bias, Integral_bias, Last_Bias;
    Bias = deviation;
    Integral_bias += Bias;
    Pwm = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
    Last_Bias = Bias; // 上一次偏差保存
    return Pwm;
}

/*@brief:位置式PID控制器2
 *        [in] deviation:偏差
 * @return:调节电机的PWM
 */
int PositionPID2(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
    int Pwm;
    static float Bias, Integral_bias, Last_Bias;
    Bias = deviation;
    Integral_bias += Bias;
    Pwm = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
    Last_Bias = Bias;
    return Pwm;
}
/*@brief:速度PID使用位置式PID控制器
 *        [in] deviation:偏差
 * @return:调节电机的PWM
 */
int PositionPIDToSpeed(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
    int Pwm;
    static float Bias, Integral_bias, Last_Bias, pwmKI = 0;
    Bias = deviation;
    pwmKI += Position_KI * Bias;

    //	if(pwmKI>MAX_MOTOR_PWM) Integral_bias=MAX_MOTOR_PWM/Position_KI;
    Pwm = Position_KP * Bias + pwmKI + Position_KD * (Bias - Last_Bias);
    Last_Bias = Bias;
    return Pwm;
}
/*@brief:速度PID使用位置式PID控制器，对积分值进行限幅
 *        [in] deviation:偏差
 * @return:调节电机的PWM
 */
int PositionPIDToSpeed2(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
    int Pwm;
    static float Bias, Integral_bias, Last_Bias, pwmKI = 0;
    Bias = deviation;
    pwmKI += Position_KI * Bias;
    //	if(pwmKI>MAX_MOTOR_PWM) Integral_bias=MAX_MOTOR_PWM/Position_KI;
    Pwm = Position_KP * Bias + pwmKI + Position_KD * (Bias - Last_Bias);
    Last_Bias = Bias;
    return Pwm;
}
