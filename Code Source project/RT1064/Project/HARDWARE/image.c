#include "image.h"
#include "mymath.h"
#include "control.h"
#include "stdlib.h"
#include "cross.h"
#include "circle.h"
#include "math.h" //后期处理掉
#define Sobel_Gx(addr, y, x) (addr[UP][RR] + 2 * addr[y][RR] + addr[DN][RR] - (addr[UP][LL] + 2 * addr[y][LL] + addr[DN][LL]))
#define Sobel_Gy(addr, y, x) (addr[UP][LL] + 2 * addr[UP][x] + addr[UP][RR] - (addr[DN][LL] + 2 * addr[DN][x] + addr[DN][RR]))
#define Sobel_G(addr, y, x) (abs(Sobel_Gx(addr, y, x)) + abs(Sobel_Gy(addr, y, x)))
// 转向矩阵
const int dir_front[4][2] = {{0, -1},
                             {1, 0},
                             {0, 1},
                             {-1, 0}};
const int dir_frontleft[4][2] = {{-1, -1},
                                 {1, -1},
                                 {1, 1},
                                 {-1, 1}};
const int dir_frontright[4][2] = {{1, -1},
                                  {1, 1},
                                  {-1, 1},
                                  {-1, -1}};

uint8 Image_Use_Robert[120][160]; // sobel二值化图像

float Finnal_err;
int ipts0_up_index, ipts1_up_index; // 定义左上和右上拐点的下标
int mid_row_first; // 中线行的起点
// flash参数统一定义
float begin_x = 5;   // 起始点距离图像中心的左右偏移量	8
float begin_y = 118; // 起始点距离图像底部的上下偏移量 120高度：35;100高	58
/*beginy值越小，初始的生长点与上框越近*/

float block_size = 7; // 自适应阈值的block大小
float clip_value = 2; // 自适应阈值的阈值裁减量

float thres = 120;                                  // 二值化阈值，主要用于找起始点(边线使用自适应阈值，不使用该阈值)
float line_blur_kernel = 9;                         // 边线三角滤波核的大小
float pixel_per_meter = 110;                        // 俯视图中，每个像素对应的长度 (厘米*10^3)/像素个数 调大意味着平移距离增大 原：102
float sample_dist = 0.022;                          // 边线等距采样的间距 动态调整，使其每两点距离刚好为1cm 为0.02时10个点间隔约为9cm
float angle_dist = 0.2;                             // 计算边线转角时，三个计算点的距离
float far_rate = 0.5;                               //
float aim_distance_flash = 0.68;                    // 预锚点长度
float aim_dist[5] = {0.68, 0.78, 0.88, 0.98, 1.08}; // 多个预瞄点长度，间隔5cm，34~54个点，用于速度模糊控制，而不是偏差角计算！

// float xielv_left_y_to_end, xielv_right_y_to_end; // 在逆透视后得坐标系建得斜率

// bool Lpt00_found, Lpt11_found;     // 其中 far_N_Lpt0_found 表示是否找到了反向逆透视后的左边L角点，far_N_Lpt1_found 表示是否找到了反向逆透视后的右边L角点。
// bool N_Lpt00_found, N_Lpt11_found; // 这个变量用于表示是否找到了逆逆透视后的左边L角点和右边L角点。其中，far_N_Lpt0_found表示左边L角点是否被找到，far_N_Lpt1_found表示右边L角点是否被找到。

// int Lpt00_rpts00s_id, Lpt11_rpts11s_id;   // 远角点L角点的数组索引值
// int N_Lpt00_rpts00s_id, Lpt11_rpts11s_id; // 这两个变量是用于记录L角点在反向逆透视后的rpts0s数组中的索引值

uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH];
uint8 Image_Use_Robert[IMAGE_HEIGHT][IMAGE_WIDTH];

/**
 * @brief 压缩图像为我们需要的大小，这里我们采用不压缩，即直接对原图进行处理
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */
void Image_Compress(void)
{
    //    int i, j, row, line;
    //    const float pro_h = PRIMEVAL_HEIGHT / IMAGE_HEIGHT, pro_w = PRIMEVAL_WIDTH / IMAGE_WIDTH; // 根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
    //    if (mt9v03x_finish_flag)                                                                  // 采集完成再进行转换
    //    {
    //        for (i = 0; i < IMAGE_HEIGHT; i++) // 遍历图像的每一行，从第零行到第59行。
    //        {
    //            row = i * pro_h + 0.5;
    //            for (j = 0; j < IMAGE_WIDTH; j++) // 遍历图像的每一列，从第零列到第79列。
    //            {
    //                line = j * pro_w + 0.5;
    //                Image_Use[i][j] = mt9v03x_image[row][line]; // mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
    //            }
    //        }
    //        mt9v03x_finish_flag = 0; // 清除标志位
    //    }
    uint8_t div_h, div_w;
    uint32_t temp_h = 0;
    uint32_t temp_w = 0;
    uint32_t row_start = 0;
    uint32_t lin_start = 0;

    div_h = PRIMEVAL_HEIGHT / IMAGE_HEIGHT;
    div_w = PRIMEVAL_WIDTH / IMAGE_WIDTH;

    // 从中心取图像
    if (IMAGE_HEIGHT * div_h != PRIMEVAL_HEIGHT)
    {
        row_start = (PRIMEVAL_HEIGHT - IMAGE_HEIGHT * div_h) / 2;
        temp_h = row_start;
    }
    if (IMAGE_WIDTH * div_w != PRIMEVAL_WIDTH)
    {
        lin_start = (PRIMEVAL_WIDTH - IMAGE_WIDTH * div_w) / 2;
    }
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        temp_w = lin_start;
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            Image_Use[i][j] = mt9v03x_image[temp_h][temp_w];
            temp_w += div_w;
        }
        temp_h += div_h;
    }
}
/**
 * @brief GitHub copilot优化版本大津法
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
/**
 * @brief 全局sobel方案
 *
 * @param Image_in 输入图像
 * @param Image_out 输出图像
 * @param Threshold 阈值
 */
