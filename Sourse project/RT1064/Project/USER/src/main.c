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
//        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_l_data);           // ���������������Ϣ
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}



///**
// * @brief �ж϶�ȡ����������
// *
// */
//void pit_handler (void)
//{
//    encoder_l_data = encoder_get_count(ENCODER_L);                          // ��ȡ����������
//    encoder_r_data = encoder_get_count(ENCODER_R);                          // ��ȡ����������

//    encoder_clear_count(ENCODER_L);                                       // ��ձ���������
//    encoder_clear_count(ENCODER_R);                                       // ��ձ���������
//}

