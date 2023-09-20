#include "zf_common_headfile.h"
#include "encoder.h"
#include "camera.h"
#include "image.h"

// uint8 test[Image_With][Image_With];

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // 不可删除
    Encoder_Init();
    debug_init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL); // 初始化 LED1 输出 默认高电平 推挽输出模式
    tft180_set_dir(TFT180_CROSSWISE);             // 需要先横屏 不然显示不下
    tft180_init();
    tft180_show_string(0, 0, "mt9v03x init.");

    //    Camera_Init();
    while (1)
    {
        if (mt9v03x_init())
            gpio_toggle_level(B9); // 翻转 LED 引脚输出电平 控制 LED 亮灭 初始化出错这个灯会闪的很慢
        else
            break;
        system_delay_ms(1000); // 闪灯表示异常
    }

    while (1)
    {
        //        Image_Compress(mt9v03x_image, test);
        // printf("ENCODER_1 counter \t%d .\r\n", encoder_l_data); //
        // printf("ENCODER_2 counter \t%d .\r\n", encoder_r_data); //

        // camera_send_image(DEBUG_UART_INDEX, (const uint8 *)Image_Use, 6000);
        tft180_displayimage03x((const uint8 *)Image_Use, 100, 60); // 需要注意 直接显示 188*120 分辨率是显示不下的 会直接进入断言报错
        mt9v03x_finish_flag = 0;
    }
}
