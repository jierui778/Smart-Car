// /**
//  ******************************************************************************
//  * @文件名 pid.c
//  * @文件说明：本文件为PID相关函数文件
//  ******************************************************************************/
#include "pid.h"
#include "math.h"
#include "servo.h"
#include "motor.h"
#include "control.h"
float sPidInfo[3][5] = { // IncrPID{Kp,Ki,Kd,MaxOutput}
	{0.14, 0.12, 0, 3000, 0},

    {0.14, 0.12, 0, 3000, 0},
    
    {1.0,0.02,0,0,14.8}};

	/*--p过荡 0.7   初始为0.5*/
sPosiPID_Info  Servo_PIDInfo={0};
sIncrPID_Info  Motor_PIDInfo[2] = {0};
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
	for(uint8 i=0;i<2;i++)
	{
		Motor_PIDInfo[i].Kp=sPidInfo[i][0];
		Motor_PIDInfo[i].Ki=sPidInfo[i][1];
		Motor_PIDInfo[i].Kd=sPidInfo[i][2];
		Motor_PIDInfo[i].MaxOutput=sPidInfo[i][3];
		Motor_PIDInfo[i].Err=0;
		Motor_PIDInfo[i].LastErr=0;
		Motor_PIDInfo[i].Output=0;
	}

    Servo_PIDInfo.Kp = sPidInfo[2][0];
    Servo_PIDInfo.Ki = sPidInfo[2][1];
    Servo_PIDInfo.Kd = sPidInfo[2][2];
	Servo_PIDInfo.MaxIntegral_Err=sPidInfo[2][3];
    Servo_PIDInfo.MaxOutput = sPidInfo[2][4];
    Servo_PIDInfo.Err = 0;
    Servo_PIDInfo.LastErr = 0;
    Servo_PIDInfo.Integral_Err = 0;
    Servo_PIDInfo.Output = 0;

<<<<<<< HEAD
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
    static float Current_Bias, Integral_bias, Last_Bias;
    Current_Bias = deviation;                                                                             // 输入偏差
    Integral_bias += Current_Bias;                                                                        // 累加误差
    Result = Position_KP * Current_Bias + Position_KI * Integral_bias + Position_KD * (Current_Bias - Last_Bias); // 位置式PID控制器
    Last_Bias = Current_Bias;                                                                             // 上一次偏差保存
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
//     static float Current_Bias, Integral_bias, Last_Bias;
//     Current_Bias = deviation;
//     Integral_bias += Current_Bias;
//     Result = Position_KP * Current_Bias + Position_KI * Integral_bias + Position_KD * (Current_Bias - Last_Bias);
//     Last_Bias = Current_Bias;
//     return Result;
// }
=======
}
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
/**
 * @brief 增量式PID控制器
 *
 * @param IncrPID 增量式PID参数
 * @param MotorInfo 电机输出参数
 */
void IncrPID(sIncrPID_Info *IncrPID, sMotor_Info *MotorInfo)
{
<<<<<<< HEAD
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
    // Integral_bias += Current_Bias;
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
=======
	// 更新误差
	IncrPID->LastLastErr = IncrPID->LastErr;				  // 上次误差更新为上上次误差
	IncrPID->LastErr = IncrPID->Err;						  // 误差更新为上次误差
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
	PosiPID->LastErr = PosiPID->Err;														// 误差更新为上次误差
	PosiPID->Err = *test;																	// 传入新的误差
	PosiPID->Integral_Err += PosiPID->Err;													// 积分误差
	PosiPID->Integral_Err = PIDInfo_Limit(PosiPID->Integral_Err, PosiPID->MaxIntegral_Err); // 积分限幅，否者积分会超出变量最大取值范围
	// 计算输出
	PosiPID->Output = PosiPID->Kp * PosiPID->Err + PosiPID->Ki * PosiPID->Integral_Err + PosiPID->Kd * (PosiPID->Err - PosiPID->LastErr);
	// 限制PID输出
	PosiPID->Output = PIDInfo_Limit(PosiPID->Output, PosiPID->MaxOutput);
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
}

/**
 * @brief 动态位置式PID控制器
 *
 * @param Deviation 偏差
 * @param pid pid参数
 */
void Dynamic_PositionPID(float Deviation, PID pid)
{
    static float Bias_Last, Bias_Integral, Current_Bias;
    int16 Result;
    Bias_Integral += Bias_Current;
    Bias_Last = Bias_Current;

    return Result;
}

<<<<<<< HEAD
// 动态PID听说性能较普通PID好，时间充裕再进行补充
=======

/*------------下面的都没有用，不用动----------------*/



tPid pidMotorLeftSpeed;	 // 左电机的闭环PID
tPid pidMotorRightSpeed; // 右电机的闭环PID
tPid pidServo;			 // 舵机PID

void PID_Init_New()
{
	pidMotorLeftSpeed.actual_val = 0.0;
	pidMotorLeftSpeed.target_val = 0.00;
	pidMotorLeftSpeed.err = 0.0;
	pidMotorLeftSpeed.err_last = 0.0;
	pidMotorLeftSpeed.err_sum = 0.0;
	pidMotorLeftSpeed.Kp = 0;
	pidMotorLeftSpeed.Ki = 0;
	pidMotorLeftSpeed.Kd = 0;

	pidMotorRightSpeed.actual_val = 0.0;
	pidMotorRightSpeed.target_val = 0.00;
	pidMotorRightSpeed.err = 0.0;
	pidMotorRightSpeed.err_last = 0.0;
	pidMotorRightSpeed.err_sum = 0.0;
	pidMotorRightSpeed.Kp = 0;
	pidMotorRightSpeed.Ki = 0;
	pidMotorRightSpeed.Kd = 0;

	pidServo.actual_val = 0.0;
	pidServo.target_val = 0.00;
	pidServo.err = 0.0;
	pidServo.err_last = 0.0;
	pidServo.err_sum = 0.0;
	pidServo.Kp = 0.2;
	pidServo.Ki = 0;
	pidServo.Kd = 0;
}

/*
简介：比例调节控制函数
原理：通过比例系数Kp，对每次求出的误差值进行比例赋值然后返回比例后的结果
注意：无
*/
float P_Realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // 传递真实值
	pid->err = pid->target_val - pid->actual_val; // 求出误差值
	pid->actual_val = pid->Kp * pid->err;

	return pid->actual_val;
}

/*
简介：比例积分控制函数
原理&注意：无
*/
float PI_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;
	pid->err = pid->target_val - pid->actual_val;
	pid->err_sum += pid->err; // 误差累积
	pid->actual_val = pid->Kp * pid->err + pid->err_sum;
	return pid->actual_val;
}

/*
简介：比例积分微分调节
原理&注意：无
*/
float PID_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;
	pid->err = pid->target_val - pid->actual_val; // 当前误差=目标值-真实值
	pid->err_sum += pid->err;					  // 误差累加
	pid->actual_val = pid->Kp * pid->err + pid->err_sum * pid->Ki + pid->Kd * (pid->err - pid->err_last);

	pid->err_last = pid->err; // 误差赋值PID_realize(&pidMotorLeftSpeed,Motor_LeftSpeed)

	return pid->actual_val;
}

void Servo_PidSetSpeed(float Target_Angle)
{
	pidServo.target_val = Target_Angle;
	Servo_SetAngle(PID_realize(&pidServo, Target_Angle));
}
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
