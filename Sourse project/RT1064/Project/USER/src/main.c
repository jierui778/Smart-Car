#include "camera.h"
#include "image.h"
#include "buzzer.h"

unsigned char TH;
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
        TH=BMP_GetThreshold();					//大津法求阈值，这个大津法有问题，暂时输入不了
		Binarization(TH);						//阈值的二值化处理
		tft180_displayimage03x((uint8 *)Image_Use, 100, 60); // 只用这一个函数即可，不要用输入参数有阈值的函数
        mt9v03x_finish_flag = 0;
    }
}
