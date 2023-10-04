#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define WHITE 255
#define BLACK 0
#define IMAGE_WIDTH 160          // 用于处理的图像高度
#define IMAGE_HEIGHT 120          //用于处理的图像宽度\

#define Primeval_Hight MT9V03X_H // 原始图像高度
#define Primeval_With MT9V03X_W  // 原始图像宽度

#define THRESHOLD_MAX 255*6         //滤波阈值
#define THRESHOLD_MIN 255*3

#define LINE_K 0.5 //斜率，判断是否为直道
extern uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH]; // 全局声明用于处理的图像数组

/*图像处理*/
void Image_Compress();                             // 对原始图像进行压缩
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height);//优化大津法获取阈值
void Image_Sobel(uint8 Image_in[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t Image_out[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 Threshold); // 全局sobel方案
void Image_Binarization(unsigned char threshold, uint8 (*Image_Use)[IMAGE_WIDTH]); // 根据阈值对图像进行二值化
void Image_DrawRectangle(void);//画黑框
void Image_Get_neighborhoods(uint8 (*Image_Use)[IMAGE_WIDTH]);//八邻域巡线
void Image_Filter(void);//腐蚀滤波函数（简单的早点过滤）
uint8 Image_Get_RightPoint(uint8 start_row);//求左边界起始点坐标函数
uint8 Image_Get_LeftPoint(uint8 start_row);//求右边界起始点坐标函数
void Image_blur_points_Left(int num,int kernel);//三角滤波左边线
void Image_blur_points_Right(int num,int kernel);//三角滤波右边线

/*辅助计算*/
float Image_ab_value(float a,float b);//求浮点型的绝对值
float Image_Getk(int8 start_y,int8 end_y,int8 interval);//简单计算直线斜率
int abs_int(int a, int b);//求两整型绝对值
int min(int a, int b);//求两整型最小值
int Image_LeftGrowDirection(uint8 end,uint8 Direction);//计算左边线中生长某方向的总个数
int Image_RightGrowDirection(uint8 end,uint8 Direction);//计算右边线中生长某方向的总个数
uint8 Image_Scan_Row(uint8(*Image_Use)[IMAGE_WIDTH],uint8 target_row);//扫某行的黑白跳变点（斑马线判断）
uint8 Image_Scan_Column(uint8(*Image_Use)[IMAGE_WIDTH],uint8 target_column);//扫某列的黑白跳变点（斑马线判断）
void Image_pointsleft(uint8 x1,uint8 y1,uint8 x2,uint8 y2);//两点坐标求斜率和截距
void Image_pointsright(uint8 x1,uint8 y1,uint8 x2,uint8 y2);//两点坐标求斜率和截距（存的数组不一样）
float Imgae_SlopeLeft(uint8 begin,uint8 end);//最小二乘法求左边线斜率
void Image_CountLeftKB_L(uint8 start,uint8 end);

/*元素*/
void Image_FillCross(uint8(*Image_Use)[IMAGE_WIDTH]);//十字
uint8 Image_Stretch_Left(void);//左直道元素判断
uint8 Image_Stretch_Right(void);//右直道元素判断
void Image_Ramp(void);//坡道判断
void Image_LeftRound(uint8(*Image_Use)[IMAGE_WIDTH]);//左环岛判断补线

/*运行*/
void Image_Run(void);//图像处理主函数

#endif
