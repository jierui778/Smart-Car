#ifndef _IMAGE_H
#define _IMAGE_H

<<<<<<< HEAD


#define IN
#define OUT
#define IN_OUT




void Image_Get();//获取图片
void Image_
void Image_Binarization(void);//二值化

#endif
=======
#define CAMERA_WITH 188
#define CAMERA_HEIGHT 120

unsigned char first_image[CAMERA_HEIGHT][CAMERA_WITH];//第一次经过DMA采集得到的图像，灰度值为0-255（0为黑，255为白）

void BMP_Get();//获取图片
void BMP_Binarization(void);//二值化
unsigned char BMP_GetThreshold(unsigned char *image[CAMERA_HEIGHT][CAMERA_WITH]);


#endif
>>>>>>> 5d9b1137c6d9d75c82f1252f159abafceb94fc34
