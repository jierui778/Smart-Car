/**
 ******************************************************************************
 * @文件名 pid.c
 * @文件说明：本文件为PID相关函数文件
 ******************************************************************************/
#include "pid.h"
// #include "motor.h"
// tPid pidMotor1Speed;//电机1速度PID闭环参数
// tPid pidMotor2Speed;//电机2速度PID闭环参数
PID SpeedParam;
PID TraceTurnParam;
/**
 * @brief 初始化两个电机的PID控制器。
 *
 * 此函数为两个电机的PID控制器设置初始值。
 * 它将实际值和目标值设置为0，将误差和误差总和设置为0，
 * 将比例、积分和微分增益设置为0。
 *
 */
void PID_Init(void)
{
    SpeedParam.kp = 100;
    SpeedParam.ki = 5;
    SpeedParam.kd = 0;

    TraceTurnParam.kd = 0;
    TraceTurnParam.ki = 0;
    TraceTurnParam.kp = 0;

}


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
int IncrementPID(float Deviation, PID pid)
{
    float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
    static float Current_Bias, Last_Bias, Lastlast_Bias;
    int32 Result;                                                      // 定义静态变量
    Current_Bias = Deviation;                                    // 当前偏差
    // Integral_bias += Bias;

    Result = (int32)(Increment_KP * (Current_Bias - Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2 * Last_Bias + Lastlast_Bias)); // 增量式PID控制器
    Lastlast_Bias = Last_Bias;                                                                                                                        // 上上一次偏差保存
    Last_Bias = Current_Bias;                                                                                                                         // 上一次偏差保存
    return Result;                                                                                                                                    // 返回结果
}
int IncrementPID2(float Deviation, PID pid)
{
    float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
    static float Current_Bias, Last_Bias, Lastlast_Bias;
    int32 Result;                                                      // 定义静态变量
    Current_Bias = Deviation;                                    // 当前偏差
    // Integral_bias += Bias;
    Result = (int32)(Increment_KP * (Current_Bias - Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2 * Last_Bias + Lastlast_Bias)); // 增量式PID控制器
    Lastlast_Bias = Last_Bias;                                                                                                                        // 上上一次偏差保存
    Last_Bias = Current_Bias;                                                                                                                         // 上一次偏差保存
    return Result;                                                                                                                                    // 返回结果
}





