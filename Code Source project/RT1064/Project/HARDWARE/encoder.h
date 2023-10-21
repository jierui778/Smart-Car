#include "zf_common_headfile.h"
#ifndef _ENCODER_H
#define _ENCODER_H
#define ENCODER_L (QTIMER1_ENCODER1) // 正交编码器对应的编码器接口
#define ENCODER_R (QTIMER1_ENCODER2) // 正交编码器对应的编码器

#define ENCODER_L_A (QTIMER1_ENCODER1_CH1_C0) // A相对应的引脚
#define ENCODER_L_B (QTIMER1_ENCODER1_CH2_C1) // B相对应的引脚

#define ENCODER_R_A (QTIMER1_ENCODER2_CH1_C2)  // A相对应的引脚
#define ENCODER_R_B (QTIMER1_ENCODER2_CH2_C24) // B相对应的引脚

extern int32 Encoder_L_Data, Encoder_R_Data;                                         // 编码器的计数值，全局声明
extern int32 Encoder_L_Dis, Encoder_R_Dis;                                           // 读取编码器距离，全局声明
extern int8 DisCnt_Flag;                                                             // 是否开启距离计数标志位
#define Car_RoundCount 2300                                                          // 小车转一圈，对应计数器的计数值（得手动测）
extern void Encoder_Init(void);                                                      // 初始化编码器
void Encoder_Update(int32 *Data_L, int32 *Data_R, int32 *Data_L_D, int32 *Data_R_D); // 更新编码器计数值和积分距离
extern void Encoder_Int_Enable(void);                                                // 开启编码器积分
extern void Encoder_Int_Clear(void);                                                 // 清除编码器积分
// void pit_handler (void);

#endif