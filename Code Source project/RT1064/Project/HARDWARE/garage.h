#ifndef IMAGE_GARAGE_H_
#define IMAGE_GARAGE_H_

#include "zf_common_headfile.h"

#define GARAGE_IN_SERVO_ANGLE 0  // 进库时舵机角度
#define GARAGE_OUT_SERVO_ANGLE 0 // 出库时舵机角度
#define GARAGE_DIS 1000
#define ZIBRA_THRES 200
enum garage_type_e
{
    GARAGE_NONE = 0, // 非车库模式
    GARAGE_OUT_LEFT,
    GARAGE_OUT_RIGHT, // 出库，陀螺仪转过45°，即出库完毕
    GARAGE_FOUND_LEFT,
    GARAGE_FOUND_RIGHT, // 发现车库，即斑马线+单侧L角点(未使用)
    GARAGE_IN_LEFT,
    GARAGE_IN_RIGHT, // 进库，发现车库后判断第几次，从而决定是否进库
    GARAGE_PASS_LEFT,
    GARAGE_PASS_RIGHT, // 不进库，发现车库后判断第几次，从而决定是否进库
    GARAGE_STOP,       // 进库完毕，停车
    GARAGE_NUM,
};
extern enum garage_type_e garage_type; // 枚举类型全局定义
void Garage_Check(void);
void Garage_Run(void);
#endif /* IMAGE_GARAGE_H_ */
