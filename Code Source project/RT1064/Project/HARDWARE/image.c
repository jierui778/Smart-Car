#include "image.h"
#include "math.h"
#include "control.h"
#include "stdlib.h"
uint8 Image_Use_Robert[120][160];//二值化图像

//flash参数统一定义
float begin_x = 8;  // 起始点距离图像中心的左右偏移量	8
float begin_y = 118; // 起始点距离图像底部的上下偏移量 120高度：35;100高	58
/*beginy值越小，初始的生长点与上框越近*/

float block_size = 7; // 自适应阈值的block大小
float clip_value = 2; // 自适应阈值的阈值裁减量

float thres = 120;                 // 二值化阈值，主要用于找起始点(边线使用自适应阈值，不使用该阈值)
float line_blur_kernel = 7;      // 边线三角滤波核的大小
float pixel_per_meter = 102;       // 俯视图中，每个像素对应的长度 (厘米*10^3)/像素个数 调大意味着平移距离增大 原：102
float sample_dist = 0.022;           // 边线等距采样的间距 动态调整，使其每两点距离刚好为1cm 为0.02时10个点间隔约为9cm
float angle_dist = 0.2;            // 计算边线转角时，三个计算点的距离
float far_rate = 0.5;              //
float aim_distance_flash = 0.68;          // 预锚点长度
float aim_dist[5] = {0.68,0.78,0.88,0.98,1.08};//多个预瞄点长度，间隔5cm，34~54个点，用于速度模糊控制，而不是偏差角计算！

float xielv_left_y_to_end,xielv_right_y_to_end;                 //在逆透视后得坐标系建得斜率

void check()
{
	while(1)
	{
		
	}
}
//八邻域扫的左线的结构体
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