void Image_Sobel(uint8 Image_in[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t Image_out[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 Threshold)
{
    uint8_t i, j;
    uint8_t UP, DN, LL, RR;
    if (Threshold == 0) // 观察每点梯度值
    {
        for (i = 1; i < IMAGE_HEIGHT - 1; i++)
        {
            DN = i + 1;
            UP = i - 1;
            for (j = 1; j < IMAGE_WIDTH - 1; j++)
            {
                RR = j + 1;
                LL = j - 1;
                Image_out[i][j] = Sobel_G(Image_in, i, j);
            }
        }
    }
    else // 根据梯度值二值化
    {
        for (i = 1; i < IMAGE_HEIGHT - 1; i++)
        {
            DN = i + 1;
            UP = i - 1;
            for (j = 1; j < IMAGE_WIDTH - 1; j++)
            {
                RR = j + 1;
                LL = j - 1;
                Image_out[i][j] = (Sobel_G(Image_in, i, j) >= Threshold ? 0 : 255); // 修改的tft二值化图库函数：黑0；白1
            }
        }
    }
}

/**
 * @brief 将输入的灰度图像转化为二值化图像，弃用
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
void Image_Binarization(uint8 threshold, uint8 (*Image)[IMAGE_WIDTH])
{
    uint32 i, j;
    uint16 temp = 0;

    for (j = 0; j < IMAGE_HEIGHT; j++)
    {
        for (i = 0; i < IMAGE_WIDTH; i++)
        {
            temp = *(Image[0] + j * IMAGE_WIDTH + i);                              // 读取像素点
            if (j == 0 || j == IMAGE_HEIGHT - 1 || i == 0 || i == IMAGE_WIDTH - 1) // 大津法加一个黑框
            {
                //                *(Image[0] + j * IMAGE_WIDTH + i) = 0;
            }
            else
            {
                if (temp >= threshold)
                    *(Image[0] + j * IMAGE_WIDTH + i) = 255;
                else
                    *(Image[0] + j * IMAGE_WIDTH + i) = 0;
            }
        }
    }
}

//

/**
 * @brief 滤波函数，将图像中部分噪声去除
 *
 *
 * @return 无
 */
void Image_Filter(void)
{
    uint16 i, j;
    uint32 num = 0;
    for (i = 1; i < IMAGE_HEIGHT - 1; i++)
    {
        for (j = 1; j < IMAGE_WIDTH - 1; j++)
        {
            // 统计8个方向的像素值
            num = Image_Use_Robert[i - 1][j - 1] + Image_Use_Robert[i - 1][j] + Image_Use_Robert[i - 1][j + 1] + Image_Use_Robert[i][j - 1] + Image_Use_Robert[i][j + 1] + Image_Use_Robert[i + 1][j - 1] + Image_Use_Robert[i + 1][j] + Image_Use_Robert[i + 1][j + 1];

            if (num >= THRESHOLD_MAX && Image_Use_Robert[i][j] == 0) // 如果黑点四周的8个点只有2个黑点
            {
                Image_Use_Robert[i][j] = 255;
            }
            if (num <= THRESHOLD_MIN && Image_Use_Robert[i][j] == 255) // 如果白点周围只有2个白点
            {
                Image_Use_Robert[i][j] = 0; // 过滤成黑
            }
        }
    }
}

/**
 * @brief 绘制一个矩形，将图片的边缘变为黑色
 * @example Image_DrawRectangle();
 * @explain 该函数将图片的边缘变为黑色，以便后面的边缘提取,
 * 给图像绘黑边，不然八邻域会出错，位置一定要放在显示的前面
 */
void Image_DrawRectangle(void)
{
    uint8 i = 0;
    for (i = 0; i < IMAGE_HEIGHT; i++)
    {
        Image_Use[i][0] = 0;
        Image_Use[i][1] = 0;
        Image_Use[i][IMAGE_WIDTH - 1] = 0;
        Image_Use[i][IMAGE_WIDTH - 2] = 0;
    }
    for (i = 0; i < IMAGE_WIDTH; i++)
    {
        Image_Use[0][i] = 0;
        Image_Use[1][i] = 0;
    }
}

// /**
//  * @brief 坡道判断函数
//  *
//  * @param void
//  * @example Image_Run()
//  * @explanation  判断左右线相交的行坐标不断增加，从而判断出为坡道
//  * （想过用坡道的拐点左右的斜率来判断坡道，但是应该找不到）
//  */
// void Image_Ramp(void)
// {
//     uint8 i;                        // 中间变量
//     uint8 Ramp_flag, Last_Rampflag; // 坡道标志位，以及上一个状态标志位
//     if (Gather_flag == 1)           // 找到坡道
//     {
//         for (i = 0; i < Gather_Count; i++)
//         {
//             if ((Gather_row[i] > Gather_row[i + 1]) && (Gather_row[i + 1] > Gather_row[i + 2])) // 表示正在靠近坡道，最高行不断上升
//             {
//                 Last_Rampflag = Ramp_flag; // 记录上一个状态
//                 Ramp_flag = 1;             // 标志位为1，表示还没上坡道，还在坡道附近的直道
//             }
//             else if ((Gather_row[i] < Gather_row[i + 1]) && (Gather_row[i + 1] < Gather_row[i + 2])) // 最高行变小，表示已经上坡道了
//             {
//                 Last_Rampflag = Ramp_flag;
//                 Ramp_flag = 2; // 标志位为2，表示已经上坡道了，此时需要加速
//                 // Control_SpeedUp();//小车加速（电控的代码）
//             }
//             else
//             {
//                 Last_Rampflag = Ramp_flag;
//                 Ramp_flag = 0;
//             }
//         }
//     }
//     if (!Gather_flag && !Ramp_flag && Last_Rampflag == 2) // 判断为下坡状态
//     {
//         // Control_SpeedDown();//小车减速（电控的代码）
//     }
// }

// /**
//  * @brief 计算左侧图像中左边线生长方向与给定方向相同的数量
//  *
//  * @param Direction 给定的生长方向
//  * @return uint8 相同方向的数量
//  */
// int Image_LeftGrowDirection(uint8 end, uint8 Direction)
// {
//     uint8 i, direction_count;
//     for (i = 0; i < end; i++)
//     {
//         if (Left[i].grow == Direction) // 如果方向一致的话，就计数自增
//         {
//             direction_count++;
//         }
//     }
//     return direction_count;
// }

// /**
//  * @brief 扫描60行中的某一行的黑白跳变的函数（检测斑马线）
//  *
//  * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
//  * @return uint8 返回黑白跳变的次数
//  */
// uint8 Image_Scan_Row(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_row)
// {
//     uint8 i;                     // 中间变量
//     uint8 BLACK_WHITE_count = 0; // 黑白跳变的计数
//     for (i = 0; i < IMAGE_WIDTH - 1; i++)
//     {
//         if (Image_Use[target_row][i] != Image_Use[target_row][i + 1]) // 如果不相等的话，就说明是黑白跳变
//         {
//             BLACK_WHITE_count++;
//         }
//     }
//     return BLACK_WHITE_count;
// }

/*************************************************************************
 *  函数名称：void blur_points();
 *  功能说明：点集三角滤波
 *  参数说明：输入边线数组，边线总个数，输出边线数组，点集范围
 *  函数返回：无
 *  修改时间：2022年10月4日
 *  备    注：
 * 例如：kernel = 5
 *  xi = (0*xi-3 + 1*xi-2 + 2*xi-1 + 3*xi + 2*xi+1 + 1*xi+2 + 0*xi+3)/9
 *  yi 同理
 *************************************************************************/

void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++)
        {
            pts_out[i][0] += (float)pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
            pts_out[i][1] += (float)pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}

uint8 Image_Scan_Column(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_column)
{
    uint8 i;
    uint8 BLACK_WHITE_count = 0; // 黑白跳变的计数
    uint8 black_white_count = 0; // 黑白跳变的计数
    for (i = 0; i < IMAGE_HEIGHT; i++)
    {
        if (Image_Use[i][target_column] != Image_Use[i + 1][target_column]) // 如果不相等的话，就说明是黑白跳变
        {
            black_white_count++;
        }
    }
    return black_white_count;
}

uint8 touch_boundary0; // 左边线走到图像左边界
uint8 touch_boundary1; // 右边线走到图像右边界

uint8 touch_boundary_up0; // 左边线走到图像上边界
uint8 touch_boundary_up1; // 右边线走到图像上边界

uint8 touch_boundary00; // 左边线走到图像左边界
uint8 touch_boundary11; // 右边线走到图像右边界

uint8 touch_boundary_up00; // 左边线走到图像上边界
uint8 touch_boundary_up11; // 右边线走到图像上边界

#define ROAD_WIDTH (0.39) // 赛道宽度45cm 适时调整 注意：应用方案三时情况特殊为负数-0.40,正常0.43

float xielv_left, xielv_right; // 找到拐点时左右边线到拐点的斜率

int L_x0, L_x1, L_y0, L_y1; // 十字得近处两个L点的坐标

int low_x0, low_y0, low_x1, low_y1; // 十字得近处两个L点的坐标得下面2个点，用来计算斜率来补线

image_t img_raw = DEF_IMAGE(NULL, IMAGE_WIDTH, IMAGE_HEIGHT);

// 左右边丢线
uint8 loseline0;
uint8 loseline1;
int ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int ipts0_num;                // 存放边线像素点个数(左)
int ipts1_num;                // 存放边线像素点个数(右)

int right_up[POINTS_MAX_LEN][2];
int right_up_num;

uint8 loseline00;
uint8 loseline11;

uint8 loseline000;
uint8 loseline111;

int ipts00[100][2];
int ipts11[100][2]; // 巡线上边线点的个数
int ipts00_num;
int ipts11_num;

int ipts000[POINTS_MAX_LEN][2];
int ipts111[POINTS_MAX_LEN][2]; // 巡线上边线点的个数
int ipts000_num;
int ipts111_num;

// 逆透视变换后左右边线
float rpts0[POINTS_MAX_LEN][2];
float rpts1[POINTS_MAX_LEN][2];
int rpts0_num, rpts1_num;

// // 逆透视变换后左右边线（上边线）
// float rpts00[POINTS_MAX_LEN][2];
// float rpts11[POINTS_MAX_LEN][2];
// int rpts00_num, rpts11_num;

// 逆透视变换后左右边线再三角滤波后的边线数组
float rpts0b[POINTS_MAX_LEN][2];
float rpts1b[POINTS_MAX_LEN][2];
int rpts0b_num, rpts1b_num;

// // 逆透视变换后左右边线再三角滤波后的边线数组（上边线）
// float rpts00b[POINTS_MAX_LEN][2];
// float rpts11b[POINTS_MAX_LEN][2];
// int rpts00b_num, rpts11b_num;

// 逆透视变换后左右边线再三角滤波后再等距采样的数组
float rpts0s[POINTS_MAX_LEN][2];
float rpts1s[POINTS_MAX_LEN][2];
int rpts0s_num, rpts1s_num;

// // 逆透视变换后左右边线再三角滤波后再等距采样的数组（上边线）
// float rpts00s[100][2];
// float rpts11s[100][2]; // 限制长度前面的一样
// int rpts00s_num, rpts11s_num;

// 局部角度变化率
float rpts0a[POINTS_MAX_LEN];
float rpts1a[POINTS_MAX_LEN];
int rpts0a_num, rpts1a_num;

// // 局部角度变化率（上边线）
// float rpts00a[POINTS_MAX_LEN];
// float rpts11a[POINTS_MAX_LEN];
// int rpts00a_num, rpts11a_num;

// 左右边线局部角度变化率+非极大抑制
float rpts0an[POINTS_MAX_LEN];
float rpts1an[POINTS_MAX_LEN];

int rpts0an_num, rpts1an_num;

// // 左右边线局部角度变化率+非极大抑制（上边线）
// float rpts00an[POINTS_MAX_LEN];
// float rpts11an[POINTS_MAX_LEN];

// int rpts00an_num, rpts11an_num;
// int rpts00_num, rpts11_num;

int x0_first, y0_first, x1_first, y1_first; // 左右边线第一个点的坐标

int Mid_line[POINTS_MAX_LEN][2]; // 定义中线数组
int mid_num;                     // 中线数组点数

int rptsc0_num, rptsc1_num;

int rptsc00_num, rptsc11_num;
// SOBEL二值化图像
// 左右跟踪后的中线
float (*rpts)[2];
int rpts_num;
// 归一化最终中线
float rptsn[POINTS_MAX_LEN][2];
int rptsn_num;
int aim_idx[1]; // 多个预锚点位置

float aim_distance;

float error[1];

float ave_error; // 速度控制输入变量

void Find_Borderline(void)
{
    int x1, y1;
    int x2, y2;
    // 迷宫巡线是否走到左右边界
    touch_boundary0 = 0; // 清零
    touch_boundary1 = 0;

    // 迷宫巡线是否走到上边界
    touch_boundary_up0 = 0; // 清零
    touch_boundary_up1 = 0;

    // 底边扫线防止丢线 注意：由于sobel边缘检测特殊性（黑框），此丢线标志仅适用Ostu方案
    loseline0 = 0;
    loseline1 = 0;
    /*添加*/
    //	begin_y=Image_Get_LeftPoint(117);
    uint8 uthres = 1;
    // 寻左边线
    x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
    // 标记种子起始点(后续元素处理要用到)
    x0_first = x1;
    y0_first = y1;
    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]); // 求数组的长度
    // 扫底下五行，寻找跳变点

    for (; y0_first > begin_y - 100; y0_first--) // 从所选的行，向上扫50次，每次从中间向左线扫
    {
        for (; x0_first > 0; x0_first--)                             // 在选的每行中，从中间向左线扫
            if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres) // 如果扫到黑点（灰度值为0），就从该点开始扫线
                goto out1;                                           // 开始扫左线
        x0_first = img_raw.width / 2 - begin_x;                      // 每次每一行扫完，都把x0_first归位
    }
    // 如果扫不到的话，判定左边的底边丢线
    loseline0 = 1; // 底边丢线
out1:              // 从起始点开始执行扫线
{
    if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres) // 如果这个点是白色（且左边是黑色的话）
    {
        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num); // 开始跑迷宫
    }
    else
        ipts0_num = 0; // 如果不是的话，就不用跑了，求得的number记为0
}

    // 寻右边线
    x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

    // 标记种子起始点(后续元素处理要用到)
    x1_first = x2;
    y1_first = y2;

    ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
    for (; y1_first > begin_y - 50; y1_first--)
    {
        for (; x1_first < img_raw.width - 1; x1_first++)
            if (AT_IMAGE(&img_raw, x1_first + 1, y1_first) < uthres)
                goto out2;
        x1_first = img_raw.width / 2 + begin_x;
    }
    loseline1 = 1; // 底边丢线
out2:
{
    if (AT_IMAGE(&img_raw, x1_first, y1_first) >= uthres)
        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts1, &ipts1_num);
    else
        ipts1_num = 0;
}
}

// void Find_Borderline_Second_Left(void)
// {
//     uint8 uthres = 1;
//     if (touch_boundary0 == 1)
//     {
//         // 迷宫巡线是否走到左右边界
//         touch_boundary00 = 0;    // 清零
//         touch_boundary_up00 = 0; // 清零
//         loseline00 = 0;
//         x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
//         x0_first = 2;
//         y0_first = ipts0[ipts0_num - 1][1] - 5;

