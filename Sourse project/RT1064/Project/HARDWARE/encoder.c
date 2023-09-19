#include "encoder.h"

int32 encoder_l_data = 0, encoder_r_data = 0;


void Encoder_Init(void)
{
//    encoder_quad_init(ENCODER_L, ENCODER_L_A, ENCODER_L_B); // 初始化编码器模块与引脚 正交解码编码器模式
//    encoder_quad_init(ENCODER_R, ENCODER_R_A, ENCODER_R_B); // 初始化编码器模块与引脚 正交解码编码器模式

    pit_ms_init(PIT_CH, 1000);                                                   // 初始化 PIT1 为周期中断 100ms 周期
    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT1 对周期中断的中断优先级为 0
}


/**
 * @brief 中断读取编码器计数
 *
 */
void pit_handler (void)
{
//    encoder_l_data = encoder_get_count(ENCODER_L);                          // 获取编码器计数
//    encoder_r_data = encoder_get_count(ENCODER_R);                          // 获取编码器计数

//    encoder_clear_count(ENCODER_L);                                       // 清空编码器计数
//    encoder_clear_count(ENCODER_R);                                       // 清空编码器计数
    encoder_l_data++;
}