struct Left_Edge Left[200];                   //左边界结构体
struct Right_Edge Right[200];                 //右边界结构体
struct Left_Edge Left_New[200];//滤波后左边界结构体
struct Right_Edge Right_New[200];//滤波后右边界结构体
//定义坐标结构体
struct coordinate
{
    unsigned char row;                        //行坐标，省点内存就没设int
    unsigned char column;                     //列坐标，同上
	uint8 index;//下标索引
	uint8 flag;
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
uint8 left_point;                     //记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
uint8 Left_Find_Flag;//左线起始点找到标志
uint8 Image_Get_LeftPoint(uint8 start_row)
{
    for(left_point=(IMAGE_WIDTH/2);left_point>3;left_point--)
    {
        if((Image_Use_Robert[start_row][left_point]==255)&&(Image_Use_Robert[start_row][left_point-1]==0)&&(Image_Use_Robert[start_row][left_point-2]==0))
        {
            left_point-=1;
			Left_Find_Flag=1;//
            break;
        }
    }
	if(!Left_Find_Flag)
	{
		left_point=2;
	}
    return start_row;
}

uint8 Image_Get_LeftPoint2(uint8 start_row)
{
    for(left_point=(IMAGE_WIDTH/2);left_point>3;left_point--)
    {
        if((Image_Use_Robert[start_row][left_point]==255)&&(Image_Use_Robert[start_row][left_point-1]==0))
        {
            left_point-=1;
			Left_Find_Flag=1;//
            break;
        }
    }
	if(!Left_Find_Flag)
	{
		left_point=2;
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
uint8 right_point;                     //记录第一个关键点的列坐标
uint8 Right_Find_Flag;//右线起始点找到标志
uint8 Image_Get_RightPoint(uint8 start_row)
{
    for(right_point=(IMAGE_WIDTH/2);right_point<(IMAGE_WIDTH-2);right_point++)//扫到
    {
        if((Image_Use_Robert[start_row][right_point]==255)&&(Image_Use_Robert[start_row][right_point+1]==0)&&(Image_Use_Robert[start_row][right_point+2]==0)) //这里指针变量不能直接和值比较，需要解地址
        {
            right_point=right_point+1;
			Right_Find_Flag=1;
            break;                            //这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
	if(!Right_Find_Flag)//如果找不到
	{
		right_point=IMAGE_WIDTH-3;//取边界点，这里的点就是白点了
	}
    return start_row;
}

uint8 Image_Get_RightPoint2(uint8 start_row)
{
    for(right_point=(IMAGE_WIDTH/2);right_point<(IMAGE_WIDTH-2);right_point++)//扫到
    {
        if((Image_Use_Robert[start_row][right_point]==255)&&(Image_Use_Robert[start_row][right_point+1]==0)) //这里指针变量不能直接和值比较，需要解地址
        {
            right_point=right_point+1;
			Right_Find_Flag=1;
            break;                            //这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
	if(!Right_Find_Flag)//如果找不到
	{
		right_point=IMAGE_WIDTH-3;//取边界点，这里的点就是白点了
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
    uint16 i,j;
    uint32 num=0;
    for(i=1;i<IMAGE_HEIGHT-1;i++)
    {
        for(j=1;j<IMAGE_WIDTH-1;j++)
        {
            //统计8个方向的像素值
            num=Image_Use_Robert[i-1][j-1]+Image_Use_Robert[i-1][j]+Image_Use_Robert[i-1][j+1]
            +Image_Use_Robert[i][j-1]+Image_Use_Robert[i][j+1]+Image_Use_Robert[i+1][j-1]
            +Image_Use_Robert[i+1][j]+Image_Use_Robert[i+1][j+1];

            if(num>=THRESHOLD_MAX && Image_Use_Robert[i][j]==0)    //如果黑点四周的8个点只有2个黑点
            {
                Image_Use_Robert[i][j]=255;
            }
            if(num<=THRESHOLD_MIN && Image_Use_Robert[i][j]==255)//如果白点周围只有2个白点
            {
                Image_Use_Robert[i][j]=0;//过滤成黑
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
    uint8 i=0;
    for(i=0;i<IMAGE_HEIGHT;i++)
    {
        Image_Use[i][0]=0;
		Image_Use[i][1]=0;
        Image_Use[i][IMAGE_WIDTH-1]=0;
        Image_Use[i][IMAGE_WIDTH-2]=0;
    }
    for(i=0;i<IMAGE_WIDTH;i++)
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
 * @return 通过一组点的直线的斜率
 */
float Imgae_SlopeLeft(uint8 begin, uint8 end)
{
    float xsum=0,ysum=0,xysum=0,x2sum=0;
    int16 i=0;
    float result =0;
    static float resultlast=0;//记录上次结果，用来比对
    for(i=begin;i<end;i++)//从起点开始向终点自增（这个应该指的是下标）

    for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）
    {
        xsum +=Left[i].row;//对x坐标自增（行坐标）
        ysum +=Left[i].column;//对y坐标自增（列坐标）
        xysum += Left[i].row*(Left[i].column);//xy坐标乘积自增
        x2sum +=Left[i].row*Left[i].row;//x坐标平方自增
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
float Imgae_SlopeRight(uint8 begin,uint8 end)
{
    float xsum=0,ysum=0,xysum=0,x2sum=0;
    int16 i=0;
    float result =0;
    static float resultlast=0;//记录上次结果，用来比对

    for(i=begin;i<end;i++)//从起点开始向终点自增（这个应该指的是下标）
    {
        xsum +=Right[i].row;//对x坐标自增（行坐标）
        ysum +=Right[i].column;//对y坐标自增（列坐标）
        xysum += Right[i].row*(Right[i].column);//xy坐标乘积自增
        x2sum +=Right[i].row*Right[i].row;//x坐标平方自增
    }
    if((end-begin)*x2sum-xsum*xsum)//这个在求斜率中会作为分母，因此需要判断是否为0
    {
        result = ((end - begin)*xysum - xsum * ysum) / ((end - begin)*x2sum - xsum * xsum);
        resultlast =result;
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
float slope_rate_l,intercept_l;
void Image_CountLeftKB_L(uint8 start,uint8 end)
{
    uint16 i,num=0;
    uint16 xsum=0,ysum=0;
    float y_average,x_average;
    num=0;
    xsum=0;
    ysum=0;
    y_average=0;
    x_average=0;
    for(i=start;i<end;i++)//计算xy坐标的总值（行列坐标）
    {
        xsum +=Left[i].row;//行坐标累加
        ysum +=Left[i].column;//列坐标累加
        num++;
    }
    //计算x,y坐标的平均值
    if(num)
    {
        x_average=(float)(xsum/num);
        y_average=(float)(ysum/num);
    }
    slope_rate_l =Imgae_SlopeLeft(start,end);//计算斜率
    intercept_l=y_average-(slope_rate_l)*x_average;//计算截距
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
float slope_rate_r,intercept_r;
void Image_CountRightKB_R(uint8 start,uint8 end)
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
        xsum +=Right[i].row;//行坐标累加
        ysum +=Right[i].column;//列坐标累加
        num++;
    }
    // 计算x,y坐标的平均值
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);
    }
    slope_rate_r =Imgae_SlopeRight(start,end);//计算斜率
    intercept_r=y_average-(slope_rate_r)*x_average;//计算截距
}

uint8 Left_Count,Right_Count;         //记录左右边界点的个数
uint8 grow_left,grow_right;           //记录左右边界在八邻域时寻点的相对位置
uint8 Left_Max=200,Right_Max=200;     //左右边界搜点时允许最大的搜点量
uint8 Boundary_search_end=30;         //搜寻行数的最高行

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

void Image_Get_neighborhoods(uint8(*Image_Use)[IMAGE_WIDTH])
{
	Left_Count=0;
	Right_Count=0;
	uint16 cur_row,cur_col;//当前行列
	uint8 Pixel_Change;//灰白像素变化标志位——恢复原来的bug
	uint8 Pixel_Count_r,Pixel_Count_l;//计算变化对应的下标值
	uint8 Pixel_row,Pixel_col;//记录变化像素点的坐标（右坐标）
	if(Image_Get_LeftPoint2(117))
	{
		Left[0].row=Image_Get_LeftPoint(117);
		Left[0].column=left_point-1;
		Left[0].flag=1;
		Left[0].grow=2;//初始生长方向为2
		cur_row=Left[0].row;//返回起始的行数
		cur_col=left_point;//这是全局变量，可以调用
		Left_Count++;
		while(Left_Max--)//找140个
		{
			//一 寻点生长
			//0黑1白
			if(Image_Use_Robert[cur_row+1][cur_col]==BLACK&&Image_Use_Robert[cur_row+1][cur_col-1]==WHITE)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=0;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use_Robert[cur_row+1][cur_col-1]==BLACK&&Image_Use_Robert[cur_row][cur_col-1]==WHITE)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=1;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use_Robert[cur_row][cur_col-1]==BLACK&&Image_Use_Robert[cur_row-1][cur_col-1]==WHITE)
			{
				Left[Left_Count].row=cur_row;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=2;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use_Robert[cur_row-1][cur_col-1]==BLACK&&Image_Use_Robert[cur_row-1][cur_col]==WHITE)
			{
				if(Image_Use_Robert[cur_row][cur_col+1]==BLACK&&Image_Use_Robert[cur_row-1][cur_col+1]==WHITE&&
					Image_Use_Robert[cur_row+1][cur_col+1]==WHITE)
				/*生长方向选择为6（正左），详情解析在Right里面*/
				{
					Left[Left_Count].row=cur_row;
					Left[Left_Count].column=cur_col+1;
					Left[Left_Count].flag=1;
					Left[Left_Count].grow=6;
					cur_row=Left[Left_Count].row;
					cur_col=Left[Left_Count].column;
					Left_Count++;
				}
				else
				{
					if(cur_col!=2)//处理分界点问题
					{
						Left[Left_Count].row=cur_row-1;
						Left[Left_Count].column=cur_col-1;
						Left[Left_Count].flag=1;
						Left[Left_Count].grow=3;
						cur_row =Left[Left_Count].row;
						cur_col=Left[Left_Count].column;//更新中心坐标点
						Left_Count++;
					}
					else
					{
						Image_Use_Robert[cur_row-1][cur_col]=BLACK;//将跳变的白点变成黑色，下次就找得到了
						Pixel_row=cur_row-1;
						Pixel_col=cur_col;
						Pixel_Count_l=Left_Count;//记录此时的计数值
						Pixel_Change=1;//1为变成了黑色
					}
					
				}
			}
			else if(Image_Use_Robert[cur_row-1][cur_col]==BLACK&&Image_Use_Robert[cur_row-1][cur_col+1]==WHITE)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=4;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use_Robert[cur_row-1][cur_col+1]==BLACK&&Image_Use_Robert[cur_row][cur_col+1]==WHITE)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=5;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use_Robert[cur_row][cur_col+1]==BLACK&&Image_Use_Robert[cur_row+1][cur_col+1]==WHITE)
			{
				Left[Left_Count].row=cur_row;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=6;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use_Robert[cur_row+1][cur_col+1]==BLACK&&Image_Use_Robert[cur_row+1][cur_col]==WHITE)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=7;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else
			{
				break;
			}
			//二 1.检验越界
			if(cur_row<2||cur_row>IMAGE_HEIGHT-2||cur_col<1||cur_col>IMAGE_WIDTH-2)
			{
				break;
			}
			//2 黑框分点的还原
			if((Left_Count-Pixel_Count_l)==1)
			{
				Image_Use_Robert[Pixel_row][Pixel_col]=WHITE;
			}
		}
	}
	//采取左右对称，后面好写
	/*
	5	4	3
	6		2
	7	0	1
	*/
	if(Image_Get_RightPoint2(117))
	{
		Right[0].row=Image_Get_RightPoint(117);
		Right[0].column=right_point+1;
		Right[0].flag=1;
		Right[0].grow=2;
		cur_row=Right[0].row;
		cur_col=right_point;
		Right_Count++;
		while(Right_Max--)
		{
			//0黑1白
			if(Image_Use_Robert[cur_row+1][cur_col]==BLACK&&Image_Use_Robert[cur_row+1][cur_col+1]==WHITE)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=0;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use_Robert[cur_row+1][cur_col+1]==BLACK&&Image_Use_Robert[cur_row][cur_col+1]==WHITE)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=1;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use_Robert[cur_row][cur_col+1]==BLACK&&Image_Use_Robert[cur_row-1][cur_col+1]==WHITE)
			{
				Right[Right_Count].row=cur_row;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=2;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use_Robert[cur_row-1][cur_col+1]==BLACK&&Image_Use_Robert[cur_row-1][cur_col]==WHITE)
			{
				/*
				5 7检测为白，6为黑——检测竖直状态拐点问题（十字路口扫线扫不上去的问题）
				注：放在这里的话扫线速度更快（因为都是else if结构，提高算法的速度）
				*/
				if(Image_Use_Robert[cur_row][cur_col-1]==BLACK&&Image_Use_Robert[cur_row-1][cur_col-1]==WHITE&&
					Image_Use_Robert[cur_row+1][cur_col-1]==WHITE)
				/*生长方向选择为6（正左）*/
				{
					Right[Right_Count].row=cur_row;
					Right[Right_Count].column=cur_col-1;
					Right[Right_Count].flag=1;
					Right[Right_Count].grow=6;
					cur_row=Right[Right_Count].row;
					cur_col=Right[Right_Count].column;
					Right_Count++;
				}
				else//提高优先级90°巡线判断的优先级
				{
					if(cur_col!=157)//处理分界点问题：当4黑 5白 6黑 7白 的时候，由于（右边）是逆时针扫描，就会有bug
					{
						Right[Right_Count].row=cur_row-1;
						Right[Right_Count].column=cur_col+1;
						Right[Right_Count].flag=1;
						Right[Right_Count].grow=3;
						cur_row=Right[Right_Count].row;
						cur_col=Right[Right_Count].column;
						Right_Count++;
					}
					else
					{
						Image_Use_Robert[cur_row-1][cur_col]=BLACK;//将跳变的白点变成黑色，下次就找得到了
						Pixel_row=cur_row-1;
						Pixel_col=cur_col;
						Pixel_Count_r=Right_Count;//记录此时的计数值
						Pixel_Change=1;//1为变成了黑色
					}
				}
			}
			else if(Image_Use_Robert[cur_row-1][cur_col]==BLACK&&Image_Use_Robert[cur_row-1][cur_col-1]==WHITE)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=4;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use_Robert[cur_row-1][cur_col-1]==BLACK&&Image_Use_Robert[cur_row][cur_col-1]==WHITE)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=5;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use_Robert[cur_row][cur_col-1]==BLACK&&Image_Use_Robert[cur_row+1][cur_col-1]==WHITE)
			{
				Right[Right_Count].row=cur_row;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=6;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use_Robert[cur_row+1][cur_col-1]==BLACK&&Image_Use_Robert[cur_row+1][cur_col]==WHITE)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=7;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else
				break;
			//下面是巡线以后其他的处理
			//1  越界处理
			if(cur_row<2||cur_row>IMAGE_HEIGHT-2||cur_col<1||cur_col>IMAGE_WIDTH-2)
			{
				break;
			}
			//2 变化灰度的还原的处理（因为不处理的话下次就还是会扫错）
			if((Right_Count-Pixel_Count_r)==1)//确保是变黑的下一次就变白（记录次数，防止误扫）
			{
				Image_Use_Robert[Pixel_row][Pixel_col]=WHITE;//一般都是白->黑->白
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

void Image_Get_neighborhoods2(uint8(*Image_Use)[IMAGE_WIDTH])
{
	Left_Count=0;
	Right_Count=0;
	uint16 cur_row,cur_col;//当前行列
	uint8 Pixel_Change;//灰白像素变化标志位——恢复原来的bug
	uint8 Pixel_Count_r,Pixel_Count_l;//计算变化对应的下标值
	uint8 Pixel_row,Pixel_col;//记录变化像素点的坐标（右坐标）
	if(Image_Get_LeftPoint(117))
	{
		Left[0].row=Image_Get_LeftPoint(117);
		Left[0].column=left_point-1;
		Left[0].flag=1;
		Left[0].grow=2;//初始生长方向为2
		cur_row=Left[0].row;//返回起始的行数
		cur_col=left_point;//这是全局变量，可以调用
		Left_Count++;
		while(Left_Max--)//找140个
		{
			//一 寻点生长
			//0黑1白
			if(Image_Use[cur_row+1][cur_col]==BLACK&&Image_Use[cur_row+1][cur_col-1]==WHITE)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=0;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col-1]==BLACK&&Image_Use[cur_row][cur_col-1]==WHITE)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=1;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row][cur_col-1]==BLACK&&Image_Use[cur_row-1][cur_col-1]==WHITE)
			{
				Left[Left_Count].row=cur_row;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=2;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col-1]==BLACK&&Image_Use[cur_row-1][cur_col]==WHITE)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=3;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col]==BLACK&&Image_Use[cur_row-1][cur_col+1]==WHITE)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=4;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col+1]==BLACK&&Image_Use[cur_row][cur_col+1]==WHITE)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=5;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row][cur_col+1]==BLACK&&Image_Use[cur_row+1][cur_col+1]==WHITE)
			{
				Left[Left_Count].row=cur_row;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=6;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col+1]==BLACK&&Image_Use[cur_row+1][cur_col]==WHITE)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=7;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else
			{
				break;
			}
			//二 1.检验越界
			if(cur_row<2||cur_row>IMAGE_HEIGHT-2||cur_col<1||cur_col>IMAGE_WIDTH-2)
			{
				break;
			}
			//2 黑框分点的还原
			if((Left_Count-Pixel_Count_l)==1)
			{
				Image_Use[Pixel_row][Pixel_col]=WHITE;
			}
		}
	}
	//采取左右对称，后面好写
	/*
	5	4	3
	6		2
	7	0	1
	*/
	if(Image_Get_RightPoint(117))
	{
		Right[0].row=Image_Get_RightPoint(117);
		Right[0].column=right_point+1;
		Right[0].flag=1;
		Right[0].grow=2;
		cur_row=Right[0].row;
		cur_col=right_point;
		Right_Count++;
		while(Right_Max--)
		{
			//0黑1白
			if(Image_Use[cur_row+1][cur_col]==BLACK&&Image_Use[cur_row+1][cur_col+1]==WHITE)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=0;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col+1]==BLACK&&Image_Use[cur_row][cur_col+1]==WHITE)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=1;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row][cur_col+1]==BLACK&&Image_Use[cur_row-1][cur_col+1]==WHITE)
			{
				Right[Right_Count].row=cur_row;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=2;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col+1]==BLACK&&Image_Use[cur_row-1][cur_col]==WHITE)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=3;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col]==BLACK&&Image_Use[cur_row-1][cur_col-1]==WHITE)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=4;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col-1]==BLACK&&Image_Use[cur_row][cur_col-1]==WHITE)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=5;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row][cur_col-1]==BLACK&&Image_Use[cur_row+1][cur_col-1]==WHITE)
			{
				Right[Right_Count].row=cur_row;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=6;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col-1]==BLACK&&Image_Use[cur_row+1][cur_col]==WHITE)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=7;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else
				break;
			//下面是巡线以后其他的处理
			//1  越界处理
			if(cur_row<2||cur_row>IMAGE_HEIGHT-2||cur_col<1||cur_col>IMAGE_WIDTH-2)
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
            //二 连续3次都是同一个点就不再寻找
            if(Right[Right_Count].row==Right[Right_Count-1].row&&Right[Right_Count].column==Right[Right_Count-1].column&&
                Right[Right_Count-1].row==Right[Right_Count-2].row&&Right[Right_Count-1].column==Right[Right_Count-2].column)
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
uint8 Mid_Count;
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
struct coordinate last_right_low;//定义有拐点的上次坐标
void Image_FillCross(uint8(*Image_Use)[IMAGE_WIDTH])
{
    uint8 i, j;             // 中间变量
    uint8 l_ready, r_ready; // 定义左右线扫完的标志位

    //先扫左拐点，从左下角开始扫，左右线的计数值不一定相等
    //两种写法，一种生长方向由5-->2，另外一种直接判断多个2（因为不能保证斜线一直是5），这里用方法一，后面可能会改，觉得这个不太行
    //判断是否进入十字，或者未进入十字
//    if(Image_Use[IMAGE_HEIGHT-4][4]==BLACK&&Image_Use[IMAGE_HEIGHT-4][6]==BLACK&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-6]==BLACK&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-4]==BLACK)
    
        for(i=0;i<Left_Count;i++)//实际上理想情况下是i不会=left_count，因为找到左下拐点时肯定还有很多点没找
        {
            /*plan2：生长方向优化*/
			if((Left[i-1].grow==5||Left[i-4].grow==5)&&(Left[i-2].grow==5||Left[i-5].grow==5)&&(Left[i-3].grow==5||Left[i-6].grow==5)
				&&(Left[i+1].grow!=5||Left[i+4].grow!=5)&&(Left[i+2].grow!=5||Left[i+5].grow!=5)&&(Left[i+3].grow!=5||Left[i+6].grow!=5))
			{
				left_low.row=Left[i].row;
                left_low.column=Left[i].column;
				left_low.index=i;
				left_low.flag=1;
                break;
			}
        }
        //找的是左上拐点，这里可以不放在同一个for循环，因为肯定是先找到左下拐点，然后才能找到左拐点
//        for(i=0;i<Left_Count;i++)
//        {
//            //图片上方的有效点比较少，判断就简单一点，不然找不到，注意生长方向就行
////            if((Left[i-1].grow==6||Left[i-4].grow==6)&&(Left[i-2].grow==6||Left[i-5].grow==6)&&(Left[i-3].grow==6||Left[i-6].grow==6)&&
////				(Left[i+1].grow!=6||Left[i+4].grow!=6)&&(Left[i+2].grow!=6||Left[i+5].grow!=6)&&(Left[i+3].grow!=6||Left[i+6].grow!=6))
//			// if(Left[i].grow!=5&&(Left[i-1].grow!=5||Left[i-4].grow!=5)&&(Left[i-2].grow!=5||Left[i-5].grow!=5)&&
//			// 	(Left[i+1].grow==5||Left[i+4].grow==5)&&(Left[i+2].grow==5||Left[i+5].grow==5))
//            // {
//            //     left_high.row=Left[i].row;
//            //     left_high.column=Left[i].column;
//			// 	left_high.index=i;
//            //     break;
//            // }
//            /*plan3 斜率最大法*/
//            // if(Image_Getk(Left[i].column,Left[i].row-120)>)
//        }
//        //左右对称，生长方向一样，就不用改了
        for(i=0;i<Right_Count;i++)
        {
			/*取点1*/
//            if((Right[i-1].grow==5||Right[i-4].grow==5)&&(Right[i-2].grow==5||Right[i-5].grow==5)&&(Right[i-3].grow==5||Right[i-6].grow==5)
//				&&(Right[i+1].grow==2||Right[i+4].grow==2)&&(Right[i+2].grow==2||Right[i+5].grow==2)&&(Right[i+3].grow==2||Right[i+6].grow==2)
//			&&Right[i].row<(IMAGE_HEIGHT/2)&&my_abs(Right[i].row-Right[i+4].row)<3)
			/*取点2*/
			if((Right[i-1].grow==5||Right[i-2].grow==5)&&(Right[i].column<Right[i-1].column)&&(Right[i+1].grow==2)&&Right[i].row>(IMAGE_HEIGHT/2)
				&&((Right[i-3].row-Right[i].row)>=1))
			/*取点3*/
//			if(Right[i-1].grow==5&&(Right[i-2].grow==5||Right[i-3].grow==5)&&Right[i-4].grow==5&&Right[i].grow==5&&Right[i+1].grow!=5)
			{
				
				right_low.row=Right[i].row;
                right_low.column=Right[i].column;
				right_low.index=i;
                break;
			}
        }
		for(i=0;i<Right_Count;i++)
        {
			if(Right[i].grow!=5&&(Right[i-1].grow!=5||Right[i-4].grow!=5)&&(Right[i-2].grow!=5||Right[i-5].grow!=5)&&
				(Right[i+1].grow==5||Right[i+4].grow==5)&&(Right[i+2].grow==5||Right[i+5].grow==5))
            {
                right_high.row=Right[i].row;
                right_high.column=Right[i].column;
				right_high.index=i;
                break;
            }
		}
		//左边线补线
        if(left_low.index!=0)//判断是否为0
        {
            left_line.k=Image_Getk((left_low.column-Left[left_low.index-3].column),(left_low.row-Left[left_low.index-3].row));
            left_line.b=Image_Getb(left_low.column,left_low.row,left_line.k);
            //求出左线斜率和截距：column=k*row+b（十字的k一般都是1或-1）
            //补线有两种方法：一种把全部的点都补上，一种只补两个拐点拐点，这里用第二种，后面再用基础扫线，扫出补线后的边线
            
			for(i=left_low.row;i>10;i--)//从左下拐点开始向上补线
            {
				int new_column_l=(int)(left_line.k*i+left_line.b);
				if(new_column_l>0)
				{
					Image_Use[i][new_column_l]=BLACK;//从拐点向上补线
					ips200_draw_line(0,0,i,new_column_l,RGB565_RED);
				}
				else
				{
					break;
				}
            }
			//记录上一次的斜率和截距
			last_left_line.k=left_line.k;
			last_left_line.b=left_line.b;
			//记录完后清零
            left_low.index=0;
            left_line.k=0;
            left_line.b=0;
        }
		else//如果找不到的话（下标为0）
		{
			left_line.k=last_left_line.k;//就用上次的斜率和截距
			left_line.b=last_left_line.b;
			for(i=left_low.row;i>10;i--)//从左下拐点开始向上补线
            {
				int new_column_l=(int)(left_line.k*i+left_line.b);
				if(new_column_l>0)
				{
					Image_Use[i][new_column_l]=BLACK;//从拐点向上补线
					ips200_draw_line(0,0,i,new_column_l,RGB565_RED);
				}
				else
				{
					break;
				}
            }
		}
		//右边线补线
        if(right_low.index!=0)
        {
		    right_line.k=Image_Getk((right_low.column-Right[right_low.index-3].column),(right_low.row-Right[right_low.index-3].row));
//			right_line.k=1.0;
            right_line.b=Image_Getb(right_low.column,right_low.row,right_line.k);
            for(i=right_low.row;i>10;i--)//从左下拐点开始向上补线
            {
				int new_column_r=(int)(right_line.k*i+right_line.b);
				if(new_column_r>0)
				{
					Image_Use[i][new_column_r]=BLACK;//从拐点向上补线
					ips200_draw_line(0,0,new_column_r,i,RGB565_BLUE);
				}
				else
					break;
            }
			last_right_line.k=right_line.k;
			last_right_line.b=right_line.b;
			//记录完后清零
//            right_low.index=0;
//            right_line.k=0;
//            right_line.b=0;
        }
		else//如果找不到的话（下标为0）
		{
			right_line.k=last_right_line.k;//就用上次的斜率和截距
			right_line.b=last_right_line.b;
			for(i=right_low.row;i>10;i--)//从左下拐点开始向上补线
            {
				int new_column_r=(int)(right_line.k*i+right_line.b);
				if(new_column_r>0)
				{
					Image_Use[i][new_column_r]=BLACK;//从拐点向上补线
					ips200_draw_line(0,0,i,new_column_r,RGB565_BLUE);
				}
				else
				{
					break;
				}
            }
		}
//		Image_CountLeftKB_L(right_low.index,right_low.index-3);//求出右线的斜率和截距slope_rate_r
//		right_line.k=slope_rate_r;
//		right_line.b=intercept_r;//左线补线
//		slope_rate_r=0.0;
//		intercept_r=0.0;
//        //补线有两种方法：一种把全部的点都补上，一种只补两个拐点拐点，这里用第二种，后面再用基础扫线，扫出补线后的边线
//        for(i=right_low.row;i<IMAGE_HEIGHT-1;i++)//从左下拐点开始向上补线
//        {
//            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=BLACK;//补线,这里传入的k是浮点型,后面要转换
//			//补线方程：column=k*row+b
//            if(i<=IMAGE_HEIGHT-10)
//            {
//                r_ready=1;//左线补完了
//                break;
//            }
//        }
//        Image_pointsright(right_high.row,right_high.column,right_low.row,right_low.column);//求出右线的斜率和截距
//        for(i=right_low.row;i<IMAGE_HEIGHT-1;i++)//从右下拐点开始补线
//        {
//            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=BLACK;//补线
//            if(i>=IMAGE_HEIGHT-4)
//            {
//                r_ready=1;//右线补完了
//                break;
//            }
//        }
//    
//    /*如果未进十字的补线判断完成，那么接下来就要进入十字中途的补线，这里和前面的最大区别就是左下角和右下角是不是白色*/
//		/*用的sobel就判断不出来左下角是不是黑色了*/
////    if(l_ready&&r_ready&&Image_Use[IMAGE_HEIGHT-4][4]==WHITE&&Image_Use[IMAGE_HEIGHT-4][6]==WHITE&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-6]==WHITE&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-4]==WHITE)//等左右线补完，只要补完就说明有过，那么就顺便看左下角和右小角是否为白色
//    
//        //此时只有左上拐点和右上拐点，因为左下角和右下角都是白色，所以只需要找左上拐点和右上拐点，取直线就往拐点往上扫
//        for(i=0;i<Left_Count;i++)
//        {
//            if(Left[i].grow==5&&Left[i-2].grow==5&&Left[i-5].grow==5
//            &&Left[i-2].grow==6&&Left[i-4].grow==6&&Left[i-6].grow==6)
//            {
//                left_high.row=Left[i].row;
//                left_high.column=Left[i].column;
//                break;
//            }
//        }
//        for(i=0;i<Right_Count;i++)
//        {
//            if(Right[i].grow==5&&Right[i-2].grow==5&&Right[i-5].grow==5
//            &&Right[i-2].grow==6&&Right[i-4].grow==6&&Right[i-6].grow==6)
//            {
//                right_high.row=Right[i].row;
//                right_high.column=Right[i].column;
//                break;
//            }
//        }
//        //求出拐点后开始补线,这里求斜率有三种方法:1.直接用两点(拐点和左下角)求斜率 2.用最小二乘法求斜率(用拐点往上的点) 3.用最小二乘法求斜率，但是只用拐点附近的点
//        Image_pointsleft(left_high.row,left_high.column,IMAGE_HEIGHT-4,4);//求出左线的斜率和截距
//        for(i=IMAGE_HEIGHT-4;i>4;i--)
//        {
//            Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]=BLACK;//补线
//            if(i<=left_high.row-1)
//            {
//                l_ready=1;//左线补完了
//                break;
//            }
//        }
//        Image_pointsright(right_high.row,right_high.column,IMAGE_HEIGHT-4,IMAGE_WIDTH-4);//求出右线的斜率和截距
//        for(i=IMAGE_HEIGHT-4;i>4;i--)
//        {
//            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=BLACK;//补线
//            if(i<=right_high.row-1)
//            {
//                r_ready=1;//右线补完了
//                break;
//            }
//        }
    
}

