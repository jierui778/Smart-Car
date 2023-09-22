#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"

unsigned int TH;
unsigned int i,j;
// uint8 test[Image_With][Image_With];MT9V03X_W, MT9V03X_H
	
int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // ï¿½ï¿½ï¿½ï¿½É¾ï¿½ï¿½
//    Encoder_Init();
    debug_init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ï¿½ï¿½Ê¼ï¿½ï¿½ LED1 ï¿½ï¿½ï¿? Ä¬ï¿½Ï¸ßµï¿½Æ½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä£Ê?
    tft180_set_dir(TFT180_PORTAIT);             // ï¿½ï¿½Òªï¿½Èºï¿½ï¿½ï¿½ ï¿½ï¿½È»ï¿½ï¿½Ê¾ï¿½ï¿½ï¿½ï¿½
    tft180_init();
    Buzzer_Init();
//    tft180_show_string(0, 0, "mt9v03x init.");

    Camera_Init();
//    while (1)
//    {
//        if (mt9v03x_init())
//            gpio_toggle_level(B9); // ï¿½ï¿½×ª LED ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ? ï¿½ï¿½ï¿½ï¿½ LED ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ»ï¿½ï¿½ï¿½ï¿½Äºï¿½ï¿½ï¿?
//        else
//            break;
//        system_delay_ms(1000); // ï¿½ï¿½ï¿½Æ±ï¿½Ê¾ï¿½ì³£
//    }

    while (1)
    {
		Image_Compress();
        TH=GetThreshold();					//½«ãÐÖµ´«Êä
		tft180_show_uint(4,80,TH,3);
		Binarization(TH);
		tft180_displayimage03x((uint8 *)Image_Use, 100, 60); // ÏÔÊ¾£¬²»ÓÃ¶þÖµ»¯µÄº¯Êý£¬ÄÇ¸ö²»Ì«ÐÐ
    }
}
