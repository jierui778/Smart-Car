#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"

unsigned int TH;
unsigned int i,j;
// uint8 test[Image_With][Image_With];MT9V03X_W, MT9V03X_H
	
int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // ����ɾ��
//    Encoder_Init();
    debug_init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ��ʼ�� LED1 ���? Ĭ�ϸߵ�ƽ �������ģ�?
    tft180_set_dir(TFT180_PORTAIT);             // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
    tft180_init();
    Buzzer_Init();
//    tft180_show_string(0, 0, "mt9v03x init.");

    Camera_Init();
//    while (1)
//    {
//        if (mt9v03x_init())
//            gpio_toggle_level(B9); // ��ת LED ����������? ���� LED ���� ��ʼ����������ƻ����ĺ���?
//        else
//            break;
//        system_delay_ms(1000); // ���Ʊ�ʾ�쳣
//    }

    while (1)
    {
		Image_Compress();
        TH=GetThreshold();					//����ֵ����
		tft180_show_uint(4,80,TH,3);
		Binarization(TH);
		tft180_displayimage03x((uint8 *)Image_Use, 100, 60); // ��ʾ�����ö�ֵ���ĺ������Ǹ���̫��
    }
}
