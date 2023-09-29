#include "image.h"
#include "math.h"
//八邻域扫的左线的结构体
struct Left_Edge
{
    unsigned char row;                        //行坐标，省点内存就没设int
    unsigned char column;                     //列坐标，同上
    unsigned char flag;                       //判断边界点是否找到
	uint8 grow;
};
//八邻域扫的右线的结构体
struct Right_Edge
{
    unsigned char row;                        //行坐标，省点内存就没设int
    unsigned char column;                     //列坐标，同上
    unsigned char flag;                       //判断边界点是否找到
	uint8 grow;
};

struct Left_Edge Left[140];                   //左边界结构体
struct Right_Edge Right[140];                 //右边界结构体

//定义坐标结构体
struct coordinate
{
    unsigned char row;                        //行坐标，省点内存就没设int
    unsigned char column;                     //列坐标，同上
};
struct coordinate mid[140];//中线坐标结构体

//直线结构体
struct Line
{
    float k;
    float b;
};//定义直线方程
struct Line left_line;
struct Line right_line;//定义左右线方程
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
        row = (int)i * pro_h + 0.5;
        for (j = 0; j < Image_Width; j++) // 遍历图像的每一列，从第零列到第79列。
        {
            line = (int)j * pro_w + 0.5;
            Image_Use[i][j] = mt9v03x_image[row][line]; // mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
        }
    }
    mt9v03x_finish_flag = 0;
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


/**
 * @brief 将输入的灰度图像转化为二值化图像
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
void Image_Binarization(uint8 threshold,uint8(*Image_Use)[Image_Width])
{
    unsigned char i, j;
    for (i = 0; i < Image_Height; i++)
    {
        for (j = 0; j < Image_Width; j++)
        {
            if (Image_Use[i][j] > threshold)
            {
                Image_Use[i][j] = 255;
            }
            else
                Image_Use[i][j] = 0;
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
    if(value>=0) return value;
    else return -value;
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
    if(x<a) x = a;
    if(x>b) x = b;
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
unsigned char left_point;                     //记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
unsigned char Image_Get_LeftFlag(void)
{
    for(left_point=(Image_Width/2);left_point>3;left_point--)
    {
        if((Image_Use[56][left_point]==255)&&(Image_Use[56][left_point-1]==0)&&(Image_Use[56][left_point-2]==0))
        {
            break;
        }
    }
    return 1;
}

/**
 * @brief 获取图像右侧起始生长点--寻找起始点函数，从第56行开始数
 * 
 * 
 * @return unsigned char 返回1表示获取成功，返回0表示获取失败
 */
