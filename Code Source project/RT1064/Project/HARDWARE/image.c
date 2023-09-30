#include "image.h"
#include "mymath.h"
uint8 Image_Use[Image_Height][Image_Width];
/**
 * @brief 截取我们需要的图像大小
 *
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */

//void Image_Compress(void)
//{
//    int i, j, row, line;
//    const float pro_h = Primeval_Hight / Image_Height, pro_w = Primeval_With / Image_Width; // 根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
//    for (i = 0; i < Image_Height; i++)                                                      // 遍历图像的每一行，从第零行到第59行。
//    {
//        row = i * pro_h + 0.5;
//        for (j = 0; j < Image_Width; j++) // 遍历图像的每一列，从第零列到第79列。
//        {
//            line = j * pro_w + 0.5;
//            Image_Use[i][j] = mt9v03x_image[row][line]; // mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
//        }
//    }
//    mt9v03x_finish_flag = 0;
//}

#define UROW 60
#define UCOL 80
void compressimage(void)
{
    int i, j, row, line;
      const float div_h = MT9V03X_H / UROW, div_w = MT9V03X_W / UCOL;
      for (i = 0; i < UROW; i++) {
        row = i * div_h + 0.5;
        for (j = 0; j < UCOL; j++) {
          line = j * div_w + 0.5;
          Image_Use[i][j] = mt9v03x_image[row][line];
        }
      }
      tft180_displayimage03x(Image_Use[0], 80, 60);
      mt9v03x_finish_flag = 0;  //使用完一帧DMA传输的图像图像  可以开始传输下一帧
}
    
    

/**
 * @brief 解压图像，主要是为了显示
 *
 */
void Image_Uncompress(void)
{
}

/**
 * @brief 手搓版本
 *
 * @param image
 * @param Width
 * @param Height
 * @return uint8
 */
// uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height)
//{
//     uint8 HistGram[256] = {
//         0,
//     };
//     uint16 x, y;
//     int16 Y;
//     uint32 Amount = 0;
//     uint32 PixelBack = 0;
//     uint32 PixelIntegralBack = 0;
//     uint32 PixelIntegral = 0;
//     int32 PixelIntegralFore = 0;
//     int32 PixelFore = 0;
//     double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
//     int16 MinValue, MaxValue;
//     uint8 Threshold = 0;
//     uint8 *data = image;

//    for (y = 0; y < 256; y++)
//    {
//        HistGram[y] = 0; // 初始化灰度直方图
//    }
//    for (y = 0; y < Height; y++)
//    {
//        for (x = 0; x < Width; x++)
//        {
//            HistGram[(int)data[y * Width + x]]++; // 统计每个灰度值的个数信息
//        }
//    }

//    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++)
//        ; // 获取最小灰度的值
//    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--)
//        ; // 获取最大灰度的值

//    if (MaxValue == MinValue)
//    {
//        return MaxValue; // 图像中只有一个颜色
//    }
//    if (MinValue + 1 == MaxValue)
//    {
//        return MinValue; // 图像中只有二个颜色
//    }

//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        Amount += HistGram[Y]; //  像素总数
//    }

//    PixelIntegral = 0;
//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        PixelIntegral += HistGram[Y] * Y; // 灰度值总数
//    }
//    SigmaB = -1;
//    for (Y = MinValue; Y < MaxValue; Y++)
//    {
//        PixelBack = PixelBack + HistGram[Y];                                               // 前景像素点数
//        PixelFore = Amount - PixelBack;                                                    // 背景像素点数
//        OmegaBack = (double)PixelBack / Amount;                                            // 前景像素百分比
//        OmegaFore = (double)PixelFore / Amount;                                            // 背景像素百分比
//        PixelIntegralBack += HistGram[Y] * Y;                                              // 前景灰度值
//        PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             // 背景灰度值
//        MicroBack = (double)PixelIntegralBack / PixelBack;                                 // 前景灰度百分比
//        MicroFore = (double)PixelIntegralFore / PixelFore;                                 // 背景灰度百分比
//        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); // g
//        if (Sigma > SigmaB)                                                                // 遍历最大的类间方差g
//        {
//            SigmaB = Sigma;
//            Threshold = Y;
//        }
//        if (Sigma < SigmaB) // 遍历最大的类间方差g
//        {
//            break;
//        }
//    }
//    return Threshold;
//}

/**
 * @brief GitHub copilot优化版本
 *
 * @param image
 * @param Width
 * @param Height
 * @return uint8
 */

uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height)
{
    uint8 HistGram[257] = {0}; // 将数组大小改为 257
    uint16 x, y;
    int16 Y;
    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 *data = image;

    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            HistGram[data[y * Width + x]]++;
        }
    }
    HistGram[255] = 0; // 将像素值为 255 的像素单独处理

    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++)
        ;
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MaxValue] == 0; MaxValue--)
        ;

    if (MaxValue == MinValue)
    {
        return MaxValue;
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;
    }
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
        PixelBack = PixelBack + HistGram[Y];
        PixelFore = Amount - PixelBack;
        OmegaBack = (double)PixelBack / Amount;
        OmegaFore = (double)PixelFore / Amount;
        PixelIntegralBack += HistGram[Y] * Y;
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;
        MicroBack = (double)PixelIntegralBack / PixelBack;
        MicroFore = (double)PixelIntegralFore / PixelFore;
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
        if (Sigma > SigmaB)
        {
            SigmaB = Sigma;
            Threshold = Y;
        }
    }

    return Threshold;
}
#define Sobel_Gx(addr, y, x) (addr[UP][RR] + 2 * addr[y][RR] + addr[DN][RR] - (addr[UP][LL] + 2 * addr[y][LL] + addr[DN][LL]))
#define Sobel_Gy(addr, y, x) (addr[UP][LL] + 2 * addr[UP][x] + addr[UP][RR] - (addr[DN][LL] + 2 * addr[DN][x] + addr[DN][RR]))
#define Sobel_G(addr, y, x) (abs(Sobel_Gx(addr, y, x)) + abs(Sobel_Gy(addr, y, x)))

