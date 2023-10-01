#include "image.h"
#include "math.h"
#include "control.h"
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

struct Left_Edge Left[140];   // 左边界结构体
struct Right_Edge Right[140]; // 右边界结构体

// 定义坐标结构体
struct coordinate
{
    unsigned char row;    // 行坐标，省点内存就没设int
    unsigned char column; // 列坐标，同上
};
struct coordinate mid[140]; // 中线坐标结构体

// 直线结构体
struct Line
{
    float k;
    float b;
}; // 定义直线方程
struct Line left_line;
struct Line right_line; // 定义左右线方程
uint8 Image_Use[Image_Height][Image_Width];
/**
 * @brief 截取我们需要的图像大小
 *
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */

void Image_Compress(void)
{
    int i, j, row, line;
    const float pro_h = Primeval_Hight / Image_Height, pro_w = Primeval_With / Image_Width; // 根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
    for (i = 0; i < Image_Height; i++)                                                      // 遍历图像的每一行，从第零行到第59行。
    {
        row = i * pro_h + 0.5;
        for (j = 0; j < Image_Width; j++) // 遍历图像的每一列，从第零列到第79列。
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

/**
 * @brief 将输入的灰度图像转化为二值化图像
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
void Image_Binarization(uint8 threshold, uint8 (*Image)[Image_Width])
{
    uint32 i, j;
    uint16 temp = 0;

    for (j = 0; j < Image_Height; j++)
    {
        for (i = 0; i < Image_Width; i++)
        {
            temp = *(Image[0] + j * Image_Width + i);                              // 读取像素点
            if (j == 0 || j == Image_Height - 1 || i == 0 || i == Image_Width - 1) // 大津法加一个黑框
            {
                //                *(Image[0] + j * Image_Width + i) = 0;
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
 *
 *
 * @return unsigned char 返回1表示获取成功，返回0表示获取失败
 */
unsigned char left_point; // 记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
uint8 Left_Find_Flag;     // 左线起始点找到标志
unsigned char Image_Get_LeftFlag(void)
{
    for (left_point = (Image_Width / 2); left_point > 3; left_point--)
    {
        if ((Image_Use[56][left_point] == 255) && (Image_Use[56][left_point - 1] == 0) && (Image_Use[56][left_point - 2] == 0))
        {
            Left_Find_Flag = 1; //
            break;
        }
    }
    if (!Left_Find_Flag)
    {
        left_point = 4;
    }
    return 1;
}

/**
 * @brief 获取图像右侧起始生长点--寻找起始点函数，从第56行开始数
 *
 *
 * @return unsigned char 返回1表示获取成功，返回0表示获取失败
 */
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

/**
 * @brief 绘制一个矩形，将图片的边缘变为黑色
 * @example Image_DrawRectangle();
 * @explain 该函数将图片的边缘变为黑色，以便后面的边缘提取,
 * 给图像绘黑边，不然八邻域会出错，位置一定要放在显示的前面
 */
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
        Image_Use[1][i] = 0;
    }
}

/**
 * @brief 通过两个点的坐标计算直线斜率和截距，并将结果存储在全局变量left_line中
 *
 * @param x1 第一个点的x坐标
 * @param y1 第一个点的y坐标
 * @param x2 第二个点的x坐标
 * @param y2 第二个点的y坐标
 */
void Image_pointsleft(uint8 x1, uint8 y1, uint8 x2, uint8 y2)
{
    float two_points_k, two_points_b;
    two_points_k = (float)((y2 - y1) / (x2 - x1));
    two_points_b = (float)((y1 * x2 - x1 * y2) / (x2 - x1));
    left_line.k = two_points_k;
    left_line.b = two_points_b;
}
/**
 * @brief 通过两个点的坐标计算直线斜率和截距，并将结果存储在全局变量right_line中
 *
 * @param x1 第一个点的x坐标
 * @param y1 第一个点的y坐标
 * @param x2 第二个点的x坐标
 * @param y2 第二个点的y坐标
 */
void Image_pointsright(uint8 x1, uint8 y1, uint8 x2, uint8 y2)
{
    float two_points_k, two_points_b;
    two_points_k = (float)((y2 - y1) / (x2 - x1));
    two_points_b = (float)((y1 * x2 - x1 * y2) / (x2 - x1));
    right_line.k = two_points_k;
    right_line.b = two_points_b;
}

