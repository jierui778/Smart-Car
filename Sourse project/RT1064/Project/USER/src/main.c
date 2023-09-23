#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"
#include "servo.h"
#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)50)*(0.5+(float)(x)/90.0))
unsigned int TH;
//unsigned int i,j;

static float i=0.0;
int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // ����ɾ��
//    Encoder_Init();
    debug_init();
//    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ��ʼ�� LED1 ���? Ĭ�ϸߵ�ƽ �������ģ�?
//    tft180_set_dir(TFT180_PORTAIT);             // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
//    tft180_init();
//    Buzzer_Init();
////    tft180_show_string(0, 0, "mt9v03x init.");

//    Camera_Init();
    Servo_Init();
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
////////		Image_Compress();
//////        TH=GetThreshold();					//����ֵ����
//////		tft180_show_uint(4,80,TH,3);
//////		Binarization(TH);
//////		tft180_displayimage03x((uint8 *)Image_Use, 100, 60); //
//        system_delay_ms(10);
//        pwm_set_duty(SERVO_CH, 800);

//        pwm_set_duty(SERVO_CH, 1000);
//        pwm_set_duty(PWM4_MODULE2_CHA_C30, (uint32)SERVO_MOTOR_DUTY(i));

    }
}





//#define SERVO_MOTOR_PWM             (PWM4_MODULE2_CHA_C30)                          // ���������϶����Ӧ����
//#define SERVO_MOTOR_FREQ            (50 )                                           // ���������϶��Ƶ��  �����ע�ⷶΧ 50-300
//                                         // ���������϶�����Χ �Ƕ�

//#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))



//float servo_motor_duty = 0.0;                                                  // ��������Ƕ�
//float servo_motor_dir = 1;                                                      // �������״̬

//int main(void)
//{
//    clock_init(SYSTEM_CLOCK_600M);  // ����ɾ��
//    debug_init();                   // ���Զ˿ڳ�ʼ��
//    Servo_Init();
//
//
//
//
////    interrupt_global_enable(0);
//
//    while(1)
//    {
//        pwm_set_duty(SERVO_MOTOR_PWM, (uint32)SERVO_MOTOR_DUTY(servo_motor_duty));

////        if(servo_motor_dir)
////        {
//            servo_motor_duty ++;
////            if(servo_motor_duty >= SERVO_MOTOR_R_MAX)
////            {
////                servo_motor_dir = 0x00;
////            }
////        }
////        else
////        {
////            servo_motor_duty --;
////            if(servo_motor_duty <= SERVO_MOTOR_L_MAX)
////            {
////                servo_motor_dir = 0x01;
////            }
////        }
//        system_delay_ms(100);
//        if(servo_motor_duty>180)
//        {
//            servo_motor_duty=0;
//
//        }
//
//    }
//}