//         ipts00_num = sizeof(ipts00) / sizeof(ipts00[0]); // 求数组的长度
//         // 扫底下五行，寻找跳变点
//         for (; y0_first > 20; y0_first--) // 从所选的行，向上扫5次，每次从中间向左线扫
//         {
//             if (AT_IMAGE(&img_raw, x0_first, y0_first) < uthres) // 如果扫到黑点（灰度值为0），就从该点开始扫线
//             {
//                 goto out1; // 开始扫左线
//             }
//         }
//         // 如果扫不到的话，判定左边的底边丢线
//         loseline00 = 1; // 底边丢线
//     out1:               // 从起始点开始执行扫线
//     {
//         // if (AT_IMAGE(&img_raw, x0_first+1, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
//         Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts00, &ipts00_num); // 开始跑迷宫
//         // else
//         // 	ipts00_num = 0;//如果不是的话，就不用跑了，求得的number记为0
//     }
//     }
// }

void Find_Borderline_Second(void)
{
    int x1, y1;
    int x2, y2;
    uint8 uthres = 1;
    if (touch_boundary0 == 1)
    {
        // 迷宫巡线是否走到左右边界
        touch_boundary00 = 0; // 清零
        touch_boundary11 = 0;

        // 迷宫巡线是否走到上边界
        touch_boundary_up00 = 0; // 清零
        touch_boundary_up11 = 0;

        // 底边扫线防止丢线 注意：由于sobel边缘检测特殊性（黑框），此丢线标志仅适用Ostu方案
        loseline00 = 0;
        loseline11 = 0;

        /*添加*/
        //	begin_y=Image_Get_LeftPoint(117);ipts0_num

        //    uint8 uthres = ostu();
        // 寻左边线
        x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
        int TH;

        // 标记种子起始点(后续元素处理要用到)
        // x0_first = x1;
        // y0_first = ipts0[ipts0_num-1][1]-5;

        // ipts00_num = sizeof(ipts00) / sizeof(ipts00[0]); // 求数组的长度
        // // 扫底下五行，寻找跳变点
        // for (; y0_first >20; y0_first--)//从所选的行，向上扫5次，每次从中间向左线扫
        // {
        //     for (; x0_first > 0; x0_first--)//在选的每行中，从中间向左线扫
        //         if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)//如果扫到黑点（灰度值为0），就从该点开始扫线
        //             goto out1;//开始扫左线
        //     x0_first = img_raw.width / 2 - begin_x;//每次每一行扫完，都把x0_first归位
        // }
        x0_first = 10;
        y0_first = ipts0[ipts1_num - 0][1] - 5;

        ipts00_num = sizeof(ipts00) / sizeof(ipts00[0]); // 求数组的长度
        // 扫底下五行，寻找跳变点
        for (; y0_first > 10; y0_first--) // 从所选的行，向上扫5次，每次从中间向左线扫
        {
            if (AT_IMAGE(&img_raw, x0_first, y0_first) < uthres) // 如果扫到黑点（灰度值为0），就从该点开始扫线
            {
                goto out1; // 开始扫左线
            }
        }
        // 如果扫不到的话，判定左边的底边丢线
        loseline00 = 1; // 底边丢线
    out1:               // 从起始点开始执行扫线
    {
        // if (AT_IMAGE(&img_raw, x0_first+1, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts00, &ipts00_num); // 开始跑迷宫
        // else
        // 	ipts00_num = 0;//如果不是的话，就不用跑了，求得的number记为0
    }
    }
    if (touch_boundary1 == 1)
    {
        // 寻右边线
        x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

        // 标记种子起始点(后续元素处理要用到)
        x1_first = 149;
        y1_first = ipts1[ipts1_num - 1][1] - 5;
        ;

        ipts11_num = sizeof(ipts11) / sizeof(ipts11[0]);
        for (; y1_first > 10; y1_first--)
        {
            if (AT_IMAGE(&img_raw, x1_first, y1_first) < uthres)
            {
                goto out2;
            }
        }
        loseline11 = 1; // 底边丢线
    out2:
    {
        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts11, &ipts11_num);
    }
    }
}

/*三次扫线*/
void Find_Borderline_Third(void)
{
    int x1, y1;
    int x2, y2;
    uint8 uthres = 1;
    if (loseline0 == 1)
    {
        loseline000 = 0;
        loseline111 = 0;

        // 寻左边线
        x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
        int TH;

        x0_first = 10;
        y0_first = 60;

        ipts000_num = sizeof(ipts000) / sizeof(ipts000[0]); // 求数组的长度
        // 扫底下五行，寻找跳变点
        for (; y0_first > 10; y0_first--) // 从所选的行，向上扫5次，每次从中间向左线扫
        {
            if (AT_IMAGE(&img_raw, x0_first, y0_first) < uthres) // 如果扫到黑点（灰度值为0），就从该点开始扫线
            {
                goto out1; // 开始扫左线
            }
        }
        // 如果扫不到的话，判定左边的底边丢线
        ipts000_num = 0;
        loseline000 = 1; // 底边丢线
    out1:                // 从起始点开始执行扫线
    {
        // if (AT_IMAGE(&img_raw, x0_first+1, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts000, &ipts000_num); // 开始跑迷宫
        // else
        // 	ipts00_num = 0;//如果不是的话，就不用跑了，求得的number记为0
    }
    }
    if (loseline1 == 1)
    {
        // 寻右边线
        x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

        // 标记种子起始点(后续元素处理要用到)
        x1_first = 149;
        y1_first = 60;

        ipts111_num = sizeof(ipts111) / sizeof(ipts111[0]);
        for (; y1_first > 10; y1_first--)
        {
            if (AT_IMAGE(&img_raw, x1_first, y1_first) < uthres)
            {
                goto out2;
            }
        }
        loseline11 = 1; // 底边丢线
    out2:
    {
        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts111, &ipts111_num);
    }
    }
}

#define AT AT_IMAGE
#define MAX_WIDTH 88 // 定义图像中
void Left_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
{
    zf_assert(img && img->data); // 不满足则退出执行
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1); // 保证block_size为奇数
    //    int half = block_size / 2;                       //上交方案
    int half = 0;                                                                                                      // 方案二
    int step = 0, dir = 0, turn = 0;                                                                                   // step表示前进的步数；dir通过改变索引改变当前小人朝向的方向
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4) // 保证block不出界
    /*保证step步数小于Num   保证x（列）坐标>0同时小于宽度-1   保证y（行）坐标大于0小于高度-1*/
    {
        int local_thres = 1;
        //        int local_thres;
        // 自适应二值化
        /*
            for (int dy = -half; dy <= half; dy++)  // for循环用来计算block区域的像素值之和（自适应阈值）
            {
                for (int dx = -half; dx <= half; dx++)
                {
                    local_thres += AT(img, x + dx, y + dy);
                }
            }
            local_thres /= block_size * block_size;
            local_thres -= clip_value;   // (x,y)点block区域内的阈值
        */
        int current_value = AT(img, x, y);                                                   // 当前像素点灰度值
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);             // 正前方像素点灰度值 （dir=0 下；dir=1 右；dir=2 上；dir=3 左）
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]); // 左前方像素点灰度值 （dir=0左下；dir=1 右下；dir=2 右上；dir=3 左上 ）
        //=======添加部分=======（限制条件）
        /*  当扫点的列坐标到左黑框边界且行坐标小于20    列坐标到右边的黑框边界  行坐标为1   行坐标为88的同时步数已经大于19*/
        if ((x == 2 && y < img->height - 70) || x == img->width - 2 || y == 1 || (y == 10 && step > 19)) // 30修改后能扫线
        {
            if (x == 2 /*|| x== img->width - 2*/)
                touch_boundary0 = 1; // 左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
            if (y == 2)
                touch_boundary_up0 = 1; // 走到顶边，判断坡道or障碍
            break;
        }
        //=======添加部分=======
        if (front_value < local_thres) // 前进方向像素为黑色
        {
            dir = (dir + 1) % 4; // 遇到前方为黑色需要右转一次
            turn++;
        }
        else if (frontleft_value < local_thres) // 前方像素为白色，且左前方像素为黑色
        {

            x += dir_front[dir][0];
            y += dir_front[dir][1];
            if (x > 2) // 判断起始点
            {
                pts[step][0] = x; // 用来存放边线坐标信息
                pts[step][1] = y;
                step++;
                turn = 0;
            }

            // AT(img,x,y) = RGB565_GREEN;
        }
        else // 前方为白色，左前方为白色（墙角）
        {
            x += dir_frontleft[dir][0]; // 遇到墙角要斜着走
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4; // 遇到墙角要左转一次
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            // AT(img,x,y) = RGB565_GREEN;
        }
    }
    // 丢线标志，否则由于sobel特殊性会一直往上巡线
    if (step < 20 && touch_boundary0)
        loseline0 = 1;
    // 记录边线数目
    *num = step;
}

/*************************************************************************
 *  函数名称：void Right_Adaptive_Threshold();
 *  功能说明：自定义阈值二值化+迷宫算法（右边线）
 *  参数说明：获取的图像数组，输出数组（备用image0），图像宽，图像高
 *  函数返回：无
 *  修改时间：2023年5月23日
 *  备    注： 增加丢线判断
 * 前进方向定义：
 *   0
 * 3   1
 *   2
 *************************************************************************/
void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
{
    uint8 i;
    zf_assert(img && img->data);
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1);
    //       int half = block_size / 2;        //上交方案
    int half = 0; // 方案二
    int step = 0, dir = 0, turn = 0;
    while (step < *num && 0 < x && x < img->width - 1 && half < y && y < img->height - half - 1 && turn < 4)
    {
        int local_thres = 1;
        //        int local_thres;
        // 自适应二值化
        /*
                for (int dy = -half; dy <= half; dy++) {
                    for (int dx = -half; dx <= half; dx++) {
                        local_thres += AT(img, x + dx, y + dy);
                    }
                }
                local_thres /= block_size * block_size;
                local_thres -= clip_value;
        */
        int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
        //=======添加部分=======
        if ((x == img->width - 2 && y < img->height - 70) || x == 1 || y == 1 || (y == 10 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线，直到边线个数达到最大为止
        {
            if (x == img->width - 2 /*|| x==1*/)
            {
                touch_boundary1 = 1; // 右边界是因为到最右边才停下来的，触碰到最右边/最左边，可能是环岛，十字等，
            }
            if (y == 1)
            {
                touch_boundary_up1 = 1; // 走到顶边，判断坡道
            }
            break;
        }
        //=======添加部分=======
        if (front_value < local_thres)
        {
            dir = (dir + 3) % 4;
            turn++;
        }
        else if (frontright_value < local_thres)
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            if (x < 158)
            {
                pts[step][0] = x;
                pts[step][1] = y;
                step++;
                turn = 0;
            }
            // AT(img,x,y) = RGB565_YELLOW;
        }
        else
        {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            // AT(img,x,y) = RGB565_YELLOW;
        }
    }
    // 丢线标志，否则由于sobel特殊性会一直往上巡线
    if (step < 20 && touch_boundary1)
        loseline1 = 1;
    // 记录边线数目
    *num = step;
    //     for (i = 0; i < ipts1_num; i++)
    //     {
    //         ips200_draw_line(160,0,ipts1[i][0] + 2, ipts1[i][1] + 2, RGB565_BLUE);
    //     }
    // ips200_show_int(43,140,ipts1_num,3);
    // ips200_show_int(43,160,loseline1,3);
    // ips200_show_int(43,180,*num,3);
}

