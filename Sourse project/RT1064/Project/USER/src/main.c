#include "camera.h"
#include "image.h"
#include "buzzer.h"
#include "zf_common_headfile.h"
#include "servo.h"
#include "math.h"

#define GrayScale 256

#define SERVO_MOTOR_DUTY(x) ((float)PWM_DUTY_MAX / (1000.0 / (float)50) * (0.5 + (float)(x) / 90.0))
unsigned int TH;

uint8 get_Threshold(uint8 *image, uint16 UCOL, uint16 UROW);

static float i = 0.0;
int main(void)
{
  clock_init(SYSTEM_CLOCK_600M); //
                                 //    Encoder_Init();
  debug_init();
  //    gpio_init(B9, GPO, GPIO_HIGH, GPO_PUSH_PULL);
  tft180_set_dir(TFT180_PORTAIT);
  tft180_init();
  //    Buzzer_Init();
  tft180_show_string(0, 0, "mt9v03x init.");

  Camera_Init();
  Servo_Init();
  //    while (1)
  //    {
  //        if (mt9v03x_init())
  //            gpio_toggle_level(B9);
  //        else
  //            break;
  //        system_delay_ms(1000);
  //    }

  while (1)
  {
    Image_Compress();
    //    TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height, 150); // ����ֵ����
    TH = get_Threshold(Image_Use[0], Image_Width, Image_Height);
    tft180_show_uint(4, 80, TH, 3);
    Binarization(TH);
    tft180_displayimage03x((uint8 *)Image_Use, 100, 60); // ��ʾ�����ö�ֵ���ĺ�
  }
}

// uint8 get_Threshold(uint8 *image, uint16 col, uint16 row)
//{

//  uint16 width = col;
//  uint16 height = row;
//  int pixelCount[GrayScale];
//  float pixelPro[GrayScale];
//  int i, j, pixelSum = width * height;
//  uint8 threshold = 0;
//  uint16 threshold_j = 0;
//  uint8 *data = image;            // 定义一个指向传进来这个image数组的指针变量data
//  for (i = 0; i < GrayScale; i++) // 先把pixelCount和pixelPro两个数组元素全部赋值为0
//  {
//    pixelCount[i] = 0;
//    pixelPro[i] = 0;
//  }

//  uint32 gray_sum = 0;
//  /**************************************统计每个灰度值(0-255)在整幅图像中出现的次数**************************************/
//  for (i = 0; i < height; i += 1) // 遍历图像的每一行，从第零行到第59行。
//  {
//    for (j = 0; j < width; j += 1) // 遍历图像的每一列，从第零列到第79列。
//    {
//      pixelCount[(int)data[i * width + j]]++; // 将当前的像素点的像素值（灰度值）作为计数数组的下标。
//      gray_sum += (int)data[i * width + j];   // 计算整幅灰度图像的灰度值总和。
//    }
//  }
//  /**************************************统计每个灰度值(0-255)在整幅图像中出现的次数**************************************/

//  /**************************************计算每个像素值（灰度值）在整幅灰度图像中所占的比例*************************************************/
//  for (i = 0; i < GrayScale; i++)
//  {
//    pixelPro[i] = (float)pixelCount[i] / pixelSum;
//  }
//  /**************************************计算每个像素值（灰度值）在整幅灰度图像中所占的比例**************************************************/

//  /**************************************开始遍历整幅图像的灰度值（0-255），这一步也是大津法最难理解的一步***************************/
//  /*******************为什么说他难理解？因为我也是不理解！！反正好像就是一个数学问题，你可以理解为数学公式。***************************/
//  float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//  w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//  for (threshold_j = 0; threshold_j < GrayScale; threshold_j++)
//  {
//    w0 += pixelPro[threshold_j]; // 求出背景部分每个灰度值的像素点所占的比例之和，即背景部分的比例。
//    u0tmp += threshold_j * pixelPro[threshold_j];

//    w1 = 1 - w0;
//    u1tmp = gray_sum / pixelSum - u0tmp;

//    u0 = u0tmp / w0;   // 背景平均灰度
//    u1 = u1tmp / w1;   // 前景平均灰度
//    u = u0tmp + u1tmp; // 全局平均灰度
//    deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
//    if (deltaTmp > deltaMax)
//    {
//      deltaMax = deltaTmp;
//      threshold = threshold_j;
//    }
//    if (deltaTmp < deltaMax)
//    {
//      break;
//    }
//  }