/**
 * @brief 全局sobel方案
 *
 * @param Image_in 输入图像
 * @param Image_out 输出图像
 * @param Threshold 阈值
 */
void Image_Sobel(uint8 Image_in[Image_Height][Image_Width], uint8_t Image_out[Image_Height][Image_Width], uint16 Threshold)
{
    uint8_t i, j;
    uint8_t UP, DN, LL, RR;
    if (Threshold == 0) // 观察每点梯度值
    {
        for (i = 1; i < Image_Height - 1; i++)
        {
            DN = i + 1;
            UP = i - 1;
            for (j = 1; j < Image_Width - 1; j++)
            {
                RR = j + 1;
                LL = j - 1;
                Image_out[i][j] = Sobel_G(Image_in, i, j);
            }
        }
    }
    else // 根据梯度值二值化
    {
        for (i = 1; i < Image_Height - 1; i++)
        {
            DN = i + 1;
            UP = i - 1;
            for (j = 1; j < Image_Width - 1; j++)
            {
                RR = j + 1;
                LL = j - 1;
                Image_out[i][j] = (Sobel_G(Image_in, i, j) >= Threshold ? 0 : 255); // 修改的tft二值化图库函数：黑0；白1
            }
        }
    }
}




// canny测试代码，未进行修改维护
uint8 Sector[Image_Height][Image_Width]; // 像素梯度所在扇区
uint8 A[Image_Height][Image_Width];      // 梯度幅值
// uint8 ImgEdge[UROW][UCOL];//输出二值化图像

void CannyEdgeTest(uint8 org[Image_Height][Image_Width], uint8 lowThr)
{
    // wrh:这个标志位不能定义在函数内部，因为是一个大数组，百度 大数组，局部变量，会出bug。 你注意下，我当时这个问题困扰了三天。
    //    uint8 Sector[70][120]; //像素梯度所在扇区
    //    uint8 A[70][120];//幅值
    //    uint8 ImgEdge[70][120];
    float Temp[9] = {0};
    // 双阈值检测临时数组
    float TemMax = 0; // 临时最大值
    float theta = 0;  // 梯度方向
    int Xg = 0;       // X方向梯度
    int Yg = 0;       // Y方向梯度

    uint8 x, y;

    uint8 ratio = 2 / 1;            // 高低阈值检测比例
    uint8 highThr = ratio * lowThr; // 高阈值 50
    uint8 loss = 2;                 // 非极大值抑制补偿,增加边缘

    //==========求解梯度，幅度和方向===========
    for (y = 0; y < Image_Height; y++) // 最下面一行不处理
    {
        for (x = 0; x < Image_Width; x++) // 最上一列不处理
        {
            Xg = org[y][x] + org[y + 1][x] - org[y][x + 1] - org[y + 1][x + 1];  // 计算X方向梯度
            Yg = -org[y][x] + org[y + 1][x] - org[y][x + 1] + org[y + 1][x + 1]; // 计算Y方向梯度
            A[y][x] = Fsqrt(Xg * Xg + Yg * Yg);                                  // 求幅值，快速开平方算法
            Sector[y][x] = Atan2(Yg, Xg);                                        // 求梯度方向分区
        }                                                                        // end of for(i)
    }                                                                            // end of for(j)

    //===============非极大值抑制================
    for (y = 0; y < Image_Height; y++)
    {
        for (x = 0; x < Image_Width; x++)
        {
            if (0 == Sector[y][x]) // 水平方向
            {
                if (!((A[y][x] > A[y][x + 1] - loss) && A[y][x] > (A[y][x - 1] - loss)))
                {
                    A[y][x] = 0;
                }
            }                      // end of 0
            if (1 == Sector[y][x]) // 右上、左下
            {
                if (!(A[y][x] > (A[y - 1][x + 1] - loss) && A[y][x] > (A[y + 1][x - 1] - loss)))
                {
                    A[y][x] = 0;
                }
            }                      // end of 1
            if (2 == Sector[y][x]) // 竖直方向
            {
                if (!(A[y][x] > (A[y - 1][x] - loss) && A[y][x] > (A[y + 1][x] - loss)))
                {
                    A[y][x] = 0;
                }
            }                      // end of 2
            if (3 == Sector[y][x]) // 左上、右下
            {
                if (!(A[y][x] > (A[y - 1][x - 1] - loss) && A[y][x] > (A[y + 1][x + 1] - loss)))
                {
                    A[y][x] = 0;
                }
            } // end of 3
        }     // end of for(x)
    }         // end of for(y)

    //=============双阈值检测=================
    //    双阀值：
    //    一般的边缘检测算法用一个阀值来滤除噪声或颜色变化引起的小的梯度值，而保留大的梯度值。
    //    Canny算法应用双阀值，即一个高阀值和一个低阀值来区分边缘像素。如果边缘像素点梯度值大于高阀值，则被认为是强边缘点。
    //    如果边缘梯度值小于高阀值，大于低阀值，则标记为弱边缘点。小于低阀值的点则被抑制掉。这一步算法很简单。
    for (y = 0; y < Image_Height; y++)
    {
        for (x = 0; x < Image_Width; x++)
        {
            if (A[y][x] < lowThr) // 低于低阈值
            {
                Image_Use[y][x] = 255;
            }
            else if (A[y][x] > highThr) // 高于高阈值
            {
                Image_Use[y][x] = 0;
            }
            else
            {
                Temp[0] = A[y + 1][x - 1];
                Temp[1] = A[y + 1][x];
                Temp[2] = A[y + 1][x + 1];
                Temp[3] = A[y][x - 1];
                Temp[4] = A[y][x];
                Temp[5] = A[y][x + 1];
                Temp[6] = A[y - 1][x - 1];
                Temp[7] = A[y - 1][x];
                Temp[8] = A[y - 1][x + 1]; // 3x3区域内点
                QuickSort(Temp, 0, 8);     // 快速排序,低到高
                if (Temp[8] > highThr)
                {
                    Image_Use[y][x] = 0;
                    A[y][x] = 1;
                } // end of if
                else
                {
                    Image_Use[y][x] = 255;

                    A[y][x] = 0;
                } // end of if else
            }     // end of if else
        }         // end of for(x)
    }             // end of for(y)
} // end of