unsigned char right_point;                     //记录第一个关键点的列坐标
unsigned char Image_Get_Rightflag(void)
{
    
    for(right_point=(Image_Width/2);right_point<(Image_Width-2);right_point++)
    {
        if((Image_Use[56][right_point]==255)&&(Image_Use[56][right_point+1]==0)&&(Image_Use[56][right_point+2]==0)) //这里指针变量不能直接和值比较，需要解地址
        {
            break;                            //这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
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
    uint16 i,j;
    uint32 num=0;
    for(i=1;i<Image_Height-1;i++)
    {
        for(j=1;j<Image_Width-1;j++)
        {
            //统计8个方向的像素值
            num=Image_Use[i-1][j-1]+Image_Use[i-1][j]+Image_Use[i-1][j+1]
            +Image_Use[i][j-1]+Image_Use[i][j+1]+Image_Use[i+1][j-1]
            +Image_Use[i+1][j]+Image_Use[i+1][j+1];

            if(num>=threshold_max && Image_Use[i][j]==0)    //如果黑点四周的8个点只有2个黑点
            {
                Image_Use[i][j]=255;
            }
            if(num<=threshold_min && Image_Use[i][j]==255)//如果白点周围只有2个白点
            {
                Image_Use[i][j]=0;//过滤成黑
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
    for(i=0;i<Image_Height;i++)
    {
        Image_Use[i][0]=0;
        Image_Use[i][Image_Width-1]=0;
        Image_Use[i][Image_Width-2]=0;
    }
    for(i=0;i<Image_Width;i++)
    {
        Image_Use[0][i]=0;
        Image_Use[1][i]=0;//图片底下两层变黑
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
void Image_pointsleft(uint8 x1,uint8 y1,uint8 x2,uint8 y2)
{
    float two_points_k,two_points_b;
    two_points_k=(float)((y2-y1)/(x2-x1));
    two_points_b=(float)((y1*x2-x1*y2)/(x2-x1));
    left_line.k=two_points_k;
    left_line.b=two_points_b;
}
/**
 * @brief 通过两个点的坐标计算直线斜率和截距，并将结果存储在全局变量right_line中
 * 
 * @param x1 第一个点的x坐标
 * @param y1 第一个点的y坐标
 * @param x2 第二个点的x坐标
 * @param y2 第二个点的y坐标
 */
void Image_pointsright(uint8 x1,uint8 y1,uint8 x2,uint8 y2)
{
    float two_points_k,two_points_b;
    two_points_k=(float)((y2-y1)/(x2-x1));
    two_points_b=(float)((y1*x2-x1*y2)/(x2-x1));
    right_line.k=two_points_k;
    right_line.b=two_points_b;
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
float Imgae_Slope(uint8 begin,uint8 end,uint8 *border)
{
    float xsum=0,ysum=0,xysum=0,x2sum=0;
    int16 i=0;
    float result =0;
    static float resultlast=0;//记录上次结果，用来比对

    for(i=begin;i<end;i++)//从起点开始向终点自增（这个应该指的是下标）
    {
        xsum +=i;//对x坐标自增（行坐标）
        ysum +=border[i];//对y坐标自增（列坐标）
        xysum += i*(border[i]);//xy坐标乘积自增
        x2sum +=i*i;//x坐标平方自增
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
 * @brief 计算斜率和截距,配合上面的最小二乘法使用
 * 
 * 
 * @param begin 线的起点
 * @param end 线的终点
 * @param border 线上点的y坐标数组
 * @return 通过一组点的直线的斜率,通过指针来返回斜率和截距
 */
void Image_CountKB(uint8 start,uint8 end,uint8 *border, float *slope_rate,float *intercept)
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
        xsum +=i;
        ysum +=border[i];
        num++;
    }
    //计算x,y坐标的平均值
    if(num)
    {
        x_average=(float)(xsum/num);
        y_average=(float)(ysum/num);
    }
    *slope_rate =Imgae_Slope(start,end,border);//计算斜率
    *intercept=y_average-(*slope_rate)*x_average;//计算截距
}

unsigned char Left_Count,Right_Count;         //记录左右边界点的个数
unsigned char grow_left,grow_right;           //记录左右边界在八邻域时寻点的相对位置
unsigned char Left_Max=140,Right_Max=140;     //左右边界搜点时允许最大的搜点量
unsigned char Boundary_search_end=30;         //搜寻行数的最高行
uint16 cur_row,cur_col;//当前行列
uint8 Gather_flag;
uint8 Gather_row[50];//两线会聚数组，用来存储行坐标
uint8 Gather_Count;//计数
/**
 * @brief 八邻域巡线
 *
 * @param uint8 输入值为二值化处理后的数组
 * @example Image_Get_neighborhoods(Image_Use)
 */

void Image_Get_neighborhoods(uint8(*Image_Use)[Image_Width])
{
	Left_Count=0;
	Right_Count=0;
	
	if(left_point!=3)
	{
		Left[0].row=56;
		Left[0].column=left_point;
		Left[0].flag=1;
		Left[0].grow=2;//初始生长方向为2
		cur_row=56;
		cur_col=left_point;
		Left_Count++;
		while(Left_Max--)//找140个
		{
			//一 寻点生长
			//0白1黑
			if(Image_Use[cur_row+1][cur_col]==black&&Image_Use[cur_row+1][cur_col-1]==white)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=0;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col-1]==black&&Image_Use[cur_row][cur_col-1]==white)
			{
				Left[Left_Count].row=cur_row+1;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=1;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row][cur_col-1]==black&&Image_Use[cur_row-1][cur_col-1]==white)
			{
				Left[Left_Count].row=cur_row;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=2;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col-1]==black&&Image_Use[cur_row-1][cur_col]==white)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col-1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=3;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col]==black&&Image_Use[cur_row-1][cur_col+1]==white)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=4;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col+1]==black&&Image_Use[cur_row][cur_col+1]==white)
			{
				Left[Left_Count].row=cur_row-1;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=5;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row][cur_col+1]==black&&Image_Use[cur_row+1][cur_col+1]==white)
			{
				Left[Left_Count].row=cur_row;
				Left[Left_Count].column=cur_col+1;
				Left[Left_Count].flag=1;
				Left[Left_Count].grow=6;
				cur_row =Left[Left_Count].row;
				cur_col=Left[Left_Count].column;//更新中心坐标点
				Left_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col+1]==black&&Image_Use[cur_row+1][cur_col]==white)
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
			//二 检验越界
			if(cur_row<=3||cur_row>=57||cur_col<=3||cur_col>=97)
			{
				break;
			}
		}
	}
	//采取左右对称，后面好写
	/*
	5	4	3
	6		2
	7	0	1
	*/
	if(right_point!=97)
	{
		Right[0].row=56;
		Right[0].column=right_point;
		Right[0].flag=1;
		Right[0].grow=2;
		cur_row=56;
		cur_col=right_point;
		Right_Count++;
		while(Right_Max--)
		{
			//0黑1白
			if(Image_Use[cur_row+1][cur_col]==black&&Image_Use[cur_row+1][cur_col+1]==white)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=0;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col+1]==black&&Image_Use[cur_row][cur_col+1]==white)
			{
				Right[Right_Count].row=cur_row+1;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=1;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row][cur_col+1]==black&&Image_Use[cur_row-1][cur_col+1]==white)
			{
				Right[Right_Count].row=cur_row;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=2;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col+1]==black&&Image_Use[cur_row-1][cur_col]==white)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col+1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=3;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col]==black&&Image_Use[cur_row-1][cur_col-1]==white)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=4;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row-1][cur_col-1]==black&&Image_Use[cur_row][cur_col-1]==white)
			{
				Right[Right_Count].row=cur_row-1;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=5;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row][cur_col-1]==black&&Image_Use[cur_row+1][cur_col-1]==white)
			{
				Right[Right_Count].row=cur_row;
				Right[Right_Count].column=cur_col-1;
				Right[Right_Count].flag=1;
				Right[Right_Count].grow=6;
				cur_row=Right[Right_Count].row;
				cur_col=Right[Right_Count].column;
				Right_Count++;
			}
			else if(Image_Use[cur_row+1][cur_col-1]==black&&Image_Use[cur_row+1][cur_col]==white)
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
            //一 越界处理
			if(cur_row<=3||cur_row>=57||cur_col<=3||cur_col>=97)
			{
				break;
			}
            //以下是待测试的代码
            //二 连续3次都是同一个点就不再寻找
            if(Right[Right_Count].row==Right[Right_Count-1].row&&Right[Right_Count].column==Right[Right_Count-1].column&&
                Right[Right_Count-1].row==Right[Right_Count-2].row&&Right[Right_Count-1].column==Right[Right_Count-2].column)
            {
                break;
            }
            //三 当扫到的点多个在同一行时，只保留最后一个点（这个目前不写，因为处理的是压缩后的图像，点多一点也没关系，如果需要的话后期再改进）
            //四 当左右线会聚时退出循环，并记录此时的行数（判断坡道）
            if((Left[Left_Count-1].row==Right[Right_Count-1].row)&&(Left[Left_Count-1].column==Right[Right_Count-1].column||
            Right[Right_Count-1].column-Left[Left_Count-1].column<=3))
            //判断条件：行相等，列也相等or列相差在3以内
            {
                Gather_row[Gather_Count]=Left[Left_Count-1].row;
                Gather_flag=1;
                break;
            }
            else
            {
                Gather_flag=0;
            }
		}
	}
}


