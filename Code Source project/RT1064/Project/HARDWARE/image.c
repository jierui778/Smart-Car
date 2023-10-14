#include "image.h"
#include "math.h"
#include "control.h"
#include "stdlib.h"
uint8 Image_Use_Robert[120][160]; // 二值化图像

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

float xielv_left_y_to_end, xielv_right_y_to_end; // 在逆透视后得坐标系建得斜率

void check()
{
    while (1)
    {
    }
}
// 八邻域扫的左线的结构体
struct Left_Edge
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
    unsigned char flag;   // 判断边界点是否找到
    uint8 grow;
};
// 八邻域扫的右线的结构体
struct Right_Edge
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
    unsigned char flag;   // 判断边界点是否找到
    uint8 grow;
};

struct Left_Edge Left[200];       // 左边界结构体
struct Right_Edge Right[200];     // 右边界结构体
struct Left_Edge Left_New[200];   // 滤波后左边界结构体
struct Right_Edge Right_New[200]; // 滤波后右边界结构体
// 定义坐标结构体
struct coordinate
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
    uint8 index;          // 下标索引
    uint8 flag;
};
struct coordinate mid[140]; // 中线坐标结构体

// 直线结构体
struct Line
{
    float k;
    float b;
}; // 定义直线方程

struct Line last_left_line;
struct Line last_right_line;
uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH];
uint8 Image_Use_Robert[IMAGE_HEIGHT][IMAGE_WIDTH];

/**
 * @brief 截取我们需要的图像大小
 *
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */

void Image_Compress(void)
{
    int i, j, row, line;
    const float pro_h = PRIMEVAL_HEIGHT / IMAGE_HEIGHT, pro_w = PRIMEVAL_WIDTH / IMAGE_WIDTH; // 根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
    for (i = 0; i < IMAGE_HEIGHT; i++)                                                        // 遍历图像的每一行，从第零行到第59行。
    {
        row = i * pro_h + 0.5;
        for (j = 0; j < IMAGE_WIDTH; j++) // 遍历图像的每一列，从第零列到第79列。
        {
            line = j * pro_w + 0.5;
            Image_Use[i][j] = mt9v03x_image[row][line]; // mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
        }
    }
    mt9v03x_finish_flag = 0;
}

#define UROW 120
#define UCOL 160
void compressimage(void)
{
    int i, j, row, line;
    const float div_h = MT9V03X_H / UROW, div_w = MT9V03X_W / UCOL;
    for (i = 0; i < UROW; i++)
    {
        row = i * div_h + 0.5;
        for (j = 0; j < UCOL; j++)
        {
            line = j * div_w + 0.5;
            Image_Use[i][j] = mt9v03x_image[row][line];
        }
    }
    tft180_displayimage03x(Image_Use[0], 80, 60);
    mt9v03x_finish_flag = 0; // 使用完一帧DMA传输的图像图像  可以开始传输下一帧
}

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
 * @brief 将输入的灰度图像转化为二值化图像
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

/**
 * @brief 计算一个整数的绝对值
 *
 * @param value 需要计算绝对值的整数
 * @return int 返回value的绝对值
 */
int my_abs(int value)
{
    if (value >= 0)
        return value;
    else
        return -value;
}

/**
 * @brief 限制一个整数的范围在 [a, b] 之间
 *
 * @param x 待限制的整数
 * @param a 范围下限
 * @param b 范围上限
 * @return int16 限制后的整数
 */
int16 limit_a_b(int16 x, int a, int b)
{
    if (x < a)
        x = a;
    if (x > b)
        x = b;
    return x;
}

/**
 * @brief 比较两个整数的大小，返回其中较小的一个
 *
 * @param a 整数a
 * @param b 整数b
 * @return int 返回a和b中较小的一个
 */
int min(int a, int b)
{
    return a < b ? a : b;
}

/**
 * @brief 获取图像左侧起始生长点--寻找起始点函数，从第56行开始数
 * @input start_line 开始找起始点的行
 *
 * @return unsigned char start_row 将输入的起始行的值返回，因为加了黑框肯定会找得到，没必要关注找没找得到
 * start_row 一般默认是110-119内（119是最低的行），无论怎么选都是可以找到起始生长点的
    一般建议118-117就行，不然最下面没有的话数组很容易越界啥的
*  这里返回star_row是为了在八邻域里面确定起始点，且可以操作
 */
uint8 left_point;     // 记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
uint8 Left_Find_Flag; // 左线起始点找到标志
uint8 Image_Get_LeftPoint(uint8 start_row)
{
    for (left_point = (IMAGE_WIDTH / 2); left_point > 3; left_point--)
    {
        if ((Image_Use_Robert[start_row][left_point] == 255) && (Image_Use_Robert[start_row][left_point - 1] == 0) && (Image_Use_Robert[start_row][left_point - 2] == 0))
        {
            left_point -= 1;
            Left_Find_Flag = 1; //
            break;
        }
    }
    if (!Left_Find_Flag)
    {
        left_point = 2;
    }
    return start_row;
}

uint8 Image_Get_LeftPoint2(uint8 start_row)
{
    for (left_point = (IMAGE_WIDTH / 2); left_point > 3; left_point--)
    {
        if ((Image_Use_Robert[start_row][left_point] == 255) && (Image_Use_Robert[start_row][left_point - 1] == 0))
        {
            left_point -= 1;
            Left_Find_Flag = 1; //
            break;
        }
    }
    if (!Left_Find_Flag)
    {
        left_point = 2;
    }
    return start_row;
}

/**
 * @brief 获取图像右侧起始生长点--寻找起始点函数，从第start_row行开始数
 * @input start_line 开始找起始点的行
 *
 * @return unsigned char start_row 将输入的起始行的值返回
 * @explaination 由于最右边两行画了黑框，正常情况下是肯定能扫描出来的，也就是一定能扫得到在157列坐标
 */
uint8 right_point;     // 记录第一个关键点的列坐标
uint8 Right_Find_Flag; // 右线起始点找到标志
uint8 Image_Get_RightPoint(uint8 start_row)
{
    for (right_point = (IMAGE_WIDTH / 2); right_point < (IMAGE_WIDTH - 2); right_point++) // 扫到
    {
        if ((Image_Use_Robert[start_row][right_point] == 255) && (Image_Use_Robert[start_row][right_point + 1] == 0) && (Image_Use_Robert[start_row][right_point + 2] == 0)) // 这里指针变量不能直接和值比较，需要解地址
        {
            right_point = right_point + 1;
            Right_Find_Flag = 1;
            break; // 这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
    if (!Right_Find_Flag) // 如果找不到
    {
        right_point = IMAGE_WIDTH - 3; // 取边界点，这里的点就是白点了
    }
    return start_row;
}

uint8 Image_Get_RightPoint2(uint8 start_row)
{
    for (right_point = (IMAGE_WIDTH / 2); right_point < (IMAGE_WIDTH - 2); right_point++) // 扫到
    {
        if ((Image_Use_Robert[start_row][right_point] == 255) && (Image_Use_Robert[start_row][right_point + 1] == 0)) // 这里指针变量不能直接和值比较，需要解地址
        {
            right_point = right_point + 1;
            Right_Find_Flag = 1;
            break; // 这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
    if (!Right_Find_Flag) // 如果找不到
    {
        right_point = IMAGE_WIDTH - 3; // 取边界点，这里的点就是白点了
    }
    return start_row;
}

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


/**
 * @brief 计算通过一组点的直线的斜率
 *
 * 此函数计算通过一组点的直线的斜率,接受线的起点和终点以及线上点的y坐标数组作为输入,返回直线的斜率
 *
 * @param begin 线的起点
 * @param end 线的终点
 * @return 通过一组点的直线的斜率
 */
float Imgae_SlopeLeft(uint8 begin, uint8 end)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16 i = 0;
    float result = 0;
    static float resultlast = 0;  // 记录上次结果，用来比对
    for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）

        for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）
        {
            xsum += Left[i].row;                     // 对x坐标自增（行坐标）
            ysum += Left[i].column;                  // 对y坐标自增（列坐标）
            xysum += Left[i].row * (Left[i].column); // xy坐标乘积自增
            x2sum += Left[i].row * Left[i].row;      // x坐标平方自增
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
/**
 * @brief 计算通过一组点的直线的斜率
 * @param begin 线的起点
 * @param end 线的终点
 * @return 通过一组点的直线的斜率
 */
/**
 * @brief 计算右边车道线的斜率
 *
 * @param begin 起点下标
 * @param end 终点下标
 * @return float 斜率值
 */
float Imgae_SlopeRight(uint8 begin, uint8 end)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16 i = 0;
    float result = 0;
    static float resultlast = 0; // 记录上次结果，用来比对

    for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）
    {
        xsum += Right[i].row;                      // 对x坐标自增（行坐标）
        ysum += Right[i].column;                   // 对y坐标自增（列坐标）
        xysum += Right[i].row * (Right[i].column); // xy坐标乘积自增
        x2sum += Right[i].row * Right[i].row;      // x坐标平方自增
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

/**
 * @brief 计算斜率和截距,配合上面的最小二乘法使用（求的是左边线的）
 * @param begin 线的起点
 * @param end 线的终点
 * @return 通过一组点的直线的斜率,通过全局变量
 * @example Image_CountLeftKB(1,45) 得到的方程为column=k*row+b;
 */
float slope_rate_l, intercept_l;
void Image_CountLeftKB_L(uint8 start, uint8 end)
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
        xsum += Left[i].row;    // 行坐标累加
        ysum += Left[i].column; // 列坐标累加
        num++;
    }
    // 计算x,y坐标的平均值
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);
    }
    slope_rate_l = Imgae_SlopeLeft(start, end);         // 计算斜率
    intercept_l = y_average - (slope_rate_l)*x_average; // 计算截距
}
/**
 * @brief 计算斜率和截距,配合上面的最小二乘法使用（求的是右边线的）
 * @brief 计算斜率和截距,配合上面的最小二乘法使用
 *
 *
 * @param begin 线的起点
 * @param end 线的终点
 * @param border 线上点的y坐标数组
 * @return 通过一组点的直线的斜率,通过指针来返回斜率和截距
 * @example Image_CountRightKB_R(1,45)
 */
float slope_rate_r, intercept_r;
void Image_CountRightKB_R(uint8 start, uint8 end)
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
        xsum += Right[i].row;    // 行坐标累加
        ysum += Right[i].column; // 列坐标累加
        num++;
    }
    // 计算x,y坐标的平均值
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);
    }
    slope_rate_r = Imgae_SlopeRight(start, end);        // 计算斜率
    intercept_r = y_average - (slope_rate_r)*x_average; // 计算截距
}

