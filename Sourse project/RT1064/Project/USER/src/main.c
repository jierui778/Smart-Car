#include "zf_common_headfile.h"
#include "encoder.h" 






int main (void)
{
    clock_init(SYSTEM_CLOCK_600M);  // ����ɾ��
    Encoder_Init();
    debug_init();
    


    while(1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        printf("ENCODER_QUADDEC counter \t%d .\r\n", encoder_l_data);     // ���������������Ϣ
        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_r_data);           // ���������������Ϣ
        // �˴���д��Ҫѭ��ִ�еĴ���
        system_delay_ms(100);
    }
}



