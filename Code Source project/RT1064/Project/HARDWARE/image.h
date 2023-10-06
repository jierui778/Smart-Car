#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define white 255
#define black 0
#define IMAGE_WIDTH 160           // 用于处理的图像高度
#define IMAGE_HEIGHT 120          //用于处理的图像宽度\

#define PRIMEVAL_HEIGHT MT9V03X_H // 原始图像高度
#define PRIMEVAL_WIDTH MT9V03X_W  // 原始图像宽度

#define threshold_max 255 * 6 // 滤波阈值
#define threshold_min 255 * 2

#define Line_k 0.5 // 斜率，判断是否为直道
extern uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH];
extern uint8 Image_Use_Robert[IMAGE_HEIGHT][IMAGE_WIDTH]; // 全局声明用于处理的图像数组
void Image_Compress();                                    // 对原始图像进行压缩
void compressimage();
void halve_image(unsigned char *p_in, unsigned char *p_out, unsigned char row, unsigned char col);
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height); // 优化大津贴法获取阈值

void Image_Sobel(uint8 Image_in[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t Image_out[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 Threshold); // 全局sobel方案

void Image_Binarization(unsigned char threshold, uint8 (*Image_Use)[IMAGE_WIDTH]); // 根据阈值对图像进行二值化

void Image_FillCross(uint8 *l_border, uint8 *r_border, uint16 total_num_l, uint16 total_num_r,
                     uint16 *dir_l, uint16 *dir_r, uint16 (*points_l)[2], uint16 (*points_r)[2]);
void Image_DrawRectangle(void);
void Image_Get_neighborhoods(uint8 (*Image_Use)[IMAGE_WIDTH]);
void Image_Run(void);                                                   // 图像处理主函数
void CannyEdgeTest(uint8 org[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 lowThr); // CANNY边缘检测代码，目前不准备使用
float Image_ab_value(float a, float b);

typedef struct img
{
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t step; // 走过路径的长度
} image_t;
#define AT_IMAGE(img, x, y) ((img)->data[(y) * (img)->step + (x)])                                          // 访问图像像素 二维数组转换一维数组
#define AT_IMAGE_CLIP(img, x, y) AT_IMAGE(img, clip(x, 0, (img)->width - 1), clip(y, 0, (img)->height - 1)) // 防止访问超出边界

#define DEF_IMAGE(ptr, w, h)                            \
    {                                                   \
        .data = ptr, .width = w, .height = h, .step = w \
    } // 定义图像结构体基本信息
#define ROI_IMAGE(img, x1, y1, w, h)                                               \
    {                                                                              \
        .data = &AT_IMAGE(img, x1, y1), .width = w, .height = h, .step = img.width \
    }
extern image_t img_raw;
void Left_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);


void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);


void Find_Borderline(void);

void test(void);

#endif
