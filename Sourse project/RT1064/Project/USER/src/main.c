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
        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_r_data);           // 输出编码器计数信息
        // 此处编写需要循环执行的代码
        system_delay_ms(100);
    }
}