// 补线 原图

/**
 * @brief 两点求直线，直线颜色为value
 *
 * @param img image_t类型的指针img
 * @param pt0[2] 两个点的坐标
 * @return 无（主要的作用是在图上画出直线）
 * @explanation 函数首先计算出pt0到pt1的水平和垂直方向的差值dx和dy。如果dx的绝对值大于dy的绝对值，
 * 那么函数就从pt0[0]开始遍历到pt1[0]，每次增加1或减少1，取决于dx的正负。在遍历的过程中，函数计算出当前点的纵坐标y，
 * 并将value赋值给img图像上的对应像素点。计算y的方法是：y = pt0[1] + (x - pt0[0]) * dy / dx，其中x是当前遍历到的横坐标。
 * 如果dx的绝对值小于等于dy的绝对值，那么函数就从pt0[1]开始遍历到pt1[1]，每次增加1或减少1，取决于dy的正负。在遍历的过程中，
 * 函数计算出当前点的横坐标x，并将value赋值给img图像上的对应像素点。计算x的方法是：x = pt0[0] + (y - pt0[1]) * dx / dy，
 * 其中y是当前遍历到的纵坐标。
 */
void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value)
{
    int dx = pt1[0] - pt0[0]; // 求出x,y坐标的差值
    int dy = pt1[1] - pt0[1];
    if (abs(dx) > abs(dy)) // 如果x的差值大于y的差值，那么直线形式就是y=kx+b
    {
        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1))
        {
            int y = pt0[1] + (x - pt0[0]) * dy / dx;                                  // y = 左线横坐标 + x遍历差值占总差值比例 * y方向差值
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; // （x，y）坐标像素（不超出边界）赋值
        }
    }
    else // 如果y的差值大于x的差值，那么直线形式就是x=ky+b
    {
        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1))
        {
            int x = pt0[0] + (y - pt0[1]) * dx / dy;
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    }
}

/**
 * @brief 线性回归求斜率
 *
 * @param pts_in 二维数组
 * @param num 数组长度
 * @return float 返回斜率
 */
/*
    add:这个函数有两处优化的地方：1.如果直线超过60行，就舍弃掉60行以上的部分
    2.防止斜率突变，应该要加入低通滤波
*/
float LineRession(int pts_in[][2], int num)
{
    float slope;
    // int num = sizeof(pts_in) / sizeof(pts_in[0]); // 求数组的长度
    int i = 0, SumX = 0, SumY = 0, SumLines = 0;
    float SumUp = 0, SumDown = 0, avrX = 0, avrY = 0, A;
    SumLines = pts_in[0][1] - pts_in[num][1]; // pts_in[0][1] 为开始行， //pts_in[num][1] 结束行 //SumLines

    for (i = 0; i < num; i++)
    {
        SumY += pts_in[i][1];
        SumX += pts_in[i][0]; // 对边界X,Y求和
    }
    avrY = SumY / SumLines; // Y的平均值
    avrX = SumX / SumLines; // X的平均值
    SumUp = 0;
    SumDown = 0;
    for (i = 0; i < num; i++)
    {
        SumUp += (pts_in[i][0] - avrX) * (pts_in[i][1] - avrY);
        SumDown += (pts_in[i][1] - avrY) * (pts_in[i][1] - avrY);
    }
    if (SumDown == 0)
        slope = 0;
    else
        slope = (SumUp / SumDown);
    A = (SumX - slope * SumY) / SumLines; // 截距
    return slope;                         // 返回斜率
}

/**
 * @brief 快速计算 平方根Sqrt(x),牛顿迭代法
 *
 * @param x 浮点数
 * @return float 平方根
 * @explanation    较快于invSqrt(a)*a; invSqrt()==1.0/sqrt(value);
 这个mySqrt计算平方根速度比invSqrt(a)*a快，而且最大误差只有0.6%
 */
float mySqrt(float x)
{
    float a = x;
    unsigned int i = *(unsigned int *)&x;
    i = (i + 0x3f76cf62) >> 1;
    x = *(float *)&i;
    x = (x + a / x) * 0.5;
    return x;
}

/*************************************************************************
 *  函数名称：void draw_line2();
 *  功能说明：补线 逆透视
 *  参数说明：这个函数的输入参数是两个浮点型数组 pt0 和 pt1，分别表示直线的起点和终点的坐标。
 *           每个数组包含两个元素，分别表示横坐标和纵坐标。
 *           pts_out[][2]是一个二维浮点型数组，用于存储等距采样后的折线段上的点的坐标。
 *           第一维表示点的数量，第二维表示点的坐标，其中第一个元素是横坐标，第二个元素是纵坐标。
             num是一个指向整数的指针，用于存储等距采样后的折线段上的点的数量。
             dist是一个浮点型变量，表示等距采样的折线段距离，即相邻两个采样点之间的距离。
 *  函数返回：无
 *  修改时间：2023年6月1日
 *  备    注： 本质是等距采样
 *************************************************************************/
void draw_line2(float pt0[2], float pt1[2], float pts_out[][2], int *num, float dist)
{
    int remain = 0, len = 0; // 下一次采样折线段距离
    float x0 = pt0[0];
    float y0 = pt0[1];
    float dx = pt1[0] - x0; // 求出x,y坐标的差值
    float dy = pt1[1] - y0;
    float dn = mySqrt(dx * dx + dy * dy); // 求平方根 求弧长积分，即输入线段前后两点距离
    // float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
    dx /= dn; // 此点处的cosθ
    dy /= dn; // 此点处的sinθ

    // 每次等距采样处理
    while (remain < dn)
    {
        x0 += dx * remain;
        pts_out[len][0] = x0;
        y0 += dy * remain;
        pts_out[len][1] = y0;

        len++;
        dn -= remain;
        remain = dist;
    }
    *num = len;
}

/**
 * @brief 寻找远左拐点
 *
 * @param pts_in 左远线数组
 * @param pts_num 左边线数组长度
 * @param pts_out 拐点坐标
 * @param flag 标志位
 */
void FarCorners_Find_Left(int pts_in[][2], int pts_num)
{
    int Is_Corner = 0; // 角点判断标志位
    for (int i = 10; i < pts_num - 10; i++)
    {
        if ((pts_in[i][0] - pts_in[i - 2][0] > 0 && pts_in[i][0] - pts_in[i - 4][0] > 0 && pts_in[i][0] - pts_in[i - 6][0] > 0 && pts_in[i][0] - pts_in[i - 8][0] > 0 && pts_in[i][0] - pts_in[i - 10][0] > 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0) || (pts_in[i][0] - pts_in[i - 2][0] > 0 && pts_in[i][0] - pts_in[i - 4][0] > 0 && pts_in[i][0] - pts_in[i - 6][0] > 0 && pts_in[i][0] - pts_in[i - 8][0] > 0 && pts_in[i][0] - pts_in[i - 10][0] > 0 && pts_in[i][1] - pts_in[i + 2][2] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0)) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i - 1][1])
            {
                continue;
            }
            ipts0_up_index = i; // 记录拐点的下标
            break;
        }
        else
        {
            ipts0_up_index = 0;
        }
    }
}

/**
 * @brief 寻右远拐点
 *
 * @param pts_in 右远线数组
 * @param pts_num 右远线数组长度
 * @param pts_out 拐点坐标
 * @param flag 标志位
 */
void FarCorners_Find_Right(int pts_in[][2], int pts_num)
{
    int Is_Corner = 0; // 角点判断标志位

    for (int i = 10; i < pts_num - 10; i++)
    {
        if ((pts_in[i][0] - pts_in[i - 2][0] < 0 && pts_in[i][0] - pts_in[i - 4][0] < 0 && pts_in[i][0] - pts_in[i - 6][0] < 0 && pts_in[i][0] - pts_in[i - 8][0] < 0 && pts_in[i][0] - pts_in[i - 10][0] < 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0) || (pts_in[i][0] - pts_in[i - 2][0] < 0 && pts_in[i][0] - pts_in[i - 4][0] < 0 && pts_in[i][0] - pts_in[i - 6][0] < 0 && pts_in[i][0] - pts_in[i - 8][0] < 0 && pts_in[i][1] - pts_in[i - 10][1] < 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0)) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i - 1][1])
            {
                continue;
            }
            ipts1_up_index = i;
            break;
        }
        else
        {
            ipts1_up_index = 0;
        }
    }
}

/*************************************************************************
 *  函数名称：void SplicingArray();
 *  功能说明：数组拼接
 *  参数说明：输入的两个数组和容量，输出数组以及容量，x=0正向拼接，x=1反向拼接
 *           pt_out就是拼接以后输出的数组
 *  函数返回：无
 *  修改时间：2023年6月2日
 *  备    注： 用于连接补线的数组
 *************************************************************************/
void SplicingArray(float pt0[][2], int num1, float pt1[][2], int num2, float pt_out[][2], int *num, uint8 x)
{
    int i, count;              // 用来计数
    for (i = 0; i < num1; i++) // 复制数组1
    {
        pt_out[i][0] = pt0[i][0];
        pt_out[i][1] = pt0[i][1];
        count++;
    }

    if (x) // 反向拼接
    {
        for (i = 0; i < num2; i++)
        {
            pt_out[num1 + i][0] = pt1[num2 - i - 1][0];
            pt_out[num1 + i][1] = pt1[num2 - i - 1][1];
            count++;
        }
    }
    else // 正向拼接
    {
        for (i = 0; i < num2; i++)
        {
            pt_out[num1 + i][0] = pt1[i][0];
            pt_out[num1 + i][1] = pt1[i][1];
            count++;
        }
    }
    *num = count;
}
/*检测元素步骤：判断元素是否有标志位-_-->判断拐点是否能找到---->判断是否能补线---->判断补线后再找左右边线*/
uint8 Cross_State_b;  // 十字路口前
uint8 Cross_State_d;  // 十字路口前中
uint8 Cross_State_c;  // 十字路口中
uint8 Straight_State; // 直道
uint8 Left_Turn;      // 左弯道
uint8 Right_Turn;     // 右弯道
uint8 Left_Turn_Mid;  // 左弯道中
uint8 Right_Turn_Mid; // 右弯道中
uint8 Huandao_a;
uint8 Huandao_b;
uint8 Huandao_c;
uint8 Huandao_d;
uint8 Huandao_e;