//------------------------------------------------------------------------------------------------------------------
//  @brief     动态阈值
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
// uint8 OSTU_GetThreshold(uint8 *image, uint16 col, uint16 row)
// {

//     uint16 Width = col;
//     uint16 Height = row;
//     int X;
//     uint16 Y;
//     uint8 *data = image;
//     int HistGram[GrayScale] = {0};

//     uint32 Amount = 0;
//     uint32 PixelBack = 0;
//     uint32 PixelIntegralBack = 0;
//     uint32 PixelIntegral = 0;
//     int32 PixelIntegralFore = 0;
//     int32 PixelFore = 0;
//     double OmegaBack = 0, OmegaFore = 0, MicroBack = 0, MicroFore = 0, SigmaB = 0, Sigma = 0; // 类间方差;
//     uint8 MinValue = 0, MaxValue = 0;
//     uint8 Threshold = 0;

//     for (Y = 0; Y < Image_Height; Y++) // Y<Image_Height改为Y =Image_Height；以便进行 行二值化
//     {
//         // Y=Image_Height;
//         for (X = 0; X < Width; X++)
//         {
//             HistGram[(int)data[Y * Width + X]]++; // 统计每个灰度值的个数信息
//         }
//     }

//     for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++)
//         ; // 获取最小灰度的值
//     for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--)
//         ; // 获取最大灰度的值

//     if (MaxValue == MinValue)
//     {
//         return MaxValue; // 图像中只有一个颜色
//     }
//     if (MinValue + 1 == MaxValue)
//     {
//         return MinValue; // 图像中只有二个颜色
//     }

//     for (Y = MinValue; Y <= MaxValue; Y++)
//     {
//         Amount += HistGram[Y]; //  像素总数
//     }

//     PixelIntegral = 0;
//     for (Y = MinValue; Y <= MaxValue; Y++)
//     {
//         PixelIntegral += HistGram[Y] * Y; // 灰度值总数
//     }
//     SigmaB = -1;
//     for (Y = MinValue; Y < MaxValue; Y++)
//     {
//         PixelBack = PixelBack + HistGram[Y];                                               // 前景像素点数
//         PixelFore = Amount - PixelBack;                                                    // 背景像素点数
//         OmegaBack = (double)PixelBack / Amount;                                            // 前景像素百分比
//         OmegaFore = (double)PixelFore / Amount;                                            // 背景像素百分比
//         PixelIntegralBack += HistGram[Y] * Y;                                              // 前景灰度值
//         PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             // 背景灰度值
//         MicroBack = (double)PixelIntegralBack / PixelBack;                                 // 前景灰度百分比
//         MicroFore = (double)PixelIntegralFore / PixelFore;                                 // 背景灰度百分比
//         Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); // g
//         if (Sigma > SigmaB)                                                                // 遍历最大的类间方差g
//         {
//             SigmaB = Sigma;
//             Threshold = (uint8)Y;
//         }
//     }
//     return Threshold;
// }
//////copilot优化的动态大津1.0
// uint8 OSTU_GetThreshold(uint8 *image, uint16 col, uint16 row)
// {
//     uint16 Width = col;
//     uint16 Height = row;
//     uint8 *data = image;
//     int HistGram[GrayScale] = {0};
//     uint32 Amount = Width * Height;
//     uint32 PixelBack = 0;
//     uint32 PixelIntegralBack = 0;
//     uint32 PixelIntegral = 0;
//     int32 PixelIntegralFore = 0;
//     int32 PixelFore = 0;
//     double OmegaBack = 0, OmegaFore = 0, MicroBack = 0, MicroFore = 0, SigmaB = 0, Sigma = 0;
//     uint8 MinValue = 0, MaxValue = 0;
//     uint8 Threshold = 0;

//     for (int i = 0; i < Amount; i++)
//     {
//         HistGram[data[i]]++;
//     }

//     for (MinValue = 0; MinValue < 255 && HistGram[MinValue] == 0; MinValue++)
//         ;
//     for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--)
//         ;

//     if (MaxValue == MinValue)
//     {
//         return MaxValue;
//     }
//     if (MinValue + 1 == MaxValue)
//     {
//         return MinValue;
//     }

//     PixelIntegral = 0;
//     for (int i = MinValue; i <= MaxValue; i++)
//     {
//         PixelIntegral += HistGram[i] * i;
//     }

//     SigmaB = -1;
//     for (int i = MinValue; i < MaxValue; i++)
//     {
//         PixelBack += HistGram[i];
//         PixelFore = Amount - PixelBack;
//         OmegaBack = (double)PixelBack / Amount;
//         OmegaFore = (double)PixelFore / Amount;
//         PixelIntegralBack += HistGram[i] * i;
//         PixelIntegralFore = PixelIntegral - PixelIntegralBack;
//         MicroBack = (double)PixelIntegralBack / PixelBack;
//         MicroFore = (double)PixelIntegralFore / PixelFore;
//         Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
//         if (Sigma > SigmaB)
//         {
//             SigmaB = Sigma;
//             Threshold = (uint8)i;
//         }
//     }
//     return Threshold;
// }

