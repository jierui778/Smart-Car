#include "control.h"
#include "pid.h"
#include "motor.h"
#include "servo.h"
#include "isr.h"
extern tPid pidMotor1Speed;
extern tPid pidMotor2Speed;
/**
 * @brief 控制初始化函数，调用电机初始化函数
 * 
 */
void Control_Init()
{
    Motor_Init();
}

/**
 * @brief 根据摄像头给的中线误差值进行转向，调节角度
 * @param current_err 与中线的误差值
 */
void Control_Angle(float current_err)
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
void Control_Setspeed(float left_speed,float right_speed)
{
    if(Motor_Flag==1)
    {
        pidMotor1Speed.target_val=left_speed;
        pidMotor2Speed.target_val=right_speed;
        Motor_SetL(IncrementPID_Speed(&pidMotor1Speed,left_speed));
        Motor_SetR(IncrementPID_Speed(&pidMotor2Speed,right_speed));
        Motor_Flag=0;
    }
}