//  return threshold;
//}
// uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)
//{
//    #define GrayScale 256
//    int pixelCount[GrayScale] = {0};//每个灰度值所占像素个数
//    float pixelPro[GrayScale] = {0};//每个灰度值所占总像素比例
//    int i,j;
//    int Sumpix = width * height;   //总像素点
//    uint8 threshold = 0;
//    uint8* data = image;  //指向像素数据的指针

//    //统计灰度级中每个像素在整幅图像中的个数
//    for (i = 0; i < height; i++)
//    {
//        for (j = 0; j < width; j++)
//        {
//            pixelCount[(int)data[i * width + j]]++;  //将像素值作为计数数组的下标
//          //   pixelCount[(int)image[i][j]]++;    若不用指针用这个
//        }
//    }
//    float u = 0;
//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //计算每个像素在整幅图像中的比例
//        u += i * pixelPro[i];  //总平均灰度
//    }

//
//    float maxVariance=0.0;  //最大类间方差
//    float w0 = 0, avgValue  = 0;  //w0 前景比例 ，avgValue 前景平均灰度
//    for(int i = 0; i < 256; i++)     //每一次循环都是一次完整类间方差计算 (两个for叠加为1个)
//    {
//        w0 += pixelPro[i];  //假设当前灰度i为阈值, 0~i 灰度像素所占整幅图像的比例即前景比例
//        avgValue  += i * pixelPro[i];
//
//        float variance = pow((avgValue/w0 - u), 2) * w0 /(1 - w0);    //类间方差
//        if(variance > maxVariance)
//        {
//            maxVariance = variance;
//            threshold = i;
//        }
//    }

//    return threshold;
//
//}

//////////////////目前效果最好
// uint8 get_Threshold(uint8 *image, uint16 col, uint16 row)
//{
//     uint16 Width  = col;
//     uint16 Height = row;
//     int X; uint16 Y;
//     uint8* data = image;
//     int HistGram[GrayScale];

//    for (Y = 0; Y < GrayScale; Y++)
//    {
//        HistGram[Y] = 0; //初始化灰度直方图
//    }
//    for (Y = 0; Y <Height; Y++) //Y<Image_Height改为Y =Image_Height；以便进行 行二值化
//    {
//        //Y=Image_Height;
//        for (X = 0; X < Width; X++)
//        {
//        HistGram[(int)data[Y*Width + X]]++; //统计每个灰度值的个数信息
//        }
//    }

//    uint32 Amount = 0;
//    uint32 PixelBack = 0;
//    uint32 PixelIntegralBack = 0;
//    uint32 PixelIntegral = 0;
//    int32 PixelIntegralFore = 0;
//    int32 PixelFore = 0;
//    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0; // 类间方差;
//    int16 MinValue=0, MaxValue=0;
//    uint8 Threshold = 0;

//    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
//    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值

//    if (MaxValue == MinValue)
//    {
//        return MaxValue;          // 图像中只有一个颜色
//    }
//    if (MinValue + 1 == MaxValue)
//    {
//        return MinValue;      // 图像中只有二个颜色
//    }

//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        Amount += HistGram[Y];        //  像素总数
//    }

//    PixelIntegral = 0;
//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        PixelIntegral += HistGram[Y] * Y;//灰度值总数
//    }
//    SigmaB = -1;
//    for (Y = MinValue; Y < MaxValue; Y++)
//    {
//          PixelBack = PixelBack + HistGram[Y];    //前景像素点数
//          PixelFore = Amount - PixelBack;         //背景像素点数
//          OmegaBack = (double)PixelBack / Amount;//前景像素百分比
//          OmegaFore = (double)PixelFore / Amount;//背景像素百分比
//          PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
//          PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
//          MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
//          MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
//          Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
//          if (Sigma > SigmaB)//遍历最大的类间方差g
//          {
//              SigmaB = Sigma;
//              Threshold = Y;
//          }
//    }
//   return Threshold;
//}



/////////////////////////效果也非常好,最好

