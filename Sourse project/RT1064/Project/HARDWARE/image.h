#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define Image_Width 100            // 用于处理的图像高度
#define Image_Height 60           //用于处理的图像宽度\

#define Primeval_Hight MT9V03X_H // 原始图像高度
#define Primeval_With MT9V03X_W  // 原始图像宽度

<<<<<<< HEAD
extern uint8 Image_Use[Image_Hight][Image_With];          //先定义灰度直方图
=======
void Image_Compress();
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height);
extern uint8 Image_Use[Image_Height][Image_Width]; // 先定义灰度直方图
void Binarization(uint8 threshold);
// uint8 GetThreshold(void); // 注意计算阈值的一定要是原图像
>>>>>>> 4f9af65e17217bbcca95ffe5c6252049ca288b59

void Image_Compress();
unsigned int Image_GetThreshold1(void);
void Image_Binarization(unsigned char threshold);
uint8 Image_GetThreshold(void);   //注意计算阈值的一定要是原图像
void Image_Draw_Frame(unsigned char *picture[Image_Hight][Image_With]);


#endif
