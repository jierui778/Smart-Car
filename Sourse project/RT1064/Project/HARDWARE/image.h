#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H



#define IN
#define OUT
#define IN_OUT

#define IMAGE_WITH 60//用于处理的图像高度
#define IMAGE_HIGHT 80//用于处理的图像宽度

void Image_Get();//获取图片
void Image_
void Image_Binarization(void);//二值化



int8 first_image[CAMERA_HEIGHT][CAMERA_WITH];//第一次经过DMA采集得到的图像，灰度值为0-255（0为黑，255为白）




#endif
