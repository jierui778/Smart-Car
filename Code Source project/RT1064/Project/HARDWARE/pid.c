// /**
//  ******************************************************************************
//  * @文件名 pid.c
//  * @文件说明：本文件为PID相关函数文件
//  ******************************************************************************/
#include "pid.h"
#include "math.h"
float sPidInfo[3][5] = { // IncrPID{Kp,Ki,Kd,MaxOutput}
    {0, 0, 0, 0,0},

    {0, 0, 0, 0,0},
    
    {0.003,0,0,10,15}};

sPosiPID_Info ServoInfo = {0};
// float PidInfo[2][4] = { // IncrPID{Kp,Ki,Kd,MaxOutput}
//     {0, 0, 0, 0},

//     {0, 0, 0, 0}};
/**
 * @brief PID限幅函数
 *
 * @param Value 输入值
 * @param MaxValue 最大值
 * @return float
 */
float PIDInfo_Limit(float Value, float MaxValue)
{
    if (fabs(Value) > MaxValue)
    {
        if (Value >= 0)
            Value = MaxValue;
        else
            Value = -MaxValue;
    }

    return Value;
}

/**
 * @brief 初始化PID控制器参数
 *
 */
void PID_Init(void)
{

    ServoInfo.Kp = sPidInfo[2][0];
    ServoInfo.Ki = sPidInfo[2][1];
    ServoInfo.Kd = sPidInfo[2][2];
    ServoInfo.MaxOutput = sPidInfo[2][3];
    ServoInfo.Err = 0;
    ServoInfo.LastErr = 0;
    ServoInfo.Integral_Err = 0;
    ServoInfo.Output = 0;

}
/**
 * @brief 增量式PID控制器
 *
 * @param IncrPID 增量式PID参数
 * @param MotorInfo 电机输出参数
 */
void IncrPID(sIncrPID_Info *IncrPID, sMotor_Info *MotorInfo)
{
    // 更新误差
    IncrPID->LastLastErr = IncrPID->LastErr;                  // 上次误差更新为上上次误差
    IncrPID->LastErr = IncrPID->Err;                          // 误差更新为上次误差
    IncrPID->Err = MotorInfo->TargetSpeed - MotorInfo->Speed; // 传入新的误差
    // 计算输出
    IncrPID->Output += IncrPID->Kp * (IncrPID->Err - IncrPID->LastErr) + IncrPID->Ki * IncrPID->Err + IncrPID->Kd * (IncrPID->Err - 2 * IncrPID->LastErr + IncrPID->LastLastErr);
    // 限制PID输出
    IncrPID->Output = PIDInfo_Limit(IncrPID->Output, IncrPID->MaxOutput);
}
/**
* @brief 位置式PID控制器
* @param PosPID 位置式PID参数
* @param MotorInfo 舵机输出参数
*/
void PosiPID(sPosiPID_Info *PosiPID, int *test)
{
   // 更新误差
   PosiPID->LastErr = PosiPID->Err;                          // 误差更新为上次误差
   PosiPID->Err = *test; // 传入新的误差
   PosiPID->Integral_Err += PosiPID->Err;                    // 积分误差
   PosiPID->Integral_Err = PIDInfo_Limit(PosiPID->Integral_Err, PosiPID->MaxIntegral_Err);//积分限幅，否者积分会超出变量最大取值范围
   // 计算输出
   PosiPID->Output = PosiPID->Kp * PosiPID->Err + PosiPID->Ki * PosiPID->Integral_Err + PosiPID->Kd * (PosiPID->Err - PosiPID->LastErr);
   // 限制PID输出
   PosiPID->Output = PIDInfo_Limit(PosiPID->Output, PosiPID->MaxOutput);
}       