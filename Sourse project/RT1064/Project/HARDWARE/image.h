#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define Image_With 100            // 用于处理的图像高度
#define Image_Hight 60           //用于处理的图像宽度\

#define Primeval_Hight MT9V03X_H // 原始图像高度
#define Primeval_With MT9V03X_W  // 原始图像宽度

void Image_Compress();
unsigned int BMP_GetThreshold(void);
extern uint8 Image_Use[Image_Hight][Image_With];          //先定义灰度直方图
void Binarization(unsigned char threshold);
uint8 GetThreshold(void);   //注意计算阈值的一定要是原图像

// void Image_Get();//获取图片
//// void Image_
// void Image_Binarization(void);//二值化

// int8 first_image[CAMERA_HEIGHT][CAMERA_WITH];//第一次经过DMA采集得到的图像，灰度值为0-255（0为黑，255为白）

#endif