/**
 * @brief 将输入的灰度图像转化为二值化图像
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
void Image_Binarization(uint8 threshold, uint8 (*Image)[Image_Width])
{
    // unsigned char i, j;
    // for (i = 0; i < Image_Height; i++)
    // {
    //     for (j = 0; j < Image_Width; j++)
    //     {
    //         if (Image_Use[i][j] > threshold)
    //         {
    //             Image_Use[i][j] = 255;
    //         }
    //         else
    //             Image_Use[i][j] = 0;
    //     }
    // }
    uint32 i, j;

    uint16 temp = 0;

    for (j = 0; j < Image_Height; j++)
    {
        for (i = 0; i < Image_Width; i++)
        {
            temp = *(Image[0] + j * Image_Width + i);                              // 读取像素点
            if (j == 0 || j == Image_Height - 1 || i == 0 || i == Image_Width - 1) // 大津法加一个黑框
            {
                *(Image[0] + j * Image_Width + i) = 0;
            }
            else
            {
                if (temp >= threshold)
                    *(Image[0] + j * Image_Width + i) = 255;
                else
                    *(Image[0] + j * Image_Width + i) = 0;
            }
        }
    }
}
/*-----------------第二版八邻域-------------------*/
// 求绝对值函数
int my_abs(int value)
{
    if (value >= 0)
        return value;
    else
        return -value;
}
// 限幅函数
int16 limit_a_b(int16 x, int a, int b)
{
    if (x < a)
        x = a;
    if (x > b)
        x = b;
    return x;
}
// 求最小值
int16 limit1(int16 x, int16 y)
{
    if (x > y)
        return y;
    else if (x < -y)
        return -y;
    else
        return x;
}

// 寻找起始点函数，从第56行开始数
unsigned char left_point; // 记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
unsigned char Image_Get_LeftFlag(void)
{
    for (left_point = (Image_Width / 2); left_point > 3; left_point--)
    {
        if ((Image_Use[56][left_point] == 255) && (Image_Use[56][left_point - 1] == 0) && (Image_Use[56][left_point - 2] == 0))
        {
            break;
        }
    }
    return 1;
}

unsigned char right_point; // 记录第一个关键点的列坐标
unsigned char Image_Get_Rightflag(void)
{

    for (right_point = (Image_Width / 2); right_point < (Image_Width - 2); right_point++)
    {
        if ((Image_Use[56][right_point] == 255) && (Image_Use[56][right_point + 1] == 0) && (Image_Use[56][right_point + 2] == 0)) // 这里指针变量不能直接和值比较，需要解地址
        {
            break; // 这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
    return 1;
}

// 简介：通过已知的点来提取出所需要的边线
// 参数：total_L 需要找到点的数量，一般都是data_statics_left
uint8 l_border[Image_Height];    // 定义左线的数组，下标为行坐标，下标对应的值为列坐标，每行只有一个数组
uint8 r_border[Image_Height];    // 定义右线数组
uint8 center_line[Image_Height]; // 定义中线数组

#define threshold_max 255 * 6
#define threshold_min 255 * 2
// 简介：滤波函数，将图像中部分噪声去除
void Image_Filter(void)
{
    uint16 i, j;
    uint32 num = 0;
    for (i = 1; i < Image_Height - 1; i++)
    {
        for (j = 1; j < Image_Width - 1; j++)
        {
            // 统计8个方向的像素值
            num = Image_Use[i - 1][j - 1] + Image_Use[i - 1][j] + Image_Use[i - 1][j + 1] + Image_Use[i][j - 1] + Image_Use[i][j + 1] + Image_Use[i + 1][j - 1] + Image_Use[i + 1][j] + Image_Use[i + 1][j + 1];

            if (num >= threshold_max && Image_Use[i][j] == 0) // 如果黑点四周的8个点只有2个黑点
            {
                Image_Use[i][j] = 255;
            }
            if (num <= threshold_min && Image_Use[i][j] == 255) // 如果白点周围只有2个白点
            {
                Image_Use[i][j] = 0; // 过滤成黑
            }
        }
    }
}

// 给图像绘黑边，不然八邻域会出错，位置一定要放在显示的前面
void Image_DrawRectangle(void)
{
    uint8 i = 0;
    for (i = 0; i < Image_Height; i++)
    {
        Image_Use[i][0] = 0;
        Image_Use[i][Image_Width - 1] = 0;
        Image_Use[i][Image_Width - 2] = 0;
    }
    for (i = 0; i < Image_Width; i++)
    {
        Image_Use[0][i] = 0;
        Image_Use[1][i] = 0; // 图片底下两层变黑
    }
}

// 已知两点求图像的y=kx+b，截距我手动算的，应该没有太大问题
float two_points_k, two_points_b; // 全局变量代替指针传递斜率和截距
void Image_2points(uint8 x1, uint8 y1, uint8 x2, uint8 y2)
{
    two_points_k = (float)((y2 - y1) / (x2 - x1));
    two_points_b = (float)((y1 * x2 - x1 * y2) / (x2 - x1));
}

// 最小二乘法求斜率
float Imgae_Slope(uint8 begin, uint8 end, uint8 *border)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16 i = 0;
    float result = 0;
    static float resultlast = 0; // 记录上次结果，用来比对

    for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）
    {
        xsum += i;                // 对x坐标自增（行坐标）
        ysum += border[i];        // 对y坐标自增（列坐标）
        xysum += i * (border[i]); // xy坐标乘积自增
        x2sum += i * i;           // x坐标平方自增
    }
    if ((end - begin) * x2sum - xsum * xsum) // 这个在求斜率中会作为分母，因此需要判断是否为0
    {
        result = ((end - begin) * xysum - xsum * ysum) / ((end - begin) * x2sum - xsum * xsum);
        resultlast = result;
    }
    else
    {
        result = resultlast;
    }

    return result;
}

// 简介：计算斜率和截距
void Image_CountKB(uint8 start, uint8 end, uint8 *border, float *slope_rate, float *intercept)
{
    uint16 i, num = 0;
    uint16 xsum = 0, ysum = 0;
    float y_average, x_average;
    num = 0;
    xsum = 0;
    ysum = 0;
    y_average = 0;
    x_average = 0;
    for (i = start; i < end; i++) // 计算xy坐标的总值（行列坐标）
    {
        xsum += i;
        ysum += border[i];
        num++;
    }
    // 计算x,y坐标的平均值
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);
    }
    *slope_rate = Imgae_Slope(start, end, border);      // 计算斜率
    *intercept = y_average - (*slope_rate) * x_average; // 计算截距
}
struct Left_Edge
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
    unsigned char flag;   // 判断边界点是否找到
    uint8 grow;
};
struct Right_Edge
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
    unsigned char flag;   // 判断边界点是否找到
    uint8 grow;
};

