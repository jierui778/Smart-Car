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

uint8_t  Image_Use_Robert[120][160];//二值化图像
float left_speed,right_speed;
int main(void)
{
    uint16 data = 8;
    clock_init(SYSTEM_CLOCK_600M); //
    Encoder_Init();
    debug_init();
	Motor_Init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    tft180_set_dir(TFT180_PORTAIT);
    tft180_init();
//    //  //    Buzzer_Init();
//    //  tft180_show_string(0, 0, "mt9v03x init.");

    Camera_Init();
//    //  Servo_Init();
//    pit_ms_init(PIT_CH0, 200);
//    interrupt_set_priority(PIT_IRQn, 0);
	PID_init();
    while (1)
    {
        int TH;
//        Schedule_Run();
        Image_Compress();
        TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height);
//        Image_Binarization(TH,Image_Use);
		tft180_show_int(3,80,TH,3);//最后有一点点
        Image_Sobel( Image_Use, Image_Use_Robert ,TH);//全局Sobel得二值图(方案二) 2.8ms
        tft180_displayimage03x((uint8 *)Image_Use_Robert, 100, 60); //pidMotor1Speed
//		Image_Run();
//		tft180_show_float(3,80,IncrementPID_Speed(&pidMotor1Speed,speed_left),4,4);//750
//		tft180_show_float(3,100,speed_right,4,4);//5
//		tft180_show_float(3,120,speed_left,4,4);//最后有一点点
//		tft180_show_float(3,140,speed_left-speed_right,4,4);//150
//		Control_Setspeed(3,3);

    }
}









