/**
 * @brief 计算通过一组点的直线的斜率
 *
 * 此函数计算通过一组点的直线的斜率,接受线的起点和终点以及线上点的y坐标数组作为输入,返回直线的斜率
 *
 * @param begin 线的起点
 * @param end 线的终点
 * @param border 线上点的y坐标数组
 * @return 通过一组点的直线的斜率
 */
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

/**
 * @brief 计算斜率和截距,配合上面的最小二乘法使用
 *
 *
 * @param begin 线的起点
 * @param end 线的终点
 * @param border 线上点的y坐标数组
 * @return 通过一组点的直线的斜率,通过指针来返回斜率和截距
 */
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

unsigned char Left_Count, Right_Count;         // 记录左右边界点的个数
unsigned char grow_left, grow_right;           // 记录左右边界在八邻域时寻点的相对位置
unsigned char Left_Max = 140, Right_Max = 140; // 左右边界搜点时允许最大的搜点量
unsigned char Boundary_search_end = 30;        // 搜寻行数的最高行
uint16 cur_row, cur_col;                       // 当前行列
uint8 Gather_flag;
uint8 Gather_row[50]; // 两线会聚数组，用来存储行坐标
uint8 Gather_Count;   // 计数
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
            // 下面是巡线以后其他的处理
            // 一 越界处理
            if (cur_row <= 3 || cur_row >= 57 || cur_col <= 3 || cur_col >= 97)
            {
                break;
            }
            // 以下是待测试的代码
            // 二 连续3次都是同一个点就不再寻找
            if (Right[Right_Count].row == Right[Right_Count - 1].row && Right[Right_Count].column == Right[Right_Count - 1].column &&
                Right[Right_Count - 1].row == Right[Right_Count - 2].row && Right[Right_Count - 1].column == Right[Right_Count - 2].column)
            {
                break;
            }
            // 三 当扫到的点多个在同一行时，只保留最后一个点（这个目前不写，因为处理的是压缩后的图像，点多一点也没关系，如果需要的话后期再改进）
            // 四 当左右线会聚时退出循环，并记录此时的行数（判断坡道）
            if ((Left[Left_Count - 1].row == Right[Right_Count - 1].row) && (Left[Left_Count - 1].column == Right[Right_Count - 1].column ||
                                                                             Right[Right_Count - 1].column - Left[Left_Count - 1].column <= 3))
            // 判断条件：行相等，列也相等or列相差在3以内
            {
                Gather_row[Gather_Count] = Left[Left_Count - 1].row;
                Gather_flag = 1;
                break;
            }
            else
            {
                Gather_flag = 0;
            }
        }
    }
}

uint8 Mid_Count;
uint16 cur_row, cur_col; // 当前行列
/**
 * @brief 取中线函数（有bug，待验证）
 *
 * @param uint8 无
 * @example Get_Midpoint(void)
 */
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

/**
 * @brief 十字补线函数
 *
 * @param uint8 已经经过八邻域扫线的图像Image_Use
 * @example Image_FillCross(Image_Use)
 * @explanation  处理十字不难，只需要找到4个拐点，然后补线即可，找到4个拐点的处理方法：1.判断多个点的生长方向 2.多个点构成的斜率的急剧变化 这里用方法1
 */