struct Left_Edge Left[140];                    // 左边界结构体
struct Right_Edge Right[140];                  // 右边界结构体
unsigned char Left_Count, Right_Count;         // 记录左右边界点的个数
unsigned char grow_left, grow_right;           // 记录左右边界在八邻域时寻点的相对位置
unsigned char Left_Max = 140, Right_Max = 140; // 左右边界搜点时允许最大的搜点量
unsigned char Boundary_search_end = 30;        // 搜寻行数的最高行
uint16 cur_row, cur_col;                       // 当前行列
/**
 * @brief 八邻域巡线
 *
 * @param uint8 输入值为二值化处理后的数组
 * @example Image_Get_neighborhoods(Image_Use)
 */

void Image_Get_neighborhoods(uint8 (*Image_Use)[Image_Width])
{
    Left_Count = 0;
    Right_Count = 0;

    if (left_point != 3)
    {
        Left[0].row = 56;
        Left[0].column = left_point;
        Left[0].flag = 1;
        Left[0].grow = 2; // 初始生长方向为2
        cur_row = 56;
        cur_col = left_point;
        Left_Count++;
        while (Left_Max--) // 找140个
        {
            // 一 寻点生长
            // 0白1黑
            if (Image_Use[cur_row + 1][cur_col] == black && Image_Use[cur_row + 1][cur_col - 1] == white)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 0;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col - 1] == black && Image_Use[cur_row][cur_col - 1] == white)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 1;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row][cur_col - 1] == black && Image_Use[cur_row - 1][cur_col - 1] == white)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 2;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col - 1] == black && Image_Use[cur_row - 1][cur_col] == white)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 3;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col] == black && Image_Use[cur_row - 1][cur_col + 1] == white)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 4;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col + 1] == black && Image_Use[cur_row][cur_col + 1] == white)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 5;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row][cur_col + 1] == black && Image_Use[cur_row + 1][cur_col + 1] == white)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 6;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col + 1] == black && Image_Use[cur_row + 1][cur_col] == white)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 7;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else
            {
                break;
            }
            // 二 检验越界
            if (cur_row <= 3 || cur_row >= 57 || cur_col <= 3 || cur_col >= 97)
            {
                break;
            }
        }
    }
    // 采取左右对称，后面好写
    /*
    5	4	3
    6		2
    7	0	1
    */
    if (right_point != 97)
    {
        Right[0].row = 56;
        Right[0].column = right_point;
        Right[0].flag = 1;
        Right[0].grow = 2;
        cur_row = 56;
        cur_col = right_point;
        Right_Count++;
        while (Right_Max--)
        {
            // 0黑1白
            if (Image_Use[cur_row + 1][cur_col] == black && Image_Use[cur_row + 1][cur_col + 1] == white)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 0;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col + 1] == black && Image_Use[cur_row][cur_col + 1] == white)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 1;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row][cur_col + 1] == black && Image_Use[cur_row - 1][cur_col + 1] == white)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 2;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col + 1] == black && Image_Use[cur_row - 1][cur_col] == white)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 3;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col] == black && Image_Use[cur_row - 1][cur_col - 1] == white)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 4;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col - 1] == black && Image_Use[cur_row][cur_col - 1] == white)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 5;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row][cur_col - 1] == black && Image_Use[cur_row + 1][cur_col - 1] == white)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 6;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col - 1] == black && Image_Use[cur_row + 1][cur_col] == white)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 7;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else
                break;

            if (cur_row <= 3 || cur_row >= 57 || cur_col <= 3 || cur_col >= 97)
            {
                break;
            }
        }
    }
}

