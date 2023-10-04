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
float Image_Getk(int16 derta_column,int16 derta_row);//简单计算直线斜率
float Image_Getb(int16 example_column,int16 example_row,float k);//简单计算截距
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


/*
学到的思路：

十字：
1. 十字处理中，如何判断为十字状态？（只要有拐点，且两个拐点的行坐标相差不大即为十字
同时设置标志位far_Lpt0_found，便于判断）
2. 十字处理中，如何补线？（未进十字时，在找到下拐点的时候，再往前找2个拐点，取斜率补线即可）
（若已经进入十字，则在找到下拐点的时候，可以取上拐点往后的2个拐点进行斜率补线（或者找下拐点进行补线））
3. 十字处理的时候设置一个状态机：未进十字（检测左右拐点且行坐标相差不大）—（左右拐点坐标不断减小最终为0）—进十字（出现两个上下拐点）
4. ***通过编码器测距求出跑1m后判断十字结束，直接退出十字状态

车库：
1. 扫行，看黑点有多少个（或黑白跳变点）sobel的话有120个黑点
2. 转弯时通过偏航角的角度变化来判断出库
新思路：
1. 在十字中，如何找到上拐点？
——for循环嵌套遍历，求出每一点与左下角（120,0）的直线的斜率，当斜率最大时：便是上拐点
注：在未进十字时，上拐点和下拐点的坐标是一样的，当上拐点和下拐点的行坐标相差较大时，就说明已经处在十字的中央

明天要干的：
1. 通过斜率最大法测出上拐点
2. 十字不同状态的补线
3. 判断十字的不同状态
4. 弯道函数判断
*/