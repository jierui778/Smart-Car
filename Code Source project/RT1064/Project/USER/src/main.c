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
#include "SEGGER_RTT.h"

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
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "Hello world !");

    //    Motor_SetPwmL(2000);
    //    Motor_SetPwmR(1888.88);
    while (1)
    {

        //        Schedule_Run();//任务运行总函数，开始任务调度
        test();
        ips200_displayimage03x(*Image_Use_Robert, 160, 120);

//        test();
//
//        //        Schedule_Run();//任务运行总函数，开始任务调度
		//	bluetooth_ch9141_send_image(*Image_Use_Robert, 19200);
    }
}


void Turn_Ring(void)//转向环
{

    //特殊情况处理，起跑打死固定角度
    if ((garage_type == GARAGE_OUT_LEFT || garage_type == GARAGE_OUT_RIGHT) && Z.all_length < ENCODER_PER_METER * 0.6 && rpts0s_num < 100 && rpts1s_num < 100)
    {
        if (garage_type == GARAGE_OUT_LEFT)//左出库打死角度
        {
            error[0] = -35;
        }
        else if (garage_type == GARAGE_OUT_RIGHT)//右出库打死角度
        {
            error[0] = 35;
        }
    }

    else if (garage_type == GARAGE_IN_LEFT)//左进库打死角度
    {
        //        error[0] = -(0.015*Z.motol.nowspeed*Z.motol.nowspeed + 15);
        error[0] = -35;
    }
    // 右边入库打角
    else if (garage_type == GARAGE_IN_RIGHT)//右进库打死角度
    {
        //        error[0] = 0.015*Z.motol.nowspeed*Z.motol.nowspeed + 15;
        error[0] = 35;
    }

    //考虑不同路段给予不同的速度/pid控制
}


void Speed_Ring(void)//速度环
{
    //考虑不同路段给予不同的速度/pid控制
    if()




}