// 定义中线结构体
struct Mid_Line
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
};
struct Mid_Line mid[140];
uint8 Mid_Count;
// 取中线函数
void Get_Midpoint(void)
{
    uint8 left_c, right_c;
    uint8 i; // 中间值
    left_c = Left_Count;
    right_c = Right_Count; // 存入左右计数值
    for (i = 0; i < left_c; i--)
    {
        mid[i].row = (Left[i].row + Right[i].row) / 2;
        mid[i].column = (Left[i].column + Right[i].column) / 2;
    }
}
void Image_FillCross(uint8 *l_border, uint8 *r_border, uint16 total_num_l, uint16 total_num_r,
                     uint16 *dir_l, uint16 *dir_r, uint16 (*points_l)[2], uint16 (*points_r)[2])
{
    uint8 i;
    uint8 break_num_l = 0;
    uint8 break_num_r = 0;
    uint8 start, end;
    float slope_l_rate = 0, intercept_l = 0; // 左线斜率和截距
    uint8 break_num_l_low;
    uint8 break_num_r_low; // 定义左右相对较低的拐点，只会在未进十字的时候才会出现
    // 十字识别一：出十字（未入十字的情况也写道里面了）
    // 情景1：十字路口走到一半的时候，左线是由向上生长到向右生长，此时判断为一半的十字路口元素
    for (i = 1; i < total_num_l; i++) // 从左线的第一个点开始往上找
    {
        /*
        生长方向表（左边是右线的生长方向表，右边是左线的生长方向表）
        5   4   3       3   4   5
        6       2       2       6
        7   0   1       1   0   7
        */
        // 判断左线的较高拐点（在入十字的时候是唯一拐点）
        if (dir_l[i - 1] == 4 && dir_l[i] == 4 && dir_l[i + 3] == 6 && dir_l[i + 5] == 6 && dir_l[i + 7] == 6)
        {
            break_num_l = points_l[i][1]; // 传递y坐标，注意，这个坐标在图像中的点是由正上方向变为正左方向的转折点
            break;
        }
        if (dir_l[i - 1] == 2 && dir_l[i - 3] == 2 && dir_l[i - 5] == 2 && dir_l[i - 7] == 2 && dir_l[i] == 4 && dir_l[i + 1] == 4)
        {
            break_num_l_low = points_l[i][1]; // 传递低拐点的坐标
        }
    }
    for (i = 1; i < total_num_r; i++)
    {
        if (dir_r[i - 1] == 4 && dir_r[i] == 4 && dir_r[i + 3] == 6 && dir_r[i + 5] == 6 && dir_r[i + 7] == 6)
        {
            break_num_r = points_r[i][1]; // 传递y坐标
            break;
        }
        // 因为左右线完全对称，所以程序不会有太大的改动
        if (dir_r[i - 1] == 2 && dir_r[i - 3] == 2 && dir_r[i - 5] == 2 && dir_r[i - 7] == 2 && dir_r[i] == 4 && dir_l[i] == 4)
        {
            break_num_r_low = points_r[i][1]; // 传递低拐点的y坐标
        }
    }
    // 进一步判断是否处于十字之中，如果左下角和右下角的点均为白色那么就确实在10字之中
    if (break_num_l && break_num_r && Image_Use[Image_Height - 1][4] && Image_Use[Image_Height - 1][Image_Width - 4])
    {
        start = break_num_l - 15;                      // 将取得的点前移15个点做起始点
        start = limit_a_b(start, 0, Image_Height - 1); // 限幅
        end = break_num_l - 5;                         // 将取得的点前移5个点做终点
        Image_CountKB(start, end, l_border, &slope_l_rate, &intercept_l);
        // 线上补点
        for (i = break_num_l - 5; i < Image_Height - 1; i++)
        {
            l_border[i] = slope_l_rate * (i) + intercept_l; // 把终点往下每一行的全都求直线画点
            l_border[i] = limit_a_b(l_border[i], 4, 96);    // 限幅，这里最多到旁边的4行
            l_border[i] = 0;                                // 赋值为黑色
        }
        // 这是求右线的，基本和上面的一样的原理
        start = break_num_r - 15;                      // 起点
        start = limit_a_b(start, 0, Image_Height - 1); // 限幅
        end = break_num_r - 5;                         // 终点
        Image_CountKB(start, end, r_border, &slope_l_rate, &intercept_l);
        for (i = break_num_r - 5; i < Image_Height - 1; i++)
        {
            r_border[i] = slope_l_rate * (i) + intercept_l;
            r_border[i] = limit_a_b(r_border[i], 4, 96);
            l_border[i] = 0; // 赋值为黑色
            // 这里可能缺一步：补线，这里只是把线求出来而已
        }
    }

    // 情景2：如果还没有进入到十字
    // if ：如果四个拐点全部找到而且左下角和右下角是黑色
    if (break_num_r_low && break_num_l_low && break_num_l && break_num_r && (!Image_Use[Image_Height - 1][4]) && (!Image_Use[Image_Height - 1][Image_Width - 4]))
    {
        start = break_num_l_low + 3;                                      // 懒得转换了，之间取下面的点吧
        start = limit_a_b(start, 0, Image_Height - 1);                    // 限幅
        end = break_num_l_low + 10;                                       // 图像下方点更准确，取近一点
        end = limit_a_b(end, 0, Image_Height - 1);                        // 限幅
        Image_CountKB(start, end, l_border, &slope_l_rate, &intercept_l); // 以下方的点写出斜率
        // 开始向上补线，方向向上，故坐标自减
        for (i = break_num_l_low + 10; i > 2; i--)
        {
            l_border[i] = slope_l_rate * (i) + intercept_l;
            l_border[i] = limit_a_b(l_border[i], 4, 96);
            l_border[i] = 0;
        }
        start = break_num_r_low + 3;                                      // 懒得转换了，之间取下面的点吧
        start = limit_a_b(start, 0, Image_Height - 1);                    // 限幅
        end = break_num_r_low + 10;                                       // 图像下方点更准确，取近一点
        end = limit_a_b(end, 0, Image_Height - 1);                        // 限幅
        Image_CountKB(start, end, r_border, &slope_l_rate, &intercept_l); // 以下方的点写出斜率
        for (i = break_num_r_low + 10; i > 2; i--)
        {
            r_border[i] = slope_l_rate * (i) + intercept_l;
            r_border[i] = limit_a_b(l_border[i], 4, 96);
            r_border[i] = 0;
        }
    }
}