void Image_Check(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r)
{
    if(touch_boundary0==1&&in_put_num_r>80&&cross_type==CROSS_NONE&&in_put_r[in_put_num_r][0]<IMAGE_WIDTH/2)
    {
        track_type=TRACK_LEFT_MID;
    }
    if (loseline0 == 1 && in_put_num_r > 80 && in_put_r[in_put_num_r - 1][0] < 60)
    {
        track_type = TRACK_LEFT;
    }
    if (touch_boundary1 == 1 && in_put_l[in_put_num_l - 1][0] > 120 && in_put_num_l > 100)
    {
        track_type = TRACK_RIGHT_MID;
    }
    if (loseline1 == 1 && in_put_l[in_put_num_l - 1][0] > 120 && in_put_num_l > 100 && Right_Turn_Mid == 1)
    {
        track_type = TRACK_RIGHT;
    }
}

/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 *
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */
void Image_CheckState(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r)
{
    /*十字路口前：左右不丢线 左边线丢线和右边线丢线的列坐标相差较大，行坐标相差较少*/
    if ((loseline0 == 0 && loseline1 == 0 && (in_put_r[in_put_num_r - 1][0] - in_put_l[in_put_num_l - 3][0]) > (IMAGE_WIDTH - 10)) && (my_abs(in_put_l[in_put_num_l - 3][1] - in_put_r[in_put_num_r - 1][1]) < 10) && Cross_State_d == 0)
    {
        Cross_State_b = 1;
        Cross_State_c = 0;
        Cross_State_d = 0;
        Left_Turn = 0;
        Right_Turn = 0;
        Straight_State = 0;
        Left_Turn_Mid = 0;
    }
    /*十字路口中：左边线不丢线 右边线不丢线 左边线丢线和右边线丢线的列坐标相差较小，行坐标相差较少*/
    if (touch_boundary0 == 1 && touch_boundary1 == 1 && Cross_State_b == 1 && (ipts0[ipts0_num - 1][1] > 50 || ipts1[ipts1_num - 1][1] > 50))
    {
        Cross_State_c = 1;
        Cross_State_b = 0;
        Cross_State_d = 0;
        Left_Turn = 0;
        Right_Turn = 0;
        Straight_State = 0;
        Left_Turn_Mid = 0;
        
    }
    /*十字状态末：丢线判断*/
    if ((loseline0 == 1 && loseline1 == 1 )&& Cross_State_c == 1)
    {
        Cross_State_d = 1;
        Cross_State_b = 0;
        Cross_State_c = 0;
        Left_Turn = 0;
        Right_Turn = 0;
        Straight_State = 0;
        Left_Turn_Mid = 0;
    }

    /*直道：左右线找到的点超过100，左右线的最后一个点的行坐标在30以内*/
    if (ipts0_num > 80 && ipts1_num > 80 && ipts0[ipts0_num - 1][1] < 30 && ipts1[ipts1_num - 1][1] < 30)
    {
        
        Straight_State = 1;
        Right_Turn = 0;
        Left_Turn = 0;
        Cross_State_b = 0;
        Cross_State_c = 0;
        Cross_State_d = 0;
        Left_Turn_Mid = 0;
    }

    /*左弯道中：左边不丢线，右边也不丢线，左边到边界，左右点数差异大*/
    if(touch_boundary0==1&&in_put_num_r>80&&Cross_State_c==0)
    {
        Left_Turn_Mid = 1;
        Left_Turn = 0;
        Right_Turn = 0;
        Cross_State_b = 0;
        Cross_State_c = 0;
        Cross_State_d = 0;
        Straight_State = 0;
    }
    /*左弯道后：左丢线，右边不丢线，右边线最大点距离左边小于50个的单位*/
    if (loseline0 == 1 && in_put_num_r > 80 && in_put_r[in_put_num_r - 1][0] < 60)
    {
        Left_Turn = 1;
        Right_Turn = 0;
        Cross_State_b = 0;
        Cross_State_c = 0;
        Cross_State_d = 0;
        Straight_State = 0;
        Left_Turn_Mid = 0;
        Right_Turn_Mid = 0;
    }

    /*右弯道中：右丢线，左边不丢线，左边线最大点距离右边小于50个的单位*/
    if (touch_boundary1 == 1 && in_put_l[in_put_num_l - 1][0] > 120 && in_put_num_l > 100)
    {
        Right_Turn_Mid = 1;
        Right_Turn = 0;
        Left_Turn = 0;
        Cross_State_b = 0;
        Cross_State_c = 0;
        Cross_State_d = 0;
        Straight_State = 0;
        Left_Turn_Mid = 0;
    }
    /*右弯道后 ：右边找不到线，左边不丢线，左边点的个数大于100，左线最后一个点在较右处*/
    if (loseline1 == 1 && in_put_l[in_put_num_l - 1][0] > 120 && in_put_num_l > 100 && Right_Turn_Mid == 1)
    {
        Right_Turn = 1;
        Right_Turn_Mid = 0;
        Left_Turn = 0;
        Left_Turn_Mid = 0;
        Cross_State_b = 0;
        Cross_State_c = 0;
        Cross_State_d = 0;
        Straight_State = 0;
    }
	// ips200_draw_line(in_put_l[in_put_num_l-1][0],in_put_l[in_put_num_l-1][1],in_put_r[in_put_num_r-1][0],in_put_r[in_put_num_r-1][1],RGB565_RED);
	ips200_show_uint(3,200,Cross_State_b,2);
	ips200_show_uint(3,220,Cross_State_c,2);
	ips200_show_uint(3,240,Cross_State_d,2);
	ips200_show_uint(3,260,Straight_State,2);
	ips200_show_uint(3,280,Left_Turn,2);
	ips200_show_uint(3,300,Left_Turn_Mid,2);
	ips200_show_uint(43,200,in_put_num_l,3);
	ips200_show_uint(43,220,in_put_num_r,3);
	ips200_show_uint(43,240,ipts0_num,3);
	ips200_show_uint(43,260,ipts1_num,3);
	ips200_show_uint(43,280,ipts0[ipts0_num-1][1],2);
	ips200_show_uint(43,300,ipts1[ipts1_num-1][1],2);
	
	ips200_show_uint(83,200,touch_boundary0,3);
	ips200_show_uint(83,220,touch_boundary1,3);
	ips200_show_uint(83,240,loseline0,3);
	ips200_show_uint(83,260,loseline1,3);
	
}

/*左下线坐标变换，用于求拐点，原始坐标原点为左上角，变换后为左下角*/
void Coordinate_transformation_left(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][1] = IMAGE_HEIGHT - pt0_in[i][1] - 1;//y坐标倒置，x坐标保持不变
        pt0_out[i][0] = pt0_in[i][0];
    }
}

/*右下线坐标变换，用于求拐点，将左上坐标移到右下坐标，左上角坐标原点迁移到右下角*/
void Coordinate_transformation_right(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][0] = IMAGE_WIDTH - pt0_in[i][0] - 1;//y和x坐标均倒置
        pt0_out[i][1] = IMAGE_HEIGHT - pt0_in[i][1] - 1;
    }
}

/*右上线坐标变换，将左上的坐标原点移到右上*/
void Coordinate_transformation_rightup(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][0] = IMAGE_WIDTH - pt0_in[i][0] - 1;//x坐标倒置，y坐标不倒置
        pt0_out[i][1] = pt0_in[i][1];
    }
}

/*坐标复原，找完十字以后坐标就和以前一样，目前用不到，直接调用ipts0,ipts1即可*/
void Coordinate_restore_left(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][0] = IMAGE_HEIGHT - pt0_in[i][0] - 1;
    }
}

void Coordinate_restore_right(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][0] = IMAGE_WIDTH - pt0_in[i][0] - 1;
        pt0_out[i][1] = IMAGE_HEIGHT - pt0_in[i][1] - 1;
    }
}

