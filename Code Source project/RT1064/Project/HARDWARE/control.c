#include "control.h"
#include "pid.h"
#include "motor.h"
#include "servo.h"
#include "isr.h"
#include "encoder.h"
// extern tPid pidMotor1Speed;
// extern tPid pidMotor2Speed;
float Speed_Vary=0.3;//单次速度的增值（用来加速减速）




/**
 * @brief 控制初始化函数，调用电机初始化函数
 *
 */
void Control_Init()
{
    PID_init();
    Motor_Init();
}

/**
 * @brief 根据摄像头给的中线误差值进行转向，调节角度
 * @param current_err 与中线的误差值
 */
void Angle_Control(float current_err)
{
    if(Servo_Flag==1)
    {
        Servo_SetAngle(PositionPID_Servo(current_err,TraceTurnPdate));//PID里面会有限幅
        Servo_Flag=0;
    }
}


/**
 * @brief 设置左右电机的速度
 *
 * @param left_speed 左电机的速度
 * @param right_speed 右电机的速度
 */
void Speed_Control(float left_speed,float right_speed)
{
    pidMotor1Speed.target_val=left_speed;
	pidMotor2Speed.target_val=right_speed;
	Motor_SetL(IncrementPID_Speed(&pidMotor1Speed,speed_left));
	Motor_SetR(IncrementPID_Speed(&pidMotor2Speed,speed_right));
}

/**
 * @brief 加速函数，用于增加电机的目标速度，上坡用
 *
 */
void SpeedUp_Control(void)
{
    if(Speed_Vary<=Max_Speed)//限制最大速度
    {
        pidMotor1Speed.target_val+=Speed_Vary;
        pidMotor2Speed.target_val+=Speed_Vary;
    }
    Motor_SetL(IncrementPID_Speed(&pidMotor1Speed,pidMotor1Speed.target_val));
    Motor_SetR(IncrementPID_Speed(&pidMotor2Speed,pidMotor2Speed.target_val));
}

/**
 * @brief 减速函数，用于减小电机的目标速度，下坡用
 *
 */
void SpeedDown_Control(void)
{
    if(Speed_Vary>=Min_Speed)//限制最小速度
    {
        pidMotor1Speed.target_val-=Speed_Vary;
        pidMotor2Speed.target_val-=Speed_Vary;
    }
    Motor_SetL(IncrementPID_Speed(&pidMotor1Speed,pidMotor1Speed.target_val));
    Motor_SetR(IncrementPID_Speed(&pidMotor2Speed,pidMotor2Speed.target_val));
}