uint8 Left_Count, Right_Count;         // 记录左右边界点的个数
uint8 grow_left, grow_right;           // 记录左右边界在八邻域时寻点的相对位置
uint8 Left_Max = 200, Right_Max = 200; // 左右边界搜点时允许最大的搜点量
uint8 Boundary_search_end = 30;        // 搜寻行数的最高行

uint8 Gather_flag;
uint8 Gather_row[50]; // 两线会聚数组，用来存储行坐标
uint8 Gather_Count;   // 计数
/**
 * @brief 八邻域寻边界
 *
 * @param uint8 必须为经过边框处理Draw_Frame后的图像才可以进行八领域扫线
 * @return 无
 * @exception 1.我默认左上角为（0，0）
 * 2.巡线原理就是找到第一个点，然后找附近的8个点，再选择下一个点来作为衍生，将得到的点记录即可得到边界
 * 3.这里是从第start_row行的点开始寻找，因为要留一行做起始八邻域点，还有边界两行已经改成黑了
 * 4.生长方向表和坐标表（可以随便定，这里也是一种方法）
 * row-1 col-1      row-1 col       row-1 col+1         7   0   6
 * row   col-1      row   col       row   col+1         4   *   5
 * row+1 col-1      row+1 col       row+1 col+1         3   1   2
 */

void Image_Get_neighborhoods(uint8 (*Image_Use)[IMAGE_WIDTH])
{
    Left_Count = 0;
    Right_Count = 0;
    uint16 cur_row, cur_col;            // 当前行列
    uint8 Pixel_Change;                 // 灰白像素变化标志位——恢复原来的bug
    uint8 Pixel_Count_r, Pixel_Count_l; // 计算变化对应的下标值
    uint8 Pixel_row, Pixel_col;         // 记录变化像素点的坐标（右坐标）
    if (Image_Get_LeftPoint2(117))
    {
        Left[0].row = Image_Get_LeftPoint(117);
        Left[0].column = left_point - 1;
        Left[0].flag = 1;
        Left[0].grow = 2;      // 初始生长方向为2
        cur_row = Left[0].row; // 返回起始的行数
        cur_col = left_point;  // 这是全局变量，可以调用
        Left_Count++;
        while (Left_Max--) // 找140个
        {
            // 一 寻点生长
            // 0黑1白
            if (Image_Use_Robert[cur_row + 1][cur_col] == BLACK && Image_Use_Robert[cur_row + 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 0;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 1;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col - 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 2;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col] == WHITE)
            {
                if (Image_Use_Robert[cur_row][cur_col + 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col + 1] == WHITE &&
                    Image_Use_Robert[cur_row + 1][cur_col + 1] == WHITE)
                /*生长方向选择为6（正左），详情解析在Right里面*/
                {
                    Left[Left_Count].row = cur_row;
                    Left[Left_Count].column = cur_col + 1;
                    Left[Left_Count].flag = 1;
                    Left[Left_Count].grow = 6;
                    cur_row = Left[Left_Count].row;
                    cur_col = Left[Left_Count].column;
                    Left_Count++;
                }
                else
                {
                    if (cur_col != 2) // 处理分界点问题
                    {
                        Left[Left_Count].row = cur_row - 1;
                        Left[Left_Count].column = cur_col - 1;
                        Left[Left_Count].flag = 1;
                        Left[Left_Count].grow = 3;
                        cur_row = Left[Left_Count].row;
                        cur_col = Left[Left_Count].column; // 更新中心坐标点
                        Left_Count++;
                    }
                    else
                    {
                        Image_Use_Robert[cur_row - 1][cur_col] = BLACK; // 将跳变的白点变成黑色，下次就找得到了
                        Pixel_row = cur_row - 1;
                        Pixel_col = cur_col;
                        Pixel_Count_l = Left_Count; // 记录此时的计数值
                        Pixel_Change = 1;           // 1为变成了黑色
                    }
                }
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col] == BLACK && Image_Use_Robert[cur_row - 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 4;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 5;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col + 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 6;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col] == WHITE)
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
            // 二 1.检验越界
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            // 2 黑框分点的还原
            if ((Left_Count - Pixel_Count_l) == 1)
            {
                Image_Use_Robert[Pixel_row][Pixel_col] = WHITE;
            }
        }
    }
    // 采取左右对称，后面好写
    /*
    5	4	3
    6		2
    7	0	1
    */
    if (Image_Get_RightPoint2(117))
    {
        Right[0].row = Image_Get_RightPoint(117);
        Right[0].column = right_point + 1;
        Right[0].flag = 1;
        Right[0].grow = 2;
        cur_row = Right[0].row;
        cur_col = right_point;
        Right_Count++;
        while (Right_Max--)
        {
            // 0黑1白
            if (Image_Use_Robert[cur_row + 1][cur_col] == BLACK && Image_Use_Robert[cur_row + 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 0;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 1;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col + 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 2;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col] == WHITE)
            {
                /*
                5 7检测为白，6为黑——检测竖直状态拐点问题（十字路口扫线扫不上去的问题）
                注：放在这里的话扫线速度更快（因为都是else if结构，提高算法的速度）
                */
                if (Image_Use_Robert[cur_row][cur_col - 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col - 1] == WHITE &&
                    Image_Use_Robert[cur_row + 1][cur_col - 1] == WHITE)
                /*生长方向选择为6（正左）*/
                {
                    Right[Right_Count].row = cur_row;
                    Right[Right_Count].column = cur_col - 1;
                    Right[Right_Count].flag = 1;
                    Right[Right_Count].grow = 6;
                    cur_row = Right[Right_Count].row;
                    cur_col = Right[Right_Count].column;
                    Right_Count++;
                }
                else // 提高优先级90°巡线判断的优先级
                {
                    if (cur_col != 157) // 处理分界点问题：当4黑 5白 6黑 7白 的时候，由于（右边）是逆时针扫描，就会有bug
                    {
                        Right[Right_Count].row = cur_row - 1;
                        Right[Right_Count].column = cur_col + 1;
                        Right[Right_Count].flag = 1;
                        Right[Right_Count].grow = 3;
                        cur_row = Right[Right_Count].row;
                        cur_col = Right[Right_Count].column;
                        Right_Count++;
                    }
                    else
                    {
                        Image_Use_Robert[cur_row - 1][cur_col] = BLACK; // 将跳变的白点变成黑色，下次就找得到了
                        Pixel_row = cur_row - 1;
                        Pixel_col = cur_col;
                        Pixel_Count_r = Right_Count; // 记录此时的计数值
                        Pixel_Change = 1;            // 1为变成了黑色
                    }
                }
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col] == BLACK && Image_Use_Robert[cur_row - 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 4;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 5;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col - 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 6;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col] == WHITE)
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
            // 下面是巡线以后其他的处理
            // 1  越界处理
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            // 2 变化灰度的还原的处理（因为不处理的话下次就还是会扫错）
            if ((Right_Count - Pixel_Count_r) == 1) // 确保是变黑的下一次就变白（记录次数，防止误扫）
            {
                Image_Use_Robert[Pixel_row][Pixel_col] = WHITE; // 一般都是白->黑->白
            }
            //			//下面是巡线以后其他的处理
            //            //一 越界处理
            //			if(cur_row<=2||cur_row>=(IMAGE_WIDTH-3)||cur_col<=2||cur_col>=(IMAGE_WIDTH-3))
            //			{
            //				break;
            //			}
            //            //以下是待测试的代码
            //            //二 连续3次都是同一个点就不再寻找
            //            if(Right[Right_Count].row==Right[Right_Count-1].row&&Right[Right_Count].column==Right[Right_Count-1].column&&
            //                Right[Right_Count-1].row==Right[Right_Count-2].row&&Right[Right_Count-1].column==Right[Right_Count-2].column)
            //            {
            //                break;
            //            }
            //            //三 当扫到的点多个在同一行时，只保留最后一个点（这个目前不写，因为处理的是压缩后的图像，点多一点也没关系，如果需要的话后期再改进）
            //            //四 当左右线会聚时退出循环，并记录此时的行数（判断坡道）
            //            if((Left[Left_Count-1].row==Right[Right_Count-1].row)&&(Left[Left_Count-1].column==Right[Right_Count-1].column||
            //            Right[Right_Count-1].column-Left[Left_Count-1].column<=3))
            //            //判断条件：行相等，列也相等or列相差在3以内
            //            {
            //                Gather_row[Gather_Count]=Left[Left_Count-1].row;
            //                Gather_flag=1;
            //                break;
            //            }
            //            else
            //            {
            //                Gather_flag=0;
            //            }
        }
    }
}

void Image_Get_neighborhoods2(uint8 (*Image_Use)[IMAGE_WIDTH])
{
    Left_Count = 0;
    Right_Count = 0;
    uint16 cur_row, cur_col;            // 当前行列
    uint8 Pixel_Change;                 // 灰白像素变化标志位——恢复原来的bug
    uint8 Pixel_Count_r, Pixel_Count_l; // 计算变化对应的下标值
    uint8 Pixel_row, Pixel_col;         // 记录变化像素点的坐标（右坐标）
    if (Image_Get_LeftPoint(117))
    {
        Left[0].row = Image_Get_LeftPoint(117);
        Left[0].column = left_point - 1;
        Left[0].flag = 1;
        Left[0].grow = 2;      // 初始生长方向为2
        cur_row = Left[0].row; // 返回起始的行数
        cur_col = left_point;  // 这是全局变量，可以调用
        Left_Count++;
        while (Left_Max--) // 找140个
        {
            // 一 寻点生长
            // 0黑1白
            if (Image_Use[cur_row + 1][cur_col] == BLACK && Image_Use[cur_row + 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 0;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col - 1] == BLACK && Image_Use[cur_row][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 1;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row][cur_col - 1] == BLACK && Image_Use[cur_row - 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 2;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col - 1] == BLACK && Image_Use[cur_row - 1][cur_col] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 3;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col] == BLACK && Image_Use[cur_row - 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 4;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col + 1] == BLACK && Image_Use[cur_row][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 5;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row][cur_col + 1] == BLACK && Image_Use[cur_row + 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 6;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col + 1] == BLACK && Image_Use[cur_row + 1][cur_col] == WHITE)
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
            // 二 1.检验越界
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            // 2 黑框分点的还原
            if ((Left_Count - Pixel_Count_l) == 1)
            {
                Image_Use[Pixel_row][Pixel_col] = WHITE;
            }
        }
    }
    // 采取左右对称，后面好写
    /*
    5	4	3
    6		2
    7	0	1
    */
    if (Image_Get_RightPoint(117))
    {
        Right[0].row = Image_Get_RightPoint(117);
        Right[0].column = right_point + 1;
        Right[0].flag = 1;
        Right[0].grow = 2;
        cur_row = Right[0].row;
        cur_col = right_point;
        Right_Count++;
        while (Right_Max--)
        {
            // 0黑1白
            if (Image_Use[cur_row + 1][cur_col] == BLACK && Image_Use[cur_row + 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 0;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col + 1] == BLACK && Image_Use[cur_row][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 1;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row][cur_col + 1] == BLACK && Image_Use[cur_row - 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 2;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col + 1] == BLACK && Image_Use[cur_row - 1][cur_col] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 3;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col] == BLACK && Image_Use[cur_row - 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 4;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col - 1] == BLACK && Image_Use[cur_row][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 5;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row][cur_col - 1] == BLACK && Image_Use[cur_row + 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 6;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col - 1] == BLACK && Image_Use[cur_row + 1][cur_col] == WHITE)
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
            // 下面是巡线以后其他的处理
            // 1  越界处理
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            //			//下面是巡线以后其他的处理
            //            //一 越界处理
            //			if(cur_row<=2||cur_row>=(IMAGE_WIDTH-3)||cur_col<=2||cur_col>=(IMAGE_WIDTH-3))
            //			{
            //				break;
            //			}
            //            //以下是待测试的代码
            // 二 连续3次都是同一个点就不再寻找
            if (Right[Right_Count].row == Right[Right_Count - 1].row && Right[Right_Count].column == Right[Right_Count - 1].column &&
                Right[Right_Count - 1].row == Right[Right_Count - 2].row && Right[Right_Count - 1].column == Right[Right_Count - 2].column)
            {
                break;
            }
            //            //四 当左右线会聚时退出循环，并记录此时的行数（判断坡道）
            //            if((Left[Left_Count-1].row==Right[Right_Count-1].row)&&(Left[Left_Count-1].column==Right[Right_Count-1].column||
            //            Right[Right_Count-1].column-Left[Left_Count-1].column<=3))
            //            //判断条件：行相等，列也相等or列相差在3以内
            //            {
            //                Gather_row[Gather_Count]=Left[Left_Count-1].row;
            //                Gather_flag=1;
            //                break;
            //            }
            //            else
            //            {
            //                Gather_flag=0;
            //            }
        }
    }
}



