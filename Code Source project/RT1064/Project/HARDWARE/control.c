#include "control.h"
#include "pid.h"
#include "motor.h"
#include "servo.h"
#include "isr.h"
#include "encoder.h"
// extern tPid pidMotor1Speed;
// extern tPid pidMotor2Speed;
float Speed_Vary = 0.3; // 单次速度的增值（用来加速减速）

/**
 * @brief 控制初始化函数，调用电机初始化函数
 *
 */
void Control_Init()
{
        //     PID_Init();
        Motor_Init();
}

sMotor_Info Motor_Info[2]={0};
sServo_Info ServoInfo = {0};//舵机驱动结构体


// /**
//  * @brief 根据摄像头给的中线误差值进行转向，调节角度
//  * @param current_err 与中线的误差值
//  */
// void Angle_Control(float current_err) // 待修改
// {
//     // static float Angle_

//     // 根据中线误差值进行转向，待修改
// }

/**
 * @brief 设置左右电机的速度
 *
 * @param left_speed 左电机的速度
 * @param right_speed 右电机的速度
 */
void Speed_Control(float left_speed, float right_speed)
{
        // pidMotor1Speed.target_val=left_speed;
        // pidMotor2Speed.target_val=right_speed;
        float Speed_L, Speed_R;

        Speed_L = Encoder_L_Data / WHEEL_COUNT * WHEEL_C / 0.01; // 计算左轮速度(读取编码器的周期为10ms)
        Speed_R = Encoder_R_Data / WHEEL_COUNT * WHEEL_C / 0.01; // 计算右轮速度
        float Deviation_L = left_speed - Speed_L;                // 偏差=目标速度-实际速度
        float Deviation_R = right_speed - Speed_R;               //

        //    Motor_SetPwmL(MINMAX(IncrementPID(Deviation_L, SpeedParam), -PWM_MAX, PWM_MAX));  // 将累加增量进行限幅输出
        //    Motor_SetPwmR(MINMAX(IncrementPID2(Deviation_R, SpeedParam), -PWM_MAX, PWM_MAX)); // 将累加增量进行限幅输出
}



//在电机pid之前进行一下阿克曼运算就行
void ackermann(float err, float v,sMotor_Info *Motor_Info_L,sMotor_Info *Motor_Info_R)
{
    // 差速计算
    float R = wheelbase / (2 * (1 - (track * track) / (4 * wheelbase * wheelbase))); // 转弯半径
    float delta = (track / 2) - (length * err) / 2;                                  // 左右轮距离车辆中心的水平偏移量

    // 漂移调整
    float drift_offset = drift_factor * v * v / R; // 根据速度和转弯半径计算漂移偏移量
    Motor_Info_L->TargetSpeed = v * (1 - delta / R - drift_offset) * k;
    Motor_Info_R->TargetSpeed = v * (1 + delta / R + drift_offset) * k; 
}

/**
//  * @brief 加速函数，用于增加电机的目标速度，上坡用
//  *
//  */
// void SpeedUp_Control(void)
// {
//     if(Speed_Vary<=SPEED_MAX)//限制最大速度
//     {
//         pidMotor1Speed.target_val+=Speed_Vary;
//         pidMotor2Speed.target_val+=Speed_Vary;
//     }
//     Motor_SetL(IncrementPID_Speed(&pidMotor1Speed,pidMotor1Speed.target_val));
//     Motor_SetR(IncrementPID_Speed(&pidMotor2Speed,pidMotor2Speed.target_val));
// }

// /**
//  * @brief 减速函数，用于减小电机的目标速度，下坡用
//  *
//  */
// void SpeedDown_Control(void)
// {
//     if(Speed_Vary>=SPEED_MIN)//限制最小速度
//     {
//         pidMotor1Speed.target_val-=Speed_Vary;
//         pidMotor2Speed.target_val-=Speed_Vary;
//     }
//     Motor_SetL(IncrementPID_Speed(&pidMotor1Speed,pidMotor1Speed.target_val));
//     Motor_SetR(IncrementPID_Speed(&pidMotor2Speed,pidMotor2Speed.target_val));
// }

/**
 * @brief 根据角度误差,通过舵机和电机进行调整
 *
 * @param current_err 角度误差（-90~90）
 */
void Angle_Control(float current_err)
{

        float Deviation = -current_err; // 理想情况应该为误差角度为0
        float x;
        //        x= PositionPID(Deviation, TraceTurnParam);

        Servo_SetAngle(x); // 根据误差角度进行转
                           //    MINMAX(IncrementPID(Deviation_L, SpeedParam), -PWM_MAX, PWM_MAX)
        ips200_show_int(100, 100, x, 4);
        ips200_show_int(100, 120, Deviation, 4);
}

void SpeedUp_Control()
{
<<<<<<< HEAD


=======
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
}