// 图像处理的函数都放在这里，这样就避免了定义问题
void Image_Run(void)
{
    uint8 i;

    i = Image_Get_LeftFlag();
    i = Image_Get_Rightflag();
    //	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count
    tft180_show_int(3, 80, left_point, 3);
    Image_Get_neighborhoods(Image_Use);
    //	tft180_show_int(3,120,points_l[l_count-1][0],3);
    //	Image_Get_neighborhoods(100,Image_Use);
    tft180_draw_line(0, 0, cur_col, cur_row, RGB565_RED); // 行坐标l_countl_count
    //	Get_Midpoint();
    for (i = 0; i < Left_Count; i++)
    {
        tft180_draw_point(Left[i].column, Left[i].row, RGB565_BLUE);
    }
    for (i = 0; i < Left_Count; i++)
    {
        tft180_draw_point(Left[i].column + 1, Left[i].row, RGB565_BLUE);
    }
    for (i = 0; i < Left_Count; i++)
    {
        tft180_draw_point(Left[i].column - 1, Left[i].row, RGB565_BLUE);
    }
    for (i = 0; i < Right_Count; i++)
    {
        tft180_draw_point(Right[i].column, Right[i].row, RGB565_RED);
    }
}

////一个函数的定义
// struct Left_Edge
//{
//     unsigned char row;                        //行坐标，省点内存就没设int
//     unsigned char column;                     //列坐标，同上
//     unsigned char flag;                       //判断边界点是否找到
// };
// struct Right_Edge
//{
//     unsigned char row;                        //行坐标，省点内存就没设int
//     unsigned char column;                     //列坐标，同上
//     unsigned char flag;                       //判断边界点是否找到
// };

// struct Left_Edge Left[140];                   //左边界结构体
// struct Right_Edge Right[140];                 //右边界结构体
// unsigned char Left_Count,Right_Count;         //记录左右边界点的个数
// unsigned char grow_left,grow_right;           //记录左右边界在八邻域时寻点的相对位置
// unsigned char Left_Max=140,Right_Max=140;     //左右边界搜点时允许最大的搜点量
// unsigned char Boundary_search_end=50;         //搜寻行数的最高行
// unsigned int temp[Image_Width];
////  * @brief 将输入的灰度图像转化为二值化图像
////  *
////  * @param Uint8 输入图像的地址
////  * @param Uint8 输出图像的地址
////  * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
////  */
// unsigned char left_point;                     //记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
// unsigned char Image_Get_LeftFlag(void)
//{
//     for(left_point=(Image_Width/2);left_point>0;left_point--)
//     {
//         if((temp[left_point]==1)&&(temp[left_point-1]==0)&&(temp[left_point-2]==0))
//         {
//             break;
//         }
//     }
//     return 1;
// }
///**
// * @brief 寻找是否存在有边界，无输入参数，参数选择全局变量，方便调用
// * @param 同上
// * @return 同上
// * @exception 同上
// *  */
// unsigned char right_point;                     //记录第一个关键点的列坐标
// unsigned char Image_Get_Rightflag(void)
//{
//
//    for(right_point=(Image_Width/2);right_point>0;right_point++)
//    {
//        if((temp[right_point]==1)&&(temp[right_point+1]==0)&&(temp[right_point+2]==0)) //这里指针变量不能直接和值比较，需要解地址
//        {
//            break;                            //这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
//        }
//    }
//    return 1;
//}

///**
// * @brief 八邻域寻边界
// *
// * @param unchar 必须为经过边框处理Draw_Frame后的图像才可以进行八领域扫线
// * @return 无
// * @exception 1.我默认左下角为（0，0）
// * 2.巡线原理就是找到第一个点，然后找附近的8个点，再选择下一个点来作为衍生，将得到的点记录即可得到边界
// * 3.这里是从第4行的点开始寻找，因为要留一行做起始八邻域点，还有边界两行已经改成黑了
// * 4.生长方向表和坐标表（可以随便定，这里也是一种方法）
// * row-1 col-1      row-1 col       row-1 col+1         7   0   6
// * row   col-1      row   col       row   col+1         4   *   5
// * row+1 col-1      row+1 col       row+1 col+1         3   1   2
// * 5.有些判断会存在grow_left!=是因为这种情况基本不会存在，而且出现的话就难以判断生长的方向
// */
// void Image_Get_neighborhoods(void)
//{
//    unsigned char i;                          //中间变量
//    Left_Count=0;                             //左右线最大计数值清0
//    Right_Count=0;

//    for(i=0;i<Image_Width;i++)
//    {
//        temp[i]=Image_Use[57][i];          //设置一个中间数组，（全局变量），避免参数的传入的问题
//    }