// 分别设置左上，左下，右上，右下四个拐点坐标
struct coordinate left_high;
struct coordinate left_low;
struct coordinate right_high;
struct coordinate right_low;
void Image_FillCross_1(uint8 (*Image_Use)[Image_Width])
{
    uint8 i, j;             // 中间变量
    uint8 l_ready, r_ready; // 定义左右线扫完的标志位

    // 先扫左拐点，从左下角开始扫，左右线的计数值不一定相等
    // 两种写法，一种生长方向由5-->2，另外一种直接判断多个2（因为不能保证斜线一直是5），这里用方法一，后面可能会改，觉得这个不太行
    // 判断是否进入十字，或者未进入十字
    if (Image_Use[56][4] == black && Image_Use[56][6] == black && Image_Use[56][94] == black && Image_Use[56][96] == black)
    {
        for (i = 0; i < Left_Count; i++) // 实际上理想情况下是i不会=left_count，因为找到左下拐点时肯定还有很多点没找
        {
            // 这里判断写越多越好，找的拐点就越符合条件，但是也有可能找不到，所以要权衡
            if (Left[i].grow == 5 && Left[i - 2].grow == 5 && Left[i + 2].grow == 2 && Left[i + 3].grow == 2 && Left[i + 5].grow == 2 && Left[i + 7].grow == 2 && Left[i - 5].grow == 5)
            {
                left_low.row = Left[i].row;
                left_low.column = Left[i].column;
                break;
            }
        }
        // 找的是左上拐点，这里可以不放在同一个for循环，因为肯定是先找到左下拐点，然后才能找到左拐点
        for (i = 0; i < Left_Count; i++)
        {
            // 图片上方的有效点比较少，判断就简单一点，不然找不到，注意生长方向就行
            if (Left[i].grow == 5 && Left[i - 2].grow == 5 && Left[i - 5].grow == 5 && Left[i - 2].grow == 6 && Left[i - 4].grow == 6 && Left[i - 6].grow == 6)
            {
                left_high.row = Left[i].row;
                left_high.column = Left[i].column;
                break;
            }
        }
        // 左右对称，生长方向一样，就不用改了
        for (i = 0; i < Right_Count; i++)
        {
            if (Right[i].grow == 5 && Right[i - 2].grow == 5 && Right[i + 2].grow == 2 && Right[i + 3].grow == 2 && Right[i + 5].grow == 2 && Right[i + 7].grow == 2 && Right[i - 5].grow == 5)
            {
                right_low.row = Right[i].row;
                right_low.column = Right[i].column;
                break;
            }
        }
        for (i = 0; i < Right_Count; i++)
        {
            if (Right[i].grow == 5 && Right[i - 2].grow == 5 && Right[i - 5].grow == 5 && Right[i - 2].grow == 6 && Right[i - 4].grow == 6 && Right[i - 6].grow == 6)
            {
                right_high.row = Right[i].row;
                right_high.column = Right[i].column;
                break;
            }
        }
        // 找完点开始补线，先求左线
        Image_pointsleft(left_high.row, left_high.column, left_low.row, left_low.column); // 求出左线的斜率和截距
        // 补线有两种方法：一种把全部的点都补上，一种只补两个拐点拐点，这里用第二种，后面再用基础扫线，扫出补线后的边线
        for (i = left_low.row; i < Image_Height - 1; i++) // 从左下拐点开始补线
        {
            Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] = black; // 补线,这里传入的k是浮点型,后面要换
            if (i >= Image_Height - 4)
            {
                l_ready = 1; // 左线补完了
                break;
            }
        }
        Image_pointsright(right_high.row, right_high.column, right_low.row, right_low.column); // 求出右线的斜率和截距
        for (i = right_low.row; i < Image_Height - 1; i++)                                     // 从右下拐点开始补线
        {
            Image_Use[i][(int)(right_line.k) * i + (int)(right_line.b)] = black; // 补线
            if (i >= Image_Height - 4)
            {
                r_ready = 1; // 右线补完了
                break;
            }
        }
    }
    /*如果未进十字的补线判断完成，那么接下来就要进入十字中途的补线，这里和前面的最大区别就是左下角和右下角是不是白色*/
    if (l_ready && r_ready && Image_Use[56][4] == white && Image_Use[56][6] == white && Image_Use[56][94] == white && Image_Use[56][96] == white) // 等左右线补完，只要补完就说明有过，那么就顺便看左下角和右小角是否为白色
    {
        // 此时只有左上拐点和右上拐点，因为左下角和右下角都是白色，所以只需要找左上拐点和右上拐点，取直线就往拐点往上扫
        for (i = 0; i < Left_Count; i++)
        {
            if (Left[i].grow == 5 && Left[i - 2].grow == 5 && Left[i - 5].grow == 5 && Left[i - 2].grow == 6 && Left[i - 4].grow == 6 && Left[i - 6].grow == 6)
            {
                left_high.row = Left[i].row;
                left_high.column = Left[i].column;
                break;
            }
        }
        for (i = 0; i < Right_Count; i++)
        {
            if (Right[i].grow == 5 && Right[i - 2].grow == 5 && Right[i - 5].grow == 5 && Right[i - 2].grow == 6 && Right[i - 4].grow == 6 && Right[i - 6].grow == 6)
            {
                right_high.row = Right[i].row;
                right_high.column = Right[i].column;
                break;
            }
        }
        // 求出拐点后开始补线,这里求斜率有三种方法:1.直接用两点(拐点和左下角)求斜率 2.用最小二乘法求斜率(用拐点往上的点) 3.用最小二乘法求斜率，但是只用拐点附近的点
        Image_pointsleft(left_high.row, left_high.column, 56, 4); // 求出左线的斜率和截距
        for (i = 56; i > 4; i--)
        {
            Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] = black; // 补线
            if (i <= left_high.row - 1)
            {
                l_ready = 1; // 左线补完了
                break;
            }
        }
        Image_pointsright(right_high.row, right_high.column, 56, 96); // 求出右线的斜率和截距
        for (i = 56; i > 4; i--)
        {
            Image_Use[i][(int)(right_line.k) * i + (int)(right_line.b)] = black; // 补线
            if (i <= right_high.row - 1)
            {
                r_ready = 1; // 右线补完了
                break;
            }
        }
    }
}