/**
 * @brief 坡道判断函数
 *
 * @param void
 * @example Image_Run()
 * @explanation  判断左右线相交的行坐标不断增加，从而判断出为坡道
 * （想过用坡道的拐点左右的斜率来判断坡道，但是应该找不到）
 */
void Image_Ramp(void)
{
    uint8 i;                        // 中间变量
    uint8 Ramp_flag, Last_Rampflag; // 坡道标志位，以及上一个状态标志位
    if (Gather_flag == 1)           // 找到坡道
    {
        for (i = 0; i < Gather_Count; i++)
        {
            if ((Gather_row[i] > Gather_row[i + 1]) && (Gather_row[i + 1] > Gather_row[i + 2])) // 表示正在靠近坡道，最高行不断上升
            {
                Last_Rampflag = Ramp_flag; // 记录上一个状态
                Ramp_flag = 1;             // 标志位为1，表示还没上坡道，还在坡道附近的直道
            }
            else if ((Gather_row[i] < Gather_row[i + 1]) && (Gather_row[i + 1] < Gather_row[i + 2])) // 最高行变小，表示已经上坡道了
            {
                Last_Rampflag = Ramp_flag;
                Ramp_flag = 2; // 标志位为2，表示已经上坡道了，此时需要加速
                // Control_SpeedUp();//小车加速（电控的代码）
            }
            else
            {
                Last_Rampflag = Ramp_flag;
                Ramp_flag = 0;
            }
        }
    }
    if (!Gather_flag && !Ramp_flag && Last_Rampflag == 2) // 判断为下坡状态
    {
        // Control_SpeedDown();//小车减速（电控的代码）
    }
}

/**
 * @brief 计算两个整数的绝对值之差
 *
 * @param a 整数a
 * @param b 整数b
 * @return int 返回a和b的绝对值之差
 */
int abs_int(int a, int b)
{
    int diff = a - b;
    if (diff < 0)
    {
        return -diff;
    }
    else
    {
        return diff;
    }
}

/**
 * @brief 计算左侧图像中左边线生长方向与给定方向相同的数量
 *
 * @param Direction 给定的生长方向
 * @return uint8 相同方向的数量
 */
int Image_LeftGrowDirection(uint8 end, uint8 Direction)
{
    uint8 i, direction_count;
    for (i = 0; i < end; i++)
    {
        if (Left[i].grow == Direction) // 如果方向一致的话，就计数自增
        {
            direction_count++;
        }
    }
    return direction_count;
}

/**
 * @brief 扫描60行中的某一行的黑白跳变的函数（检测斑马线）
 *
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */
uint8 Image_Scan_Row(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_row)
{
    uint8 i;                     // 中间变量
    uint8 BLACK_WHITE_count = 0; // 黑白跳变的计数
    for (i = 0; i < IMAGE_WIDTH - 1; i++)
    {
        if (Image_Use[target_row][i] != Image_Use[target_row][i + 1]) // 如果不相等的话，就说明是黑白跳变
        {
            BLACK_WHITE_count++;
        }
    }
    return BLACK_WHITE_count;
}

/**
 * @brief 扫描100行中的某一行的黑白跳变的函数（检测斑马线）
 *
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 *
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */

// 限幅函数
int clip(int x, int low, int up)
{
    return x > up ? up : x < low ? low
                                 : x;
}

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
/**
 * @brief 左线的三角滤波
 *
 * @param int num 左线的点数总个数 int kernel 内核大小
 * @return 无，输出值为边线Left_New数组
 * @example void Image_blur_points_Left(Left_Count,5);
 */
void Image_blur_points_Left(int num, int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        Left_New[i].row = Left_New[i].column = 0;
        for (int j = -half; j <= half; j++)
        {
            Left_New[i].row += (float)Left[clip(i + j, 0, num - 1)].row * (half + 1 - abs(j));
            Left_New[i].column += (float)Left[clip(i + j, 0, num - 1)].column * (half + 1 - abs(j));
        }
        Left_New[i].row /= (2 * half + 2) * (half + 1) / 2;
        Left_New[i].column /= (2 * half + 2) * (half + 1) / 2;
        Left_New[i].grow = Left[i].grow;
    }
}
/**
 * @brief 右线的三角滤波
 *
 * @param int num 右线的点数总个数 int kernel 内核大小
 * @return 无
 * @example void Image_blur_points_Left(Left_Count,5);
 */
