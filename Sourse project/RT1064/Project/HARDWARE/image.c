#include "image.h"
#include "stdlib.h"
//以下是八邻域的寻找边界的算法
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
unsigned int temp[188];
/**
 * @brief 获取我们所需要大小的图片
 *
 */
void BMP_Get()
{

    // 通常我们在一张188*120的图像上截取我们所需大小的图像
    // 这里的Y轴偏移量就是我们截取的图像的高度
    //	// //如果是原图,直接使用拷贝函数即可
    ////	  OSG_Memcpy((Uint8*)OutImg,t_pInImg,IMGH*IMGW);

}


/**
 * @brief 将输入的灰度图像转化为二值化图像
 *
 * @param Uint8 输入图像的地址
 * @param Uint8 输出图像的地址
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */


/**
 * @brief 大津法求阈值（和其他的版本有些出入）
 *
 * @param Uint8 *image 第一次原始图像（灰度为0-255，有188*120个像素点的图像）
 * @return 求得的阈值
 * @exception 场地得到的灰度图像一般颜色不一，但是图像里面占的面积最大的灰度肯定只有赛道内的灰度值A1和赛道外的灰度值A2，因此得到的直方图会在A1和A2
 * 出现两个峰值，而所求的阈值就是两个峰值之间的最低值
 */
unsigned char BMP_GetThreshold(unsigned char *image[CAMERA_HEIGHT][CAMERA_WITH])
{
    unsigned int i,j;                        //用来遍历灰度直方图
    unsigned int High1=0;                     //第一高峰
    unsigned char High1_Position=0;           //第一高峰对应的灰度值
    unsigned int High2=0;                     //第二高峰
    unsigned char High2_Position=0;           //第二高峰对应的灰度值
    unsigned char HistoGram[256];             //先定义灰度直方图
    unsigned char *gray;                      //灰度值（中间值）
    unsigned int High_temp;                   //高度中间值
    unsigned int Low1=20000;                  //定义低谷1
    unsigned char Low1_Position;              //定义低谷1的灰度值位置，即所求的阈值
    for(i=0;i<CAMERA_HEIGHT;i++)              //先遍历灰度值第一高峰
    {
        for(j=0;j<CAMERA_WITH;j++)
        {
            gray=image[i][j];                 //指针传递
            HistoGram[*gray]++;               //gray解码传值，得到的HistoGram[256]即为灰度直方图
        }
    }
    for(i=0;i<256;i++)
    {
        if(HistoGram[i]>High1)
        {
            High1=HistoGram[i];               //遍历所有点，存储最高峰的值
            High1_Position=i;                 //存储当前最高峰值的灰度值
        }
    }
    for(High_temp=High1-5;High_temp>0;High_temp-=5)//从第一高峰开始向下寻找第二高峰
    {
        for(i=0;i<256;i++)                    //在高度值固定时，对灰度值进行遍历
        {
            if((HistoGram[i]>High_temp)&&(i-High1_Position)>30)    //防止找到最高峰旁边的点
            {
                High2=HistoGram[i];           //记录次高峰峰值（会有0-5内的误差）
                High2_Position=i;             //记录次高峰峰值对应的灰度值
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
 * @brief 图像加框，防止八邻域扫到附近的点，将边界上的点的灰度值设为0
 *
 * @param Uint8 *image 第一次原始图像（灰度为0-255，有188*120个像素点的图像）
 * @return 无
 * @exception 无
 */
void Draw_Frame(unsigned char *picture[CAMERA_HEIGHT][CAMERA_WITH])
{
    unsigned char i;                          //用来循环的变量
    for(i=0;i<CAMERA_HEIGHT;i++)
    {
        picture[i][0]=0;
        picture[i][1]=0;
        picture[i][CAMERA_WITH-1]=0;
        picture[i][CAMERA_WITH-2]=0;          //图片的旁边的两列全设为黑点
    }
    for(i=0;i<CAMERA_WITH;i++)
    {
        picture[0][i]=0;
        picture[1][i]=0;                      //图片上下两列全设为黑点（不用搞角落，角落已经搞过了）
    }
}

/**
 * @brief 寻找是否存在左边界（不用，参数问题比较难处理，就换个函数）
 * @param Uint8 *image 二值化处理过后的图像，只扫一行，且扫离图片最下面的没有被清除的那一行，
 * @return 1（找到的话就回1）
 * @exception 1. 从第二行开始扫，因为边界两行都扫没了（但是不知道0从哪开始扫起，这里默认0从左下角开始算了）
 * 2.这里传入的参数也可以简化运行的速度（只传一行的数据，因为只要扫一行）
 * 3.这个函数有个缺陷就是，可能第二行全黑（全白），这样的话就扫不到了——不过一般不可能吧？如果有的话就把行数加1行就行了
 
unsigned char left_point;                     //记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
unsigned char Get_LeftFlag(unsigned int *image3[1][CAMERA_WITH])
{
    for(left_point=(CAMERA_WITH/2);left_point>0;left_point--)
    {
        if((*image3[1][left_point]==1)&&(*image3[1][left_point-1]==0)&&(*image3[1][left_point-2]==0))
        {
            break;
        }
    }
    return 1;
}
//
 * @brief 将输入的灰度图像转化为二值化图像
 *
 * @param Uint8 输入图像的地址
 * @param Uint8 输出图像的地址
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
unsigned char left_point;                     //记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
unsigned char Get_LeftFlag(void)
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
unsigned char Get_Rightflag(void)
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
void Get_neighborhoods(unsigned char *image[CAMERA_HEIGHT][CAMERA_WITH])
{
    unsigned char i;                          //中间变量
    Left_Count=0;                             //左右线最大计数值清0
    Right_Count=0;


    for(i=0;i<CAMERA_WITH;i++)
    {
        temp[i]=(int)*image[116][i];          //设置一个中间数组，（全局变量），避免参数的传入的问题
    }

    if(Get_LeftFlag())                       //如果找到左边界点的话
    {
        Left[0].row=116;                 //第一个点初始行设为3
        Left[0].column=left_point;       //第一个点的列坐标设为left_point
        Left[0].flag=1;                  //第一个点已经找到
        unsigned char current_row=116;        //初始化当前的行和列坐标
        unsigned char current_column=left_point;  
        grow_left=8;                          //记录上一个生长点相对于现在的生长点的位置，这里设为8不干扰其他点
        for(i=1;i<Left_Max;i++)               //开始找点，最多140个（已经有一个了）
        {
            if((current_row+1<Boundary_search_end)||(current_row>CAMERA_HEIGHT-1))
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
        if(Get_Rightflag())                   //如果找到右边的点的话
        {
            Right[0].row=116;                 //第一个点初始行设为3
            Right[0].column=left_point;       //第一个点的列坐标设为right_point
            Right[0].flag=1;                  //第一个点已经找到
            current_row=116;                  //初始化当前的行和列坐标
            current_column=right_point;  
            grow_right=8;                     //生长起始点为原点
            for(i=0;i<Right_Max;i++)          //开始生长
            {
                if((current_row<Boundary_search_end)||(current_row>(CAMERA_HEIGHT-1))||(current_row+1<Boundary_search_end))
                //防越界
                {
                    break;
                }
                //一 右上黑，正上白（左白右黑），向右上生长
                if((current_column<CAMERA_WITH)&&(grow_right!=3)&&(*image[current_row-1][current_column+1])&&(*image[current_row-1][current_column]==1))
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