/*下面是直道函数的辅助函数*/
/**
 * @brief 获取图像的k值
 *
 * @param start_y 起始y坐标
 * @param end_y 结束y坐标
 * @param interval y坐标间隔
 * @return float k值
 */
float Image_Getk(uint8 start_y, uint8 end_y, uint8 interval)
{
    return (float)((end_y - start_y) / interval);
}
/**
 * @brief 计算两个浮点数之间的绝对值差
 *
 * @param a 第一个浮点数
 * @param b 第二个浮点数
 * @return float 两个浮点数之间的绝对值差
 */
float Image_ab_value(float a, float b)
{
    if (a > b)
    {
        return a - b;
    }
    else
    {
        return b - a;
    }
}
float k_buff[5]; // 存放5个斜率值
/**
@brief 判断左直道函数，如果为直道，会通过返回值用来判断是否为直道
@param 二值化后的图像，实际上用不上
@return 1，则存在左直道，0就没有
@example Image_Stretch(void)
@note 将直线分为2个地方，看上下斜率相差是否相近，近的话就判断为直道
*/
uint8 Image_Stretch_Left(void)
{
    uint8 i;
    float k_left_low, k_left_high; // 左下线斜率，左上线斜率
    float k_all;                   // 多断小斜率叠加
    k_all = 0.0;
    for (i = 0; i < 5; i++) // 求左线下10个点的平均斜率
    {
        k_buff[i] = Image_Getk(Left[i].column, Left[i + 5].column, (Left[i + 5].row - Left[i].row)); // x是行坐标,y是列坐标
    }
    for (i = 0; i < 5; i++)
    {
        k_all += k_buff[i];
    }
    k_left_low = k_all / 5;
    k_all = 0.0;
    for (i = 0; i < 5; i++) // 求左线上10个点的平均斜率
    {
        k_buff[i] = Image_Getk(Left[40 + i].column, Left[i + 45].column, (Left[i + 45].row - Left[i + 40].row)); // x是行坐标,y是列坐标
    }
    for (i = 0; i < 5; i++)
    {
        k_all += k_buff[i];
    }
    k_left_high = k_all / 5;
    if (Image_ab_value(k_left_high, k_left_low) < Line_k) // 看直线的斜率是否符合要求
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
@brief 判断右直道函数，如果为直道，通过返回值用来判断是否为直道
@param 二值化后的图像
@return 1，则存在右直道，0就没有
@example Image_Stretch_Right(void)
@note 将直线分为2个地方，看上下斜率相差是否相近，近的话就判断为直道
*/
uint8 Image_Stretch_Right(void)
{
    uint8 i;
    float k_right_low, k_right_high, k_all;
    k_all = 0.0;
    for (i = 0; i < 5; i++) // 求右线下10个点的平均斜率
    {
        k_buff[i] = Image_Getk(Right[i].column, Right[i + 5].column, (Right[i + 5].row - Right[i].row)); // x是行坐标,y是列坐标
    }
    for (i = 0; i < 5; i++)
    {
        k_all += k_buff[i];
    }
    k_right_low = k_all / 5;
    k_all = 0.0;
    for (i = 0; i < 5; i++) // 求右线上10个点的平均斜率
    {
        k_buff[i] = Image_Getk(Right[40 + i].column, Right[i + 45].column, (Right[i + 45].row - Right[i + 40].row)); // x是行坐标,y是列坐标
    }
    for (i = 0; i < 5; i++)
    {
        k_all += k_buff[i];
    }
    k_right_high = k_all / 5;
    if (Image_ab_value(k_right_high, k_right_low) < Line_k) // 看直线的斜率是否符合要求
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief 图像运行函数
 *
 * @param void
 * @example Image_Run(Image_Use)
 * @explanation  无
 */
// 图像处理的函数都放在这里，这样就避免了定义问题
void Image_Run(void)
{
    uint8 i;

    i = Image_Get_LeftFlag();
    i = Image_Get_Rightflag();
    Image_DrawRectangle();
    //	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count
    tft180_show_int(3, 80, left_point, 3);
    Image_Get_neighborhoods(Image_Use);
    //	tft180_show_int(3,120,points_l[l_count-1][0],3);
    //	Image_Get_neighborhoods(100,Image_Use);
    //	tft180_draw_line(0,0,cur_col,cur_row,RGB565_RED);//行坐标l_countl_count
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
int Image_LeftGrowDirection(uint8 Direction)
{
    uint8 i, direction_count;
    for (i = 0; i < Left_Count; i++)
    {
        if (Left[i].grow == Direction) // 如果方向一致的话，就计数自增
        {
            direction_count++;
        }
    }
    return direction_count;
}
/**
 * @brief 计算右侧图像中右边线生长方向与给定方向相同的数量
 *
 * @param Direction 给定的生长方向
 * @return uint8 相同方向的数量
 */
int Image_RightGrowDirection(uint8 Direction)
{
    uint8 i, direction_count;
    for (i = 0; i < Right_Count; i++)
    {
        if (Right[i].grow == Direction) // 如果方向一致的话，就计数自增
        {
            direction_count++;
        }
    }
    return direction_count;
}
/**
 * @brief 左环岛判断并补线函数
 *
 * @param Image_Use
 * @example Image_LeftRound(Image_Use)
 * @explanation  右边线为直线，左边线较为弯曲，将整个环岛分为几个状态，所以搞个状态机
 * 1. 未进环岛，左边补线：直走
 * 2. 未进环岛，前面补线：左转
 * 3. 进入环岛，不用补线：一直左转
 * 4. 离开环岛，右边需要补线：左转
 * 5. 离开环岛，不用补线：一直左转
 * 6. 离开环岛，左边需要补线：直走（和状态1一样）
 */
struct coordinate Left_first_point;  // 左环岛的第一个拐点
struct coordinate Left_second_point; // 左环岛的第二个拐点
struct coordinate Right_first_point; // 右拐点
void Image_LeftRound(uint8 (*Image_Use)[Image_Width])
{
    uint8 LeftRound_State = 0;      // 左环岛状态1-5
    uint8 Last_LeftRound_State = 0; // 上一个状态
    uint8 i;                        // 中间变量
    uint8 Left_first_point_index;   // 记录左拐点在左边线数组的下标
    uint8 Left_second_point_index;
    uint8 Right_first_point_index;
    uint8 Left_third_point_index;
    uint8 index_temp; // 中间变量
    left_line.k = 0.0;
    left_line.b = 0.0;         // 清零
    uint8 black_count;         // 计算黑点值
    if (Image_Stretch_Right()) // 先看右边是不是直道，右边直道只能判断1，2，5，6
    {
        for (i = 0; i < Left_Count; i++)
        {
            if (Left[i].grow == 5 && Left[i - 2].grow == 5 && Left[i - 5].grow == 5 && Left[i + 2].grow != 5 && Left[i + 3].grow != 5 && Left[i + 4].grow != 5 && Left[i + 5].grow != 5)
            // 通过判断生长方向是否还是为右上来判断
            {
                Left_first_point.row = Left[i].row;
                Left_first_point.column = Left[i].column; // 记录左环岛第一个拐点坐标
                Left_first_point_index = i;
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 1;
                break;
            }
            // 状态2的标志判断
            if (LeftRound_State == 1 && Last_LeftRound_State == 0 && Left[i].grow == 4 && Left[i + 1].grow == 4 && Left[i + 3].grow == 4) // 只是用来判断是否进入状态2的函数
            {
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 2; // 进入状态2
            }
            // 状态2的拐点判断
            if (LeftRound_State == 2 && Last_LeftRound_State == 1 && Left[i].grow == 5 && Left[i + 2].grow == 5 && Left[i - 2].grow != 5 && Left[i - 2].grow != 4) // 找到左边第二个拐点
            {
                Left_second_point_index = i; // 记录下标，其实只需要记录下标就行？
                break;
            }
            // 状态5的标志判断：1.右侧拐点消失（或右侧拐点太低了） 2. 左线生长方向向上 3.右线基本生长方向左上5
            // 4. 左右线相交  5.和状态3差不多，唯一不同的就是右线的生长方向为直线（可以求斜率来判断）
            // 这里来的做法是统计5方向的个数，如果大于3/4就认为是状态5（也可以判断斜率的直线）
            if (LeftRound_State == 4 && Last_LeftRound_State == 3 && (Image_RightGrowDirection(5) >= ((Right_Count / 4) * 3)))
            {
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 5; // 进入状态5
            }
        }
        // 状态1的补线
        if (Last_LeftRound_State == 0 && LeftRound_State == 1) // 通过状态机进行不同的补线，这里因为LeftRound_State取值，就要==1，不能直接写LeftRound_State
        {
            left_line.k = 0.0;
            left_line.b = 0.0; // 清零
            Image_pointsleft(Left[Left_first_point_index - 2].row, Left[Left_first_point_index - 2].column, Left[Left_first_point_index - 5].row, Left[Left_first_point_index - 5].column);
            // 求出左补线的斜率和截距
            for (i = Left[Left_first_point_index - 2].row; i > Image_Height - 10; i--) // 从左下拐点，向上开始补线
            {
                if (Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] == black) // 如果要变黑的点本来就是黑点
                {
                    black_count++;
                }
                else
                {
                    Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] = black; // column=k*row+b，这里补线方程和下面不一样
                }
                if (i <= Image_Height - 13 || black_count >= 3)
                {
                    Left_first_point_index = 0; // 补完下标索引清零
                    black_count = 0;
                    break; // 左线补完了
                }
            }
        }
        // 状态2的补线
        if (Last_LeftRound_State == 1 && LeftRound_State == 2 && Left_second_point_index != 0) // 补线
        {
            left_line.k = 0.0;
            left_line.b = 0.0; // 补线前先清零
            Image_pointsleft(Left[Left_second_point_index].column, Left[Left_second_point_index].row,
                             Left[Left_second_point_index - 3].column, Left[Left_second_point_index - 3].row); // 这里补线是要跨过宽度的，因此需要变换直线
            for (i = Left[Left_second_point_index].column; i < 96; i++)                                        // 从左上拐点开始补线，向右补
            {
                if (Image_Use[(int)(left_line.k) * i + (int)(left_line.b)][i] == black) // 如果要变黑的点本来就是黑点
                {
                    black_count++; // 是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[(int)(left_line.k) * i + (int)(left_line.b)][i] = black; // 如果是白色的话就变为黑点
                }
                // 这里的直线方程为:row=k*column+b
                if ((i >= Image_Width - 4) || (black_count >= 3)) // 防止赛道上出现噪点
                {
                    Left_second_point_index = 0; // 补完下标索引清零
                    black_count = 0;             // 记得清零
                    break;                       // 左线补完了
                }
            }
        }
    }
    /*状态3不用补线*/
    // 状态3的标志判断：1. 左边线和右边线会相交 2.左右边线起始点的列坐标相差较大 3.左线的生长方向基本都是正上（4）
    // 4.右线的列坐标的起始和终点相差较大
    else if (LeftRound_State == 2 && Last_LeftRound_State == 1 && (abs_int(Right[0].column, Left[0].column) > 80) && (Image_LeftGrowDirection(4) >= (Left_Count / 2)) && (abs_int(Right[0].column, Right[Right_Count - 1].column) > 80))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 3; // 进入状态3，状态3就不用补线了，拐点也不用搞了
    }
    // 状态4的标志判断
    // 标志判断：1.左线的生长方向正上占比较少 2.右线的起点和终点列坐标相差较小 3.右线起始的列坐标和和终点的列坐标相差较小
    else if (LeftRound_State == 3 && Last_LeftRound_State == 2 && (Image_LeftGrowDirection(4) <= (Left_Count / 4)) && (abs_int(Right[0].column, Right[Right_Count - 1].column) < 20))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 4; // 进入状态4
    }
    // 状态6的标志判断
    // 1. 右线的起始点和终止点的列坐标间隔较小   2. 左线向上生长方向的比例下降 3.左线的起点和终止点的列坐标间隔较大
    else if (LeftRound_State == 5 && Last_LeftRound_State == 4 && (abs_int(Left[0].column, Left[Left_Count - 1].column) > 80) && (abs_int(Right[0].column, Right[Right_Count - 1].column) < 20) && (Image_LeftGrowDirection(4) <= (Left_Count / 4)))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 6; // 进入状态6
    }
    /*下面是补线操作*/
    // 状态4的补线：先找右拐点，然后取拐点后退做线来补线
    if (LeftRound_State == 4 && Last_LeftRound_State == 3)
    {
        for (i = 0; i < Right_Count; i++)
        {
            if (Left[i].grow == 5 && (Left[i - 3].grow == 5 || Left[i - 2].grow == 5) && (Left[i - 4].grow == 5 || Left[i - 5].grow == 5) && (Left[i + 1].grow == 3 || Left[i + 2].grow == 3) && (Left[i + 3].grow == 3 || Left[i + 4].grow == 3))
            // 这里判断条件放松了，避免找不到，后期可以再改
            {
                Right_first_point_index = i; // 记录下标
                break;
            }
        }
        // 状态4补线：从右拐点向左补线，直线方程要变换
        if (Right_first_point_index != 0) // 确认一下找没找到拐点
        {
            right_line.k = 0.0;
            right_line.b = 0.0; // 清零
            Image_pointsright(Right[Right_first_point_index].column, Right[Right_first_point_index].row,
                              Right[Right_first_point_index - 3].column, Right[Right_first_point_index - 3].row); // 这里补线是要跨过宽度的，因此需要变换直线
            for (i = Right[Right_first_point_index].column; i > 0; i--)                                           // 从右下拐点开始补线，向左补
            {
                if (Image_Use[(int)(right_line.k) * i + (int)(right_line.b)][i] == black) // 如果要变黑的点本来就是黑点
                {
                    black_count++; // 是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[(int)(right_line.k) * i + (int)(right_line.b)][i] = black; // 如果是白色的话就变为黑点
                }
                // 这里的直线方程为:row=k*column+b，列的点比较多
                if ((i <= Image_Width - 4) || (black_count >= 3)) // 防止赛道上出现噪点
                {
                    Right_first_point_index = 0; // 补完下标索引清零
                    black_count = 0;             // 记得清零
                    break;                       // 右线补完了
                }
            }
        }
    }
    // 下面是状态5的补线：
    if (LeftRound_State == 5 && Last_LeftRound_State == 4)
    {
        // 好像不用补，直接扫线就行
    }
    // 下面是状态6的补线：先找左拐点，然后取左拐点的右上点补线
    if (LeftRound_State == 6 && Last_LeftRound_State == 5)
    {
        for (i = 0; i < Left_Count; i++)
        {
            if (Image_ab_value((Image_Getk(Left[i].column, Left[i - 3].column, abs_int(Left[i - 3].row, Left[i].row))),
                               Image_Getk(Left[i].column, Left[i + 3].column, abs_int(Left[i + 3].row, Left[i].row))) > 0.4)
            {
                Left_third_point_index = i; // 记录下标
                break;
            }
        }
        // 补线是沿着行向下补，故直线方程为column=row*k+b
        if (Left_third_point_index != 0)
        {
            left_line.k = 0.0;
            left_line.b = 0.0; // 清零
            Image_pointsleft(Left[Left_third_point_index].row, Left[Left_third_point_index].column,
                             Left[Left_third_point_index + 3].row, Left[Left_third_point_index + 3].column); // 向下补线
            for (i = Left[Left_third_point_index].row; i < 56; i++)                                          // 从左下拐点开始补线，向左下补
            {
                if (Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] == black) // 如果要变黑的点本来就是黑点
                {
                    black_count++; // 是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] = black; // 如果是白色的话就变为黑点
                }
                // 这里的直线方程为:row=k*column+b，列的点比较多
                if ((i >= Image_Height - 4) || (black_count >= 3)) // 防止赛道上出现噪点
                {
                    Left_third_point_index = 0; // 补完下标索引清零
                    black_count = 0;            // 记得清零
                    break;                      // 左线补完了
                }
            }
        }
    }
}