/*
十字状态一补线函数 ：判断坐标能否正常转换（draw_line）--->判断
*/
int Left_Change[POINTS_MAX_LEN][2];
int Right_Change[POINTS_MAX_LEN][2];
float Cross_Drawline(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_r_num)
{
    uint16 i;
    uint16 left_index, right_index; // 左右拐点的坐标
    uint16 left_highest = 0, right_highest = 0;
    uint16 right_up_highest_index;
    uint16 left_up_highest_index;
    float k_left, k_right;
    float b_left, b_right;
    float k_left_low, k_right_low;
    /*一 坐标转换*/
    Coordinate_transformation_left(in_put_l, in_put_num_l, Left_Change); // 左右下线坐标变换
    Coordinate_transformation_right(in_put_r, in_put_r_num, Right_Change);

    FarCorners_Find_Right(ipts11,ipts11_num);
    FarCorners_Find_Left(ipts00,ipts00_num);
    ips200_draw_line(80,60, ipts00[ipts0_up_index][0], ipts00[ipts0_up_index][1], RGB565_BLACK);
    ips200_draw_line(80,60, ipts11[ipts1_up_index][0], ipts11[ipts1_up_index][1], RGB565_RED);

    /*二 找下拐点*/
    for (i = 0; i < in_put_num_l; i++)
    {
        if ((Left_Change[i][0] + Left_Change[i][1]) > left_highest) // 拐点的坐标之和最大
        {
            left_highest = (Left_Change[i][0] + Left_Change[i][1]);
            left_index = i;
            // 遍历完，不用break
        }
    }

    for (i = 0; i < in_put_r_num; i++)
    {
        if ((Right_Change[i][0] + Right_Change[i][1]) > right_highest) // 拐点的坐标之和最大row=k*column+b
        {
            right_highest = (Right_Change[i][0] + Right_Change[i][1]);
            right_index = i;
            // 遍历完，不用break
        }
    }

    // FarCorners_Find_Left(ipts00,ipts00_num);//找到左边线
    // FarCorners_Find_Right(ipts11,ipts11_num);//找到左边线

    // ips200_draw_line(0, 0, in_put_l[left_index][0], in_put_l[left_index][1], RGB565_RED);
    // ips200_draw_line(0, 0, in_put_r[right_index][0], in_put_r[right_index][1], RGB565_BLUE);

    // ips200_draw_line(0, 0, ipts00[ipts0_up_index][0], ipts00[ipts0_up_index][1], RGB565_RED);
    // ips200_draw_line(0, 0, ipts11[ipts1_up_index][0], ipts11[ipts1_up_index][1], RGB565_BLUE);

    /*三 求直线方程*/
    k_left = (float)(in_put_l[left_index][1] - ipts00[ipts0_up_index][1]) / (in_put_l[left_index][0] - ipts00[ipts0_up_index][0]);

    b_left = in_put_l[left_index][1] - k_left * in_put_l[left_index][0];

    // 求得的直线方程是row=column*k+b，实际上应该是column=row*k+b
    k_right = (float)(in_put_r[right_index][1] - ipts11[ipts1_up_index][1]) / (in_put_r[right_index][0] - ipts11[ipts1_up_index][0]);

    b_right = in_put_r[right_index][1] - k_right * in_put_r[right_index][0];

    k_left = (1 / k_left);     // 新斜率取倒数
    b_left = -b_left * k_left; // 新截距取相反数

    k_right = (1 / k_right);      // 新斜率取倒数
    b_right = -b_right * k_right; // 新截距取相反数

    /*增加：斜率滤波*/
    ips200_show_float(120, 140, k_left, 3, 3);
    ips200_show_float(120, 160, k_right, 3, 3);
    // 新直线方程为 column=k*row+b
    /*四 补线*/
    for (i = in_put_l[left_index][1]; i > 10; i--)
    {
        int new_column_l = (int)(k_left * i + b_left);
        if (new_column_l > 0)
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
    for (i = in_put_r[right_index][1]; i > 10; i--)
    {
        int new_column_r = (int)(k_right * i + b_right);
        if (new_column_r > 0)
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }

    return (k_left+k_right)/2;
}

int left_index_l;
int right_index_r; // 左右拐点在数组的下标

/**
 * @brief 十字状态b的找左右边线下拐点函数，无返回值，通过全局变量left_index_l,right_index_r进行索引
 * 
 */
void Get_guaidian(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_r_num)
{
    uint16 i;
    uint16 left_index, right_index; // 左右拐点的坐标
    uint16 left_highest = 0, right_highest = 0;

    /*一 坐标转换*/
    Coordinate_transformation_left(in_put_l, in_put_num_l, Left_Change); // 左右下边线坐标变换
    Coordinate_transformation_right(in_put_r, in_put_r_num, Right_Change);

    /*二 找拐点*/
    /*下边线的数组变化之后，只需要求出x+y的最大值即可找到坐标；用输出的Left_Change和Right_Change来找出拐点下标值*/
    for (i = 0; i < in_put_num_l; i++)
    {
        if ((Left_Change[i][0] + Left_Change[i][1]) > left_highest) // 拐点的坐标之和最大
        {
            left_highest = (Left_Change[i][0] + Left_Change[i][1]);
            left_index_l = i;
            // 遍历完，不用break
        }
    }

    for (i = 0; i < in_put_r_num; i++)
    {
        if ((Right_Change[i][0] + Right_Change[i][1]) > right_highest) // 拐点的坐标之和最大row=k*column+b
        {
            right_highest = (Right_Change[i][0] + Right_Change[i][1]);
            right_index_r = i;
            // 遍历完，不用break
        }
    }
}


void Get_Upguaidian(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_r_num)
{
    uint16 i;
    uint16 left_index, right_index; // 左右拐点的坐标
    uint16 left_highest = 0, right_highest = 0;

    /*一 坐标转换*/
    Coordinate_transformation_rightup(in_put_r, in_put_r_num, Right_Change);//左上不用变换

    /*二 找上拐点*/
    /*这里和下边线不一样，不能通过x+y的最大值求出拐点，通过某点yi>yi+1&&xi<xi+1即可求出拐点*/
    for(i=0;i<in_put_num_l;i++)
	{
		if ((in_put_l[i][1]>in_put_l[i+1][1])&&(in_put_l[i][0]<in_put_l[i+1][0])) // 拐点的坐标之和最大
        {
            ipts0_up_index = i;
            break;
        }
	}
    ips200_draw_line(0, 0, ipts000[ipts0_up_index][0], ipts000[ipts0_up_index][1], RGB565_RED);
	for(i=0;i<in_put_r_num;i++)
	{
		if ((Right_Change[i][1]>Right_Change[i+1][1])&&(Right_Change[i][0]<Right_Change[i+1][0])) // 拐点的坐标之和最大
        {
            ipts1_up_index = i;
            if (ipts1_up_index >= 5)
            {
                break;
            }
        }
	}
    ips200_draw_line(0, 0, ipts111[ipts1_up_index][0], ipts111[ipts1_up_index][1], RGB565_RED);
}

/*
十字补线函数2版：1. 取了上下拐点补线，更稳定，用不上
*/
void Cross_Drawline_old(int in_put_l[][2], int in_put_num_l, int in_put_lnew[][2], int in_put_num_lnew,
                        int in_put_r[][2], int in_put_r_num, int in_put_rnew[][2], int in_put_r_numnew)
{
    int Left_Change[POINTS_MAX_LEN][2];
    int Right_Change[POINTS_MAX_LEN][2];
    int Right_Change_new[150][2];

    uint16 i;
    uint16 left_index, right_index; // 左右拐点的坐标
    uint16 left_highest = 0, right_highest = 0;
    float k_left, k_right;
    float b_left, b_right;
    /*一 坐标转换*/
    Coordinate_transformation_left(in_put_l, in_put_num_l, Left_Change); // 左右下线坐标变换
    Coordinate_transformation_right(in_put_r, in_put_r_num, Right_Change);

    /*二 找下拐点*/
    for (i = 0; i < in_put_num_l; i++)
    {
        if ((Left_Change[i][0] + Left_Change[i][1]) > left_highest) // 拐点的坐标之和最大
        {
            left_highest = (Left_Change[i][0] + Left_Change[i][1]);
            left_index = i;
            // 遍历完，不用break
        }
    }

    for (i = 0; i < in_put_r_num; i++)
    {
        if ((Right_Change[i][0] + Right_Change[i][1]) > right_highest) // 拐点的坐标之和最大row=k*column+b
        {
            right_highest = (Right_Change[i][0] + Right_Change[i][1]);
            right_index = i;
            // 遍历完，不用break
        }
    }

    ips200_draw_line(0, 0, in_put_l[left_index][0], in_put_l[left_index][1], RGB565_RED);
    ips200_draw_line(0, 0, in_put_r[right_index][0], in_put_r[right_index][1], RGB565_BLUE);
    /*三 求直线方程*/
    k_left = LineRession(in_put_l, left_index - 5);

    b_left = in_put_l[left_index][1] - k_left * in_put_l[left_index][0];

    // 求得的直线方程是row=column*k+b，实际上应该是column=row*k+b
    k_right = LineRession(in_put_r, right_index - 5);

    b_right = in_put_r[right_index][1] - k_right * in_put_r[right_index][0];

    k_left = (1 / k_left);     // 新斜率取倒数
    b_left = -b_left * k_left; // 新截距取相反数

    k_right = (1 / k_right);      // 新斜率取倒数
    b_right = -b_right * k_right; // 新截距取相反数
    // 新直线方程为 column=k*row+b
    /*四 补线*/
    for (i = in_put_l[left_index][1]; i > 10; i--)
    {
        int new_column_l = (int)(k_left * i + b_left);
        if (new_column_l > 0)
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
    for (i = in_put_r[right_index][1]; i > 10; i--)
    {
        int new_column_r = (int)(k_right * i + b_right);
        if (new_column_r > 0)
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }
}

int Finnal_left[100][2];
int Finnal_right[100][2]; // 定义左右边线扫的数组
int Finnal_left_num = 0;
int Finnal_right_num = 0;
int Finnal_Mid[100][2];
int Finnal_Mid_num;
/*
十字补中线，返回值为误差（中线误差）
*/
float Get_Mid_Cross(void)
{
    int start_x, start_y;
    int i;
    start_x = IMAGE_WIDTH / 2;
    start_y = IMAGE_HEIGHT - 1;
    /*一 判断为十字状态*/
    if (Cross_State_b == 1 || Cross_State_c == 1 || Cross_State_d == 1)
    {
        // Cross_Drawline(ipts0, ipts0_num,ipts1, ipts1_num);
    }
    /*二 补线以后找中线*/
    for (start_y = IMAGE_HEIGHT - 1; start_y > 30; start_y--)
    {
        for (start_x = IMAGE_WIDTH / 2; start_x > 0; start_x--)
        {
            if (Image_Use_Robert[start_y][start_x] == BLACK)
            {
                Finnal_left[Finnal_Mid_num][0] = start_x;
                Finnal_left[Finnal_Mid_num][1] = start_y;
                Finnal_left_num++;
                break;
            }
        }
        start_x = IMAGE_WIDTH / 2; // 复位
    }
    for (start_y = IMAGE_HEIGHT - 1; start_y > 30; start_y--)
    {
        for (start_x = IMAGE_WIDTH / 2; start_x < IMAGE_WIDTH - 1; start_x++)
        {
            if (Image_Use_Robert[start_y][start_x] == BLACK)
            {
                Finnal_right[Finnal_Mid_num][0] = start_x;
                Finnal_right[Finnal_Mid_num][1] = start_y;
                Finnal_right_num++;
                break;
            }
        }
        start_x = IMAGE_WIDTH / 2; // 复位
    }

    for (i = 0; i < Finnal_left_num; i++)
    {
        Finnal_Mid[i][0] = (Finnal_left[i][0] + Finnal_right[i][0]) / 2;
        Finnal_Mid[i][1] = (Finnal_left[i][1] + Finnal_right[i][1]) / 2;
        Finnal_Mid_num++; // 一般都是90个，正常情况下都能扫到
    }
    /*三 求中线斜率误差*/
    return LineRession(Finnal_Mid, Finnal_Mid_num - 10); // 只取下面80行的点
}

/*左线坐标变换，用于求拐点，左下角为0,0*/
// void Coordinate_transformation_left(int pt0_in[][2], int in_num, int pt0_out[][2])
//{
//     zf_assert(kernel % 2 == 1);//检查变量 kernel 是否为奇数
//     int half = kernel / 2;//用于计算滤波器的大小
//     for (int i = 0; i < num; i++) //对于每个点，都会计算它周围的点的加权平均值，并将结果存储在输出点集数组中
//     {
//         pts_out[i][0] = pts_out[i][1] = 0;
//         for (int j = -half; j <= half; j++) {
//             pts_out[i][0] += (float)pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
//             pts_out[i][1] += (float)pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
//         }
//         pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
//         pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
//     }
// }
/*************************************************************************
 *  函数名称：void resample_points（）;
 *  功能说明：点集等距采样
 *  参数说明：输入边线数组pts_in[][2]，输入原边线总个数num1，输出边线数组pts_out[][2]，获取输出边线数组个数num2，采样距离dist
 *  函数返回：无
 *  修改时间：2022年10月27日
 *  备    注：使走过的采样前折线段的距离为`dist`
 *            这个函数的作用是对输入的点集进行等距采样处理，输出采样后的点集。
 *            函数的实现过程是对输入点集中的每个线段进行等距采样，采样后的点集存储在输出点集中。
 *************************************************************************/
void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist)
{
    int remain = 0, len = 0; // 下一次采样折线段距离
    for (int i = 0; i < num1 - 1 && len < *num2; i++)
    {
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float dx = pts_in[i + 1][0] - x0;
        float dy = pts_in[i + 1][1] - y0;
        float dn = mySqrt(dx * dx + dy * dy); // 求平方根 求弧长积分，即输入线段前后两点距离
        // float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
        dx /= dn; // 此点处的cosθ
        dy /= dn; // 此点处的sinθ

        // 每次等距采样处理
        while (remain < dn && len < *num2)
        {
            x0 += dx * remain;
            pts_out[len][0] = x0;
            y0 += dy * remain;
            pts_out[len][1] = y0;

            len++;
            dn -= remain;
            remain = dist;
        }
        remain -= dn; // 当跨越一点采样折线距离近似直线
    }
    *num2 = len;
}

// 角点提取&筛选 Y角点用于检测坡道，L角点用于检测圆环、十字、路障、断路

// 左右边线局部角度变化率
float rpts0a[POINTS_MAX_LEN];
float rpts1a[POINTS_MAX_LEN];
int rpts0a_num, rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
float rpts0an[POINTS_MAX_LEN];
float rpts1an[POINTS_MAX_LEN];
int rpts0an_num, rpts1an_num;
// 左/右中线
float rptsc0[POINTS_MAX_LEN][2];
float rptsc1[POINTS_MAX_LEN][2];
int rptsc0_num, rptsc1_num;
// 左右跟踪后的中线
float (*rpts)[2];
int rpts_num;
// 归一化最终中线
float rptsn[POINTS_MAX_LEN][2];
int rptsn_num;

// Y角点
int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
bool Ypt0_found, Ypt1_found;
// L角点
int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
bool Lpt0_found, Lpt1_found;
// 内L角点
int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
bool N_Lpt0_found, N_Lpt1_found;
// 长直道
bool is_straight0, is_straight1;
// 弯道
bool is_turn0, is_turn1;

int N_Xfound_num; // 面向赛道编程，双内L计数

// 当前巡线模式（发车方向）
enum track_type_e track_type = TRACK_RIGHT;

float error[1];
float ave_error; // 速度控制输入变量
// 多个预锚点位置
int aim_idx[1];

// 计算远锚点偏差值
float dx[1];
float dy[1];
float dn[1];

// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
// 计算近锚点偏差值
float dx_near;
float dy_near;
float dn_near;
float pure_angle;

// enum garage_type_e garage_type = GARAGE_NONE; // 初始化为向左出库 调试状态为NONE

uint8_t RoundaboutGetArc(uint8_t imageSide[120][2], uint8_t status, uint8_t num, uint8_t *index)
{
    int i = 0;
    uint8_t inc = 0, dec = 0, n = 0;
    switch (status)
    {
    case 1:
        for (i = 115 - 1; i > 10; i--)
        {
            if (imageSide[i][0] != 0 && imageSide[i + 1][0] != 0)
            {
                if (imageSide[i][0] == imageSide[i + 1][0])
                {
                    n++;
                    continue;
                }
                if (imageSide[i][0] > imageSide[i + 1][0])
                {
                    inc++;
                    inc += n;
                    n = 0;
                }
                else
                {
                    dec++;
                    dec += n;
                    n = 0;
                }

                /* 有弧线 */
                if (inc > num && dec > num)
                {
                    *index = i + num;
                    return 1;
                }
            }
            else
            {
                inc = 0;
                dec = 0;
                n = 0;
            }
        }

        break;

    case 2:
        for (i = 115 - 1; i > 10; i--)
        {
            if (imageSide[i][1] != 159 && imageSide[i + 1][1] != 159)
            {
                if (imageSide[i][1] == imageSide[i + 1][1])
                {
                    n++;
                    continue;
                }
                if (imageSide[i][1] > imageSide[i + 1][1])
                {
                    inc++;
                    inc += n;
                    n = 0;
                }
                else
                {
                    dec++;
                    dec += n;
                    n = 0;
                }

                /* 有弧线 */
                if (inc > num && dec > num)
                {
                    *index = i + num;
                    return 1;
                }
            }
            else
            {
                inc = 0;
                dec = 0;
                n = 0;
            }
        }

        break;
    }

    return 0;
}



uint8 mid_line_num;

/*
左弯道函数，返回值为中线误差
*/
// float run_left(void)
// {
//     /*一 求中线*/
//     int mid_line[120][2];//中线
//     mid_line_num=0;
//     uint8 height_count;
//     uint8 l_c,r_c;//左右计数值
//     uint8 i;
//     if(Left_Turn_Mid==1 && Left_Turn==0)
//     {
//         l_c = 0;
//         r_c = 0;
//         height_count=ipts1[0][1];//选择纵坐标
//         mid_row_first=ipts1[0][1];
//         for(i=0; i<ipts1_num ;i++)
//         {
//             if(i<ipts0_num)//边界不丢线
//             {
//                 if(ipts1[r_c][1]==height_count && ipts0[l_c][1]==height_count)//如果等于对应的高度的话
//                 {
//                     mid_line[i][0]=(ipts0[l_c][0]+ipts1[r_c][0])/2;//中线为左右线取平均
//                     mid_line[i][1]=height_count;
//                     l_c++;
//                     r_c++;
//                     height_count--;
//                 }
//                 else
//                 {
//                     while(1)
//                     {
//                         if(ipts0[l_c][1]<ipts1[r_c][1])//左边点比右边点的坐标小的话
//                         {
//                             r_c++;
//                         }
//                         else if(ipts0[l_c][1]>ipts1[r_c][1])//左边点的坐标比右边点的坐标大
//                         {
//                             l_c++;
//                         }
//                         if(ipts0[l_c][1]==ipts1[r_c][1]&&ipts0[l_c][1]==height_count)
//                         {
//                             l_c++;
//                             r_c++;
//                             mid_line[i][0]=(ipts0[l_c][0]+ipts1[r_c][0])/2;//中线同高取平均
//                             mid_line[i][1]=height_count;
//                             height_count--;
//                             break;
//                         }
//                     }
//                 }
//             }
//             else if(i>ipts0_num)//左边界丢线的话
//             {
//                 if(ipts1[r_c][1]==height_count)//等高取平均
//                 {
//                     mid_line[i][0]=ipts1[r_c][0]/2;
//                     mid_line[i][1]=height_count;
//                     height_count--;
//                 }
//                 else
//                 {
//                     while(1)
//                     {
//                         if(ipts1[r_c][1]>height_count)
//                         {
//                             r_c++;
//                         }
//                         else if(ipts1[r_c][1]<height_count)
//                         {
//                             height_count--;
//                         }
//                         else if(ipts1[r_c][1]==height_count)
//                         {
//                             mid_line[i][0]=ipts1[r_c][0]/2;
//                             mid_line[i][1]=height_count;
//                             r_c++;
//                             height_count--;
//                             break;
//                         }
//                     }
//                 }
//             }
//         }
//         // while(1)
//         // {
//         //     mid_line_num=i;//存储计数值
//         //     ips200_show_uint(0,0,mid_line_num,3);
//         // }
//     }
//     /*情况二：左边线完全丢线*/
//     if(Left_Turn_Mid==0 && Left_Turn==1)
//     {
//         l_c = 0;
//         r_c = 0;//清空计数值
//         height_count=ipts1[0][1];//选择纵坐标
//         mid_row_first=ipts1[0][1];
//         for(i=0;i<ipts0_num;i++)
//         {
//             if(ipts1[r_c][1]==height_count)//等高取平均
//             {
//                 mid_line[i][0]=ipts1[r_c][0]/2;
//                 mid_line[i][1]=height_count;
//             }
//             else
//             {
//                 while(1)
//                 {
//                     if(ipts1[r_c][1]>height_count)
//                     {
//                         r_c++;
//                     }
//                     else if(ipts1[r_c][1]<height_count)
//                     {
//                         height_count--;
//                     }
//                     else if(ipts1[r_c][1]==height_count)
//                     {
//                         mid_line[i][0]=ipts1[r_c][0]/2;
//                         mid_line[i][1]=height_count;
//                         r_c++;
//                         height_count--;
//                         break;
//                     }
//                     ips200_show_uint(20,20,20,2);
//                 }
//             }
//         }
//         mid_line_num=i;//存储计数值
//     }
//     ips200_show_uint(160,200,mid_line_num,3);
//     /*二 求误差*/
//     float err,last_err;
//     last_err=err;
//     err=LineRession(mid_line,mid_line_num-1);

//     /*三 误差简单滤波*/
//     err=0.8*err+last_err*0.2;
//     ips200_show_float(120,120,err,3,3);
//     return err;
// }

/**
 * @brief 十字状态d的运行函数
 * 
 */
float Draw_line_cross_d(void)
{
    float k_r,k_l,b_r,b_l;//定义左右边线斜率和截距
    float k_l_line,k_r_line;//定义左右中线斜率
    k_l=(float)(ipts0[ipts0_up_index][1]-118)/(ipts0[ipts0_up_index][0]-1);
    b_l=ipts0[ipts0_up_index][1]*k_l-ipts0[ipts0_up_index][0];//将拐点坐标和左下右下相连，求出斜率
    //直线方程为 row=k*column+b，但是补线的时候要转化为column=k*row+b（其实后面也没用补线，直接求斜率即可）
    k_l = (1 / k_l);
    b_l = k_l * (-b_l);

    k_r = (float)(ipts1[ipts1_up_index][1] - 118) / (ipts1[ipts1_up_index][0] - 158);
    b_r = ipts1[ipts1_up_index][1] * k_r - ipts1[ipts1_up_index][0];
        
    k_r=(1/k_r);
    b_r=-b_r*k_r;
    int i;
    /*这里是补线，补线后再次扫一次左右边线
        但是直接用斜率我觉得也是可以的，这部分可以去掉*/
    for (i = ipts0[ipts0_up_index][1]; i > 10; i--)
    {
        int new_column_l = (int)(k_l * i + b_l);
        if (new_column_l > 0)
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
    for (i = ipts1[ipts1_up_index][1]; i > 10; i--)
    {
        int new_column_r = (int)(k_r * i + b_r);
        if (new_column_r > 0)
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }
    //这里是觉得仅仅靠一个斜率，准确度不够高，所以取拐点的下线和拐点上边线的两个斜率
    k_l_line=(ipts000[ipts0_up_index][0]-ipts0[ipts0_up_index+5][0])/(ipts000[ipts0_up_index][1]-ipts0[ipts0_up_index+5][1]);
    k_r_line=(ipts111[ipts1_up_index][0]-ipts1[ipts1_up_index+5][0])/(ipts111[ipts1_up_index][1]-ipts1[ipts1_up_index+5][1]);

    float percent =(ipts000[ipts0_up_index][1]/120);//通过拐点的y左边来进行百分比的计算，按百分比计算斜率 percent最大为0.5（因为拐点的y坐标最大为60行）
    k_l= k_l_line*percent+ k_l*(1-percent);//按百分比进行赋值

    percent =(ipts111[ipts1_up_index][1]/120);//右线也一样

    k_r= k_r_line*percent+k_r*(1-percent);

    return (k_l+k_r)/2;
}

float err, last_err;

float run_left_mid(void)
{
    int mid_line[150][2]; // 中线
    uint8 i;
    mid_line_num = ipts0_num; // 中线赋值
    for (i = 0; i < mid_line_num; i++)
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;//直接取平均（前面有个更好的函数，但是试过不行，后面再优化）
    }
    last_err = err;
    err = LineRession(mid_line, mid_line_num - 1);
    ips200_show_uint(0, 160, mid_line_num, 3);
    /*三 误差简单滤波*/
    err=0.8*err+last_err*0.2;
    return err;

}
float run_left(void)
{
    /*一 求中线*/
    int mid_line[150][2]; // 中线
    mid_line_num = 0;
    uint8 i;
    mid_line_num = ipts1_num; // 中线赋值
    for (i = 0; i < mid_line_num; i++)
    {
        mid_line[i][0] = ipts1[i][0] / 2;
        mid_line[i][1] = ipts1[i][1];
    }
    for (i = 0; i < mid_line_num; i++)
    {
        ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    }
    /*二 求误差*/

    last_err = err;
    err = LineRession(mid_line, mid_line_num - 1);
    ips200_show_uint(0, 160, mid_line_num, 3);
    /*三 误差简单滤波*/
    err=0.8*err+last_err*0.2;
    return err;
}

float run_right_mid(void)
{
    int mid_line[150][2]; // 中线
    mid_line_num = 0;
    uint8 i;
    
    mid_line_num = ipts1_num; // 中线赋值
    for (i = 0; i < mid_line_num; i++)
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
    }
    last_err = err;
    err = LineRession(mid_line, mid_line_num - 1);
    ips200_show_uint(0, 160, mid_line_num, 3);
    /*三 误差简单滤波*/
    err = 0.8 * err + last_err * 0.2;
    ips200_show_float(120, 120, err, 3, 3);
    return err;
}


