#include "bmp.h"


/**
 * @brief 获取我们所需要大小的图片
 *
 */
void BMP_Get()
{
    static const uint32_t Y_OFFSET = 0;//Y轴偏移量传入原图像的高度
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
void BMP_GetBinaryImage(OSG_IN Uint8 (*InImg)[IMGW], OSG_OUT Uint8 (*OutImg)[IMGW], OSG_IN Uint8 Threshold)
{


}