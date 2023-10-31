#include "zf_common_headfile.h"
#ifndef _SERVO_H_
#define _SERVO_H_

<<<<<<< HEAD
#define SERVO_CH (PWM4_MODULE2_CHA_C30)//舵机PWM控制引脚
#define SERVO_FRE 50 // 舵机频率:50Hz
#define ERROR (6)    // 机械零点值误差，视具体调节修改



#define SERVO_ANGLE_MAX 30 // 防止前轮转动角度过大卡死车轮，限制舵机转动角度
#define SERVO_ANGLE_MIN -30


=======
#define SERVO_ANGLE_MAX 30 // 防止前轮转动角度过大卡死车轮，限制舵机转动角度
#define SERVO_CH (PWM4_MODULE2_CHA_C30)
#define SERVO_FRE 200 // 舵机频率:50Hz
#define ERROR (-14)   // 机械零点值误差，视具体调节修改

// #define SERVO_ANGLE_MAX 15//防止前轮转动角度过大卡死车轮，限制舵机转动角度
////#define SERVO_ANGLE_MIN -15//防止前轮转动角度过大卡死车轮，限制舵机转动角度
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5

void Servo_Init();                // 舵机初始化
void Servo_SetAngle(float Angle); // 设置舵机的角度值，舵机角度范围-90~90(实际上舵机打角需要进行限制)

#endif
