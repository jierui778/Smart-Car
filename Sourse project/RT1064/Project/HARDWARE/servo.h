#ifndef _SERVO_H_
#define _SERVO_H_

void Servo_Init();
void Servo_SetPwm(int pwm);//设置舵机的PWM值
void Servo_SetAngle(int angle);//设置舵机的角度值

#endif //