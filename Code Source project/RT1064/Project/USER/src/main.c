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
        SEGGER_RTT_printf(0, "Hello world !\n");
        uint8 t;
        t=100;
        //        Schedule_Run();//任务运行总函数，开始任务调度
    }
}
