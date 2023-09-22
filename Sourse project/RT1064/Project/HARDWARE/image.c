#include "image.h"

uint8 Image_Use[Image_Hight][Image_With]; // 定义一个二维数组存储压缩后的灰度图像

uint16 Threshold; // 图像处理后得到的二值化阈值
uint8 Enter_Straight = 0;

/**
 * @brief 截取我们需要的图像大小
 *
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */
 void Image_Compress(void)
 {
     int i, j, row, line;
     const float pro_h = Primeval_Hight / Image_Hight, pro_w = Primeval_With / Image_With; // 120/60=2 188/100=1.88
     for (i = 0; i < Image_Hight; i++)                                                     // 遍历图像的每一行，从第零行到第59行。
     {
         row = ((int)i * (int)pro_h) +0.5;
         for (j = 0; j < Primeval_With; j++) // 遍历图像的每一列，从第零列到第79列。
         {
             line = (int)j * (int)pro_w +0.5;
             Image_Use[i][j] = mt9v03x_image[row][line]; // mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
         }
     }
     mt9v03x_finish_flag = 0;
 }
// // void Image_Compress(IN uint8 (*InImg)[Image_With * 2], OUT uint8 (*OutImg)[Image_With])
// {
//     int8 i, j, row, line;
//     const float pro_h = Primeval_Hight / Image_With, pro_w = Primeval_With / Image_With; // 根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
//     for (i = 0; i < Image_Hight; i++)
//     {
//         row = i * pro_h + 0.5;
//         for (j = 0; j < Image_With; j++)
//         {
//             line = j * pro_w + 0.5;
//             Image_Use[i][j] = InImg[row][line];
//         }
//     }
// }

///**
// * @brief 将输入的灰度图像转化为二值化图像
// *
// * @param Uint8 输入图像的地址
// * @param Uint8 输出图像的地址
// * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
// */
// void Image_GetBinary(IN Uint8 (*InImg)[IMGW], OUT Uint8 (*OutImg)[IMGW], IN Uint8 Threshold)
//{
//}

///**
// * @brief 大津法求阈值（和其他的版本有些出入）
// *
// * @param Uint8 *image 第一次原始图像（灰度为0-255，有188*120个像素点的图像）
// * @return 求得的阈值
// * @exception 场地得到的灰度图像一般颜色不一，但是图像里面占的面积最大的灰度肯定只有赛道内的灰度值A1和赛道外的灰度值A2，因此得到的直方图会在A1和A2
// * 出现两个峰值，而所求的阈值就是两个峰值之间的最低值
// */
// unsigned char BMP_GetThreshold(uint8 *image[CAMERA_HEIGHT][CAMERA_WITH])
//{
//    unsigned char i,j;                        //用来遍历灰度直方图
//    unsigned int High1=0;                     //第一高峰
//    unsigned char High1_Position=0;           //第一高峰对应的灰度值
//    unsigned int High2=0;                     //第二高峰
//    unsigned char High2_Position=0;           //第二高峰对应的灰度值
//    unsigned char HistoGram[256];             //先定义灰度直方图
//    unsigned char *gray;                      //灰度值（中间值）
//    unsigned int High_temp;                   //高度中间值
//    unsigned int Low1=20000;                  //定义低谷1
//    unsigned char Low1_Position;              //定义低谷1的灰度值位置，即所求的阈值
//    for(i=0;i<CAMERA_HEIGHT;i++)              //先遍历灰度值第一高峰
//    {
//        for(j=0;j<CAMERA_WITH;j++)
//        {
//            gray=image[i][j];                 //指针传递
//            HistoGram[*gray]++;               //gray解码传值，得到的HistoGram[256]即为灰度直方图
//        }
//    }
//    for(i=0;i<256;i++)
//    {
//        if(HistoGram[i]>High1)
//        {
//            High1=HistoGram[i];               //遍历所有点，存储最高峰的值
//            High1_Position=i;                 //存储当前最高峰值的灰度值
//        }
//    }
//    for(High_temp=High1-5;High_temp>0;High_temp-=5)//从第一高峰开始向下寻找第二高峰
//    {
//        for(i=0;i<256;i++)                    //在高度值固定时，对灰度值进行遍历
//        {
//            if((HistoGram[i]>High_temp)&&(i-High1_Position)>30)    //防止找到最高峰旁边的点
//            {
//                High2=HistoGram[i];           //记录次高峰峰值（会有0-5内的误差）
//                High2_Position=i;             //记录次高峰峰值对应的灰度值
//            }
//        }
//    }
//    if(High1_Position>High2_Position)         //如果最高峰对应的灰度值大于次高峰
//    {
//        for(i=High2_Position;i<High1_Position;i++)
//        {
//            if(HistoGram[i]<Low1)             //遍历，求最小值
//            {
//                Low1=HistoGram[i];            //刷新谷底高度
//                Low1_Position=i;              //求出谷底位置
//            }
//        }
//    }
//    return Low1_Position;                     //返回谷底位置，即阈值
//}
