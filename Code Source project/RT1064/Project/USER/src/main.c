#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"
#include "servo.h"
#include "math.h"
#include "encoder.h"
#include "schedule.h"
#include "control.h"
#include "motor.h"
#include "pid.h"
#include "isr.h"
#define GrayScale 257

uint8_t  Image_Use_Robert[60][100];//二值化图像
float left_speed,right_speed;
int main(void)
{
    uint16 data = 8;
    clock_init(SYSTEM_CLOCK_600M); //
    Encoder_Init();
    debug_init();
	Motor_Init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    tft180_set_dir(TFT180_PORTAIT_180);
    tft180_init();
//    //  //    Buzzer_Init();
//    //  tft180_show_string(0, 0, "mt9v03x init.");

//    Camera_Init();
//    //  Servo_Init();
//    pit_ms_init(PIT_CH0, 200);
//    interrupt_set_priority(PIT_IRQn, 0);
	PID_init();
    while (1)
    {
        int TH;
//        Schedule_Run();
//        Image_Compress();
//        TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height);
//        Image_Binarization(TH,Image_Use);
//        Image_Sobel( Image_Use, Image_Use_Robert ,TH);//全局Sobel得二值图(方案二) 2.8ms
//        tft180_displayimage03x((uint8 *)Image_Use, 100, 60); //pidMotor1Speed
		tft180_show_float(3,80,IncrementPID_Speed(&pidMotor1Speed,speed_left),4,4);//750
		tft180_show_float(3,100,speed_right,4,4);//5
		tft180_show_float(3,120,speed_left,4,4);//最后有一点点
		tft180_show_float(3,140,speed_left-speed_right,4,4);//150
		Control_Setspeed(3,3);

    }
}
void pit_exti_handler(void)
{
  //    Scheduler_Tick();
  gpio_toggle_level(B9);
}

// #define LED1                    (B9 )

// #define PIT_CH                  (PIT_CH0 )                                      // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
// #define PIT_PRIORITY            (PIT_IRQn)                                      // 对应周期中断的中断编号

// int main (void)
//{
//     clock_init(SYSTEM_CLOCK_600M);                                              // 初始化芯片时钟 工作频率为 600MHz
//     debug_init();                                                               // 初始化默认 Debug UART

//    // 此处编写用户代码 例如外设初始化代码等
//    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // 初始化 LED1 输出 默认高电平 推挽输出模式

//    pit_ms_init(PIT_CH, 200);                                                   // 初始化 PIT 为周期中断 200ms 周期
//    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT 对周期中断的中断优先级为 1 优先级比外部中断低 会被外部中断打断
//    // 此处编写用户代码 例如外设初始化代码等

//    while(1)
//    {
//        // 此处编写需要循环执行的代码
//        // 此处编写需要循环执行的代码
//
//
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     PIT 的周期中断处理函数 这个函数将在 PIT 对应的定时器中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     pit_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
// void pit_handler(void)
//{
//    gpio_toggle_level(B9);                                                    // 触发 PIT 中断后翻转 LED 状态
//}