void Image_blur_points_Right(int num, int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        Right_New[i].row = Right_New[i].column = 0;
        for (int j = -half; j <= half; j++)
        {
            Right_New[i].row += (float)Right[clip(i + j, 0, num - 1)].row * (half + 1 - abs(j));
            Right_New[i].column += (float)Right[clip(i + j, 0, num - 1)].column * (half + 1 - abs(j));
        }
        Right_New[i].row /= (2 * half + 2) * (half + 1) / 2;
        Right_New[i].column /= (2 * half + 2) * (half + 1) / 2;
        Right_New[i].grow = Right[i].grow;
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

#define ROAD_WIDTH (0.39)    // 赛道宽度45cm 适时调整 注意：应用方案三时情况特殊为负数-0.40,正常0.43


image_t img_raw = DEF_IMAGE(NULL, UCOL, UROW);

// 左右边丢线
uint8 loseline0;
uint8 loseline1;
int ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int ipts0_num;                // 存放边线像素点个数(左)
int ipts1_num;                // 存放边线像素点个数(右)

int ipts_new0[250][2];//拼接后的总数组
int ipts_new_num0;

int ipts_new1[250][2];//拼接后的总数组
int ipts_new_num1;

uint8 loseline00;
uint8 loseline11;

int ipts00[100][2];
int ipts11[100][2];//巡线上边线点的个数
int ipts00_num;
int ipts11_num;

// 逆透视变换后左右边线
float rpts0[POINTS_MAX_LEN][2];
float rpts1[POINTS_MAX_LEN][2];
int rpts0_num, rpts1_num;

// 逆透视变换后左右边线再三角滤波后的边线数组
float rpts0b[POINTS_MAX_LEN][2];
float rpts1b[POINTS_MAX_LEN][2];
int rpts0b_num, rpts1b_num;

// 逆透视变换后左右边线再三角滤波后再等距采样的数组
float rpts0s[POINTS_MAX_LEN][2];
float rpts1s[POINTS_MAX_LEN][2];

int rpts0s_num, rpts1s_num;
int rpts0an_num, rpts1an_num;
int rpts0_num, rpts1_num;
int x0_first, y0_first, x1_first, y1_first; // 左右边线第一个点的坐标

int x1, y1;
int x2, y2;

int Mid_line[POINTS_MAX_LEN][2]; // 定义中线数组
int mid_num;                     // 中线数组点数

int rpts0a_num, rpts1a_num;

float rpts0a[POINTS_MAX_LEN];
float rpts1a[POINTS_MAX_LEN];

// 左右边线局部角度变化率+非极大抑制
float rpts0an[POINTS_MAX_LEN];
float rpts1an[POINTS_MAX_LEN];

int rptsc0_num, rptsc1_num;
// SOBEL二值化图像
// 左右跟踪后的中线
float (*rpts)[2];
int rpts_num;
// 归一化最终中线
float rptsn[POINTS_MAX_LEN][2];
int rptsn_num;
int aim_idx[1] ;// 多个预锚点位置

float aim_distance;

// 计算远锚点偏差值
float dx[1] ;
float dy[1] ;
float dn[1] ;

float error[1];

float ave_error;//速度控制输入变量

void test(void)
{
    //    int th;
    uint8 i;
    Image_Compress();
    int TH;
    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
    //    Image_Binarization(TH, Image_Use);
    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms

    img_raw.data = *Image_Use_Robert;

    Find_Borderline();

    Pespective(ipts0, ipts0_num, rpts0);
    rpts0_num = ipts0_num;

    Pespective(ipts1, ipts1_num, rpts1);
    rpts1_num = ipts1_num;

    // 三角滤波
    blur_points(rpts0, rpts0_num, rpts0b, (int)round(line_blur_kernel));
    rpts0b_num = rpts0_num;
    blur_points(rpts1, rpts1_num, rpts1b, (int)round(line_blur_kernel));
    rpts1b_num = rpts1_num;

    // 边线等距采样，num为逆透视后实际距离点，两点间距离为0.02*102=2cm
    rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]); // 求数组的长度 即等距采样后边线点个数
    resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
    rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
    resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);
    // 边线局部角度变化率 round():四舍五入
    local_angle_points(rpts0s, rpts0s_num, rpts0a, (int)round(angle_dist / sample_dist));
    rpts0a_num = rpts0s_num;
    local_angle_points(rpts1s, rpts1s_num, rpts1a, (int)round(angle_dist / sample_dist));
    rpts1a_num = rpts1s_num;
    // 角度变化率非极大抑制
    nms_angle(rpts0a, rpts0a_num, rpts0an, (int)round(angle_dist / sample_dist) * 2 + 1);
    rpts0an_num = rpts0a_num;
    nms_angle(rpts1a, rpts1a_num, rpts1an, (int)round(angle_dist / sample_dist) * 2 + 1);
    rpts1an_num = rpts1a_num;
	
    aim_distance=aim_distance_flash;//设定预瞄距离
    
    // for (i = 0; i < ipts00_num; i++)
    // {
    //     ips200_draw_point(ipts00[i][0]+5, ipts00[i][1] + 200, RGB565_RED);
    // }
    // for (i = 0; i < ipts11_num; i++)
    // {
    //     ips200_draw_point(ipts11[i][0]+5, ipts11[i][1] + 200, RGB565_BLUE);
    // }
	
	for (i = 0; i < ipts0_num; i++)
    {
        ips200_draw_point(ipts0[i][0]+5, ipts0[i][1] + 200, RGB565_RED);
    }
    for (i = 0; i < ipts1_num; i++)
    {
        ips200_draw_point(ipts1[i][0]+5, ipts1[i][1] + 200, RGB565_GREEN);
    }

    // for (i = 0; i < ipts1_num; i++)
    // {
    //     ips200_draw_point(ipts1[i][0]+5, ipts1[i][1] + 200, RGB565_BLUE);
    // }
   for (int i = 0; i < 200; i++)
   {
       for (int j = 0; j < 200; j++)
       {
           ips200_draw_point(i, j, RGB565_BLACK); // 0xffff=255区域内显示白点
       }
   }

   for (int i = 0; i < rpts0_num; i++) // 显示左边线
   {
       uint16 x, y;
       x = func_limit_ab(rpts0[i][0],  0, 200);
       y = func_limit_ab(rpts0[i][1], 0, 199);

       ips200_draw_point(x+20, 200 - y, RGB565_GREEN); // 左线为绿色 不知道为什么改成-x/2+50就能正常先显示

   }
   
   for (int i = 0; i < rpts0s_num; i++) // 显示左边线
   {
       uint16 x, y;
       x = func_limit_ab(rptsc0[i][0],  0, 200);
       y = func_limit_ab(rptsc0[i][1], 0, 199);

       ips200_draw_point(x+20, 200 - y, RGB565_RED); // 左线为绿色 不知道为什么改成-x/2+50就能正常先显示

   }
   
   for (int i = 0; i < rpts1s_num; i++) // 显示左边线
   {
       uint16 x, y;
       x = func_limit_ab(rptsc1[i][0],  0, 200);
       y = func_limit_ab(rptsc1[i][1], 0, 199);

       ips200_draw_point(x+20, 200 - y, RGB565_BLUE); // 左线为绿色 不知道为什么改成-x/2+50就能正常先显示

   }
   

   for (int i = 0; i < rpts0_num; i++) // 显示右边线
   {
       uint16 x, y;
       x = func_limit_ab(rpts1s[i][0], 0, 200);
       y = func_limit_ab(rpts1s[i][1], 0, 199);

       ips200_draw_point(x+20, 200 - y, RGB565_YELLOW);
   }
   ips200_show_uint(160,260,touch_boundary1,4);
	Find_Borderline_Second();//找到上边线

    find_corners();//找拐点

    //十字根据远线控制 地址平移+数组删减
    // if (track_type == TRACK_LEFT) {
    //     track_leftline(far_rpts0s + far_Lpt0_rpts0s_id, far_rpts0s_num - far_Lpt0_rpts0s_id, rpts,
    //                    (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
    //     rpts_num = far_rpts0s_num - far_Lpt0_rpts0s_id;
    // } else {
    //     track_rightline(far_rpts1s + far_Lpt1_rpts1s_id, far_rpts1s_num - far_Lpt1_rpts1s_id, rpts,
    //                     (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
    //     rpts_num = far_rpts1s_num - far_Lpt1_rpts1s_id;
    // }
       //十字补线方案
    //    if (track_type == TRACK_LEFT) //如果巡的是左线的话，那么就把中线赋值为左线的逆透视数组
    //    {
    //        rpts = rptsc0;
    //        rpts_num = rptsc0_num;
    //    } else {
    //        rpts  = rptsc1;
    //        rpts_num = rptsc1_num;
    //    }

    //先平移左右线，使左右线拟合（现在默认是直道的状态）,这里的左右线就默认为中线了
    track_leftline(rpts0s, rpts0s_num, rptsc0,
                       10.0, pixel_per_meter * ROAD_WIDTH / 2);
    track_rightline(rpts1s, rpts1s_num, rptsc1,
                        10.0, pixel_per_meter * ROAD_WIDTH / 2);

    if(rpts0s_num>rpts1s_num)
    {
        rpts_num = rpts1s_num;

    }
    else
    {
        rpts_num=rpts0s_num;
    }

    for(i=0;i<rpts_num;i++)
    {
        rpts[i][0]=(rptsc0[i][0]+rptsc1[i][0])/2;
        rpts[i][1]=(rptsc0[i][1]+rptsc1[i][1])/2;//行坐标不知道要不要平均
//		ips200_draw_point(rpts[i][0],rpts[i][1]+200,RGB565_WHITE);
    }
	for (int i = 0; i < rpts_num; i++) // 显示中
    {
       uint16 x, y;
       x = func_limit_ab(rpts[i][0], 0, 200);
       y = func_limit_ab(rpts[i][1], 0, 199);

       ips200_draw_point(x+20, 200 - y, RGB565_WHITE);
    }
   
    
    //纯跟踪起始点 根据方案不同调整！
    float cx, cy; //思考：由于逆透视出来的是现实坐标系，考虑把图像最低行中间起始点改为小车重心点会不会好一些；若是重心，则cx，cy为摄像头坐标距重心坐标距离
    //这里没有赋值，就默认为0
//    cx = UCOL/2+0.5;//方案一
//    cy = UROW;

    Pespective_xy( UCOL/2+0.5 ,begin_y ,  &cx, &cy);//对起始点进行逆透视，得到现实坐标系下的坐标，方便中线拟合，找到起始点
//    rot_xy( UCOL/2+0.5 ,begin_y ,  &cx, &cy);//方案三
   
    //找最近点(起始点中线归一化)
    float min_dist = 1e10;//存储逆透视的最小距离值，待改进
    int   begin_id = -1;//起始点在直线的下标索引

    //找出起始点，和最小的下标索引
   for (int i = 0; i < rpts_num; i++) 
   {
       float dx = rpts[i][0] - cx;
       float dy = rpts[i][1] - cy;//求出每个点到x,y变化值
       //float dist = mySqrt(dx * dx + dy * dy);
       float dist = sqrt(dx * dx + dy * dy);//求弧长微分
       //计算rpts上每一点到起始点cx,cy的距离
       if (dist < min_dist)//如果当前点到起始点的距离小于已知的最小距离，则更新最小距离和最近点的下标索引
       {
           min_dist = dist;
           begin_id = i;
       }
   }
//    ips200_show_float(160,240,min_dist,4,4);
	
    // 中线有点，同时最近点不是最后几个点
    if (begin_id >= 0 && rpts_num - begin_id >= 3)//这个if条件是用来判断中线是否有足够的点数，同时最近点不是最后几个点
    {
        // 归一化中线
        rpts[begin_id][0] = cx;
        rpts[begin_id][1] = cy;//选定起始点
        rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);//sizeof(rptsn) 表示 rptsn 数组的总字节数，sizeof(rptsn[0]) 表示 rptsn 数组中每个元素的字节数
        resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);
        //对中线进行采样，以便后续的计算。具体来说，它选定了起始点，然后使用 resample_points 函数对中线进行采样，将采样结果存储在 rptsn 数组中
        //rpts + begin_id 表示从中线的第 begin_id 个点开始采样，rpts_num - begin_id 表示采样的点数，rptsn 表示采样结果存储的数组，&rptsn_num 表示采样结果的点数
        
        /***************计算远处偏差均值*************/
        // 预锚点位置 第34点作为控制偏差 注意：最低行距离车头约10cm距离，距离摄像头25cm距离 故预瞄点现实中为60cm
//        int i;
//        for(i = 0;i<5;i++){
//            aim_idx[i] = clip(round(aim_dist[i] / sample_dist), 0, rptsn_num - 1);
//
//
//            // 计算远锚点偏差值      cx，cy为摄像头坐标距重心坐标距离 0,-7
//            dx[i] = rptsn[aim_idx[i]][0] - cx;
//            dy[i] = cy - rptsn[aim_idx[i]][1] ;
//            dn[i] = sqrt(dx[i] * dx[i] + dy[i] * dy[i]);
//            //dn = mySqrt(dx * dx + dy * dy);
//            //error>0 左 error<0 右 以下为角度制换算为弧度制
//            error[i] = atan2f(dx[i], -dy[i]) * 180 / PI;//1弧度=180/π度 1度=π/180弧度
//            //assert(!isnan(error));
//
//
//        }
//        ave_error = error[0]*0.3 + error[1]*0.27 + error[2]*0.21 + error[3]*0.14 + error[4]*0.08;//正态分布
        /***************计算远处偏差均值*************/

//        aim_idx[0] = clip(round(aim_dist[0] / sample_dist), 0, rptsn_num - 1);

        aim_idx[0] = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);//计算目标点在采样后的中线上的索引位置
		
		aim_idx[0]=aim_idx[0]/2;
        //这个数组只有一个数值，就是目标点在采样后的中线上的索引位置
        ips200_show_uint(160,260,aim_idx[0],4);
        // 计算远锚点偏差值      cx，cy为摄像头坐标距重心坐标距离 0,-7
        dx[0] = rptsn[aim_idx[0]][0] - cx;//这里计算了目标点到摄像头的x距离
        dy[0] = rptsn[aim_idx[0]][1] - cy;//目标点到摄像头的y距离
        dn[0] = sqrt(dx[0] * dx[0] + dy[0] * dy[0]);//用勾股定理求出目标点到摄像头的距离
        //dn = mySqrt(dx * dx + dy * dy);
        //error<0(输出为正) 左 error>0(输出为负) 右 以下为角度制换算为弧度制
        error[0] = atan2f(dx[0], dy[0]) * 180 / PI;//1弧度=180/π度 1度=π/180弧度
        //即这两个向量的夹角，返回值的单位是弧度制。然后，将这个弧度值乘以 180 / PI，即将其转换为角度制，最后将结果赋值给 error[0]，表示这两个向量的夹角
        ips200_show_float(160,240,error[0],4,4);
        ave_error = 0;
        // ips200_show_float(160,240,ave_error,4,4);
        // 远近锚点综合考虑
        //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
        // 根据偏差进行PD计算
        //float angle = pid_solve(&servo_pid, error);

        // 纯跟踪算法(只考虑远点)三轮控制用不到