uint8 Mid_Count;
uint16 cur_row,cur_col;//当前行列
/**
 * @brief 取中线函数（有bug，待验证）
 *
 * @param uint8 无
 * @example Get_Midpoint(void)
 */
void Get_Midpoint(void)
{
	uint8 left_c,right_c;
	uint8 i;//中间值
	left_c=Left_Count;
	right_c=Right_Count;//存入左右计数值
	for(i=0;i<left_c;i--)
	{
		mid[i].row=(Left[i].row+Right[i].row)/2;
		mid[i].column=(Left[i].column+Right[i].column)/2;
	}
	
}


/**
 * @brief 十字补线函数
 *
 * @param uint8 已经经过八邻域扫线的图像Image_Use
 * @example Image_FillCross(Image_Use)
 * @explanation  处理十字不难，只需要找到4个拐点，然后补线即可，找到4个拐点的处理方法：1.判断多个点的生长方向 2.多个点构成的斜率的急剧变化 这里用方法1
 */
//分别设置左上，左下，右上，右下四个拐点坐标
struct coordinate left_high;
struct coordinate left_low;
struct coordinate right_high;
struct coordinate right_low;
void Image_FillCross_1(uint8(*Image_Use)[Image_Width])
{
    uint8 i,j;//中间变量
    uint8 l_ready,r_ready;//定义左右线扫完的标志位

    //先扫左拐点，从左下角开始扫，左右线的计数值不一定相等
    //两种写法，一种生长方向由5-->2，另外一种直接判断多个2（因为不能保证斜线一直是5），这里用方法一，后面可能会改，觉得这个不太行
    //判断是否进入十字，或者未进入十字
    if(Image_Use[56][4]==black&&Image_Use[56][6]==black&&Image_Use[56][94]==black&&Image_Use[56][96]==black)
    {
        for(i=0;i<Left_Count;i++)//实际上理想情况下是i不会=left_count，因为找到左下拐点时肯定还有很多点没找
        {
            //这里判断写越多越好，找的拐点就越符合条件，但是也有可能找不到，所以要权衡
            if(Left[i].grow==5&&Left[i-2].grow==5&&Left[i+2].grow==2&&Left[i+3].grow==2&&Left[i+5].grow==2
                &&Left[i+7].grow==2&&Left[i-5].grow==5)
            {
                left_low.row=Left[i].row;
                left_low.column=Left[i].column;
                break;
            }
        }
        //找的是左上拐点，这里可以不放在同一个for循环，因为肯定是先找到左下拐点，然后才能找到左拐点
        for(i=0;i<Left_Count;i++)
        {
            //图片上方的有效点比较少，判断就简单一点，不然找不到，注意生长方向就行
            if(Left[i].grow==5&&Left[i-2].grow==5&&Left[i-5].grow==5
            &&Left[i-2].grow==6&&Left[i-4].grow==6&&Left[i-6].grow==6)
            {
                left_high.row=Left[i].row;
                left_high.column=Left[i].column;
                break;
            }
        }
        //左右对称，生长方向一样，就不用改了
        for(i=0;i<Right_Count;i++)
        {
            if(Right[i].grow==5&&Right[i-2].grow==5&&Right[i+2].grow==2&&Right[i+3].grow==2&&Right[i+5].grow==2
                &&Right[i+7].grow==2&&Right[i-5].grow==5)
            {
                right_low.row=Right[i].row;
                right_low.column=Right[i].column;
                break;
            }
        }
        for(i=0;i<Right_Count;i++)
        {
            if(Right[i].grow==5&&Right[i-2].grow==5&&Right[i-5].grow==5
            &&Right[i-2].grow==6&&Right[i-4].grow==6&&Right[i-6].grow==6)
            {
                right_high.row=Right[i].row;
                right_high.column=Right[i].column;
                break;
            }
        }
        //找完点开始补线，先求左线
        Image_pointsleft(left_high.row,left_high.column,left_low.row,left_low.column);//求出左线的斜率和截距
        //补线有两种方法：一种把全部的点都补上，一种只补两个拐点拐点，这里用第二种，后面再用基础扫线，扫出补线后的边线
        for(i=left_low.row;i<Image_Height-1;i++)//从左下拐点开始补线
        {
            Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]=black;//补线,这里传入的k是浮点型,后面要换
            if(i>=Image_Height-4)
            {
                l_ready=1;//左线补完了
                break;
            }
            
        }
        Image_pointsright(right_high.row,right_high.column,right_low.row,right_low.column);//求出右线的斜率和截距
        for(i=right_low.row;i<Image_Height-1;i++)//从右下拐点开始补线
        {
            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=black;//补线
            if(i>=Image_Height-4)
            {
                r_ready=1;//右线补完了
                break;
            }
        }
    }
    /*如果未进十字的补线判断完成，那么接下来就要进入十字中途的补线，这里和前面的最大区别就是左下角和右下角是不是白色*/
    if(l_ready&&r_ready&&Image_Use[56][4]==white&&Image_Use[56][6]==white&&Image_Use[56][94]==white&&Image_Use[56][96]==white)//等左右线补完，只要补完就说明有过，那么就顺便看左下角和右小角是否为白色
    {
        //此时只有左上拐点和右上拐点，因为左下角和右下角都是白色，所以只需要找左上拐点和右上拐点，取直线就往拐点往上扫
        for(i=0;i<Left_Count;i++)
        {
            if(Left[i].grow==5&&Left[i-2].grow==5&&Left[i-5].grow==5
            &&Left[i-2].grow==6&&Left[i-4].grow==6&&Left[i-6].grow==6)
            {
                left_high.row=Left[i].row;
                left_high.column=Left[i].column;
                break;
            }
        }
        for(i=0;i<Right_Count;i++)
        {
            if(Right[i].grow==5&&Right[i-2].grow==5&&Right[i-5].grow==5
            &&Right[i-2].grow==6&&Right[i-4].grow==6&&Right[i-6].grow==6)
            {
                right_high.row=Right[i].row;
                right_high.column=Right[i].column;
                break;
            }
        }
        //求出拐点后开始补线,这里求斜率有三种方法:1.直接用两点(拐点和左下角)求斜率 2.用最小二乘法求斜率(用拐点往上的点) 3.用最小二乘法求斜率，但是只用拐点附近的点
        Image_pointsleft(left_high.row,left_high.column,56,4);//求出左线的斜率和截距
        for(i=56;i>4;i--)
        {
            Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]=black;//补线
            if(i<=left_high.row-1)
            {
                l_ready=1;//左线补完了
                break;
            }
        }
        Image_pointsright(right_high.row,right_high.column,56,96);//求出右线的斜率和截距
        for(i=56;i>4;i--)
        {
            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=black;//补线
            if(i<=right_high.row-1)
            {
                r_ready=1;//右线补完了
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
float Image_Getk(uint8 start_y,uint8 end_y,uint8 interval)
{
    return (float)((end_y-start_y)/interval);
}
/**
 * @brief 计算两个浮点数之间的绝对值差
 * 
 * @param a 第一个浮点数
 * @param b 第二个浮点数
 * @return float 两个浮点数之间的绝对值差
 */
float Image_ab_value(float a,float b)
{
    if(a>b)
    {
        return a-b;
    }
    else
    {
        return b-a;
    }
}
float k_buff[5];//存放5个斜率值
/**
@brief 判断左直道函数，如果为直道，会通过返回值用来判断是否为直道
@param 二值化后的图像，实际上用不上
@return 1，则存在左直道，0就没有
@example Image_Stretch(Image_Use)
@note 将直线分为2个地方，看上下斜率相差是否相近，近的话就判断为直道
*/
uint8 Image_Stretch_Left(uint8(*Image_Use)[Image_Width])
{
    uint8 i;
    float k_left_low,k_left_high;//左下线斜率，左上线斜率
    float k_all;//多断小斜率叠加
    k_all=0.0;
    for(i=0;i<5;i++)//求左线下10个点的平均斜率
    {
        k_buff[i]=Image_Getk(Left[i].column,Left[i+5].column,(Left[i+5].row-Left[i].row));//x是行坐标,y是列坐标
    }
    for(i=0;i<5;i++)
    {
        k_all+=k_buff[i];
    }
    k_left_low=k_all/5;
    k_all=0.0;
    for(i=0;i<5;i++)//求左线上10个点的平均斜率
    {
        k_buff[i]=Image_Getk(Left[40+i].column,Left[i+45].column,(Left[i+45].row-Left[i+40].row));//x是行坐标,y是列坐标
    }
    for(i=0;i<5;i++)
    {
        k_all+=k_buff[i];
    }
    k_left_high=k_all/5;
    if(Image_ab_value(k_left_high,k_left_low)<Line_k)//看直线的斜率是否符合要求
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
@example Image_Stretch_Right(Image_Use)
@note 将直线分为2个地方，看上下斜率相差是否相近，近的话就判断为直道
*/
uint8 Image_Stretch_Right(uint8(*Image_Use)[Image_Width])
{
    uint8 i;
    float k_right_low,k_right_high,k_all;
    k_all=0.0;
    for(i=0;i<5;i++)//求右线下10个点的平均斜率
    {
        k_buff[i]=Image_Getk(Right[i].column,Right[i+5].column,(Right[i+5].row-Right[i].row));//x是行坐标,y是列坐标
    }
    for(i=0;i<5;i++)
    {
        k_all+=k_buff[i];
    }
    k_right_low=k_all/5;
    k_all=0.0;
    for(i=0;i<5;i++)//求右线上10个点的平均斜率
    {
        k_buff[i]=Image_Getk(Right[40+i].column,Right[i+45].column,(Right[i+45].row-Right[i+40].row));//x是行坐标,y是列坐标
    }
    for(i=0;i<5;i++)
    {
        k_all+=k_buff[i];
    }
    k_right_high=k_all/5;
    if(Image_ab_value(k_right_high,k_right_low)<Line_k)//看直线的斜率是否符合要求
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
//图像处理的函数都放在这里，这样就避免了定义问题
void Image_Run(void)
{
	uint8 i;

	i=Image_Get_LeftFlag();
	i=Image_Get_Rightflag();
//	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count
	tft180_show_int(3,80,left_point,3);
	Image_Get_neighborhoods(Image_Use);
//	tft180_show_int(3,120,points_l[l_count-1][0],3);
//	Image_Get_neighborhoods(100,Image_Use);
	tft180_draw_line(0,0,cur_col,cur_row,RGB565_RED);//行坐标l_countl_count
//	Get_Midpoint();
	for(i=0;i<Left_Count;i++)
	{
		tft180_draw_point(Left[i].column,Left[i].row,RGB565_BLUE);
	}
	for(i=0;i<Left_Count;i++)
	{
		tft180_draw_point(Left[i].column+1,Left[i].row,RGB565_BLUE);
	}
	for(i=0;i<Left_Count;i++)
	{
		tft180_draw_point(Left[i].column-1,Left[i].row,RGB565_BLUE);
	}
	for(i=0;i<Right_Count;i++)
	{
		tft180_draw_point(Right[i].column,Right[i].row,RGB565_RED);
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
    uint8 i;//中间变量
    uint8 Ramp_flag,Last_Rampflag;//坡道标志位，以及上一个状态标志位
    if(Gather_flag==1)//找到坡道
    {
        for(i=0;i<Gather_Count;i++)
        {
            if((Gather_row[i]>Gather_row[i+1])&&(Gather_row[i+1]>Gather_row[i+2]))//表示正在靠近坡道，最高行不断上升
            {
                Last_Rampflag=Ramp_flag;//记录上一个状态
                Ramp_flag=1;//标志位为1，表示还没上坡道，还在坡道附近的直道
            }
            else if((Gather_row[i]<Gather_row[i+1])&&(Gather_row[i+1]<Gather_row[i+2]))//最高行变小，表示已经上坡道了
            {
                Last_Rampflag=Ramp_flag;
                Ramp_flag=2;//标志位为2，表示已经上坡道了，此时需要加速
                //加速的代码
            }
            else
            {
                Last_Rampflag=Ramp_flag;
                Ramp_flag=0;
            }
        }
    }
    if(!Gather_flag&&!Ramp_flag&&Last_Rampflag==2)//判断为下坡状态
    {
        //减速的代码
    }
    
}