uint8 get_Threshold(uint8 *image, uint16 Width, uint16 Height)
{
  uint8 HistGram[256] = {
      0,
  };
  uint16 x, y;
  int16 Y;
  uint32 Amount = 0;
  uint32 PixelBack = 0;
  uint32 PixelIntegralBack = 0;
  uint32 PixelIntegral = 0;
  int32 PixelIntegralFore = 0;
  int32 PixelFore = 0;
  double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
  int16 MinValue, MaxValue;
  uint8 Threshold = 0;
  uint8 *data = image;

  for (y = 0; y < 256; y++)
  {
    HistGram[y] = 0; // 初始化灰度直方图
  }
  for (y = 0; y < MT9V03X_H; y++)
  {
    for (x = 0; x < MT9V03X_W; x++)
    {
      HistGram[mt9v03x_image[y][x]]++; // 统计每个灰度值的个数信息
    }
  }

  for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++)
    ; // 获取最小灰度的值
  for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--)
    ; // 获取最大灰度的值

  if (MaxValue == MinValue)
  {
    return MaxValue; // 图像中只有一个颜色
  }
  if (MinValue + 1 == MaxValue)
  {
    return MinValue; // 图像中只有二个颜色
  }

  for (Y = MinValue; Y <= MaxValue; Y++)
  {
    Amount += HistGram[Y]; //  像素总数
  }

  PixelIntegral = 0;
  for (Y = MinValue; Y <= MaxValue; Y++)
  {
    PixelIntegral += HistGram[Y] * Y; // 灰度值总数
  }
  SigmaB = -1;
  for (Y = MinValue; Y < MaxValue; Y++)
  {
    PixelBack = PixelBack + HistGram[Y];                                               // 前景像素点数
    PixelFore = Amount - PixelBack;                                                    // 背景像素点数
    OmegaBack = (double)PixelBack / Amount;                                            // 前景像素百分比
    OmegaFore = (double)PixelFore / Amount;                                            // 背景像素百分比
    PixelIntegralBack += HistGram[Y] * Y;                                              // 前景灰度值
    PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             // 背景灰度值
    MicroBack = (double)PixelIntegralBack / PixelBack;                                 // 前景灰度百分比
    MicroFore = (double)PixelIntegralFore / PixelFore;                                 // 背景灰度百分比
    Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); // g
    if (Sigma > SigmaB)                                                                // 遍历最大的类间方差g
    {
      SigmaB = Sigma;
      Threshold = Y;
    }
    if (Sigma < SigmaB) // 遍历最大的类间方差g
    {
      break;
    }
  }
  return Threshold;
}

//uint8 get_Threshold(uint8 *image, uint16 UCOL, uint16 UROW)
//{

//static uint8 lasthreshold=0;
//    uint16 width = UCOL; //自己图像的宽度
//    uint16 height = UROW; //自己图像的高度
//    //uint16 width = MT9V03X_W;
//    //uint16 height = MT9V03X_H;
//    int pixelCount[GrayScale];
//    float pixelPro[GrayScale];
//    int i, j, pixelSum = width * height/4;
//    uint8 threshold = 0;
//    uint8* data = Image_Use[0];//uint8* data = mt9v03x_image[0];  //指向像素数据的指针
//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelCount[i] = 0;
//        pixelPro[i] = 0;
//    }

//    uint32 gray_sum=0;
//    //统计灰度级中每个像素在整幅图像中的个数
//    for (i = 0; i < height; i+=2)
//    {
//        for (j = 0; j < width; j+=2)
//        {
//            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
//            gray_sum+=(int)data[i * width + j];       //灰度值总和
//        }
//    }

//    //计算每个像素值的点在整幅图像中的比例

//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / pixelSum;

//    }

//    //遍历灰度级[0,255]
//    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


//        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//        if( lasthreshold-50>0)
//    j= lasthreshold-50;
//        else
//    j=0;
//        for (; j < GrayScale; j++)
//        {

//                w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
//                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

//               w1=1-w0;
//               u1tmp=gray_sum/pixelSum-u0tmp;

//                u0 = u0tmp / w0;              //背景平均灰度
//                u1 = u1tmp / w1;              //前景平均灰度
//                u = u0tmp + u1tmp;            //全局平均灰度
//                deltaTmp = w0 * (u0 - u) * (u0 - u) + w1 * (u1 - u) * (u1 - u);
//                if (deltaTmp > deltaMax)
//                {
//                    deltaMax = deltaTmp;
//                    threshold = j;
//                }
//                if (deltaTmp < deltaMax)
//                {
//                break;
//                }

//         }

//    return threshold;
//}
