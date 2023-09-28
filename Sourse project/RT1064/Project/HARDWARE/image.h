#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define white 255
#define black 0
#define Image_Width 100          // 用于处理的图像高度
#define Image_Height 60          //用于处理的图像宽度\

#define Primeval_Hight MT9V03X_H // 原始图像高度
#define Primeval_With MT9V03X_W  // 原始图像宽度

extern uint8 Image_Use[Image_Height][Image_Width]; // 全局声明用于处理的图像数组
void Image_Compress();                             // 对原始图像进行压缩
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height);
// uint8 OSTU_GetThreshold(uint8 *image, uint16 col, uint16 row);

void Image_Sobel(uint8 Image_in[Image_Height][Image_Width], uint8_t Image_out[Image_Height][Image_Width], uint16 Threshold); // 全局sobel方案

void Image_Binarization(unsigned char threshold, uint8 (*Image_Use)[Image_Width]); // 根据阈值对图像进行二值化

void Image_FillCross(uint8 *l_border, uint8 *r_border, uint16 total_num_l, uint16 total_num_r,
                     uint16 *dir_l, uint16 *dir_r, uint16 (*points_l)[2], uint16 (*points_r)[2]);
void Image_DrawRectangle(void);
void Image_Run(void);
void Image_Get_neighborhoods(uint8 (*Image_Use)[Image_Width]);
#endif