/*下面是直道函数的辅助函数*/
/**
 * @brief 获取图像的k值
 * 
 * @param int16 derta_column,derta_row，坐标差值，别算反
 * @return float k值
 */
float Image_Getk(int16 derta_column,int16 derta_row)
{
    return (float)(derta_column/derta_row);
}
/*下面是直道函数的辅助函数*/
/**
 * @brief 获取图像的b值
 * 
 * @param int16 example_row,example_column, k
 * @return float k值
 */
float Image_Getb(int16 example_column,int16 example_row,float k)
{
    return (float)(example_column-k*example_row);
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
    // uint8 i;
    // float k_left_low,k_left_high;//左下线斜率，左上线斜率
    // float k_all;//多断小斜率叠加
    // k_all=0.0;
    // for(i=0;i<5;i++)//求左线下10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Left[i].column,Left[i+5].column,(Left[i+5].row-Left[i].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_left_low=k_all/5;
    // k_all=0.0;
    // for(i=0;i<5;i++)//求左线上10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Left[40+i].column,Left[i+45].column,(Left[i+45].row-Left[i+40].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_left_high=k_all/5;
    // if(Image_ab_value(k_left_high,k_left_low)<LINE_K)//看直线的斜率是否符合要求
    // {
    //     return 1;
    // }
    // else
    // {
    //     return 0;
    // }
	return 1;
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
    // uint8 i;
    // float k_right_low,k_right_high,k_all;
    // k_all=0.0;
    // for(i=0;i<5;i++)//求右线下10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Right[i].column,Right[i+5].column,(Right[i+5].row-Right[i].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_right_low=k_all/5;
    // k_all=0.0;
    // for(i=0;i<5;i++)//求右线上10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Right[40+i].column,Right[i+45].column,(Right[i+45].row-Right[i+40].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_right_high=k_all/5;
    // if(Image_ab_value(k_right_high,k_right_low)<LINE_K)//看直线的斜率是否符合要求
    // {
    //     return 1;
    // }
    // else
    // {
    //     return 0;
    // }
	return 1;
}
float Image_B(uint8 point_row,uint8 point_column,float k)
{
	float b;
	b=k*point_row-point_column;
	return b;
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
	uint8 i,j;
	uint8 TH;
//	i=Image_Get_LeftFlag(117);
//	j=Image_Get_Rightflag(117);
	TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
//	Image_Binarization(TH,Image_Use);
	Image_DrawRectangle();
	Image_Filter();
	Image_Sobel( Image_Use, Image_Use_Robert ,TH);//全局Sobel得二值图(方案二) 2.8ms
    ips200_displayimage03x((uint8 *)Image_Use_Robert, 160, 120); //pidMotor1Speed
//	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count
	
	Image_Get_neighborhoods(Image_Use_Robert);
//	
	Image_FillCross(Image_Use_Robert);
//	tft180_show_int(3,120,points_l[l_count-1][0],3);left_lineleft_line.b
//	Image_Get_neighborhoods(100,Image_Use);left_high.index
    ips200_draw_line(0,0,left_low.column,left_low.row,RGB565_RED);
    ips200_draw_line(0,0,right_low.column,right_low.row,RGB565_BLUE);
//    ips200_draw_line(60,80,left_high.column,left_low.row,RGB565_RED);
//	ips200_draw_line(0,0,right_high.column,right_high.row,RGB565_BLUE);
//	new_column
    ips200_show_int(3,140,left_low.column,3);
    ips200_show_int(3,160,left_low.index,3);
	ips200_show_int(3,180,Left[left_low.index-3].row,3);
    ips200_show_int(3,200,Left[left_low.index-3].column,3);
	
	ips200_show_int(43,120,right_low.row,3);
    ips200_show_int(43,140,right_low.column,3);
    ips200_show_int(43,160,right_low.index,3);
	ips200_show_int(43,180,Right[right_low.index-3].row,3);
    ips200_show_int(43,200,Right[right_low.index-3].column,3);
	
	
	ips200_show_float(3,220,right_line.k,4,4);
	ips200_show_float(3,240,right_line.b,4,4);
	
//	Get_Midpoint();
    for(i=0;i<Left_Count;i++)
    {
        ips200_draw_point(Left[i].column,Left[i].row,RGB565_RED);
        ips200_draw_point((Left[i].column)+1,Left[i].row,RGB565_RED);
//		ips200_draw_line(30,40,Left[i].column,Left[i].row,RGB565_RED);
    }
    for(i=0;i<Right_Count;i++)
    {
        ips200_draw_point(Right[i].column,Right[i].row,RGB565_GREEN);
        ips200_draw_point((Right[i].column)+1,Right[i].row,RGB565_GREEN);
    }
	
//	if(Left_Count>195)
//	{
//		check();
//	}
//	for(i=0;i<Left_Count;i++)
//	{
//		tft180_draw_point((Left[i].column)/2+1,Left[i].row/2,RGB565_RED);
//	}
//	for(i=0;i<Left_Count;i++)
//	{
//		tft180_draw_point((Left[i].column)/2-1,Left[i].row/2,RGB565_RED);
//	}
//	for(i=0;i<Right_Count;i++)
//	{
//		tft180_draw_point(Right[i].column,Right[i].row,RGB565_RED);
//	}

    Image_DrawRectangle();
    //	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count
    tft180_show_int(3, 80, left_point, 3);
    Image_Get_neighborhoods(Image_Use);
    //	tft180_show_int(3,120,points_l[l_count-1][0],3);
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
int Image_LeftGrowDirection(uint8 end,uint8 Direction)
{
    uint8 i,direction_count;
    for(i=0;i<end;i++)
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
int Image_RightGrowDirection(uint8 end,uint8 Direction)
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
struct coordinate Left_first_point;//左环岛的第一个拐点
struct coordinate Left_second_point;//左环岛的第二个拐点
struct coordinate Right_first_point;//右拐点
void Image_LeftRound(uint8(*Image_Use)[IMAGE_WIDTH])
{
    uint8 LeftRound_State = 0;      // 左环岛状态1-5
    uint8 Last_LeftRound_State = 0; // 上一个状态
    uint8 i;                        // 中间变量
    uint8 Left_first_point_index;   // 记录左拐点在左边线数组的下标
    uint8 Left_second_point_index;
    uint8 Right_first_point_index;
    uint8 Left_third_point_index;
    uint8 index_temp;//中间变量
    left_line.k=0.0;left_line.b=0.0;//清零
    uint8 BLACK_count;//计算黑点值
    if(Image_Stretch_Right())//先看右边是不是直道，右边直道只能判断1，2，5，6
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
            //状态5的标志判断：1.右侧拐点消失（或右侧拐点太低了） 2. 左线生长方向向上 3.右线基本生长方向左上5
            //4. 左右线相交  5.和状态3差不多，唯一不同的就是右线的生长方向为直线（可以求斜率来判断） 
            //这里来的做法是统计5方向的个数，如果大于3/4就认为是状态5（也可以判断斜率的直线）
            if(LeftRound_State==4&&Last_LeftRound_State==3&&(Image_RightGrowDirection(Right_Count,5)>=((Right_Count/4)*3)))
            {
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 5; // 进入状态5
            }
        }
        // 状态1的补线
        if (Last_LeftRound_State == 0 && LeftRound_State == 1) // 通过状态机进行不同的补线，这里因为LeftRound_State取值，就要==1，不能直接写LeftRound_State
        {
            left_line.k=0.0;left_line.b=0.0;//清零
            Image_pointsleft(Left[Left_first_point_index-2].row,Left[Left_first_point_index-2].column,Left[Left_first_point_index-5].row,Left[Left_first_point_index-5].column);
            //求出左补线的斜率和截距
            for(i=Left[Left_first_point_index-2].row;i>IMAGE_HEIGHT-10;i--)//从左下拐点，向上开始补线
            {
                if(Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]==BLACK)//如果要变黑的点本来就是黑点
                {
                    BLACK_count++;
                }
                else
                {
                    Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]=BLACK;//column=k*row+b，这里补线方程和下面不一样
                }
                if(i<=IMAGE_HEIGHT-13||BLACK_count>=3)
                {
                    Left_first_point_index=0;//补完下标索引清零
                    BLACK_count=0;
                    break;//左线补完了
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
                if(Image_Use[(int)(left_line.k)*i+(int)(left_line.b)][i]==BLACK)//如果要变黑的点本来就是黑点
                {
                    BLACK_count++;//是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[(int)(left_line.k)*i+(int)(left_line.b)][i]=BLACK;//如果是白色的话就变为黑点
                }
                //这里的直线方程为:row=k*column+b
                if((i>=IMAGE_WIDTH-4)||(BLACK_count>=3))//防止赛道上出现噪点
                {
                    Left_second_point_index=0;//补完下标索引清零
                    BLACK_count=0;//记得清零
                    break;//左线补完了
                }
            }
        }
    }
    /*状态3不用补线*/
    //状态3的标志判断：1. 左边线和右边线会相交 2.左右边线起始点的列坐标相差较大 3.左线的生长方向基本都是正上（4）
    //4.右线的列坐标的起始和终点相差较大
    else if(LeftRound_State==2&&Last_LeftRound_State==1&&(abs_int(Right[0].column,Left[0].column)>80)
            &&(Image_LeftGrowDirection(Left_Count,4)>=(Left_Count/2))&&(abs_int(Right[0].column,Right[Right_Count-1].column)>80))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 3; // 进入状态3，状态3就不用补线了，拐点也不用搞了
    }
    //状态4的标志判断
    //标志判断：1.左线的生长方向正上占比较少 2.右线的起点和终点列坐标相差较小 3.右线起始的列坐标和和终点的列坐标相差较小
    else if(LeftRound_State==3&&Last_LeftRound_State==2&&(Image_LeftGrowDirection(Left_Count,4)<=(Left_Count/4))
    &&(abs_int(Right[0].column,Right[Right_Count-1].column)<20))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 4; // 进入状态4
    }
    //状态6的标志判断
    //1. 右线的起始点和终止点的列坐标间隔较小   2. 左线向上生长方向的比例下降 3.左线的起点和终止点的列坐标间隔较大
    else if(LeftRound_State==5&&Last_LeftRound_State==4&&(abs_int(Left[0].column,Left[Left_Count-1].column)>80)
    &&(abs_int(Right[0].column,Right[Right_Count-1].column)<20)&&(Image_LeftGrowDirection(Left_Count,4)<=(Left_Count/4)))
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
                if(Image_Use[(int)(right_line.k)*i+(int)(right_line.b)][i]==BLACK)//如果要变黑的点本来就是黑点
                {
                    BLACK_count++;//是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[(int)(right_line.k)*i+(int)(right_line.b)][i]=BLACK;//如果是白色的话就变为黑点
                }
                //这里的直线方程为:row=k*column+b，列的点比较多
                if((i<=IMAGE_WIDTH-4)||(BLACK_count>=3))//防止赛道上出现噪点
                {
                    Right_first_point_index=0;//补完下标索引清零
                    BLACK_count=0;//记得清零
                    break;//右线补完了
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
            /*防函数报错*/
            // if(Image_ab_value((Image_Getk(Left[i].column,Left[i-3].column,abs_int(Left[i-3].row,Left[i].row))),
            // Image_Getk(Left[i].column,Left[i+3].column,abs_int(Left[i+3].row,Left[i].row)))>0.4)
            // {
            //     Left_third_point_index=i;//记录下标
            //     break;
            // }
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
                if(Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]==BLACK)//如果要变黑的点本来就是黑点
                {
                    BLACK_count++;//是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]=BLACK;//如果是白色的话就变为黑点
                }
                //这里的直线方程为:row=k*column+b，列的点比较多
                if((i>=IMAGE_HEIGHT-4)||(BLACK_count>=3))//防止赛道上出现噪点
                {
                    Left_third_point_index=0;//补完下标索引清零
                    BLACK_count=0;//记得清零
                    break;//左线补完了
                }
            }
        }
    }
}

