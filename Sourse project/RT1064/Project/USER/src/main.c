#include "zf_common_headfile.h"




// **************************** �������� ****************************
#define PIT_CH                          (PIT_CH0 )                              // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���
#define PIT_PRIORITY                    (PIT_IRQn)                              // ��Ӧ�����жϵ��жϱ�� 
    
#define ENCODER_QUADDEC                 (QTIMER1_ENCODER1)                     // ������������Ӧʹ�õı������ӿ� ����ʹ��QTIMER1��ENCODER1
#define ENCODER_QUADDEC_A               (QTIMER1_ENCODER1_CH1_C0)              // A ���Ӧ������
#define ENCODER_QUADDEC_B               (QTIMER1_ENCODER1_CH2_C1)              // B ���Ӧ������
    
#define ENCODER_DIR		                (QTIMER2_ENCODER1)                     // �������������Ӧʹ�õı������ӿ� ����ʹ��QTIMER1��ENCODER2
#define ENCODER_DIR_PULSE               (QTIMER2_ENCODER1_CH1_C3)              // PULSE ��Ӧ������
#define ENCODER_DIR_DIR                 (QTIMER2_ENCODER1_CH2_C25)             // DIR ��Ӧ������

int16 encoder_data_quaddec = 0;
int16 encoder_data_dir = 0;

int main (void)
{
    clock_init(SYSTEM_CLOCK_600M);                                              // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 600MHz
    debug_init();                                                               // ��ʼ��Ĭ�� debug uart

    // �˴���д�û����� ���������ʼ�������
    encoder_quad_init(ENCODER_QUADDEC, ENCODER_QUADDEC_A, ENCODER_QUADDEC_B);   // ��ʼ��������ģ�������� �������������ģʽ
    encoder_dir_init(ENCODER_DIR, ENCODER_DIR_PULSE, ENCODER_DIR_DIR);          // ��ʼ��������ģ�������� ����������������ģʽ

    pit_ms_init(PIT_CH, 100);                                                   // ��ʼ�� PIT1 Ϊ�����ж� 100ms ����

    interrupt_set_priority(PIT_PRIORITY, 0);                                    // ���� PIT1 �������жϵ��ж����ȼ�Ϊ 0
    // �˴���д�û����� ���������ʼ�������

    while(1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        printf("ENCODER_QUADDEC counter \t%d .\r\n", encoder_data_quaddec);     // ���������������Ϣ
        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_data_dir);           // ���������������Ϣ
        system_delay_ms(500);
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

void pit_handler (void)
{
    encoder_data_quaddec = encoder_get_count(ENCODER_QUADDEC);                  // ��ȡ����������
    encoder_data_dir = encoder_get_count(ENCODER_DIR);                          // ��ȡ����������

    encoder_clear_count(ENCODER_QUADDEC);                                       // ��ձ���������
    encoder_clear_count(ENCODER_DIR);                                           // ��ձ���������
}



