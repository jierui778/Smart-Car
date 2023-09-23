#include "image.h"



uint8 Image_Use[Image_Hight][Image_With];
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

//  {
//      int i, j, row, line;
//      const float pro_h = Primeval_Hight / Image_Hight, pro_w = Primeval_With / Image_With; // 120/60=2 188/100=1.88
//      for (i = 0; i < Image_Hight; i++)                                                     // 遍历图像的每一行，从第零行到第59行。
//      {
//          row = ((int)i * (int)pro_h) +0.5;
//          for (j = 0; j < Primeval_With; j++) // 遍历图像的每一列，从第零列到第79列。
//          {
//              line = (int)j * (int)pro_w +0.5;
//              Image_Use[i][j] = mt9v03x_image[row][line]; // mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
//          }
//      }
//      mt9v03x_finish_flag = 0;
//  }

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
 unsigned int BMP_GetThreshold(void)
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
 void Binarization(unsigned char threshold)
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

//此函数在原函数的基础上，优化了阈值：在图像出现大面积一个颜色时，此时直方图中只有一个高峰（或者两个高峰很接近），这里就直接选择最高峰对应的灰度值作为灰度值传输阈值
uint8 GetThreshold(void)   //注意计算阈值的一定要是原图像
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
        threshold = last_threshold;

	//防止强光，出现单高峰时，就选择该单高峰的灰度值作为阈值传输
	if(threshold==0)
	{
		threshold=Pixel_Max_Position;
	}
    return threshold;
}


///**
// * @brief 将输入的灰度图像转化为二值化图像
// *
// * @param Uint8 输入图像的地址
// * @param Uint8 输出图像的地址
// * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
// */
// void Image_GetBinary(IN Uint8 (*InImg)[IMGW], OUT Uint8 (*OutImg)[IMGW], IN Uint8 Threshold)

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

//此函数在原函数的基础上，优化了阈值：在图像出现大面积一个颜色时，此时直方图中只有一个高峰（或者两个高峰很接近），这里就直接选择最高峰对应的灰度值作为灰度值传输阈值
//uint8 GetThreshold(void)   //注意计算阈值的一定要是原图像
//{
//#define GrayScale 256				          //定义灰度总值
//	uint8 last_threshold;                     //定义上次的阈值，进行阈值的输出限幅
//    int Pixel_Max=0;                          //定义灰度值的最大值和最小值（0-255）
//    int Pixel_Min=255;
//	int Pixel_Max_Position;					  //灰度值最高峰对应的灰度值
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
//            if(data[i * width + j]>Pixel_Max)
//			{
//				Pixel_Max=data[i * width + j]; 										//同时记录直方图中灰度值的最高峰和最低峰
//				Pixel_Max_Position=i * width + j;
//			}
//            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
//        }
//    }
//    //二 计算每个像素值的点在整幅图像中的比例
//    for (i = Pixel_Min; i < Pixel_Max; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / pixelSum;

//    }
//    //三 遍历灰度级[0,255]，开始寻找
//    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//    //w0：后景所占总图片的总比例        w1:前景所占总图片的总比例   有关系式：（即阈值以下为w0,阈值以上为w1）w1+w0=1
//    //u0tmp：背景部分灰度值点的比例*灰度值          u1tmp：前景部分灰度值点的比例*灰度值  （后面有解析）
//    //u0,u1：背景，前景的平均灰度   u：全局平均灰度 u=(u1+u2)/2
//    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//    for (j = Pixel_Min; j < Pixel_Max; j++)
//    {

//        w0 += pixelPro[j];                    //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
//        u0tmp += j * pixelPro[j];             //背景部分每个灰度值的点的比例 *灰度值

//        w1=1-w0;
//        u1tmp=gray_sum/pixelSum-u0tmp;

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

//    }
//    //四 阈值限幅，如果超过想要的阈值，就会和上次一样
//    if(threshold>80 && threshold<130)
//        last_threshold = threshold;
//    else
//        threshold = last_threshold;

//	//防止强光，出现单高峰时，就选择该单高峰的灰度值作为阈值传输
//	if(threshold==0)
//	{
//		threshold=Pixel_Max_Position;
//	}
//    return threshold;
//}



