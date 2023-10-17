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
#include "filter.h"

int main(void)
{

    uint16 data = 8;
    clock_init(SYSTEM_CLOCK_600M); //
    Encoder_Init();
    debug_init();
    Motor_Init();
    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    ips200_init(IPS200_TYPE_PARALLEL8);
    //    //  //    Buzzer_Init();
    //    //  tft180_show_string(0, 0, "mt9v03x init.");

    Camera_Init();
    //    //  Servo_Init();
    //    pit_ms_init(PIT_CH0, 200);
    //    interrupt_set_priority(PIT_IRQn, 0);
    PID_Init();
//    bluetooth_ch9141_init();
    //    Motor_SetPwmL(2000);
    //    Motor_SetPwmR(1888.88);
    Servo_Init();//舵机初始化
    Servo_SetAngle(0);
    while (1)
    {


//        if (mt9v03x_finish_flag)
//        {
//            test();
//           ips200_displayimage03x(*Image_Use_Robert, 160, 120);
//                Image_Compress();
//    int TH;
//    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
//    //    Image_Binarization(TH, Image_Use);
//    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms
//            ips200_show_gray_image(0, 200, *Image_Use_Robert, 160, 120, 160, 120, 0);
//        }

//    test();
        //
        //        //        Schedule_Run();//任务运行总函数，开始任务调度
//        bluetooth_ch9141_send_image(*Image_Use_Robert, 115200);

//		if(mt9v03x_finish_flag)
//        {
//			test();
////			uint8 i;
////			Image_Compress();
////			int TH;
////			TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
////			//    Image_Binarization(TH, Image_Use);
////			Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms
////        
////            bluetooth_ch9141_send_image((const uint8 *)Image_Use_Robert, MT9V03X_IMAGE_SIZE);
////            mt9v03x_finish_flag = 0;
//     
//            ips200_displayimage03x(*Image_Use_Robert, 160, 120);
//			
//        }
            
//        test();
//        
        Schedule_Run();//任务运行总函数，开始任务调度
		//	bluetooth_ch9141_send_image(*Image_Use_Robert, 19200);

    }
}
