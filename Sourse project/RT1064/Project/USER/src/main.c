#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"

uint8 TH;
unsigned int i,j;
// uint8 test[Image_With][Image_With];MT9V03X_W, MT9V03X_H
	
int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); 
    debug_init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ��ʼ�� LED1 ���? Ĭ�ϸߵ�ƽ �������ģ�?
    tft180_set_dir(TFT180_PORTAIT);             // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
    tft180_init();
    Buzzer_Init();
    Camera_Init();


    while (1)
    {
		Image_Compress();
		TH=Image_GetThreshold();					//����ֵ����
		tft180_show_uint(4,80,TH,3);
		Image_Binarization(TH);
		tft180_displayimage03x((uint8 *)Image_Use, 100, 60); // ��ʾ�����ö�ֵ���ĺ������Ǹ���̫��
    }
}
