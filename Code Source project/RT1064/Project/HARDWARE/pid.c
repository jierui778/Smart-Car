/**
 ******************************************************************************
 * @文件名 pid.c
 * @文件说明：本文件为PID相关函数文件
 ******************************************************************************/
#include "pid.h"
// #include "motor.h"
// tPid pidMotor1Speed;//电机1速度PID闭环参数
// tPid pidMotor2Speed;//电机2速度PID闭环参数
PID SpeedData;
PID TraceTurnData;
/**
 * @brief 初始化两个电机的PID控制器。
 *
 * 此函数为两个电机的PID控制器设置初始值。
 * 它将实际值和目标值设置为0，将误差和误差总和设置为0，
 * 将比例、积分和微分增益设置为0。
 *
 */
void PID_init(void)
{
    SpeedData.kd = -40;
    SpeedData.ki = -5;
    SpeedData.kp = 0;

    TraceTurnData.kd = 2;
    TraceTurnData.ki = 0;
    TraceTurnData.kp = 0.6;
    // pidMotor1Speed.actual_val=0.0;
    // pidMotor1Speed.target_val=0.00;
    // pidMotor1Speed.err=0.0;
    // pidMotor1Speed.err_last=0.0;
    // pidMotor1Speed.err_sum=.0;
    // pidMotor1Speed.Kp=50;
    // pidMotor1Speed.Ki=5;
    // pidMotor1Speed.Kd=-1;

    // pidMotor2Speed.actual_val=0.0;
    // pidMotor2Speed.target_val=0.00;
    // pidMotor2Speed.err=0.0;
    // pidMotor2Speed.err_last=0.0;
    // pidMotor2Speed.err_sum=0.0;
    // pidMotor2Speed.Kp=50;
    // pidMotor2Speed.Ki=5;
    // pidMotor2Speed.Kd=-1;
}
// // 速度PID参数
// PID SpeedData =
//     {
//         -40, // 赋值比例值
//         -5,  // 赋值积分值
//         0    // 赋值微分值
// };
// // 转向PID参数
// PID TraceTurnPdate =
//     {
//         2,  // 赋值比例值
//         0,  // 赋值积分值
//         0.6 // 赋值微分值
// };

/*@brief:位置式PID控制器1
 *        [in] deviation:偏差
 * @return:调节电机的Result
 */
int PositionPID(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
    int Result;
    static float Bias, Integral_bias, Last_Bias;
    Bias = deviation;
    Integral_bias += Bias;
    Result = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
    Last_Bias = Bias; // 上一次偏差保存
    return Result;
}

/*@brief:位置式PID控制器2
 *        [in] deviation:偏差
 * @return:调节电机的Result
 */
int PositionPID2(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
    int Result;
    static float Bias, Integral_bias, Last_Bias;
    Bias = deviation;
    Integral_bias += Bias;
    Result = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
    Last_Bias = Bias;
    return Result;
}
/**
 * @brief 增量式PID控制器
 *
 * @param SetValue 期望值
 * @param ActualValue 实际值
 * @param pid PID参数
 * @return int 调节电机的Result
 */
int IncrementPID(float SetValue, float ActualValue, PID pid)
{
    float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
    static float Current_Bias, Last_Bias, Lastlast_Bias, Result; // 定义静态变量
    Current_Bias = SetValue - ActualValue;                       // 当前偏差
    // Integral_bias += Bias;
    Result = Increment_KP * (Current_Bias - Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2 * Last_Bias + Lastlast_Bias); // 增量式PID控制器
    Lastlast_Bias = Last_Bias;                                                                                                                        // 上上一次偏差保存
    Last_Bias = Current_Bias;                                                                                                                         // 上一次偏差保存
    return Result;                                                                                                                                    // 返回结果
}

// /**
//  * @brief 增量式PID控制器
//  *
//  * @param SetValue 期望值
//  * @param ActualValue 实际值
//  * @param pid PID参数
//  * @return int 调节电机的Result
//  */
// int IncrementPID_PWM(float *setspeed,float *actualspeed,PID pid)
// {
//     float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
//     static float Current_Bias, Last_Bias,Lastlast_Bias,Result;//定义静态变量
//     Current_Bias = *setspeed - *actualspeed;//当前偏差
//     // Integral_bias += Bias;
//     Result = Increment_KP * (Current_Bias-Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2*Last_Bias+Lastlast_Bias);//增量式PID控制器
//     Lastlast_Bias = Last_Bias;//上上一次偏差保存
//     Last_Bias = Current_Bias;//上一次偏差保存
//     return Result;//返回结果
// }

// /**
//  * @brief PID控制器实现函数
//  *
//  * @param pid PID控制器结构体指针
//  * @param actual_val 实际值
//  * @return float PID控制器输出值
//  */
// float IncrementPID_Speed(tPid * pid,float actual_val)
// {
//     pid->actual_val = actual_val;//传递真实值
//     pid->err = pid->target_val - pid->actual_val;////当前误差=目标值-真实值
//     pid->err_sum += pid->err;//误差累计值 = 当前误差累计和
//     //使用PID控制 输出 = Kp*当前误差  +  Ki*误差累计值 + Kd*(当前误差-上次误差)
//     pid->actual_val = pid->Kp*pid->err + pid->Ki*pid->err_sum + pid->Kd*(pid->err - pid->err_last);
//     //保存上次误差: 这次误差赋值给上次误差
//     pid->err_last = pid->err;

//     return pid->actual_val;
// }
// /*@brief:位置式PID控制器（用于舵机控制转向）
//  *        [in] deviation:偏差角度
//  * @return:调节舵机的PWM_result
//  */
// int PositionPID_Servo(float deviation, PID pid)
// {
//     float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
//     int Result;
//     static float Bias, Integral_bias, Last_Bias;
//     Bias = deviation;
//     Integral_bias += Bias;
//     Result = Servo_Mid+Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
//     Last_Bias = Bias; // 上一次偏差保存

//     //舵机限幅（防止转向过大）
//     if(Result>Servo_Max)
//     {
//         Result=Servo_Max;
//     }
//     if(Result<=Servo_Min)
//     {
//         Result=Servo_Min;
//     }
//     return Result;
// }