/**
 * @brief 扫描60行中的某一行的黑白跳变的函数（检测斑马线）
 *
 * @param uint8(*Image_Use)[Image_Width] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */
uint8 Image_Scan_Row(uint8 (*Image_Use)[Image_Width], uint8 target_row)
{
    uint8 i;                     // 中间变量
    uint8 black_white_count = 0; // 黑白跳变的计数
    for (i = 0; i < Image_Width - 1; i++)
    {
        if (Image_Use[target_row][i] != Image_Use[target_row][i + 1]) // 如果不相等的话，就说明是黑白跳变
        {
            black_white_count++;
        }
    }
    return black_white_count;
}

/**
 * @brief 扫描100行中的某一行的黑白跳变的函数（检测斑马线）
 *
 * @param uint8(*Image_Use)[Image_Width] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */
uint8 Image_Scan_Column(uint8 (*Image_Use)[Image_Width], uint8 target_column)
{
    uint8 i;
    uint8 black_white_count = 0; // 黑白跳变的计数
    for (i = 0; i < Image_Height; i++)
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

int begin_x0, begin_y0; // 找线偏移点
int begin_x1, begin_y1; // 找线偏移点

#define ROAD_WIDTH (0.39)    // 赛道宽度45cm 适时调整 注意：应用方案三时情况特殊为负数-0.40,正常0.43
#define POINTS_MAX_LEN (120) // 边线点最多的情况——>num

image_t img_raw = DEF_IMAGE(NULL, UCOL, UROW);

// 左右边丢线
uint8 loseline0;
uint8 loseline1;
int ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int ipts0_num;                // 存放边线像素点个数(左)
int ipts1_num;                // 存放边线像素点个数(右)

int x0_first, y0_first, x1_first, y1_first; // 左右边线第一个点的坐标

int x1, y1;
int x2, y2;
float begin_x = 8;  // 起始点距离图像中心的左右偏移量
float begin_y = 58; // 起始点距离图像底部的上下偏移量 120高度：35;100高

float block_size = 7; // 自适应阈值的block大小
float clip_value = 2; // 自适应阈值的阈值裁减量
uint8_t  Image_Use_Robert[120][160];//二值化图像
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

    uint8 uthres = 1;
    //    uint8 uthres = ostu();
    // 寻左边线
    x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
	int TH;
	TH = OSTU_GetThreshold(Image_Use[0], Image_Width, Image_Height);
	Image_Sobel( Image_Use, Image_Use_Robert ,TH);//全局Sobel得二值图(方案二) 2.8ms
	img_raw.data = Image_Use_Robert;

    // 标记种子起始点(后续元素处理要用到)
    x0_first = x1;
    y0_first = y1;

    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]); // 求数组的长度
    // 扫底下五行，寻找跳变点
    for (; y0_first > begin_y - 5; y0_first--)
    {
        for (; x0_first > 0; x0_first--)
            if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)
                goto out1;
        x0_first = img_raw.width / 2 - begin_x;
    }
    loseline0 = 1; // 底边丢线
