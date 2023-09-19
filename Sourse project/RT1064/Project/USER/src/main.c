#include "zf_common_headfile.h"

#include "encoder.h" 

int main (void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    Encoder_Init();
    debug_init();


    while(1)
    {
        // 此处编写需要循环执行的代码
        printf("ENCODER_QUADDEC counter \t%d .\r\n", encoder_l_data);     // 输出编码器计数信息
//        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_l_data);           // 输出编码器计数信息
        // 此处编写需要循环执行的代码
    }
}



///**
// * @brief 中断读取编码器计数
// *
// */
//void pit_handler (void)
//{
//    encoder_l_data = encoder_get_count(ENCODER_L);                          // 获取编码器计数
//    encoder_r_data = encoder_get_count(ENCODER_R);                          // 获取编码器计数

//    encoder_clear_count(ENCODER_L);                                       // 清空编码器计数
//    encoder_clear_count(ENCODER_R);                                       // 清空编码器计数
//}

