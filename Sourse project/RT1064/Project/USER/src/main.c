#include "zf_common_headfile.h"




// **************************** 代码区域 ****************************
#define PIT_CH                          (PIT_CH0 )                              // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
#define PIT_PRIORITY                    (PIT_IRQn)                              // 对应周期中断的中断编号 
    
#define ENCODER_QUADDEC                 (QTIMER1_ENCODER1)                     // 正交编码器对应使用的编码器接口 这里使用QTIMER1的ENCODER1
#define ENCODER_QUADDEC_A               (QTIMER1_ENCODER1_CH1_C0)              // A 相对应的引脚
#define ENCODER_QUADDEC_B               (QTIMER1_ENCODER1_CH2_C1)              // B 相对应的引脚
    
#define ENCODER_DIR		                (QTIMER2_ENCODER1)                     // 带方向编码器对应使用的编码器接口 这里使用QTIMER1的ENCODER2
#define ENCODER_DIR_PULSE               (QTIMER2_ENCODER1_CH1_C3)              // PULSE 对应的引脚
#define ENCODER_DIR_DIR                 (QTIMER2_ENCODER1_CH2_C25)             // DIR 对应的引脚

int16 encoder_data_quaddec = 0;
int16 encoder_data_dir = 0;

int main (void)
{
    clock_init(SYSTEM_CLOCK_600M);                                              // 初始化芯片时钟 工作频率为 600MHz
    debug_init();                                                               // 初始化默认 debug uart

    // 此处编写用户代码 例如外设初始化代码等
    encoder_quad_init(ENCODER_QUADDEC, ENCODER_QUADDEC_A, ENCODER_QUADDEC_B);   // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_dir_init(ENCODER_DIR, ENCODER_DIR_PULSE, ENCODER_DIR_DIR);          // 初始化编码器模块与引脚 带方向增量编码器模式

    pit_ms_init(PIT_CH, 100);                                                   // 初始化 PIT1 为周期中断 100ms 周期

    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT1 对周期中断的中断优先级为 0
    // 此处编写用户代码 例如外设初始化代码等

    while(1)
    {
        // 此处编写需要循环执行的代码
        printf("ENCODER_QUADDEC counter \t%d .\r\n", encoder_data_quaddec);     // 输出编码器计数信息
        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_data_dir);           // 输出编码器计数信息
        system_delay_ms(500);
        // 此处编写需要循环执行的代码
    }
}

void pit_handler (void)
{
    encoder_data_quaddec = encoder_get_count(ENCODER_QUADDEC);                  // 获取编码器计数
    encoder_data_dir = encoder_get_count(ENCODER_DIR);                          // 获取编码器计数

    encoder_clear_count(ENCODER_QUADDEC);                                       // 清空编码器计数
    encoder_clear_count(ENCODER_DIR);                                           // 清空编码器计数
}