float run_right(void)
{
    /*一 求中线*/
    int mid_line[150][2]; // 中线
    mid_line_num = 0;
    uint8 i;
    mid_line_num = ipts0_num; // 中线赋值
    for (i = 0; i < mid_line_num; i++)
    {
        mid_line[i][0] = ipts0[i][0] / 2;
        mid_line[i][1] = ipts0[i][1];
    }
   
    /*二 求误差*/

    last_err = err;
    err = LineRession(mid_line, mid_line_num - 1);
    ips200_show_uint(0, 160, mid_line_num, 3);
    /*三 误差简单滤波*/
    err = 0.8 * err + last_err * 0.2;
    ips200_show_float(120, 120, err, 3, 3);
    return err;
}

/**
 * @brief 十字状态b的运行函数
 * 
 */
void run_cross_b(void)
{
    int mid_line[150][2]; // 中线数组，150个防止溢出
    mid_line_num = 0;//中线数组的计数值
    uint8 i;
    if (ipts0_num > ipts1_num)
    {
        mid_line_num = right_index_r; // 中线个数赋值
    }
    else
    {
        mid_line_num = left_index_l; // 中线个数赋值
    }
    for (i = 0; i < mid_line_num; i++)//截断处理，从第一个点取到左右边线的最近的一个拐点的下标
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
    }
    /*显示函数，可以去掉*/
    for (i = 0; i < mid_line_num; i++)
    {
        ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    }

    last_err = err;//存储上次的误差

    err = LineRession(mid_line, mid_line_num - 1);//中线求出误差

    err = 0.8 * err + 0.2 * last_err;//简单滤波处理

    Finnal_err = err;//直接用全局变量，不返回误差
}