/**
 * @brief 扫描60行中的某一行的黑白跳变的函数（检测斑马线）
 * 
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */
uint8 Image_Scan_Row(uint8(*Image_Use)[IMAGE_WIDTH],uint8 target_row)
{
    uint8 i;//中间变量
    uint8 BLACK_WHITE_count=0;//黑白跳变的计数
    for(i=0;i<IMAGE_WIDTH-1;i++)
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

//限幅函数
int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
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
/**
 * @brief 左线的三角滤波
 * 
 * @param int num 左线的点数总个数 int kernel 内核大小
* @return 无，输出值为边线Left_New数组
 * @example void Image_blur_points_Left(Left_Count,5);
 */
void Image_blur_points_Left(int num,int kernel)
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
		Left_New[i].grow=Left[i].grow;
    }
}
/**
 * @brief 右线的三角滤波
 * 
 * @param int num 右线的点数总个数 int kernel 内核大小
 * @return 无
 * @example void Image_blur_points_Left(Left_Count,5);
 */
void Image_blur_points_Right(int num,int kernel)
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
		Right_New[i].grow=Right[i].grow;
    }
}


uint8 Image_Scan_Column(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_column)
{
    uint8 i;
    uint8 BLACK_WHITE_count=0;//黑白跳变的计数
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

// 逆透视变换后左右边线
float rpts0[POINTS_MAX_LEN][2];
float rpts1[POINTS_MAX_LEN][2];
int rpts0_num, rpts1_num;

// 逆透视变换后左右边线再三角滤波后的边线数组
float rpts0b[POINTS_MAX_LEN][2];
float rpts1b[POINTS_MAX_LEN][2];
int rpts0b_num, rpts1b_num;

//逆透视变换后左右边线再三角滤波后再等距采样的数组
float rpts0s[POINTS_MAX_LEN][2];
float rpts1s[POINTS_MAX_LEN][2];
int rpts0s_num, rpts1s_num;

int rpts0_num, rpts1_num;
int x0_first, y0_first, x1_first, y1_first; // 左右边线第一个点的坐标

int x1, y1;
int x2, y2;

                      // SOBEL二值化图像

void test(void)
{
//    int th;
    Image_Compress();
//    th = OSTU_GetThreshold(*Image_Use, IMAGE_HEIGHT, IMAGE_WIDTH);
//    Image_Sobel(Image_Use, Image_Use_Robert, th);
	
    Find_Borderline();

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
    int TH;
    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
    Image_Binarization(TH, Image_Use);
    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms
    img_raw.data = *Image_Use;

    // 标记种子起始点(后续元素处理要用到)
    x0_first = x1;
    y0_first = y1;

    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]); // 求数组的长度
    // 扫底下五行，寻找跳变点
    for (; y0_first > begin_y - 50; y0_first--)//从所选的行，向上扫5次，每次从中间向左线扫
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
			Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);//开始跑迷宫
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
        if ((x == 1 && y < img->height - 10) || x == img->width - 2 || y == 1 || (y == 100 && step > 19))
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
        if ((x == img->width - 2 && y < img->height - 20) || x == 1 || y == 1 || (y == 100 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线
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
    // for (i = 0; i < ipts1_num; i++)
    // {
    //     ips200_draw_line(160,0,ipts1[i][0] + 2, ipts1[i][1] + 2, RGB565_RED);
    // }
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
    int dx = pt1[0] - pt0[0];//求出x,y坐标的差值
    int dy = pt1[1] - pt0[1];
    if (abs(dx) > abs(dy))//如果x的差值大于y的差值，那么直线形式就是y=kx+b
    {
        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1))
        {
            int y = pt0[1] + (x - pt0[0]) * dy / dx;                                  // y = 左线横坐标 + x遍历差值占总差值比例 * y方向差值
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; // （x，y）坐标像素（不超出边界）赋值
        }
    }
    else//如果y的差值大于x的差值，那么直线形式就是x=ky+b
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
void draw_line2(float pt0[2], float pt1[2], float pts_out[][2], int *num, float dist){
    int remain = 0, len = 0;//下一次采样折线段距离
    float x0 = pt0[0];
    float y0 = pt0[1];
    float dx = pt1[0] - x0;//求出x,y坐标的差值
    float dy = pt1[1] - y0;
    float dn = mySqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
    //float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
    dx /= dn;//此点处的cosθ
    dy /= dn;//此点处的sinθ

    //每次等距采样处理
    while(remain < dn){
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
    int i ,count;//用来计数
    for(i = 0;i<num1;i++)//复制数组1
    {
        pt_out[i][0] = pt0[i][0];
        pt_out[i][1] = pt0[i][1];
        count++;
    }

    if(x)//反向拼接
    {
        for(i = 0;i<num2;i++)
        {
            pt_out[num1+i][0] = pt1[num2-i-1][0];
            pt_out[num1+i][1] = pt1[num2-i-1][1];
            count++;
        }

    }else//正向拼接
    {
        for(i = 0;i<num2;i++)
        {
            pt_out[num1+i][0] = pt1[i][0];
            pt_out[num1+i][1] = pt1[i][1];
            count++;
        }
    }
    *num = count;
}

/*************************************************************************
 *  函数名称：void blur_points();
 *  功能说明：点集三角滤波
 *  参数说明：输入边线数组pits_in[][2]，边线总个数num，输出边线数组pts_out，点集范围kernel
 *  函数返回：无
 *  修改时间：2022年10月4日
 *  备    注：这个函数中的kernel参数是用来指定点集三角滤波的范围的。具体来说，它是一个奇数，表示以每个点为中心的滤波器的大小。
 *  如果kernel为5，则每个点周围的5个点将被用于计算平均值，以平滑该点。kernel越大，平滑效果越明显，但也会导致失去一些细节
 *  例如：kernel = 5
 *  xi = (0*xi-3 + 1*xi-2 + 2*xi-1 + 3*xi + 2*xi+1 + 1*xi+2 + 0*xi+3)/9
 *  yi 同理
 *************************************************************************/
void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel)
{
    zf_assert(kernel % 2 == 1);//检查变量 kernel 是否为奇数
    int half = kernel / 2;//用于计算滤波器的大小
    for (int i = 0; i < num; i++) //对于每个点，都会计算它周围的点的加权平均值，并将结果存储在输出点集数组中
    {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++) {
            pts_out[i][0] += (float)pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
            pts_out[i][1] += (float)pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}
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
void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist){
    int remain = 0, len = 0;//下一次采样折线段距离
    for(int i=0; i<num1-1 && len < *num2; i++){
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float dx = pts_in[i+1][0] - x0;
        float dy = pts_in[i+1][1] - y0;
        float dn = mySqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
        //float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
        dx /= dn;//此点处的cosθ
        dy /= dn;//此点处的sinθ

        //每次等距采样处理
        while(remain < dn && len < *num2){
            x0 += dx * remain;
            pts_out[len][0] = x0;
            y0 += dy * remain;
            pts_out[len][1] = y0;

            len++;
            dn -= remain;
            remain = dist;
        }
        remain -= dn;//当跨越一点采样折线距离近似直线
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

int N_Xfound_num;//面向赛道编程，双内L计数

// 当前巡线模式（发车方向）
enum track_type_e track_type = TRACK_RIGHT;

float error[1];
float ave_error;//速度控制输入变量
// 多个预锚点位置
int aim_idx[1] ;


// 计算远锚点偏差值
float dx[1] ;
float dy[1] ;
float dn[1] ;

// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
// 计算近锚点偏差值
float dx_near;
float dy_near ;
float dn_near ;
float pure_angle;

enum garage_type_e garage_type = GARAGE_NONE;//初始化为向左出库 调试状态为NONE

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
void find_corners()
{
    // 识别Y,L拐点
    Ypt0_found = Ypt1_found = Lpt0_found = Lpt1_found = N_Lpt1_found = N_Lpt0_found = false;
    is_straight0 = rpts0s_num > 1.6 / sample_dist;           //逆透视之后的边线个数大于80个就是直道,也就是边线边长大于0.8m,并不是直向前0.8m
    is_straight1 = rpts1s_num > 1.6 / sample_dist;

    //左边线角度
    for (int i = 1; i < rpts0s_num; i++) {              //只要数组里的角度符合的，Y或L就为true,然后就不在继续检测Y或L
        if (rpts0an[i] == 0) continue;                      //以下检测只检测峰值（极大值）
        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
        float conf = fabs(rpts0a[i]) - (fabs(rpts0a[im1]) + fabs(rpts0a[ip1])) / 2;

        //Y角点阈值                 (30*  ,   50*)    并且距离在40个点之内 (注：30.转化浮点数)
        if (Ypt0_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
            Ypt0_rpts0s_id = i;
            Ypt0_found = true;
        }
        //L角点阈值                 (55*  ,    140*)  并且距离在40个点之内 修改距离为1.2m增长判断路障距离
        if (Lpt0_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {

            Lpt0_rpts0s_id = i;
            Lpt0_found = true;

            if(rpts0a[Lpt0_rpts0s_id] < 0)              //这一个L点如果是向右拐的，就是内L点
            {
                N_Lpt0_rpts0s_id = Lpt0_rpts0s_id;
                N_Lpt0_found = true;
//                //对内L角的线截断
//                rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
                Lpt0_found = false;
            }

        }
        //长直道阈值
        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight0 = false;           //只要中间有大角度，就不是长直道
        if (Ypt0_found == true && Lpt0_found == true && is_straight0 == false) break;
    }

    //右边线角度
    for (int i = 1; i < rpts1s_num; i++) {
        if (rpts1an[i] == 0) continue;
        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);
        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);
        float conf = fabs(rpts1a[i]) - (fabs(rpts1a[im1]) + fabs(rpts1a[ip1])) / 2;
        if (Ypt1_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
            Ypt1_rpts1s_id = i;
            Ypt1_found = true;
        }
        if (Lpt1_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {
            Lpt1_rpts1s_id = i;
            Lpt1_found = true;

            if(rpts1a[Lpt1_rpts1s_id] > 0)              //这一个L点如果是向左拐的，就是内L点
            {
                N_Lpt1_rpts1s_id = Lpt1_rpts1s_id;
                N_Lpt1_found = true;
//                //对内L角的线截断
//                rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
                Lpt1_found = false;
            }
        }

        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight1 = false;
        if (Ypt1_found == true && Lpt1_found == true && is_straight1 == false) break;
    }

    // Y点二次检查,依据两角点距离及角点后张开特性
    if (Ypt0_found && Ypt1_found) {
        //==================================================================原检查
        float dx = rpts0s[Ypt0_rpts0s_id][0] - rpts1s[Ypt1_rpts1s_id][0];
        float dy = rpts0s[Ypt0_rpts0s_id][1] - rpts1s[Ypt1_rpts1s_id][1];
        float dn = sqrtf(dx * dx + dy * dy);
        if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)             //两点距离不能大于60cm  (赛道40cm)
        {
            float dwx = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
                        rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
            float dwy = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
                        rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
            float dwn = sqrtf(dwx * dwx + dwy * dwy);
            if (!(dwn > 0.7 * pixel_per_meter &&                            //如两个Y点之后第50个 点之间的距离小于0.7m
                  rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Ypt0_rpts0s_id][0] &&
                  rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Ypt1_rpts1s_id][0]))
            {
                Ypt0_found = Ypt1_found = false;
            }
        } else
        {
            Ypt0_found = Ypt1_found = false;
        }
        //zqkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk的检查

        float dx_temp ,dy_temp ;



        dx_temp = rpts0s[Ypt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
        dy_temp = rpts0s[Ypt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
        xielv_left_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


        dx_temp = rpts1s[Ypt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
        dy_temp = rpts1s[Ypt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
        xielv_right_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


    }
    // L点二次检查，车库模式不检查, 依据L角点距离及角点后张开特性
    if (garage_type == GARAGE_NONE) {
        if (Lpt0_found && Lpt1_found) {
            float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
            float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
            float dn = sqrtf(dx * dx + dy * dy);
            if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)            //两点距离不能大于60cm  (赛道40cm)
            {
                float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
                float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
                float dwn = sqrtf(dwx * dwx + dwy * dwy);
                if (!(dwn > 0.5 * pixel_per_meter &&                                //符合直角的特征必须50个点后距离大于0.7m，
                      rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
                      rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
                {
                    Lpt0_found = Lpt1_found = false;
                }
            } else
            {
                Lpt0_found = Lpt1_found = false;
            }

            float dx_temp ,dy_temp ;



            dx_temp = rpts0s[Lpt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
            dy_temp = rpts0s[Lpt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
            xielv_left_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子  一般是负


            dx_temp = rpts1s[Lpt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
            dy_temp = rpts1s[Lpt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
            xielv_right_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


        }
    }

    //左车库第二次检查
    if (Lpt0_found && N_Lpt1_found) {                   //理论上是左车库
        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
        float dn = sqrtf(dx * dx + dy * dy);
        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //两点距离不能大于60cm  (赛道45cm)
        {
            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
            float dwn = sqrtf(dwx * dwx + dwy * dwy);
            if (!(dwn < 0.5 * pixel_per_meter &&                                //符合左车库特征必须50个点后距离小于0.5m，
                  rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
                  rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] < rpts1s[Lpt1_rpts1s_id][0]))
            {
                Lpt0_found = N_Lpt1_found = false;
            }
        } else
        {
            Lpt0_found = N_Lpt1_found = false;
        }
    }


    //右车库第二次检查
    if (Lpt1_found && N_Lpt0_found) {                   //理论上是右车库
        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
        float dn = sqrtf(dx * dx + dy * dy);
        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //两点距离不能大于60cm  (赛道45cm)
        {
            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
            float dwn = sqrtf(dwx * dwx + dwy * dwy);
            if (!(dwn < 0.5 * pixel_per_meter &&                                //符合左车库特征必须50个点后距离小于0.5m，
                  rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] > rpts0s[Lpt0_rpts0s_id][0] &&
                  rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
            {
                Lpt1_found = N_Lpt0_found = false;
            }
        } else
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
void Pespective(int pts_in[][2],int int_num ,  float pts_out[][2])
//带入提取出来的两条边线，得到透视变换后的两条边线
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

