#ifndef _IMAGE_H
#define _IMAGE_H

#define CAMERA_WITH 188
#define CAMERA_HEIGHT 120

unsigned char first_image[CAMERA_HEIGHT][CAMERA_WITH];//第一次经过DMA采集得到的图像，灰度值为0-255（0为黑，255为白）

void BMP_Get();//获取图片
void BMP_Binarization(void);//二值化
unsigned char BMP_GetThreshold(unsigned char *image[CAMERA_HEIGHT][CAMERA_WITH]);


#endif