/*中心扫线函数，用不上*/
float Center_edge(void)
{
    int Left_Edge[150][2]; // 实际上只会用到120个，怕越界
    int Right_Edge[150][2];
    int y_l, x_l, y_r, x_r;
    int i;
    y_l = 119;
    y_r = 119;
    for (i = 0; i < 110; i++) // 扫100次
    {
        for (x_l = IMAGE_WIDTH / 2; x_l > 0; x_l--)
        {
            if (Image_Use_Robert[x_l][y_l] == WHITE && Image_Use_Robert[x_l - 1][y_l] == BLACK)
            {
                Left_Edge[i][0] = x_l;
                Left_Edge[i][1] = y_l;
                y_l--;
                break;
            }
        }
        for (x_r = IMAGE_WIDTH / 2; x_r < (IMAGE_WIDTH - 2); x_r++)
        {
            if (Image_Use_Robert[x_r][y_r] == WHITE && Image_Use_Robert[x_r - 1][y_r] == BLACK)
            {
                Right_Edge[i][0] = x_r;
                Right_Edge[i][1] = y_r;
                y_r--;
                break;
            }
        }
    }

    int Mid_line[150][2];
    int Mid_line_num;
    for (i = 0; i < 110; i++)
    {
        Mid_line[i][0] = (Left_Edge[i][0] + Right_Edge[i][0]) / 2;
        Mid_line[i][1] = (Left_Edge[i][1] + Right_Edge[i][1]) / 2;
    }

    last_err = err;
    err = LineRession(Mid_line, 109);

    err = last_err * 0.2 + err * 0.8;

    return err;
}

/**
 * @brief 十字状态c的运行函数
 * 
 */
void run_cross_c(void)
{
    Find_Borderline_Second();
    Finnal_err=Cross_Drawline(ipts0,ipts0_num,ipts1,ipts1_num)*0.06;//直接返回误差（函数里面会有滤波）
}

/**
 * @brief 十字状态d的运行函数，这里会开启三次扫线（为了和二次扫线区别）
 * 
 */
void run_cross_d(void) 
{
    Find_Borderline_Third();//当左右丢线后，此时ipts000和ipts111就可以扫上去了
    /*三次扫线和二次扫线的区别在于： 二次扫线的条件依赖于ipts0是否到达左边界(touch_boundary0)    三次扫线的条件是 左右同时丢线才扫线(loseline)
        二次扫线的起始点和ipts0的最后一个点的坐标有关                三次扫线的起始点是固定的（因为loseline==1，ipts0的坐标找不到了）
        二次扫线相对来说更加灵活，能根据ipts0最后一个点的坐标进行二次扫线的灵活变化     三次扫线只能在y属于0-60的范围内扫，相对比较固定
        二次扫线在十字环岛都能用        三次扫线只能在十字的最后一个状态才能用*/
    FarCorners_Find_Left(ipts000,ipts000_num);//找到左边线
    FarCorners_Find_Right(ipts111,ipts111_num);//找到左边线
    ips200_draw_line(0,0,ipts000[left_index_l][0],ipts000[left_index_l][1],RGB565_RED);//显示左边线
    ips200_draw_line(0,0,ipts111[right_index_r][0],ipts111[right_index_r][1],RGB565_RED);//显示右边线
    uint8 i;
    Finnal_err=Draw_line_cross_d();
}


/*找到近拐点直接截断处理*/
/**
 * @brief 运行十字路口函数，根据当前的十字路口状态选择相应的处理函数（分三种状态，每种状态的标志位对应不同的函数）
 * 
 */
void run_cross(void)
{
    if (Cross_State_b == 1 && Cross_State_c == 0 && Cross_State_d == 0)//防止多状态进行
    {
        Get_guaidian(ipts0, ipts0_num, ipts1, ipts1_num); // 找拐点左下右下拐点
        run_cross_b();
    }
    if (Cross_State_c == 1 && Cross_State_b == 0 && Cross_State_d == 0)
    {
        run_cross_c();
    }
    if (Cross_State_b == 0 && Cross_State_c == 0 && Cross_State_d == 1)
    {
        run_cross_d();
    }
}


/**
 * @brief 计算直线情况下左右线斜率并返回左右边线斜率的平均值
 * 
 * @return float 左右边线斜率的平均值，即为误差
 */
float run_straight(void)
{
    float k_l,k_r;//定义左右边线斜率
    if(Straight_State==1)
    {
        k_l=(float)LineRession(ipts0,ipts0_num*0.75);//减少直线尽头点对斜率的干扰
        k_r=(float)LineRession(ipts1,ipts1_num*0.75);
    }
    return (k_l+k_r)/2;
}


void test(void)
{
    uint8 i;
    Image_Compress();
    int TH;
    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms
    img_raw.data = *Image_Use_Robert;             // 传入sobel边沿检测图像
    // 寻找左右边线
    Find_Borderline();

    //	Find_Borderline_Second();
    int test = 0;
    // test = Is_Straight(ipts0, ipts0_num, sample_dist);

    // for (i = 0; i < ipts0_num; i++)
    // {
    //     func_limit_ab(ipts0[i][0], 0, 160);
    //     func_limit_ab(ipts0[i][1], 0, 120);
    //     ips200_draw_line(0,0,ipts0[i][0], ipts0[i][1], RGB565_RED);
    // }
    // for (i = 0; i < ipts1_num; i++)
    // {
    //     func_limit_ab(ipts1[i][0], 0, 160);
    //     func_limit_ab(ipts1[i][1], 0, 120);
    //     ips200_draw_line(0,0,ipts1[i][0], ipts1[i][1], RGB565_GREEN);
    // }
    // for(i=0;i<ipts00_num;i++)
    // {
    // 	func_limit_ab(ipts00[i][0],0,160);
    // 	func_limit_ab(ipts00[i][1],0,120);
    // 	ips200_draw_line(0,0,ipts00[i][0],ipts00[i][1],RGB565_BLUE);
    // }

    // for(i=0;i<ipts11_num;i++)
    // {
    // 	func_limit_ab(ipts11[i][0],0,160);
    // 	func_limit_ab(ipts11[i][1],0,120);
    // 	ips200_draw_line(160,0,ipts11[i][0],ipts11[i][1],RGB565_RED);
    // }

    Image_Check(ipts0, ipts0_num, ipts1, ipts1_num);
    /*根据状态执行*/
    switch (track_type)
    {
        case TRACK_LEFT:
        {
            Finnal_err=run_left();
            break;
        }
        case TRACK_RIGHT:
        {
            Finnal_err=run_right();
            break;
        }
        case TRACK_LEFT_MID:
        {
            Finnal_err=run_left_mid();
            break;
        }
    }
    Finnal_err=run_left();
    Finnal_err=run_right();

}
