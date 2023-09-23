#include "image.h"

uint8 Image_Use[Image_Hight][Image_With];

unsigned int temp[188];
/**
 * @brief 截取我们需要的图像大小
 *
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */
 void Image_Compress(void)
{
    int i, j, row, line;
    const float pro_h = Primeval_Hight / Image_Hight, pro_w = Primeval_With / Image_With; // 根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。
    for (i = 0; i < Image_Hight; i++)                                                    // 遍历图像的每一行，从第零行到第59行。
    {
        row = (int)i * pro_h + 0.5;
        for (j = 0; j < Image_With; j++) // 遍历图像的每一列，从第零列到第79列。
        {
            line = (int)j * pro_w + 0.5;
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



//unsigned int HistoGram[256];             //先定义灰度直方图
/**
 * @brief 大津法求阈值1.0（和其他的版本有些出入）
 *
 * @param Uint8 *image 第一次原始图像（灰度为0-255，有188*120个像素点的图像）
 * @return 求得的阈值
 * @exception 场地得到的灰度图像一般颜色不一，但是图像里面占的面积最大的灰度肯定只有赛道内的灰度值A1和赛道外的灰度值A2，因此得到的直方图会在A1和A2
 * 出现两个峰值，而所求的阈值就是两个峰值之间的最低值
 */
 unsigned int Image_GetThreshold1(void)
{

	//以上是检测变量
    unsigned int i,j;                        //用来遍历灰度直方图
    unsigned int High1=0;                     //第一高峰
    unsigned int High1_Position=0;           //第一高峰对应的灰度值
    unsigned int High2=0;                     //第二高峰
    unsigned int High2_Position=0;           //第二高峰对应的灰度值
	unsigned int HistoGram[256]={0};
    unsigned char gray;                      //灰度值（中间值）
    unsigned int High_temp;                   //高度中间值
    unsigned int Low1=20000;                  //定义低谷1
    unsigned int Low1_Position;              //定义低谷1的灰度值位置，即所求的阈值
    for(i=0;i<Image_Hight;i++)              //先遍历灰度值第一高峰
    {
        for(j=0;j<Image_With;j++)
        {
            gray=Image_Use[i][j];                 //指针传递
            HistoGram[gray]++;               //gray解码传值，得到的HistoGram[256]即为灰度直方图
        }
    }
	
    for(i=0;i<255;i++)
    {
        if(HistoGram[i]>High1)
        {
            High1=HistoGram[i];               //遍历所有点，存储最高峰的值
            High1_Position=i;                 //存储当前最高峰值的灰度值
        }
    }
	//以上程序正常
	return (High1_Position);
	
    //下面这一段程序输出的High2_Position一直为0，且会使输出图像为全0——目前暂未找到bug （2023，9，22）
    for(High_temp=High1-5;High_temp>0;High_temp-=5)//从第一高峰开始向下寻找第二高峰
    {
        for(i=0;i<256;i++)                    //在高度值固定时，对灰度值进行遍历
        {
            if((HistoGram[i]>High_temp)&&(i-High1_Position)>25)    //防止找到最高峰旁边的点
            {
                High2=HistoGram[i];           //记录次高峰峰值（会有0-5内的误差）
                High2_Position=i;             //记录次高峰峰值对应的灰度值
				break;
            }
        }
    }
	
	
	
    if(High1_Position>High2_Position)         //如果最高峰对应的灰度值大于次高峰
    {
        for(i=High2_Position;i<High1_Position;i++)
        {
            if(HistoGram[i]<Low1)             //遍历，求最小值
            {
                Low1=HistoGram[i];            //刷新谷底高度
                Low1_Position=i;              //求出谷底位置
            }
        }
    }
    return Low1_Position;                     //返回谷底位置，即阈值
}

/**
 * @brief 将输入的灰度图像转化为二值化图像
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
 void Image_Binarization(unsigned char threshold)
 {
     unsigned char i,j;
     for(i=0;i<Image_Hight;i++)
     {
         for(j=0;j<Image_With;j++)
         {
             if(Image_Use[i][j]>threshold)
			 {
				Image_Use[i][j]=255;
			 }
			 else
				 Image_Use[i][j]=0;
         }
     }
 }


//此函数在原函数的基础上，优化了阈值：在图像出现大面积一个颜色时，此时直方图中只有一个高峰（或者两个高峰很接近），这里就直接选择最高峰对应的灰度值作为灰度值传输阈值
uint8 Image_GetThreshold(void)   //注意计算阈值的一定要是原图像
{
#define GrayScale 256				          //定义灰度总值
	uint8 last_threshold;                     //定义上次的阈值，进行阈值的输出限幅
    int Pixel_Max=0;                          //定义灰度值的最大值和最小值（0-255）
    int Pixel_Min=255;
	int Pixel_Max_Position;					  //灰度值最高峰对应的灰度值
    uint16 width = Image_With;                //定义图像的宽和高
    uint16 height = Image_Hight;
    int pixelCount[GrayScale];                //记录每个灰度对应的像素值
    float pixelPro[GrayScale];                //定义每个像素点在图中的比例的数组
    int i, j, pixelSum = width * height/4;    //定义中间变量，定义像素总数（这里只要算1/4的就行）
    uint8 threshold = 0;                      //定义返回阈值
    uint8* data = *Image_Use;                 //指向像素数据的指针，省内存
    for (i = 0; i < GrayScale; i++)           //初始化两个灰度值像素数组
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }                            
    uint32 gray_sum=0;                        //定义所有灰度值的总数
    //一 统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)             
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;                                 //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];                                     //求灰度值总和
            if(data[i * width + j]>Pixel_Max)   
			{	
				Pixel_Max=data[i * width + j]; 										//同时记录直方图中灰度值的最高峰和最低峰
				Pixel_Max_Position=i * width + j;
			}				
            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
        }
    }
    //二 计算每个像素值的点在整幅图像中的比例
    for (i = Pixel_Min; i < Pixel_Max; i++)     
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
 
    }
    //三 遍历灰度级[0,255]，开始寻找
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    //w0：后景所占总图片的总比例        w1:前景所占总图片的总比例   有关系式：（即阈值以下为w0,阈值以上为w1）w1+w0=1
    //u0tmp：背景部分灰度值点的比例*灰度值          u1tmp：前景部分灰度值点的比例*灰度值  （后面有解析）
    //u0,u1：背景，前景的平均灰度   u：全局平均灰度 u=(u1+u2)/2
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {
 
        w0 += pixelPro[j];                    //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];             //背景部分每个灰度值的点的比例 *灰度值
 
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
 
        u0 = u0tmp / w0;                      //背景平均灰度
        u1 = u1tmp / w1;                      //前景平均灰度
        u = u0tmp + u1tmp;                    //全局平均灰度
        deltaTmp = (float)(w0 *w1* (u0 - u1)* (u0 - u1)) ;
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
 
    }
    //四 阈值限幅，如果超过想要的阈值，就会和上次一样
    if(threshold>80 && threshold<130)
        last_threshold = threshold;
    else
        {
            threshold = last_threshold;
        }
	
	//防止强光，出现单高峰时，就选择该单高峰的灰度值作为阈值传输
	if(threshold==0)
	{
		threshold=Pixel_Max_Position;
	}
    return threshold;
}

/**
 * @brief 图像加框，防止八邻域扫到附近的点，将边界上的点的灰度值设为0
 *
 * @param Uint8 *image 第一次原始图像（灰度为0-255，有188*120个像素点的图像）
 * @return 无
 * @exception 无
 */
void Image_Draw_Frame(unsigned char *picture[Image_Hight][Image_With])
{
    unsigned char i;                          //用来循环的变量
    for(i=0;i<Image_Hight;i++)
    {
        picture[i][0]=0;
        picture[i][1]=0;
        picture[i][Image_With-1]=0;
        picture[i][Image_With-2]=0;          //图片的旁边的两列全设为黑点
    }
    for(i=0;i<Image_With;i++)
    {
        picture[0][i]=0;
        picture[1][i]=0;                      //图片上下两列全设为黑点（不用搞角落，角落已经搞过了）
    }
}


//
//  * @brief 将输入的灰度图像转化为二值化图像
//  *
//  * @param Uint8 输入图像的地址
//  * @param Uint8 输出图像的地址
//  * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
//  */
unsigned char left_point;                     //记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
unsigned char Image_Get_LeftFlag(void)
{
    for(left_point=(188/2);left_point>0;left_point--)
    {
        if((temp[left_point]==1)&&(temp[left_point-1]==0)&&(temp[left_point-2]==0))
        {
            break;
        }
    }
    return 1;
}
/**
 * @brief 寻找是否存在有边界，无输入参数，参数选择全局变量，方便调用
 * @param 同上
 * @return 同上
 * @exception 同上
 *  */
unsigned char right_point;                     //记录第一个关键点的列坐标
unsigned char Image_Get_Rightflag(void)
{
    
    for(right_point=(188/2);right_point>0;right_point++)
    {
        if((temp[right_point]==1)&&(temp[right_point+1]==0)&&(temp[right_point+2]==0)) //这里指针变量不能直接和值比较，需要解地址
        {
            break;                            //这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
    return 1;
}
struct Left_Edge
{
    unsigned char row;                        //行坐标，省点内存就没设int
    unsigned char column;                     //列坐标，同上
    unsigned char flag;                       //判断边界点是否找到
};
struct Right_Edge
{
    unsigned char row;                        //行坐标，省点内存就没设int
    unsigned char column;                     //列坐标，同上
    unsigned char flag;                       //判断边界点是否找到
};

struct Left_Edge Left[140];                   //左边界结构体
struct Right_Edge Right[140];                 //右边界结构体
unsigned char Left_Count,Right_Count;         //记录左右边界点的个数
unsigned char grow_left,grow_right;           //记录左右边界在八邻域时寻点的相对位置
unsigned char Left_Max=140,Right_Max=140;     //左右边界搜点时允许最大的搜点量
unsigned char Boundary_search_end=30;         //搜寻行数的最高行


/**
 * @brief 八邻域寻边界
 *
 * @param unchar 必须为经过边框处理Draw_Frame后的图像才可以进行八领域扫线
 * @return 无
 * @exception 1.我默认左下角为（0，0）
 * 2.巡线原理就是找到第一个点，然后找附近的8个点，再选择下一个点来作为衍生，将得到的点记录即可得到边界
 * 3.这里是从第4行的点开始寻找，因为要留一行做起始八邻域点，还有边界两行已经改成黑了
 * 4.生长方向表和坐标表（可以随便定，这里也是一种方法）
 * row-1 col-1      row-1 col       row-1 col+1         7   0   6
 * row   col-1      row   col       row   col+1         4   *   5
 * row+1 col-1      row+1 col       row+1 col+1         3   1   2
 * 5.有些判断会存在grow_left!=是因为这种情况基本不会存在，而且出现的话就难以判断生长的方向
 */
void Image_Get_neighborhoods(unsigned char *image[Image_Hight][Image_With])
{
    unsigned char i;                          //中间变量
    Left_Count=0;                             //左右线最大计数值清0
    Right_Count=0;


    for(i=0;i<Image_With;i++)
    {
        temp[i]=(int)*image[116][i];          //设置一个中间数组，（全局变量），避免参数的传入的问题
    }

    if(Image_Get_LeftFlag())                       //如果找到左边界点的话
    {
        Left[0].row=116;                 //第一个点初始行设为3
        Left[0].column=left_point;       //第一个点的列坐标设为left_point
        Left[0].flag=1;                  //第一个点已经找到
        unsigned char current_row=116;        //初始化当前的行和列坐标
        unsigned char current_column=left_point;  
        grow_left=8;                          //记录上一个生长点相对于现在的生长点的位置，这里设为8不干扰其他点
        for(i=1;i<Left_Max;i++)               //开始找点，最多140个（已经有一个了）
        {
            if((current_row+1<Boundary_search_end)||(current_row>Image_Hight-1))
            {
                break;                        //防止找点找到太高（因为用不上）或者防止出现点又回到原来的最大行（图片最下面那一行），即拐角
            }
            //一 左上黑，正上白，原生长正右（这个不一定），选择向左上方向生长
            if((grow_left!=2)&&(*image[current_row-1][current_column-1]==0)&&(*image[current_row-1][current_column]==1))
            {
                current_row -=1;              //当前的行列坐标全部替换为新的坐标
                current_column -=1;
                Left_Count +=1;               //左边点的计数+1
                grow_left=7;                  //更换为新生长点后，旧生长点相对于原生长点的位置（右下）
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1;          //已经找到下一个生长点
            }
            //二 右上黑，正右白，向右上方向生长
            else if((grow_left!=3)&&(*image[current_row-1][current_column+1]==0)&&(*image[current_row][current_column+1]==1))
            {
                current_row -=1;
                current_column +=1;
                Left_Count +=1;
                grow_left=6;                  //旧生长点相对于新生长点的左下
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1;          //已经找到下一个生长点
            }
            //三 正上黑，右上白，向正上生长
            else if((*image[current_row-1][current_column]==0)&&(*image[current_row-1][current_column+1])==1)
            {
                current_row -=1;
                Left_Count +=1;               //没有current_column的改变就说明不变
                grow_left=0;                  //旧生长点相对于新生长点的正下
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1;          //已经找到下一个生长点
            }
            //四 正左黑，左上白，向左生长
            else if((grow_left!=5)&&(*image[current_row][current_column-1])==0&&(*image[current_row-1][current_column-1]==1))
            {
                current_column -=1;           //向左生长，列坐标-1
                Left_Count +=1;
                grow_left=4;
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1;   
            }
            //五 正右黑，右下白，向正右生长
            else if((grow_left!=4)&&(*image[current_row][current_column+1]==0)&&(*image[current_row+1][current_column+1]==1))
            {
                current_column +=1;
                Left_Count +=1;
                grow_left=5;
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1;  
            }
            //六 左下黑，正左白，向左下生长
            else if((grow_left!=6)&&(*image[current_row+1][current_column-1]==0)&&(*image[current_row][current_column-1]==1))
            {
                current_row +=1;
                current_column -=1;
                Left_Count +=1;
                grow_left =3;
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1;  
            }
            //七 右下黑，正下白，向右下生长
            else if((grow_left!=7)&&(*image[current_row+1][current_column+1]==0)&&(*image[current_row+1][current_column]==1))
            {
                current_row +=1;
                current_column +=1;
                Left_Count +=1;
                grow_left =2;
                Left[i].row=current_row;
                Left[i].column=current_column;
                Left[i].flag=1; 
            }
            else//其他情况不会出现，出现的话就是断线了，就不找了
            {
                break;
            }
        }
        //开始向右找点，原来的temp还是可以继续用（因为是同一行的）
        if(Image_Get_Rightflag())                   //如果找到右边的点的话
        {
            Right[0].row=116;                 //第一个点初始行设为3
            Right[0].column=left_point;       //第一个点的列坐标设为right_point
            Right[0].flag=1;                  //第一个点已经找到
            current_row=116;                  //初始化当前的行和列坐标
            current_column=right_point;  
            grow_right=8;                     //生长起始点为原点
            for(i=0;i<Right_Max;i++)          //开始生长
            {
                if((current_row<Boundary_search_end)||(current_row>(Image_Hight-1))||(current_row+1<Boundary_search_end))
                //防越界
                {
                    break;
                }
                //一 右上黑，正上白（左白右黑），向右上生长
                if((current_column<Image_With)&&(grow_right!=3)&&(*image[current_row-1][current_column+1])&&(*image[current_row-1][current_column]==1))
                {
                    current_row -=1;
                    current_column +=1;
                    Right_Count +=1;
                    grow_right=6;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                //二 左上黑，正左白（上黑下白）
                else if((grow_right!=2)&&(*image[current_row-1][current_column-1]==0)&&(*image[current_row][current_column-1]==1))
                {
                    current_row -=1;
                    current_column -=1;
                    Right_Count +=1;
                    grow_right=7;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                //三 正上黑，左上白（上黑下白）
                else if((*image[current_row-1][current_column]==0)&&(*image[current_row-1][current_column-1]==1))
                {
                    current_row -=1;
                    Right_Count +=1;
                    grow_right=0;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                //四 正右黑，右上白，
                else if((*image[current_row][current_column+1]==0)&&(*image[current_row-1][current_column+1]==1))
                {
                    current_column +=1;
                    Right_Count +=1;
                    grow_right=5;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                //五 正左黑，左下白
                else if((grow_right!=5)&&(*image[current_row][current_column-1]==0)&&(*image[current_row+1][current_column-1]==1))
                {
                    current_column -=1;
                    Right_Count +=1;
                    grow_right=4;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                //六 左下黑，正下白
                else if((grow_right!=6)&&(*image[current_row+1][current_column-1]==0)&&(*image[current_row+1][current_column])==1)
                {
                    current_row +=1;
                    current_column -=1;
                    Right_Count +=1;
                    grow_right=3;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                //七 右下黑，正右白
                else if((grow_right!=7)&&(*image[current_row+1][current_column+1]==0)&&(*image[current_row][current_column+1]==1))
                {
                    current_row +=1;
                    current_column +=1;
                    Right_Count +=1;
                    grow_right=2;
                    Right[i].row=current_row;
                    Right[i].column=current_column;
                    Right[i].flag=1;
                }
                else
                {
                    break;
                }
            }
        }
    }
}

/*-----------------第二版八邻域-------------------*/
//求绝对值函数
int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}
//限幅函数
int16 limit_a_b(int16 x, int a, int b)
{
    if(x<a) x = a;
    if(x>b) x = b;
    return x;
}
//求最小值
int16 limit1(int16 x, int16 y)
{
	if (x > y)             return y;
	else if (x < -y)       return -y;
	else                return x;
}


//简介：寻找边界起始点，放在图像的二值化之后
//注意：返回值为1或0，其中1表示左右线都可以找到，0就是找不到
uint8 start_point_Left[2]={0};                //左边界起点的x,y值
uint8 start_point_right[2]={0};               //右边界起点的x,y
uint8 Image_Get_Start_Point(uint8 start_row) 
{
    uint8 i=0,Left_found=0,Right_found=0;     //中间变量，左右标志位
    start_point_Left[0]=0;
    start_point_Left[1]=0;
    start_point_right[0]=0;
    start_point_right[1]=0;                   //清零

    for(i = Image_With / 2; i > 3; i--)               //3为边界
    {
        start_point_Left[0]=i;
        start_point_Left[1]=start_row;        //x,y分别赋值
        if((Image_Use[start_row][i]==255)&&(Image_Use[start_row][i-1]==0))      //原理和1.0一样
        {
            Left_found = 1;
			break;
        }
    }

    for (i = Image_With / 2; i < 97; i++)
	{
		start_point_right[0] = i;//x
		start_point_right[1] = start_row;//y
		if (Image_Use[start_row][i] == 255 && Image_Use[start_row][i + 1] == 0)
		{
			Right_found = 1;
			break;
		}
	}

    if(Left_found&&Right_found)return 1;      //如果左右线均找到了，就返回1

	else
    {
		return 0;
	}
}

#define USE_num  180                        //定义找点的数组成员个数


 //存放点的x，y坐标
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//左线，其中USE_NUM为左线上点的最大存储值，[2]表示存储的点为x,y坐标
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//右线，其中USE_NUM为右线上点的最大存储值
uint16 dir_r[(uint16)USE_num] = { 0 };//用来存储右边每个点的生长方向，在判断十字环岛有很大的用处
uint16 dir_l[(uint16)USE_num] = { 0 };//用来存储左边每个点生长方向
uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint8 hightest = 0;//在图像中左线和右线交汇的点
//此函数和1.0的区别在于：1.这里使用for循环扫线，上面用的else if扫，这个的效果更好，但是耗时更多
void search_l_r(uint16 break_flag, uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

	uint8 i = 0, j = 0;
    //采取左右巡线对称，左边用顺时针扫，右边用逆时针扫
	//左边变量
	uint8 search_filds_l[8][2] = { {  0 } };//左边八邻域中，中心点四周的8个点的真实坐标（真实坐标=中心坐标+相对坐标）
	uint8 index_l = 0;//左线索引点的下标
	uint8 temp_l[8][2] = { {  0 } };//左线八邻域中，中心点周围8个点的相对坐标（中间存储值）
	uint8 center_point_l[2] = {  0 };//左线生长点的中心坐标
	uint16 l_data_statics;//统计左边生长点的个数
	//生长方向
	static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
	//{-1,-1},{0,-1},{+1,-1},
	//{-1, 0},	     {+1, 0},
	//{-1,+1},{0,+1},{+1,+1},
	//这个是顺时针

	//右边变量
	uint8 search_filds_r[8][2] = { {  0 } };//右边八邻域中，中心点四周的8个点的真实坐标（真实坐标=中心坐标+相对坐标）
	uint8 center_point_r[2] = { 0 };//右线生长点的中心坐标
	uint8 index_r = 0;//右线每个点的索引下标
	uint8 temp_r[8][2] = { {  0 } };//中间存储变量，相当于存储器
	uint16 r_data_statics;//统计右边
	//定义八个邻域
	static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
	//{-1,-1},{0,-1},{+1,-1},
	//{-1, 0},	     {+1, 0},
	//{-1,+1},{0,+1},{+1,+1},
	//这个是逆时针

	l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
	r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

	//第一次更新坐标点  将找到的起点值传进来
	center_point_l[0] = l_start_x;
	center_point_l[1] = l_start_y;
	center_point_r[0] = r_start_x;
	center_point_r[1] = r_start_y;

		//开启邻域循环
	while (break_flag--)//当已经找完目标的点就不再寻扎奥
	{
        //坐标填充，将相对坐标放在邻域里，得到左线每个点的起始坐标（第一步存的是八邻域数组，第二步存的是每个左线点的中心坐标值）
		//左边
		for (i = 0; i < 8; i++)//传递8F坐标
		{
			search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
			search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
		}
		//中心坐标点填充到已经找到的点内
		points_l[l_data_statics][0] = center_point_l[0];//x
		points_l[l_data_statics][1] = center_point_l[1];//y
		l_data_statics++;//索引加一

		//右边，道理同上
		for (i = 0; i < 8; i++)//传递8F坐标
		{
			search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
			search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
		}
		//中心坐标点填充到已经找到的点内
		points_r[r_data_statics][0] = center_point_r[0];//x
		points_r[r_data_statics][1] = center_point_r[1];//y

        //存储左右线数组下标初始化
		index_l = 0;//先清零，后使用
		for (i = 0; i < 8; i++)
		{
			temp_l[i][0] = 0;//先清零，后使用
			temp_l[i][1] = 0;//先清零，后使用
		}
        //二 开始进入for的循环邻域，从中心点的正下方开始扫，左线是顺时针开始扫的
		//左边判断
		for (i = 0; i < 8; i++)
		{
			if (Image_Use[search_filds_l[i][1]][search_filds_l[i][0]] == 0//注意区分横坐标和纵坐标，以及行坐标和列坐标，如果扫到为黑
				&& Image_Use[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)//&7是取余操作，防止i溢出，如果扫到为黑，且沿着顺时针下一个扫到的的点是白
			{
				temp_l[index_l][0] = search_filds_l[(i)][0];    //对先扫到的黑点进行存储，一般选择沿着黑点进行生长
				temp_l[index_l][1] = search_filds_l[(i)][1];    //同时存储x,y点的坐标
				index_l++;//左线记录坐标数组的下标自增
				dir_l[l_data_statics - 1] = (i);//记录生长方向，在0-7里面选择黑点到白点的交界点的黑点的相对方位作为生长方向
                //3     4      5
                //2            6
                //1     0      7
			}
            //如果找不到，下标不会自增，就不会更新坐标点，但是if里面的其实可以试一下放到上面的if里面去
            //如果找到了（因为单次只会找一个点）
			if (index_l)
			{
				//更新坐标点，更新新的中心坐标点
				center_point_l[0] = temp_l[0][0];//x
				center_point_l[1] = temp_l[0][1];//y
                //这个for循环目前看来没有什么必要，因为一般index_r只会在0，1之间发生变化，而且每次执行扫点的时候都会对index_r归零
				for (j = 0; j < index_l; j++)
				{
					if (center_point_l[1] > temp_l[j][1])//如果中心点的横坐标比原来的大，也就是右线点向上生长时
					{
						center_point_l[0] = temp_l[j][0];//x
						center_point_l[1] = temp_l[j][1];//y
					}
				}
			}
		}
        //三 检验所得的点是否符合条件
        //以下各行的意思如下：
        //3.1当检测到该线上的点，上次点，上上次点是同一个x,y坐标时，停止检测（左右线用或来表示）
		if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
			&& points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
			||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
				&& points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
		{
			break;//当连续检测到同一个点的时候就退出for循环
		}
        //3.2当检测到左线和右线的坐标间隔在2之间时，默认两者相交，此时记录相交时对应的最高点
		if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
			&& my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
			)
		{
			*hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点对应的纵坐标（即行数）
			break;//检测到两线相交的时候就没有必要继续扫了
		}
        //3.3当检测到左线的纵坐标比右线的高（也就是右线比左线高的时候），或者右线比左线高——待改进
		if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
		{
			continue;//如果左边比右边高了，左边等待右边
		}
        //3.4这个判断向下生长为什么不是1 0 7呢？待改进
		if (dir_l[l_data_statics - 1] == 7
			&& (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
		{
			center_point_l[0] = points_l[l_data_statics - 1][0];//重置所取点的坐标到上一个，回退操作
			center_point_l[1] = points_l[l_data_statics - 1][1];//y
			l_data_statics--;//索引减小
		}
        //当左线数组的存储数和存储下标减小的时候，这个时候右数组的索引增加，为的是他们能同时寻一个点
		r_data_statics++;//索引加一

        //右线扫描，注意：每当左边扫完一个点，右边会接着扫一个点，而不是左边全部扫完以后右边再扫
        //一 初始化八邻域中间数组坐标
		index_r = 0;//先清零，后使用
		for (i = 0; i < 8; i++)
		{
			temp_r[i][0] = 0;//先清零，后使用
			temp_r[i][1] = 0;//先清零，后使用
		}

		//二 开启八邻域扫第一个点，这是逆时针扫
		for (i = 0; i < 8; i++)
		{
			if (Image_Use[search_filds_r[i][1]][search_filds_r[i][0]] == 0
				&& Image_Use[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
			{
				temp_r[index_r][0] = search_filds_r[(i)][0];//原理和上面一样，虽然写法判断都一样，但是这里逆时针扫，差不多
				temp_r[index_r][1] = search_filds_r[(i)][1];
				index_r++;//索引加一
				dir_r[r_data_statics - 1] = (i);//记录生长方向
				//printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
			}
            //如果找到了（因为单次只会找一个点）
			if (index_r)
			{
				//更新右线的坐标点
				center_point_r[0] = temp_r[0][0];//x
				center_point_r[1] = temp_r[0][1];//y
                //这个for循环目前看来没有什么必要，因为一般index_r只会在0，1之间发生变化，而且每次执行扫点的时候都会对index_r归零
				for (j = 0; j < index_r; j++)
				{
					if (center_point_r[1] > temp_r[j][1])//如果中心点的横坐标比原来的大，也就是右线点向上生长时
					{
						center_point_r[0] = temp_r[j][0];//x
						center_point_r[1] = temp_r[j][1];//y
					}
				}

			}
		}


	}

	//取出循环次数，这里也可以通过用全局变量来代替指针
	*l_stastic = l_data_statics;
	*r_stastic = r_data_statics;

}


/**
 * @brief 大津法求阈值 2.0（由于第一版的bug极其严重，尽力修复也修复不了，暂时就用这版）
 * @param Uint8 输入图像的地址
 * @param Uint8 输出图像的地址
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 * @exception 1. 原理和1.0的是一样的
 * 2. 在二值化时，由于有阈值限幅，要避免大面积的同灰度，这样会使得屏幕闪烁频率增加（这个应该是其他原因造成的），且屏幕会大面积白色或者黑色
 * 3. 如果正常拍摄的话是不会出现太大的问题的，但是还是要在跑道上测试一下
 * 4. 2.0的方法相对于1.0的方法区别在于：不用求二次高峰再求出低谷，直接通过最大值的灰度像素点和最小值来求出1.0中想要的低谷（即low1_position）
 *                                   而1.0就是在寻找二次高峰出现了bug，2.0的方法相对来说更科学一点
 */
//uint8 GetThreshold(void)   //注意计算阈值的一定要是原图像
//{
//#define GrayScale 256				          //定义灰度总值
//	uint8 last_threshold;                     //定义上次的阈值，进行阈值的输出限幅
//    int Pixel_Max=0;                          //定义灰度值的最大值和最小值（0-255）
//    int Pixel_Min=255;
//    uint16 width = Image_With;                //定义图像的宽和高
//    uint16 height = Image_Hight;
//    int pixelCount[GrayScale];                //记录每个灰度对应的像素值
//    float pixelPro[GrayScale];                //定义每个像素点在图中的比例的数组
//    int i, j, pixelSum = width * height/4;    //定义中间变量，定义像素总数（这里只要算1/4的就行）
//    uint8 threshold = 0;                      //定义返回阈值
//    uint8* data = *Image_Use;                 //指向像素数据的指针，省内存
//    for (i = 0; i < GrayScale; i++)           //初始化两个灰度值像素数组
//    {
//        pixelCount[i] = 0;
//        pixelPro[i] = 0;
//    }                            
//    uint32 gray_sum=0;                        //定义所有灰度值的总数
//    //一 统计灰度级中每个像素在整幅图像中的个数
//    for (i = 0; i < height; i+=2)             
//    {
//        for (j = 0; j < width; j+=2)
//        {
//            pixelCount[(int)data[i * width + j]]++;                                 //将当前的点的像素值作为计数数组的下标
//            gray_sum+=(int)data[i * width + j];                                     //求灰度值总和
//            if(data[i * width + j]>Pixel_Max)   Pixel_Max=data[i * width + j];      //同时记录直方图中灰度值的最高峰和最低峰
//            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
//        }
//    }
//    //二 计算每个像素值的点在整幅图像中的比例
//    for (i = Pixel_Min; i < Pixel_Max; i++)     
//    {
//        pixelPro[i] = (float)pixelCount[i] / pixelSum;
// 
//    }
//    //三 遍历灰度级[0,255]，开始寻找
//    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//    //w0：后景所占总图片的总比例        w1:前景所占总图片的总比例   有关系式：（即阈值以下为w0,阈值以上为w1）w1+w0=1
//    //u0tmp：背景部分灰度值点的比例*灰度值          u1tmp：前景部分灰度值点的比例*灰度值  （后面有解析）
//    //u0,u1：背景，前景的平均灰度   u：全局平均灰度 u=(u1+u2)/2
//    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//    for (j = Pixel_Min; j < Pixel_Max; j++)
//    {
// 
//        w0 += pixelPro[j];                    //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
//        u0tmp += j * pixelPro[j];             //背景部分每个灰度值的点的比例 *灰度值
// 
//        w1=1-w0;
//        u1tmp=gray_sum/pixelSum-u0tmp;
// 
//        u0 = u0tmp / w0;                      //背景平均灰度
//        u1 = u1tmp / w1;                      //前景平均灰度
//        u = u0tmp + u1tmp;                    //全局平均灰度
//        deltaTmp = (float)(w0 *w1* (u0 - u1)* (u0 - u1)) ;
//        if (deltaTmp > deltaMax)
//        {
//            deltaMax = deltaTmp;
//            threshold = j;
//        }
//        if (deltaTmp < deltaMax)
//        {
//            break;
//        }
// 
//    }
//    //四 阈值限幅，如果超过想要的阈值，就会和上次一样
//    if(threshold>50 && threshold<130)
//        last_threshold = threshold;
//    else
//        threshold = last_threshold;
//	
//    return threshold;
//}