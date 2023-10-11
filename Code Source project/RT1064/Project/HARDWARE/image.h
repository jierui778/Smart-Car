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
<<<<<<< Updated upstream
=======
void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel);
void Pespective(int pts_in[][2], int int_num, float pts_out[][2]);
void Get_Midline(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num);
float Get_err1(int pts_in[][2], int num);
float LineRession(int pts_in[][2], int num);
void Get_Midline2(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num);

// W矩阵参数（原图转化成逆透视后图像的参数）
// 60*80
//#define a11 (-4.3801f)
//#define a12 (-0.0293f)
//#define a13 (347.5647f)
//#define a21 (0.0782f)
//#define a22 (3.2241f)
//#define a23 (-476.1463f)
//#define a31 (0.0070f)
//#define a32 (-0.1864f)
//#define a33 (1.0f)

//#define a11 (-0.238288f)
//#define a12 (0.210738f)
//#define a13 (-15.705617f)
//#define a21 (0.005936f)
//#define a22 (0.010945f)
//#define a23 (-8.492773f)
//#define a31 (0.000212f)
//#define a32 (0.002450f)
//#define a33 (-0.433058f)//效果较好




//#define a11 (-0.304968f)
//#define a12 (0.321069f)
//#define a13 (-16.977831f)
//#define a21 (0.000000f)
//#define a22 (0.032146f)
//#define a23 (-12.074271f)
//#define a31 (0.0000f)
//#define a32 (0.004067f)
//#define a33 (-0.527594f)

//#define a11 (0.494521f)
//#define a12 (-0.235086f)
//#define a13 (-3.302742f)
//#define a21 (0.023865f)
//#define a22 (0.004361f)
//#define a23 (3.304021f)
//#define a31 (0.000249f)
//#define a32 (-0.002777f)
//#define a33 (0.412018f)

#define a11 (-1.9446f)
#define a12 (-0.0768f)
#define a13 (152.9928f)
#define a21 (-0.0901f)
#define a22 (0.4470f)
#define a23 (-190.8998f)
#define a31 (-0.0030f)
#define a32 (-0.0846f)
#define a33 (1.0f)















#define getx(u, v) (a11 * (u) + a12 * (v) + a13)
#define gety(u, v) (a21 * (u) + a22 * (v) + a23)
#define getw(u, v) (a31 * (u) + a32 * (v) + a33)





// D矩阵参数
/*这些宏定义都是给60*80的矩阵*/
#define b11 (-0.1238f)
#define b12 (-0.0969f)
#define b13 (-1.0252f)

#define b21 (0.0000f)

#define b22 (-0.0097f)

#define b23 (-4.2726f)

#define b31 (-0.0000f)
#define b32 (-0.0023f)
#define b33 (-0.0271f)


#define getx_b(u, v) (b11 * (u) + b12 * (v) + b13)
#define gety_b(u, v) (b21 * (u) + b22 * (v) + b23)
#define getw_b(u, v) (b31 * (u) + b32 * (v) + b33)
>>>>>>> Stashed changes




void ImagePerspective_Init(void);


void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist);

void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
#endif
