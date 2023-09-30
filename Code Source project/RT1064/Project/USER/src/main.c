#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"
#include "servo.h"
#include "math.h"
#include "encoder.h"
#include "schedule.h"

#define GrayScale 257

uint8_t  Image_Use_Robert[60][100];//二值化图像

int main(void)
{
  uint16 data = 8;
  clock_init(SYSTEM_CLOCK_600M); //
                                 //  Encoder_Init();
  debug_init();
  gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
  tft180_set_dir(TFT180_CROSSWISE_180);
  tft180_init();
  //  //    Buzzer_Init();
  //  tft180_show_string(0, 0, "mt9v03x init.");

  Camera_Init();
  //  Servo_Init();
  pit_ms_init(PIT_CH0, 200);
  interrupt_set_priority(PIT_IRQn, 0);

  while (1)
  {
      int TH;
    //    Image_Compress();
    //    TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height, 150); //
    //    TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height);
    //    tft180_show_uint(4, 80, TH, 3);
    //    Binarization(TH);
    //    tft180_displayimage03x((uint8 *)Image_Use, 100, 60); //
//    Schedule_Run();
//    tft180_show_string(0, 0, "THIS IS A TEST");
    compressimage();

//    TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height);
//    Image_Binarization(TH,Image_Use);
//    Image_Sobel( Image_Use, Image_Use_Robert ,TH);//全局Sobel得二值图(方案二) 2.8ms
//    CannyEdgeTest( Image_Use, TH+10 );
//	  Image_DrawRectangle();
//    tft180_displayimage03x(Image_Use[0], 80, 60); ////////sobel和canny阈值测试
//      image_process();

//	  Image_Run();
  }
}









