//        pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / 1;
      } else {
          // 中线点过少(出现问题)，则不控制舵机
          rptsn_num = 0;
      }

    //  end = system_getval_us();
}

void test_new(void)
{
    //    int th;
    uint8 i;
    Image_Compress();
    int TH;
    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
    //    Image_Binarization(TH, Image_Use);
    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms

    img_raw.data = *Image_Use_Robert;

    Find_Borderline();

   for (int i = 0; i < 200; i++)
   {
       for (int j = 0; j < 200; j++)
       {
           ips200_draw_point(i, j, RGB565_BLACK); // 0xffff=255区域内显示白点
       }
   }
	Find_Borderline_Second();
	Cross_Drawline_plus(ipts0,ipts0_num,ipts1,ipts1_num,ipts00,ipts00_num,ipts11,ipts11_num);
}
/**
 * @brief 获取左右车道线的中线
 *
 * @param pts_l 左车道线点集
 * @param pts_l_num 左车道线点数
 * @param pts_r 右车道线点集
 * @param pts_r_num 右车道线点数
 */
/*
    2023/10/08优化（待测试）：1.判定弯道的丢线问题 2.同时舍弃掉过高以上的中线
修改以后测试成功，不过还是要继续优化
*/
void Get_Midline(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num)
{
    int i;
    /*丢线判定*/
    if (loseline0 == 1) // 左边丢线
    {
        mid_num = pts_r_num; // 中线点数取右边的点数
        for (i = 0; i < mid_num; i++)
        {
            Mid_line[i][0] = pts_r[i][0] / 2;
            Mid_line[i][1] = pts_r[i][1];
            if (Mid_line[i][0] <= 40) // 如果中线生长到2/3
                break;
        }
    }
    /*右边丢线*/
    else if (loseline1 == 1)
    {
        mid_num = pts_l_num;
        for (i = 0; i < mid_num; i++)
        {
            Mid_line[i][0] = (IMAGE_WIDTH - pts_l[i][0]) / 2 + pts_l[i][0];
            Mid_line[i][1] = pts_l[i][1];
            if (Mid_line[i][1] <= 45) // 如果中线生长到2/3
                break;
        }
    }
    /*两边都不丢线*/
    else
    {
        /*取两个边线中最小的一个做数组*/
        if (pts_l_num > pts_r_num)
        {
            mid_num = pts_r_num;
        }
        else
            mid_num = pts_l_num;
        /*取中线*/
        for (i = 0; i < mid_num; i++)
        {
            Mid_line[i][0] = (pts_l[i][0] + pts_r[i][0]) / 2;
            Mid_line[i][1] = (pts_l[i][1] + pts_r[i][1]) / 2;
            if (Mid_line[i][0] <= 40) // 如果中线生长到2/3
                break;
        }
    }
    /*显示*/
    for (i = 0; i < ipts0_num; i++)
    {
        ips200_draw_point(Mid_line[i][0], Mid_line[i][1], RGB565_RED);
        ips200_show_uint(3, 140, Mid_line[i][0], 3);
        ips200_show_uint(3, 160, Mid_line[i][1], 3);
    }
    for (i = 0; i < ipts1_num; i++)
    {
        ips200_draw_line(0, 0, pts_r[i][0], pts_r[i][1], RGB565_RED);
    }
    ips200_show_uint(3, 180, loseline0, 3);
    ips200_show_uint(3, 200, loseline1, 3);
}

void Get_Midline2(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num)
{
    int i, mid_num;
    /*取两个边线中最小的一个做数组*/
    if (pts_l_num > pts_r_num)
    {
        mid_num = pts_r_num;
    }
    else
    {
        mid_num = pts_l_num;
    }
    /*取中线*/
    for (i = 0; i < mid_num; i++)
    {
        Mid_line[i][0] = (pts_l[i][0] + pts_r[i][0]) / 2;
        Mid_line[i][1] = (pts_l[i][1] + pts_r[i][1]) / 2;
    }
    /*显示*/
    for (i = 0; i < ipts0_num; i++)
    {
        if (Mid_line[i][0] <= 60)
        {
            break;
        }
        ips200_draw_point(Mid_line[i][0], Mid_line[i][1], RGB565_RED);
    }
}

void Pespective_xy(int x_in,int y_in , float* x_out , float* y_out)
//只逆透视一个点 用于起始点跟踪
{
float x ,y ,w;

        x = getx(x_in, y_in);
        y = gety(x_in, y_in);
        w = getw(x_in, y_in);
        *x_out = x / w;
        *y_out = y / w;
}

#define TARGET_ANGEL // 目标机械中值对应的角度

/**
 * @brief 计算给定中线的偏差（只适用于直道）
 *
 * @param pts_in 给定点集
 * @param num 点集中点的数量
 * @return float 误差值，返回给舵机
 */
float Get_err1(int pts_in[][2], int num)
{
    float k, err, last_err;
    k = LineRession(Mid_line, mid_num); // 对中线线性回归，先对中线进行处理了，所以不会超过80行
    err = -k;                           // 符合舵机的斜率，左负右正
    err = err * 0.8 + last_err * 0.2;   // 滤波
    last_err = err;                     // 更新上一次的偏差值
    return err;
}

void Find_Borderline(void)
{
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
    //    uint8 uthres = ostu();
    // 寻左边线
    x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
//     int TH;
//    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
//    Image_Binarization(TH, Image_Use);
//    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms
//    img_raw.data = *Image_Use;

    // 标记种子起始点(后续元素处理要用到)
    x0_first = x1;
    y0_first = y1;

    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]); // 求数组的长度
    // 扫底下五行，寻找跳变点
    for (; y0_first > begin_y - 100; y0_first--)//从所选的行，向上扫50次，每次从中间向左线扫
    {
        for (; x0_first > 0; x0_first--)//在选的每行中，从中间向左线扫
            if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)//如果扫到黑点（灰度值为0），就从该点开始扫线
                goto out1;//开始扫左线
        x0_first = img_raw.width / 2 - begin_x;//每次每一行扫完，都把x0_first归位
    }
    //如果扫不到的话，判定左边的底边丢线
    loseline0 = 1; // 底边丢线
	out1://从起始点开始执行扫线
	{
		if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
		{	
//			while(1);
			Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);//开始跑迷宫
		}
		else
			ipts0_num = 0;//如果不是的话，就不用跑了，求得的number记为0
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

