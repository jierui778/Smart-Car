#include "encoder.h"
#include "pid.h"
#include "zf_common_headfile.h"
int32 Encoder_L_Data = 0, Encoder_R_Data = 0;
int32 Encoder_L_Dis = 0, Encoder_R_Dis = 0;
int8 DisCnt_Flag = 0;

/**
 * @brief 编码器引脚初始化
 *
 */
void Encoder_Init(void)
{
    encoder_quad_init(ENCODER_L, ENCODER_L_A, ENCODER_L_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_R, ENCODER_R_A, ENCODER_R_B); // 初始化编码器模块与引脚 正交解码编码器模式

    // pit_ms_init(PIT_CH0, 100);                     // 初始化 PIT1 为周期中断 100ms 周期
    // interrupt_set_priority((IRQn_Type)PIT_CH0, 0); // 设置 PIT1 对周期中断的中断优先级为 0

    //    pit_ms_init((pit_index_enum)PIT_CH1,200);//舵机转向调节周期为200ms
    //    interrupt_set_priority((IRQn_Type)PIT_CH1,1);

    ////    pit_ms_init((pit_index_enum)PIT_CH2,250);//电机速度调节周期为250ms
    //    interrupt_set_priority((IRQn_Type)PIT_CH2,2);
    //    pit_ms_init(PIT_CH, 100);                                                   // 初始化 PIT1 为周期中断 100ms 周期
    //    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT1 对周期中断的中断优先级为 0
}

/**
 * @brief 更新编码器计数值
 *
 * @param Data_L 编码器定时采样值
 * @param Data_R
 * @param Dis_L 编码器积分距离
 * @param Dis_R
 */
void Encoder_Update(int32 *Data_L, int32 *Data_R ,int32 *Dis_L, int32 *Dis_R)
{
    *Data_L = encoder_get_count(ENCODER_L);  // 获取编码器计数
    *Data_R = -encoder_get_count(ENCODER_R); // 获取编码器计数//预采用kalman滤波
    encoder_clear_count(ENCODER_L);          // 清空编码器计数
    encoder_clear_count(ENCODER_R);          // 清空编码器计数
    if(DisCnt_Flag)//开启计数
    {
        *Dis_L += *Data_L;
        *Dis_R += *Data_R; // 积分编码器计数
    }
}
