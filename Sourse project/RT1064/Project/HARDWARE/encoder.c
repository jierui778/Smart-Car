#include "encoder.h"
#include "pid.h"
#include "zf_common_headfile.h"
int32 encoder_l_data = 0, encoder_r_data = 0;
float speed_left,speed_right;//左右轮速度
#define Car_RoundCount 4096 //小车转一圈，对应计数器的计数值（得手动测）
/**
 * @brief 编码器引脚初始化
 *
 */
void Encoder_Init(void)
{
   encoder_quad_init(ENCODER_L, ENCODER_L_A, ENCODER_L_B); // 初始化编码器模块与引脚 正交解码编码器模式
   encoder_quad_init(ENCODER_R, ENCODER_R_A, ENCODER_R_B); // 初始化编码器模块与引脚 正交解码编码器模式

    pit_ms_init(PIT_CH, 100);                                                   // 初始化 PIT1 为周期中断 100ms 周期
    interrupt_set_priority((IRQn_Type)PIT_CH, 0);                                    // 设置 PIT1 对周期中断的中断优先级为 0

    pit_ms_init((pit_index_enum)PIT_CH1,200);//舵机转向调节周期为200ms
    interrupt_set_priority((IRQn_Type)PIT_CH1,1);

    pit_ms_init((pit_index_enum)PIT_CH2,250);//电机速度调节周期为250ms
    interrupt_set_priority((IRQn_Type)PIT_CH2,2);
}


/**
 * @brief 更新编码器数据
 *
 * @param encoder_l_data 传入指针
 * @param encoder_r_data 传入指针
 */
void Encoder_Update(int32 *encoder_l_data, int32 *encoder_r_data)
{
    *encoder_l_data = encoder_get_count(ENCODER_L);                          // 获取编码器计数
    *encoder_r_data = encoder_get_count(ENCODER_R);                          // 获取编码器计数
    speed_left=(float)(*encoder_l_data)*10/Car_RoundCount;//每隔100ms计算一次左右轮的速度，这里的速度单位为m/s
    speed_right=(float)(*encoder_r_data)*10/Car_RoundCount;
    encoder_clear_count(ENCODER_L);                                       // 清空编码器计数
    encoder_clear_count(ENCODER_R);                                       // 清空编码器计数
}
/**
 * @brief 中断读取编码器计数(100ms)
 *
 */
void pit_handler (void)
{

    Encoder_Update(&encoder_l_data, &encoder_r_data);
}

