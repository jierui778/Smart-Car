/*
 * garage.h
 *
 *  Created on: 2023年1月12日
 *      Author: HQC
 */
#ifndef IMAGE_GARAGE_H_
#define IMAGE_GARAGE_H_

#include "zf_common_headfile.h"
#define ZIBRA_THRES 200
#define GARAGE_IN_SERVO_ANGLE 0 // 进库时舵机角度
#define GARAGE_OUT_SERVO_ANGLE 0 // 出库时舵机角度

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
#define LLL 60
extern enum garage_type_e garage_type;
// extern uint8 touch_boundary0; // 左边线走到图像左边界
// extern uint8 touch_boundary1; // 右边线走到图像右边界
// extern int16_t garage_cnt ;

// extern int g_far_ipts0[LLL][2];           //种子巡线
// extern int g_far_ipts1[LLL][2];
// extern int g_far_ipts0_num, g_far_ipts1_num;
// extern float g_far_rpts0s[LLL][2];        //逆透视等距采样
// extern float g_far_rpts1s[LLL][2];
// extern int g_far_rpts0s_num, g_far_rpts1s_num;

// void check_garage(void );
// void run_garage(void );                     //所有与库有关的函数，入口
// void out_garage(void );                     //出库，只会运行在出发一段
// void garage_farline_right(void );           //路过左车库，循右边的远线
// void garage_farline_left(void );            //路过右车库，循左边的远线

void Garage_Check(void);
void Garage_Run(void);
#endif /* IMAGE_GARAGE_H_ */