void Find_Borderline_Second(void)
{
    uint8 uthres = 1;
	if(touch_boundary0==1)
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
    x0_first = 2;
    y0_first = ipts0[ipts0_num-1][1]-5;

    ipts00_num = sizeof(ipts00) / sizeof(ipts00[0]); // 求数组的长度
    // 扫底下五行，寻找跳变点
    for (; y0_first >20; y0_first--)//从所选的行，向上扫5次，每次从中间向左线扫
    {
        if (AT_IMAGE(&img_raw, x0_first, y0_first) < uthres)//如果扫到黑点（灰度值为0），就从该点开始扫线
          {  
            goto out1;//开始扫左线
          }
    }
    //如果扫不到的话，判定左边的底边丢线
    loseline00 = 1; // 底边丢线
	out1://从起始点开始执行扫线
	{
		// if (AT_IMAGE(&img_raw, x0_first+1, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
			Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts00, &ipts00_num);//开始跑迷宫
		// else
		// 	ipts00_num = 0;//如果不是的话，就不用跑了，求得的number记为0
	}
    }
    if(touch_boundary1==1)
    {
    // 寻右边线
    x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

    // 标记种子起始点(后续元素处理要用到)
    x1_first = 156;
    y1_first = ipts1[ipts1_num-1][1]-5;;

    ipts11_num = sizeof(ipts11) / sizeof(ipts11[0]);
    for (; y1_first > 20; y1_first--)
    {
        if (AT_IMAGE(&img_raw, x1_first , y1_first) < uthres)
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

#define AT AT_IMAGE
#define MAX_WIDTH 88 定义图像中
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
        if ((x == 1 && y < img->height - 30) || x == img->width - 2 || y == 1 || (y == 20 && step > 19))//30修改后能扫线
        {
            if (x == 1 /*|| x== img->width - 2*/)
                touch_boundary0 = 1; // 左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
            if (y == 1)
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
            pts[step][0] = x; // 用来存放边线坐标信息
            pts[step][1] = y;
            step++;
            turn = 0;
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
//    uint8 i;
//   	for(i=0;i<ipts0_num;i++)
//   	{
//   		ips200_draw_line(0,0,ipts0[i][0],ipts0[i][1],RGB565_RED);
//   	}
    // ips200_show_int(3,140,ipts0_num,3);
    // ips200_show_int(3,160,loseline0,3);
    // ips200_show_int(3,180,*num,3);
    //	tft180_draw_line(0,0,ipts0[20-1][1],ipts0[20-1][0],RGB565_RED);
    //	tft180_show_int(3,100,ipts0[20-1][1],4);
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
        if ((x == img->width - 2 && y < img->height - 70) || x == 1 || y == 1 || (y == 20 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线
        {
            if (x == img->width - 2 /*|| x==1*/)
                touch_boundary1 = 1; // 右边界是因为到最右边才停下来的，触碰到最右边，可能是环岛，十字等，
            if (y == 1)
                touch_boundary_up1 = 1; // 走到顶边，判断坡道or障碍
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
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
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
uint8 Cross_State_b;//十字路口前
uint8 Cross_State_d;//十字路口前中
uint8 Cross_State_c;//十字路口中
uint8 Straight_State;//直道
uint8 Left_Turn;//左弯道
uint8 Right_Turn;//右弯道

/**
 * @brief 清空图像状态变量
 * 
 */
void Image_Clear(void)
{
    Cross_State_b = 0;
    Cross_State_c = 0;
    Straight_State = 0;
    Left_Turn = 0;
    Right_Turn = 0;
}

/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 * 
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */
void Image_CheckState(int in_put_l[][2],int in_put_num_l,int in_put_r[][2],int in_put_num_r)
{
    /*十字路口前：左右不丢线 左边线丢线和右边线丢线的列坐标相差较大，行坐标相差较少*/
    if((loseline0==0&&loseline1==0&&(in_put_r[in_put_num_r-1][0]-in_put_l[in_put_num_l-3][0])>(IMAGE_WIDTH-10))
    &&(my_abs(in_put_l[in_put_num_l-3][1]-in_put_r[in_put_num_r-1][1])<10)&&Cross_State_d==0)
    {
        Cross_State_b = 1;
		Cross_State_c = 0;
		Cross_State_d = 0;
        Left_Turn = 0 ;
		Right_Turn = 0 ;
        Straight_State = 0;
    }
    /*十字路口中：左边线不丢线 右边线不丢线 左边线丢线和右边线丢线的列坐标相差较小，行坐标相差较少*/
	else if(loseline0==0&&loseline1==0&&Cross_State_d==1&&in_put_l[in_put_num_l-1][0]!=0)
    {
        Cross_State_c = 1;
		Cross_State_b = 0;
		Cross_State_d = 0;
        Left_Turn = 0 ;
		Right_Turn = 0 ;
        Straight_State = 0;
    }
	else if(loseline0==1&&loseline1==1&&Cross_State_b==1)
    {
        Cross_State_d = 1;
		Cross_State_b = 0;
		Cross_State_c = 0;
        Left_Turn = 0 ;
		Right_Turn = 0 ;
        Straight_State = 0;
    }
	
    /*直道：左右线找到的点超过100，左右线的最后一个点的行坐标在30以内*/
   else if(ipts0_num>100&&ipts1_num>100&&ipts0[ipts0_num-1][1]<30&&ipts1[ipts1_num-1][1]<30)
   {
       Straight_State=1;
       Right_Turn = 0;
		Left_Turn = 0;
        Cross_State_b = 0;
		Cross_State_c = 0;
		Cross_State_d = 0;
   }
    /*左弯道：左丢线，右边不丢线，右边线最大点距离左边小于50个的单位*/
    else if(loseline0==1&&loseline1==0&&in_put_r[in_put_num_r-1][0]<60)
    {
        Left_Turn = 1;
		Right_Turn = 0;
        Cross_State_b = 0;
		Cross_State_c = 0;
		Cross_State_d = 0;
        Straight_State = 0;
    }
    /*右弯道：右丢线，左边不丢线，左边线最大点距离右边小于50个的单位*/
    else if(loseline0==0&&loseline1==1&&in_put_l[in_put_num_l-1][0]>120)
    {
        Right_Turn = 1;
		Left_Turn = 0;
        Cross_State_b = 0;
		Cross_State_c = 0;
		Cross_State_d = 0;
        Straight_State = 0;
    }
}

/*左下线坐标变换，用于求拐点，左下角为0,0*/
void Coordinate_transformation_left(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for(i=0;i<in_num;i++)
    {
        pt0_out[i][1] = IMAGE_HEIGHT-pt0_in[i][1]-1;
		pt0_out[i][0] = pt0_in[i][0];
    }
}

/*右下线坐标变换，用于求拐点，将左上坐标移到右下坐标，左上角坐标原点迁移到右下角*/
void Coordinate_transformation_right(int pt0_in[][2], int in_num,int pt0_out[][2])
{
    int i;
    for(i=0;i<in_num;i++)
    {
        pt0_out[i][0] = IMAGE_WIDTH-pt0_in[i][0]-1;
        pt0_out[i][1] = IMAGE_HEIGHT-pt0_in[i][1]-1;
		ips200_draw_point(pt0_out[i][0],pt0_out[i][1]+200,RGB565_BLUE);
    }
}

/*右上线坐标变换，用于求拐点*/
void Coordinate_transformation_rightup(int pt0_in[][2], int in_num,int pt0_out[][2])
{
    int i;
    for(i=0;i<in_num;i++)
    {
        pt0_out[i][0] = IMAGE_WIDTH-pt0_in[i][0]-1;
        pt0_out[i][1] = IMAGE_HEIGHT;
    }
}

/*坐标复原，找完十字以后坐标就和以前一样*/
void Coordinate_restore_left(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for(i=0;i<in_num;i++)
    {
      pt0_out[i][0] = IMAGE_HEIGHT-pt0_in[i][0]-1;
    }
}

void Coordinate_restore_right(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for(i=0;i<in_num;i++)
    {
        pt0_out[i][0] = IMAGE_WIDTH-pt0_in[i][0]-1;
        pt0_out[i][1] = IMAGE_HEIGHT-pt0_in[i][1]-1;
    }
}

/*
十字状态一补线函数 ：判断坐标能否正常转换（draw_line）--->判断
*/
int Left_Change [POINTS_MAX_LEN][2];
int Right_Change [POINTS_MAX_LEN][2];
int Right_Change_new [100][2];
void Cross_Drawline(int in_put_l[][2],int in_put_num_l,int in_put_r[][2],int in_put_r_num)
{
    uint16 i;
    uint16 left_index,right_index;//左右拐点的坐标
	uint16 left_highest=0,right_highest=0;
	float k_left,k_right;
	float b_left,b_right;
    /*一 坐标转换*/
    Coordinate_transformation_left(in_put_l,in_put_num_l,Left_Change);//左右下线坐标变换
    Coordinate_transformation_right(in_put_r,in_put_r_num,Right_Change);
	
	
    /*二 找下拐点*/
    for(i=0;i<in_put_num_l;i++)
    {
        if((Left_Change[i][0]+Left_Change[i][1])>left_highest)//拐点的坐标之和最大
        {
			left_highest=(Left_Change[i][0]+Left_Change[i][1]);
            left_index = i;
            //遍历完，不用break
        }
    }

    for(i=0;i<in_put_r_num;i++)
    {
        if((Right_Change[i][0]+Right_Change[i][1])>right_highest)//拐点的坐标之和最大row=k*column+b
        {
			right_highest=(Right_Change[i][0]+Right_Change[i][1]);
            right_index = i;
            //遍历完，不用break
        }
    }

	ips200_draw_line(0,0,in_put_l[left_index][0],in_put_l[left_index][1],RGB565_RED);
	ips200_draw_line(0,0,in_put_r[right_index][0],in_put_r[right_index][1],RGB565_BLUE);
    /*三 求直线方程*/
	k_left=LineRession(in_put_l,left_index-5);
	
	b_left=in_put_l[left_index][1]-k_left*in_put_l[left_index][0];
	
	//求得的直线方程是row=column*k+b，实际上应该是column=row*k+b
	k_right=LineRession(in_put_r,right_index-5);
	
    b_right=in_put_r[right_index][1]-k_right*in_put_r[right_index][0];
	
    k_left=(1/k_left);//新斜率取倒数
    b_left= -b_left*k_left;//新截距取相反数

    k_right=(1/k_right);//新斜率取倒数
    b_right= -b_right*k_right;//新截距取相反数
    //新直线方程为 column=k*row+b
    /*四 补线*/
    for(i=in_put_l[left_index][1];i>10;i--)
    {
        int new_column_l=(int)(k_left*i+b_left);
        if(new_column_l>0)
        {
            Image_Use_Robert[i][new_column_l]=BLACK;
        }
    }
    for(i=in_put_r[right_index][1];i>10;i--)
    {
        int new_column_r=(int)(k_right*i+b_right);
        if(new_column_r>0)
        {
            Image_Use_Robert[i][new_column_r]=BLACK;
        }
    }
}

/*
十字补线函数2版：1. 取了上下拐点补线，更稳定
*/
void Cross_Drawline_plus(int in_put_l[][2],int in_put_num_l,int in_put_lnew[][2],int in_put_num_lnew,
                            int in_put_r[][2],int in_put_r_num, int in_put_rnew[][2],int in_put_r_numnew)
{
    uint16 i;
    uint16 left_index,right_index;//左右拐点的坐标
    uint16 left_index_new,right_index_new;//左右上拐点的坐标
	uint16 left_highest=0,right_highest=0;
	float k_left,k_right;
	float b_left,b_right;
    /*一 坐标转换*/
    Coordinate_transformation_left(in_put_l,in_put_num_l,Left_Change);//左右下线坐标变换，左右上线坐标不用变换
    Coordinate_transformation_right(in_put_r,in_put_r_num,Right_Change);
	Coordinate_transformation_rightup(in_put_rnew,in_put_r_numnew,Right_Change_new);//右上线坐标变换
	
    /*二 找左右下拐点*/
    for(i=0;i<in_put_num_l;i++)
    {
        if((Left_Change[i][0]+Left_Change[i][1])>left_highest)//拐点的坐标之和最大
        {
			left_highest=(Left_Change[i][0]+Left_Change[i][1]);
            left_index = i;
            //遍历完，不用break
        }
    }
    
    for(i=0;i<in_put_r_num;i++)
    {
        if((Right_Change[i][0]+Right_Change[i][1])>right_highest)//拐点的坐标之和最大row=k*column+b
        {
			right_highest=(Right_Change[i][0]+Right_Change[i][1]);
            right_index = i;
            //遍历完，不用break
        }
    }
    //峰值清零
    left_highest=0;
    right_highest=0;
    /*开始找左右上拐点*/
    for(i=0;i<in_put_num_lnew;i++)
    {
        if((in_put_lnew[i][0]+in_put_lnew[i][1])>left_highest)//拐点的坐标之和最大
        {

            left_highest=(in_put_lnew[i][0]+in_put_lnew[i][1]);
            left_index_new = i;
            //遍历完，不用break
        }
    }

    for(i=0;i<in_put_r_numnew;i++)
    {
        if((in_put_rnew[i][0]+in_put_rnew[i][1])>right_highest)//拐点的坐标之和最大row=k*column+b
        {
            right_highest=(in_put_rnew[i][0]+in_put_rnew[i][1]);
            right_index_new = i;
            //遍历完，不用break
        }
    }

	ips200_draw_line(0,200,in_put_l[left_index][0],in_put_l[left_index][1]+200,RGB565_RED);
	ips200_draw_line(0,200,in_put_r[right_index][0],in_put_r[right_index][1]+200,RGB565_BLUE);
    /*三 求直线方程（已知左右上下拐点）*/
	k_left=(in_put_l[left_index][0]-in_put_lnew[left_index_new][0]/in_put_l[left_index][1]-in_put_lnew[left_index_new][1]);//斜率
	
	b_left=in_put_l[left_index][1]-k_left*in_put_l[left_index][0];//近处点求截距更准确
	
	//求得的直线方程是row=column*k+b，实际上应该是column=row*k+b
	k_right=(in_put_r[right_index][0]-in_put_rnew[right_index_new][0]/in_put_r[right_index][1]-in_put_rnew[right_index_new][1]);//斜率
	
    b_right=in_put_r[right_index][1]-k_right*in_put_r[right_index][0];

    /*四 补线*/
    for(i=in_put_l[left_index][1];i>10;i--)
    {
        int new_column_l=(int)(k_left*i+b_left);
        if(new_column_l>0)
        {
            Image_Use_Robert[i][new_column_l]=BLACK;
        }
    }
    for(i=in_put_r[right_index][1];i>10;i--)
    {
        int new_column_r=(int)(k_right*i+b_right);
        if(new_column_r>0)
        {
            Image_Use_Robert[i][new_column_r]=BLACK;
        }
    }
}

int Finnal_left[140][2];
int Finnal_right[140][2];//定义左右边线扫的数组
int Finnal_left_num=0;
int Finnal_right_num=0;
int Finnal_Mid[90][2];
int Finnal_Mid_num;
/*
十字补中线，返回值为误差（中线误差）
*/
float Get_Mid_Cross(void)
{
    int start_x,start_y;
    int i;
    start_x=IMAGE_WIDTH/2;
    start_y=IMAGE_HEIGHT-1;
    /*一 判断为十字状态*/
    if(Cross_State_b==1||Cross_State_c==1||Cross_State_d==1)
    {
        Cross_Drawline_plus(ipts0,ipts0_num,ipts00,ipts00_num,ipts1,ipts1_num,ipts11,ipts11_num);
    }
    /*二 补线以后找中线*/
    for(start_y=IMAGE_HEIGHT-1;start_y>30;start_y--)
    {
        for(start_x=IMAGE_WIDTH/2;start_x>0;start_x--)
        {
            if(Image_Use_Robert[start_y][start_x]==BLACK)
            {
                Finnal_left[Finnal_Mid_num][0]=start_x;
                Finnal_left[Finnal_Mid_num][1]=start_y;
                Finnal_left_num++;
                break;
            }
        }
        start_x=IMAGE_WIDTH/2;//复位
    }
    for(start_y=IMAGE_HEIGHT-1;start_y>30;start_y--)
    {
        for(start_x=IMAGE_WIDTH/2;start_x<IMAGE_WIDTH-1;start_x++)
        {
            if(Image_Use_Robert[start_y][start_x]==BLACK)
            {
                Finnal_right[Finnal_Mid_num][0]=start_x;
                Finnal_right[Finnal_Mid_num][1]=start_y;
                Finnal_right_num++;
                break;
            }
        }
        start_x=IMAGE_WIDTH/2;//复位
    }
    
    for(i=0;i<Finnal_left_num;i++)
    {
        Finnal_Mid[i][0]=(Finnal_left[i][0]+Finnal_right[i][0])/2;
        Finnal_Mid[i][1]=(Finnal_left[i][1]+Finnal_right[i][1])/2;
        Finnal_Mid_num++;//一般都是90个，正常情况下都能扫到
    }
    /*三 求中线斜率误差*/
    return LineRession(Finnal_Mid,Finnal_Mid_num-10);//只取下面80行的点
}



void SplicingArray_int(int pt0[][2], int num1, int pt1[][2], int num2, int pt_out[][2],  uint8 x)
{
    int i, count;   // 用来计数
    for (i = 0; i < 10; i++) // 复制数组1
    {
        pt_out[i][0] = pt0[i][0];
        pt_out[i][1] = pt0[i][1];
        count++;
    }

    if (x) // 反向拼接
    {
        for (i = 0; i < 10; i++)
        {
            pt_out[num1 + i][0] = pt1[num2 - i - 1][0];
            pt_out[num1 + i][1] = pt1[num2 - i - 1][1];
            count++;
        }
    }
    else // 正向拼接
    {
        for (i = 0; i < 10; i++)
        {
            pt_out[num1 + i][0] = pt1[i][0];
            pt_out[num1 + i][1] = pt1[i][1];
            count++;
        }
    }
	count=30;
    ipts_new_num0 = count;
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

enum garage_type_e garage_type = GARAGE_NONE; // 初始化为向左出库 调试状态为NONE

/**
 * 识别图像中的Y型和L型拐点，并进行二次检查。
 *
 * @param sample_dist 采样距离
 * @param angle_dist 角度距离
 * @param pixel_per_meter 每米像素数
 * @param garage_type 车库类型
 * @param rpts0s_num 左边线点数
 * @param rpts0s 左边线点坐标
 * @param rpts0a 左边线点角度
 * @param rpts0an 左边线点角度是否为峰值
 * @param rpts1s_num 右边线点数
 * @param rpts1s 右边线点坐标
 * @param rpts1a 右边线点角度
 * @param rpts1an 右边线点角度是否为峰值
 * @param Ypt0_rpts0s_id 左边线Y型拐点点号
 * @param Ypt1_rpts1s_id 右边线Y型拐点点号
 * @param Lpt0_rpts0s_id 左边线L型拐点点号
 * @param Lpt1_rpts1s_id 右边线L型拐点点号
 * @param N_Lpt0_rpts0s_id 左边线内L型拐点点号
 * @param N_Lpt1_rpts1s_id 右边线内L型拐点点号
 * @param xielv_left_y_to_end 左边线Y型拐点到终点的斜率
 * @param xielv_right_y_to_end 右边线Y型拐点到终点的斜率
 */
//这里不找Y拐点，只找L拐点（十字，环岛）
void find_corners(void)
{
    // 识别Y,L拐点
    Ypt0_found = Ypt1_found = Lpt0_found = Lpt1_found = N_Lpt1_found = N_Lpt0_found = false;
    is_straight0 = rpts0s_num > 1.6 / sample_dist; // 逆透视之后的边线个数大于80个就是直道,也就是边线边长大于0.8m,并不是直向前0.8m
    is_straight1 = rpts1s_num > 1.6 / sample_dist;

    /*
    测试1：直道状态的判断是否成立
    */

    // ips200_show_uint(203,200,is_straight0,2);
    // ips200_show_uint(203,220,is_straight1,2);
    // 左边线角度
    for (int i = 1; i < rpts0s_num; i++)
    { // 只要数组里的角度符合的，Y或L就为true,然后就不在继续检测Y或L
        if (rpts0an[i] == 0)
            continue; // 以下检测只检测峰值（极大值）
        int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, rpts0s_num - 1);
        int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, rpts0s_num - 1);
        float conf = fabs(rpts0a[i]) - (fabs(rpts0a[im1]) + fabs(rpts0a[ip1])) / 2;//计算了当前点的角度与其前后两个点角度平均值的差值，判断拐点及其有用
        
        // Y角点阈值                 (30*  ,   50*)    并且距离在40个点之内 (注：30.转化浮点数)
        // if (Ypt0_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist)
        // {
        //     Ypt0_rpts0s_id = i;
        //     Ypt0_found = true;
        // }
        // L角点阈值                 (55*  ,    140*)  并且距离在40个点之内 修改距离为1.2m增长判断路障距离
        if (Lpt0_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist)
        {

            Lpt0_rpts0s_id = i;
            Lpt0_found = true;
//            ips200_draw_line(0,0,ipts0[Lpt0_rpts0s_id][0],ipts0[Lpt0_rpts0s_id][0],RGB565_GREEN);
			// ips200_show_uint(203,200,Lpt0_found,2);
//            if (rpts0a[Lpt0_rpts0s_id] < 0) // 这一个L点如果是向右拐的，就是内L点
//            {
//                N_Lpt0_rpts0s_id = Lpt0_rpts0s_id;
//                N_Lpt0_found = true;
//                //                //对内L角的线截断
//                //                rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
//                Lpt0_found = false;
//            }
        }
        // 长直道阈值
        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist)//conf大于5度（转化为弧度为 5/180 * PI），并且i小于1.4米除以采样距离sample_dist
            is_straight0 = false; // 只要中间有大角度，就不是长直道
        if (Ypt0_found == true && Lpt0_found == true && is_straight0 == false)
            break;
		
    }

    // 右边线角度
    for (int i = 1; i < rpts1s_num; i++)
    {
        if (rpts1an[i] == 0)
            continue;
        int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, rpts1s_num - 1);
        int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, rpts1s_num - 1);
        float conf = fabs(rpts1a[i]) - (fabs(rpts1a[im1]) + fabs(rpts1a[ip1])) / 2;
        // if (Ypt1_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist)
        // {
        //     Ypt1_rpts1s_id = i;
        //     Ypt1_found = true;
        // }
        if (Lpt1_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist)
        {
            Lpt1_rpts1s_id = i;
            Lpt1_found = true;

            // if (rpts1a[Lpt1_rpts1s_id] > 0) // 这一个L点如果是向左拐的，就是内L点
            // {
            //     N_Lpt1_rpts1s_id = Lpt1_rpts1s_id;
            //     N_Lpt1_found = true;
            //     //                //对内L角的线截断
            //     //                rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
            //     Lpt1_found = false;
            // }
        }

        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist)
            is_straight1 = false;
        if (Ypt1_found == true && Lpt1_found == true && is_straight1 == false)
            break;
    }


    // Y点二次检查,依据两角点距离及角点后张开特性
    // if (Ypt0_found && Ypt1_found)
    // {
    //     //==================================================================原检查
    //     float dx = rpts0s[Ypt0_rpts0s_id][0] - rpts1s[Ypt1_rpts1s_id][0];
    //     float dy = rpts0s[Ypt0_rpts0s_id][1] - rpts1s[Ypt1_rpts1s_id][1];
    //     float dn = sqrtf(dx * dx + dy * dy);
    //     if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter) // 两点距离不能大于60cm  (赛道40cm)
    //     {
    //         float dwx = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
    //                     rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
    //         float dwy = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
    //                     rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
    //         float dwn = sqrtf(dwx * dwx + dwy * dwy);
    //         if (!(dwn > 0.7 * pixel_per_meter && // 如两个Y点之后第50个 点之间的距离小于0.7m
    //               rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Ypt0_rpts0s_id][0] &&
    //               rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Ypt1_rpts1s_id][0]))
    //         {
    //             Ypt0_found = Ypt1_found = false;
    //         }
    //     }
    //     else
    //     {
    //         Ypt0_found = Ypt1_found = false;
    //     }

    //     float dx_temp, dy_temp;

    //     dx_temp = rpts0s[Ypt0_rpts0s_id][0] - rpts0s[rpts0s_num - 5][0];
    //     dy_temp = rpts0s[Ypt0_rpts0s_id][1] - rpts0s[rpts0s_num - 5][1];
    //     xielv_left_y_to_end = dy_temp / dx_temp; // 这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子

    //     dx_temp = rpts1s[Ypt1_rpts1s_id][0] - rpts1s[rpts1s_num - 5][0];
    //     dy_temp = rpts1s[Ypt1_rpts1s_id][1] - rpts1s[rpts1s_num - 5][1];
    //     xielv_right_y_to_end = dy_temp / dx_temp; // 这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子
    // }
    // L点二次检查，车库模式不检查, 依据L角点距离及角点后张开特性
    if (garage_type == GARAGE_NONE)
    {
        if (Lpt0_found && Lpt1_found)//如果两个L角点都找到的话
        {
            float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
            float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
            float dn = sqrtf(dx * dx + dy * dy);//计算弧长微分
            if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter) // 两点距离不能大于60cm  (赛道40cm)
            {
                float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
                float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
                float dwn = sqrtf(dwx * dwx + dwy * dwy);
                if (!(dwn > 0.5 * pixel_per_meter && // 符合直角的特征必须50个点后距离大于0.7m，
                      rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
                      rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
                {
                    Lpt0_found = Lpt1_found = false;
                }
            }
            else
            {
                Lpt0_found = Lpt1_found = false;
            }

            float dx_temp, dy_temp;

            dx_temp = rpts0s[Lpt0_rpts0s_id][0] - rpts0s[rpts0s_num - 5][0];
            dy_temp = rpts0s[Lpt0_rpts0s_id][1] - rpts0s[rpts0s_num - 5][1];
            xielv_left_y_to_end = dy_temp / dx_temp; // 这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子  一般是负

            dx_temp = rpts1s[Lpt1_rpts1s_id][0] - rpts1s[rpts1s_num - 5][0];
            dy_temp = rpts1s[Lpt1_rpts1s_id][1] - rpts1s[rpts1s_num - 5][1];
            xielv_right_y_to_end = dy_temp / dx_temp; // 这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子
        }
    }

    // 左车库第二次检查
    // if (Lpt0_found && N_Lpt1_found)
    // { // 理论上是左车库
    //     float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
    //     float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
    //     float dn = sqrtf(dx * dx + dy * dy);
    //     if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter) // 两点距离不能大于60cm  (赛道45cm)
    //     {
    //         float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
    //                     rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
    //         float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
    //                     rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
    //         float dwn = sqrtf(dwx * dwx + dwy * dwy);
    //         if (!(dwn < 0.5 * pixel_per_meter && // 符合左车库特征必须50个点后距离小于0.5m，
    //               rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
    //               rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] < rpts1s[Lpt1_rpts1s_id][0]))
    //         {
    //             Lpt0_found = N_Lpt1_found = false;
    //         }
    //     }
    //     else
    //     {
    //         Lpt0_found = N_Lpt1_found = false;
    //     }
    // }

    // 右车库第二次检查
    if (Lpt1_found && N_Lpt0_found)
    { // 理论上是右车库
        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
        float dn = sqrtf(dx * dx + dy * dy);//算个弧长微分
        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter) //测量两点距离 两点距离不能大于60cm  (赛道45cm)
        {
            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
            float dwn = sqrtf(dwx * dwx + dwy * dwy);//如果符合车库要求的话求出两个车库点之后第50个点的距离
            if (!(dwn < 0.5 * pixel_per_meter && // 符合右车库特征必须50个点后距离小于0.5m，
                  rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] > rpts0s[Lpt0_rpts0s_id][0] &&
                  rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
            {
                Lpt1_found = N_Lpt0_found = false;
            }
        }
        else
        {
            Lpt1_found = N_Lpt0_found = false;
        }
    }
}

/**
 * @brief 透视变换函数，将提取出来的两条边线进行透视变换
 *
 * @param pts_in 二维数组，表示输入的点集，每个点有两个坐标值
 * @param int_num 整型，表示输入的点集中点的数量
 * @param pts_out 二维数组，表示输出的点集，每个点有两个坐标值
 */
void Pespective(int pts_in[][2], int int_num, float pts_out[][2])
// 带入提取出来的两条边线，得到透视变换后的两条边线
{
    float x, y, w;
    for (int16_t i = 0; i < int_num; i++)
    {
        x = getx((pts_in[i][0]), (pts_in[i][1]));
        y = gety((pts_in[i][0]), (pts_in[i][1]));
        w = getw((pts_in[i][0]), (pts_in[i][1]));
        pts_out[i][0] = x / w;
        pts_out[i][1] = y / w;
    }
}

/*

*/
// void Normalization(float in[][2],int16 num,float out[][2])
//{
//     for (int i = 0; i < rpts_num; i++)
//     {
//         float dx = rpts[i][0] - cx;
//         float dy = rpts[i][1] - cy;//求出每个点到x,y变化值
//         //float dist = mySqrt(dx * dx + dy * dy);
//         float dist = sqrt(dx * dx + dy * dy);
//         //计算rpts上每一点到起始点cx,cy的距离
//         if (dist < min_dist)//如果当前点到起始点的距离小于已知的最小距离，则更新最小距离和最近点的下标索引
//         {
//             min_dist = dist;
//             begin_id = i;
//         }
//     }
//     // 中线有点，同时最近点不是最后几个点
//     if (begin_id >= 0 && rpts_num - begin_id >= 3)//这个if条件是用来判断中线是否有足够的点数，同时最近点不是最后几个点
//     {
//         // 归一化中线
//         rpts[begin_id][0] = cx;
//         rpts[begin_id][1] = cy;//选定起始点
//         rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);//sizeof(rptsn) 表示 rptsn 数组的总字节数，sizeof(rptsn[0]) 表示 rptsn 数组中每个元素的字节数
//         resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);
//          aim_idx[0] = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);

//        // 计算远锚点偏差值      cx，cy为摄像头坐标距重心坐标距离 0,-7
//        dx[0] = rptsn[aim_idx[0]][0] - cx;
//        dy[0] = rptsn[aim_idx[0]][1] - cy;
//        dn[0] = sqrt(dx[0] * dx[0] + dy[0] * dy[0]);
//        //dn = mySqrt(dx * dx + dy * dy);
//        //error<0(输出为正) 左 error>0(输出为负) 右 以下为角度制换算为弧度制
//        error[0] = atan2f(dx[0], dy[0]) * 180 / PI;//1弧度=180/π度 1度=π/180弧度
//        //assert(!isnan(error));

//        ave_error = 0;
//        // 远近锚点综合考虑
//        //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
//        // 根据偏差进行PD计算
//        //float angle = pid_solve(&servo_pid, error);

//        // 纯跟踪算法(只考虑远点)三轮控制用不到
////        pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / 1;
//      } else {
//          // 中线点过少(出现问题)，则不控制舵机
//          rptsn_num = 0;
//      }

//     end = system_getval_us();
//     //timer_cost = end - start;                              //处理每一帧的时间

//}


// void Cross_Inflection_point(int16 pt0_in[][2],int16 pt0_num,int16 pt1[][2],int16 pt1_num)
// {
//     uint8 i;
//     if()
// }
//*PerImg_ip定义使用的图像
//
// Created by RUPC on 2022/9/20.
//
//
// Created by RUPC on 2022/9/20.
//
#define RESULT_ROW 60 // 结果图行列
#define RESULT_COL 80
#define USED_ROW 120 // 用于透视图的行列
#define USED_COL 188
#define PER_IMG mt9v03x_image  // SimBinImage:用于透视变换的图像
#define Image_Use *PerImg_ip   //*PerImg_ip定义使用的图像，ImageUsed为用于巡线和识别的图像
typedef unsigned char uint8_t; // 无符号  8 bits
uint8_t *PerImg_ip[RESULT_ROW][RESULT_COL];

void ImagePerspective_Init(void)
{

    static uint8_t BlackColor = 0;
    double change_un_Mat[3][3] = {
        // 114w*100h
        {-0.01609759704190238, 0.01932561893613478, -2.040617594981866},
        {0.0004352209945470896, -0.000367865364438621,
         -0.7035606436969671},
        {1.115951268069474e-005,
         0.0001970185393508392, -0.03104642853440032},
    };
    for (int i = 0; i < RESULT_COL; i++)
    {
        for (int j = 0; j < RESULT_ROW; j++)
        {
            int local_x = (int)((change_un_Mat[0][0] * i + change_un_Mat[0][1] * j + change_un_Mat[0][2]) / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j + change_un_Mat[2][2]));
            int local_y = (int)((change_un_Mat[1][0] * i + change_un_Mat[1][1] * j + change_un_Mat[1][2]) / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j + change_un_Mat[2][2]));
            if (local_x >= 0 && local_y >= 0 && local_y < USED_ROW && local_x < USED_COL)
            {
                PerImg_ip[j][i] = &PER_IMG[local_y][local_x];
            }
            else
            {
                PerImg_ip[j][i] = &BlackColor; //&PER_IMG[0][0];
            }
        }
    }
}

