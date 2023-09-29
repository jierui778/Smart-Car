#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define white 255
#define black 0
#define Image_Width 100            // 用于处理的图像高度
#define Image_Height 60           //用于处理的图像宽度\

#define Primeval_Hight MT9V03X_H // 原始图像高度
#define Primeval_With MT9V03X_W  // 原始图像宽度

#define threshold_max 255*6         //滤波阈值
#define threshold_min 255*2


void Image_Compress();
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height);
extern uint8 Image_Use[Image_Height][Image_Width]; // 先定义灰度直方图
void Image_Binarization(unsigned char threshold,uint8(*Image_Use)[Image_Width]);
void Image_DrawRectangle(void);
void Image_Run(void);
void Image_Get_neighborhoods(uint8(*Image_Use)[Image_Width]);
#endif
