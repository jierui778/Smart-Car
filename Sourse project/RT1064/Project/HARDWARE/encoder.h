#include "zf_common_headfile.h"
#ifndef _ENCODER_H
#define _ENCODER_H

//#define PIT_CH (PIT_CH0) // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
//#define PIT_PRIORITY (PIT_IRQn)//

#define ENCODER_L (QTIMER1_ENCODER1)          // 正交编码器对应的编码器接口
#define ENCODER_R (QTIMER1_ENCODER2)          // 正交编码器对应的编码器

#define ENCODER_L_A (QTIMER1_ENCODER1_CH1_C0) // A相对应的引脚
#define ENCODER_L_B (QTIMER1_ENCODER1_CH2_C1) // B相对应的引脚


#define ENCODER_R_A (QTIMER1_ENCODER2_CH1_C2)  // A相对应的引脚
#define ENCODER_R_B (QTIMER1_ENCODER2_CH2_C24) // B相对应的引脚

extern int32 encoder_l_data, encoder_r_data; // 编码器的计数值

extern void Encoder_Init(void);                     // 初始化编码器
void Encoder_Update(int32 *encoder_l_data, int32 *encoder_r_data); // 更新编码器计数值
// void pit_handler (void);

#endif