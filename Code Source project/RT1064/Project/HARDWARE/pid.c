// /**
//  ******************************************************************************
//  * @文件名 pid.c
//  * @文件说明：本文件为PID相关函数文件
//  ******************************************************************************/
#include "pid.h"
#include "math.h"
#include "servo.h"
float sPidInfo[3][5] = { // IncrPID{Kp,Ki,Kd,MaxOutput}
    {0.0, 0, 0, 0,0},

    {0, 0, 0, 0,0},
    
    {0.8,0,0,10,16}};

	/*--p过荡 0.7*/
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
void PosiPID(sPosiPID_Info *PosiPID, float *test)
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

#include "pid.h"

tPid pidMotorLeftSpeed;   //左电机的闭环PID
tPid pidMotorRightSpeed;   //右电机的闭环PID
tPid pidServo;//舵机PID

void PID_Init_New()
{
	pidMotorLeftSpeed.actual_val=0.0;
	pidMotorLeftSpeed.target_val=0.00;
	pidMotorLeftSpeed.err=0.0;
	pidMotorLeftSpeed.err_last=0.0;
	pidMotorLeftSpeed.err_sum=0.0;
	pidMotorLeftSpeed.Kp=0;
	pidMotorLeftSpeed.Ki=0;
	pidMotorLeftSpeed.Kd=0;
	
	pidMotorRightSpeed.actual_val=0.0;
	pidMotorRightSpeed.target_val=0.00;
	pidMotorRightSpeed.err=0.0;
	pidMotorRightSpeed.err_last=0.0;
	pidMotorRightSpeed.err_sum=0.0;
	pidMotorRightSpeed.Kp=0;
	pidMotorRightSpeed.Ki=0;
	pidMotorRightSpeed.Kd=0;
	
	pidServo.actual_val=0.0;
	pidServo.target_val=0.00;
	pidServo.err=0.0;
	pidServo.err_last=0.0;
	pidServo.err_sum=0.0;
	pidServo.Kp=0.2;
	pidServo.Ki=0;
	pidServo.Kd=0;
}

/*
简介：比例调节控制函数
原理：通过比例系数Kp，对每次求出的误差值进行比例赋值然后返回比例后的结果
注意：无
*/
float P_Realize(tPid *pid,float actual_val)
{
	pid->actual_val=actual_val;//传递真实值
	pid->err=pid->target_val-pid->actual_val;//求出误差值
	pid->actual_val=pid->Kp*pid->err;
	
	return pid->actual_val;
}

/*
简介：比例积分控制函数
原理&注意：无
*/
float PI_realize(tPid *pid,float actual_val)
{
	pid->actual_val=actual_val;
	pid->err=pid->target_val-pid->actual_val;
	pid->err_sum +=pid->err;//误差累积
	pid->actual_val=pid->Kp*pid->err+pid->err_sum;
	return pid->actual_val;
}


/*
简介：比例积分微分调节
原理&注意：无
*/
float PID_realize(tPid*pid,float actual_val)
{
	pid->actual_val=actual_val;
	pid->err=pid->target_val-pid->actual_val;//当前误差=目标值-真实值
	pid->err_sum +=pid->err;//误差累加
	pid->actual_val=pid->Kp*pid->err+pid->err_sum*pid->Ki+pid->Kd*(pid->err-pid->err_last);
	
	pid->err_last=pid->err;//误差赋值PID_realize(&pidMotorLeftSpeed,Motor_LeftSpeed)
	
	return pid->actual_val;
}

void Servo_PidSetSpeed(float Target_Angle)
{
	pidServo.target_val=Target_Angle;
	Servo_SetAngle(PID_realize(&pidServo,Target_Angle));
}