/*************************************************************************
 *  函数名称：void nms_angle();
 *  功能说明：角度变化率非极大抑制
 *  参数说明：输入角度变化率数组，角度变换数组总个数，抑制后的角度变化数组，点集奇数核
 *  函数返回：无
 *  备    注：上交方案：kernel = 21，保存极大值的数组，即一段范围内斜率变换最大的值，非极大值均设为零
 *************************************************************************/
void nms_angle(float angle_in[], int num, float angle_out[], int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++)
        {
            // fabs():求绝对值
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i]))
            {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

/*************************************************************************
 *  函数名称：void local_angle_points();
 *  功能说明：点集局部角度变化率
 *  参数说明：输入边线数组，边线点个数，输出角度变换率数组，两点间固定距离
 *  函数返回：无
 *  修改时间：2022年11月23日
 *  备    注：计算该点前后两点连线的斜率作为该点切线斜率的近似
 *************************************************************************/
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++)
    {
        if (i <= 0 || i >= num - 1)
        {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = mySqrt(dx1 * dx1 + dy1 * dy1); // 此点与前一点间距离
        // float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);//此点与前一点间距离
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = mySqrt(dx2 * dx2 + dy2 * dy2); // 此点与后一点间距离
        // float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);//此点与后一点间距离
        float c1 = dx1 / dn1; // cosθ1
        float s1 = dy1 / dn1; // sinθ1
        float c2 = dx2 / dn2; // cosθ2
        float s2 = dy2 / dn2; // sinθ2
        // 化简展开得到角公式(k1-k2)/(1+k1k2),即tan的差角公式==>得出两点间角度变化==>曲线斜率变化
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1); // atan2f(y,x):返回以弧度表示的y/x反正切
    }
}
/*完成摄像头初始化后，调用一次ImagePerspective_Init，此后，直接调用ImageUsed   即为透视结果*/
/**
 * @brief 左侧跟踪线函数，这整个函数是起到对输入数组pts_in平移的作用（感觉没什么用的样子）
 * 
 * @param pts_in 输入的点集数组
 * @param num 点集数组的长度
 * @param pts_out 输出的点集数组
 * @param approx_num 近似点的数量
 * @param dist 距离
 */
void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist){
    for (int i = 0; i < num; i++) {
            float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
            float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
            //求x,y坐标上的偏差
            float dn = mySqrt(dx * dx + dy * dy);//求弧长
            //float dn = sqrt(dx * dx + dy * dy);
            dx /= dn;//cosθ
            dy /= dn;//sinθ
            pts_out[i][0] = pts_in[i][0] + dy * dist;
            pts_out[i][1] = pts_in[i][1] - dx * dist;
            
        }
        ips200_show_uint(210,210,approx_num,3);
}
void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist){
    for (int i = 0; i < num; i++) {
           float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
           float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
           float dn = mySqrt(dx * dx + dy * dy);
           //float dn = sqrt(dx * dx + dy * dy);
           dx /= dn;
           dy /= dn;
           pts_out[i][0] = pts_in[i][0] - dy * dist;
           pts_out[i][1] = pts_in[i][1] + dx * dist;
       }
}
