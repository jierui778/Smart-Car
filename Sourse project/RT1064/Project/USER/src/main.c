#include "camera.h"
#include "image.h"
#include "buzzer.h"

// uint8 test[Image_With][Image_With];MT9V03X_W, MT9V03X_H

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // ����ɾ��
//    Encoder_Init();
    debug_init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ��ʼ�� LED1 ��� Ĭ�ϸߵ�ƽ �������ģʽ
    tft180_set_dir(TFT180_PORTAIT);             // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
    tft180_init();
    Buzzer_Init();
//    tft180_show_string(0, 0, "mt9v03x init.");

    Camera_Init();
//    while (1)
//    {
//        if (mt9v03x_init())
//            gpio_toggle_level(B9); // ��ת LED ���������ƽ ���� LED ���� ��ʼ����������ƻ����ĺ���
//        else
//            break;
//        system_delay_ms(1000); // ���Ʊ�ʾ�쳣
//    }

    while (1)
    {
		Image_Compress();
       
        
        //        Image_Compress(mt9v03x_image, test);
        // printf("ENCODER_1 counter \t%d .\r\n", encoder_l_data); //
        // printf("ENCODER_2 counter \t%d .\r\n", encoder_r_data); //

        // camera_send_image(DEBUG_UART_INDEX, (const uint8 *)Image_Use, 6000)
//		tft180_show_gray_image(0,0,(uint8 *)Image_Use, 188, 120,100,60,0);
        tft180_displayimage03x((uint8 *)Image_Use, 100, 60); // ��Ҫע�� ֱ����ʾ 188*120 �ֱ�������ʾ���µ� ��ֱ�ӽ�����Ա���
        // mt9v03x_finish_flag = 0;
    }
}
