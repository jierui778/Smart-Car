/**
 ******************************************************************************
 * @文件名 pid.c
 * @文件说明：本文件为PID相关函数文件
 ******************************************************************************/
#include "pid.h"
#include "motor.h"
// 速度PID参数
PID SpeedDate =
    {
        -40, // 赋值比例值
        -5,  // 赋值积分值
        0    // 赋值微分值
};
// 转向PID参数
PID TraceTurnPdate =
    {
        2,  // 赋值比例值
        0,  // 赋值积分值
        0.6 // 赋值微分值
};



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
/*@brief:速度PID使用位置式PID控制器
 *        [in] deviation:偏差
 * @return:调节电机的Result
 */
// int PositionPIDToSpeed(float deviation, PID pid)
// {
//     float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
//     int Result;
//     static float Bias, Integral_bias, Last_Bias, ResultKI = 0;
//     Bias = deviation;
//     ResultKI += Position_KI * Bias;

//     //	if(ResultKI>MAX_MOTOR_Result) Integral_bias=MAX_MOTOR_Result/Position_KI;
//     Result = Position_KP * Bias + ResultKI + Position_KD * (Bias - Last_Bias);
//     Last_Bias = Bias;
//     return Result;
// }
// /*@brief:速度PID使用位置式PID控制器，对积分值进行限幅
//  *        [in] deviation:偏差
//  * @return:调节电机的Result
//  */
// int PositionPIDToSpeed2(float deviation, PID pid)
// {
//     float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
//     int Result;
//     static float Bias, Integral_bias, Last_Bias, ResultKI = 0;
//     Bias = deviation;
//     ResultKI += Position_KI * Bias;
//     //	if(ResultKI>MAX_MOTOR_Result) Integral_bias=MAX_MOTOR_Result/Position_KI;
//     Result = Position_KP * Bias + ResultKI + Position_KD * (Bias - Last_Bias);
//     Last_Bias = Bias;
//     return Result;
// }

/**
 * @brief 增量式PID控制器
 *
 * @param SetValue 期望值
 * @param ActualValue 实际值
 * @param pid PID参数
 * @return int 调节电机的Result
 */
int IncrementPID(float SetValue,float ActualValue,PID pid)
{
    float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
    static float Current_Bias, Last_Bias,Lastlast_Bias,Result;
    Current_Bias = SetValue - ActualValue;//当前偏差
    // Integral_bias += Bias;
    Result = Increment_KP * (Current_Bias-Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2*Last_Bias+Lastlast_Bias);//增量式PID控制器
    Lastlast_Bias = Last_Bias;//上上一次偏差保存
    Last_Bias = Current_Bias;//上一次偏差保存
    return Result;//返回结果
}