//    if(Image_Get_LeftFlag())                       //如果找到左边界点的话
//    {
//        Left[0].row=56;                 //第一个点初始行设为3
//        Left[0].column=left_point;       //第一个点的列坐标设为left_point
//        Left[0].flag=1;                  //第一个点已经找到
//        unsigned char current_row=56;        //初始化当前的行和列坐标
//        unsigned char current_column=left_point;
//        grow_left=8;                          //记录上一个生长点相对于现在的生长点的位置，这里设为8不干扰其他点
//        for(i=1;i<Left_Max;i++)               //开始找点，最多140个（已经有一个了）
//        {
//            if((current_row+1<Boundary_search_end)||(current_row>Image_Height-1))
//            {
//                break;                        //防止找点找到太高（因为用不上）或者防止出现点又回到原来的最大行（图片最下面那一行），即拐角
//            }
//            //一 左上黑，正上白，原生长正右（这个不一定），选择向左上方向生长
//            if((grow_left!=2)&&(Image_Use[current_row-1][current_column-1]==0)&&(Image_Use[current_row-1][current_column]==1))
//            {
//                current_row -=1;              //当前的行列坐标全部替换为新的坐标
//                current_column -=1;
//                Left_Count +=1;               //左边点的计数+1
//                grow_left=7;                  //更换为新生长点后，旧生长点相对于原生长点的位置（右下）
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;          //已经找到下一个生长点
//            }
//            //二 右上黑，正右白，向右上方向生长
//            else if((grow_left!=3)&&(Image_Use[current_row-1][current_column+1]==0)&&(Image_Use[current_row][current_column+1]==1))
//            {
//                current_row -=1;
//                current_column +=1;
//                Left_Count +=1;
//                grow_left=6;                  //旧生长点相对于新生长点的左下
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;          //已经找到下一个生长点
//            }
//            //三 正上黑，右上白，向正上生长
//            else if((Image_Use[current_row-1][current_column]==0)&&(Image_Use[current_row-1][current_column+1])==1)
//            {
//                current_row -=1;
//                Left_Count +=1;               //没有current_column的改变就说明不变
//                grow_left=0;                  //旧生长点相对于新生长点的正下
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;          //已经找到下一个生长点
//            }
//            //四 正左黑，左上白，向左生长
//            else if((grow_left!=5)&&(Image_Use[current_row][current_column-1])==0&&(Image_Use[current_row-1][current_column-1]==1))
//            {
//                current_column -=1;           //向左生长，列坐标-1
//                Left_Count +=1;
//                grow_left=4;
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;
//            }
//            //五 正右黑，右下白，向正右生长
//            else if((grow_left!=4)&&(Image_Use[current_row][current_column+1]==0)&&(Image_Use[current_row+1][current_column+1]==1))
//            {
//                current_column +=1;
//                Left_Count +=1;
//                grow_left=5;
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;
//            }
//            //六 左下黑，正左白，向左下生长
//            else if((grow_left!=6)&&(Image_Use[current_row+1][current_column-1]==0)&&(Image_Use[current_row][current_column-1]==1))
//            {
//                current_row +=1;
//                current_column -=1;
//                Left_Count +=1;
//                grow_left =3;
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;
//            }
//            //七 右下黑，正下白，向右下生长
//            else if((grow_left!=7)&&(Image_Use[current_row+1][current_column+1]==0)&&(Image_Use[current_row+1][current_column]==1))
//            {
//                current_row +=1;
//                current_column +=1;
//                Left_Count +=1;
//                grow_left =2;
//                Left[i].row=current_row;
//                Left[i].column=current_column;
//                Left[i].flag=1;
//            }
//            else//其他情况不会出现，出现的话就是断线了，就不找了
//            {
//                break;
//            }
//        }
//        //开始向右找点，原来的temp还是可以继续用（因为是同一行的）
//        if(Image_Get_Rightflag())                   //如果找到右边的点的话
//        {
//            Right[0].row=116;                 //第一个点初始行设为3
//            Right[0].column=left_point;       //第一个点的列坐标设为right_point
//            Right[0].flag=1;                  //第一个点已经找到
//            current_row=116;                  //初始化当前的行和列坐标
//            current_column=right_point;
//            grow_right=8;                     //生长起始点为原点
//            for(i=0;i<Right_Max;i++)          //开始生长
//            {
//                if((current_row<Boundary_search_end)||(current_row>(Image_Height-1))||(current_row+1<Boundary_search_end))
//                //防越界
//                {
//                    break;
//                }
//                //一 右上黑，正上白（左白右黑），向右上生长
//                if((current_column<Image_Width)&&(grow_right!=3)&&(Image_Use[current_row-1][current_column+1])&&(Image_Use[current_row-1][current_column]==1))
//                {
//                    current_row -=1;
//                    current_column +=1;
//                    Right_Count +=1;
//                    grow_right=6;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                //二 左上黑，正左白（上黑下白）
//                else if((grow_right!=2)&&(Image_Use[current_row-1][current_column-1]==0)&&(Image_Use[current_row][current_column-1]==1))
//                {
//                    current_row -=1;
//                    current_column -=1;
//                    Right_Count +=1;
//                    grow_right=7;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                //三 正上黑，左上白（上黑下白）
//                else if((Image_Use[current_row-1][current_column]==0)&&(Image_Use[current_row-1][current_column-1]==1))
//                {
//                    current_row -=1;
//                    Right_Count +=1;
//                    grow_right=0;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                //四 正右黑，右上白，
//                else if((Image_Use[current_row][current_column+1]==0)&&(Image_Use[current_row-1][current_column+1]==1))
//                {
//                    current_column +=1;
//                    Right_Count +=1;
//                    grow_right=5;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                //五 正左黑，左下白
//                else if((grow_right!=5)&&(Image_Use[current_row][current_column-1]==0)&&(Image_Use[current_row+1][current_column-1]==1))
//                {
//                    current_column -=1;
//                    Right_Count +=1;
//                    grow_right=4;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                //六 左下黑，正下白
//                else if((grow_right!=6)&&(Image_Use[current_row+1][current_column-1]==0)&&(Image_Use[current_row+1][current_column])==1)
//                {
//                    current_row +=1;
//                    current_column -=1;
//                    Right_Count +=1;
//                    grow_right=3;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                //七 右下黑，正右白
//                else if((grow_right!=7)&&(Image_Use[current_row+1][current_column+1]==0)&&(Image_Use[current_row][current_column+1]==1))
//                {
//                    current_row +=1;
//                    current_column +=1;
//                    Right_Count +=1;
//                    grow_right=2;
//                    Right[i].row=current_row;
//                    Right[i].column=current_column;
//                    Right[i].flag=1;
//                }
//                else
//                {
//                    break;
//                }
//            }
//        }
//    }
//}
