/**
 ******************************************************************************
 * @文件名 pid.c
 * @文件说明：本文件为PID相关函数文件
 ******************************************************************************/
#include "pid.h"
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

/**
 * @brief 位置式PID控制器
 *
 * @param deviation 偏差
 * @param pid PID参数
 * @return int 调节电机的Result
 */
int PositionPID(float deviation, PID pid)
{
    float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd; // PID参数
    int16 Result;
    static float Bias, Integral_bias, Last_Bias;
    Bias = deviation;                                                                             // 输入偏差
    Integral_bias += Bias;                                                                        // 累加误差
    Result = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias); // 位置式PID控制器
    Last_Bias = Bias;                                                                             // 上一次偏差保存
    return Result;
}

// /*@brief:位置式PID控制器2
//  *        [in] deviation:偏差
//  * @return:调节电机的Result
//  */
// int PositionPID2(float deviation, PID pid)
// {
//     float Position_KP = pid.kp, Position_KI = pid.ki, Position_KD = pid.kd;
//     float Result;
//     static float Bias, Integral_bias, Last_Bias;
//     Bias = deviation;
//     Integral_bias += Bias;
//     Result = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
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
int IncrementPID(float Deviation, PID pid)
{
    float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
    static float Current_Bias, Last_Bias, Lastlast_Bias;
    static int16 Result;      // 定义静态变量
    Current_Bias = Deviation; // 当前偏差

    Result += (int32)(Increment_KP * (Current_Bias - Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2 * Last_Bias + Lastlast_Bias)); // 增量式PID控制器//累加增量
    Lastlast_Bias = Last_Bias;                                                                                                                                  // 上上一次偏差保存
    Last_Bias = Current_Bias;                                                                                                                                   // 上一次偏差保存
    return Result;                                                                                                                                              // 返回结果
}
int IncrementPID2(float Deviation, PID pid)
{
    float Increment_KP = pid.kp, Increment_KI = pid.ki, Increment_KD = pid.kd;
    static float Current_Bias, Last_Bias, Lastlast_Bias;
    static int16 Result;      // 定义静态变量
    Current_Bias = Deviation; // 当前偏差
    // Integral_bias += Bias;
    Result += (int32)(Increment_KP * (Current_Bias - Last_Bias) + Increment_KI * Current_Bias + Increment_KD * (Current_Bias - 2 * Last_Bias + Lastlast_Bias)); // 增量式PID控制器//累加增量
    Lastlast_Bias = Last_Bias;                                                                                                                                  // 上上一次偏差保存
    Last_Bias = Current_Bias;                                                                                                                                   // 上一次偏差保存
    return Result;                                                                                                                                              // 返回结果
}

/*************************************************************************
 *  函数名称：float constrain_float(float amt, float low, float high)
 *  功能说明：pid位置式控制器输出
 *  参数说明：
 * @param    pid     pid参数
 * @param    error   pid输入误差
 *  函数返回：PID输出结果
 *  修改时间：2020年4月1日
 *  备    注：
//  *************************************************************************/
// float PidLocCtrl(pid_param_t *pid, float error)
// {
//     /* 累积误差 */
//     pid->integrator += error;

//     /* 误差限幅 */
//     constrain_float(pid->integrator, -pid->imax, pid->imax);

//     pid->out_p = pid->kp * error;
//     pid->out_i = pid->ki * pid->integrator;
//     pid->out_d = pid->kd * (error - pid->last_error);

//     pid->last_error = error;

//     pid->out = pid->out_p + pid->out_i + pid->out_d;

//     return pid->out;
// }

// /**************************************************************************函数名称：float constrain_float(float amt, float low, float high) * 功能说明：pid增量式控制器输出 *参数说明： *@param pid pid参数 *@param error pid输入误差 *函数返回：PID输出结果 注意输出结果已经包涵了上次结果 *修改时间：2020年4月1日 *备 注： ************************************************************************* /
//     float PidIncCtrl(pid_param_t *pid, float error)
// {

//     pid->out_p = pid->kp * (error - pid->last_error);
//     pid->out_i = pid->ki * error;
//     pid->out_d = pid->kd * ((error - pid->last_error) - pid->last_derivative);

//     pid->last_derivative = error - pid->last_error;
//     pid->last_error = error;

//     pid->out += pid->out_p + pid->out_i + pid->out_d;

//     return pid->out;
// }

/**
 * @brief 动态增量式PID控制器
 *
 * @param Deeviation 误差
 * @param pid PID参数
 * @return int 调节Result
 */
int Dynamic_IncrementPID(float Deeviation, PID pid)
{
    int16 Result;

    return Result;
}

/**
 * @brief 动态增量式PID控制器
 *
 * @param Deviation 偏差
 * @param pid pid参数
 */
void Dynamic_IncrementPID(float Deviation, PID pid)
{
    int16 Result;

    return Result;
}

// 动态PID听说性能较普通PID好，时间充裕再进行补充