out1:
{
    if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres)
        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);
    else
        ipts0_num = 0;
}

    // 寻右边线
    x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

    // 标记种子起始点(后续元素处理要用到)
    x1_first = x2;
    y1_first = y2;

    ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
    for (; y1_first > begin_y - 5; y1_first--)
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

void Left_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
{
    zf_assert(img && img->data); // 不满足则退出执行
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1); // 保证block_size为奇数
    //    int half = block_size / 2;                       //上交方案
    int half = 0;                                                                                                      // 方案二
    int step = 0, dir = 0, turn = 0;                                                                                   // step表示前进的步数；dir通过改变索引改变当前小人朝向的方向
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4) // 保证block不出界
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
        //=======添加部分=======
        if ((x == 1 && y < img->height - 20) || x == img->width - 2 || y == 1 || (y == 58 && step > 19))
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
	uint8 i;
//	for(i=0;i<ipts0_num;i++)
//	{
//		tft180_draw_point(ipts0[i][1]/2,ipts0[i][0]/2,RGB565_RED);
//	}
	for(i=0;i<ipts1_num;i++)
	{
		tft180_draw_point(ipts1[i][1]/2-5,ipts1[i][0]/2,RGB565_BLUE);
	}
//	tft180_draw_line(0,0,ipts0[20-1][1],ipts0[20-1][0],RGB565_RED);
//	tft180_show_int(3,100,ipts0[20-1][1],4);
//	tft180_show_int(3,120,ipts0[20-1][0],4);
	tft180_displayimage03x((uint8 *)Image_Use_Robert, 80, 60); //pidMotor1Speed
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
//    zf_assert(img && img->data);
//    zf_assert(num && *num >= 0);
//    zf_assert(block_size > 1 && block_size % 2 == 1);
    //    int half = block_size / 2;        //上交方案
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
        if ((x == img->width - 2 && y < img->height - 20) || x == 1 || y == 1 || (y == 58 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线
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
}
// 补线 原图

int clip(int x, int low, int up)
{
    return x > up ? up : x < low ? low
                                 : x;
}

void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value)
{
    int dx = pt1[0] - pt0[0];
    int dy = pt1[1] - pt0[1];
    if (abs(dx) > abs(dy))
    {
        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1))
        {
            int y = pt0[1] + (x - pt0[0]) * dy / dx;                                  // y = 左线横坐标 + x遍历差值占总差值比例 * y方向差值
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; // （x，y）坐标像素（不超出边界）赋值
        }
    }
    else
    {
        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1))
        {
            int x = pt0[0] + (y - pt0[1]) * dx / dy;
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    }
}