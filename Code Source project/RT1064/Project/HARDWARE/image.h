#include "zf_common_headfile.h"
#include "mymath.h"
#include "cross.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define WHITE 255
#define BLACK 0
#define IMAGE_WIDTH 160           // 用于处理的图像高度
#define IMAGE_HEIGHT 120          //用于处理的图像宽度\

#define PRIMEVAL_HEIGHT MT9V03X_H // 原始图像高度
#define PRIMEVAL_WIDTH MT9V03X_W  // 原始图像宽度

#define THRESHOLD_MAX 255 * 6 // 滤波阈值
#define THRESHOLD_MIN 255 * 3

#define LINE_K 0.5                                 // 斜率，判断是否为直道
extern uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH]; // 全局声明用于处理的图像数组
extern float Finnal_err;
/*图像处理*/
void Image_Compress();                                                                                                       // 对原始图像进行压缩
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height);                                                          // 优化大津法获取阈值
void Image_Sobel(uint8 Image_in[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t Image_out[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 Threshold); // 全局sobel方案
void Image_Binarization(unsigned char threshold, uint8 (*Image_Use)[IMAGE_WIDTH]);                                           // 根据阈值对图像进行二值化
void Image_DrawRectangle(void);                                                                                              // 画黑框
void Image_Get_neighborhoods(uint8 (*Image_Use)[IMAGE_WIDTH]);                                                               // 八邻域巡线
void Image_Filter(void);                                                                                                     // 腐蚀滤波函数（简单的早点过滤）
uint8 Image_Get_RightPoint(uint8 start_row);                                                                                 // 求左边界起始点坐标函数
uint8 Image_Get_LeftPoint(uint8 start_row);                                                                                  // 求右边界起始点坐标函数
void Image_blur_points_Left(int num, int kernel);                                                                            // 三角滤波左边线
void Image_blur_points_Right(int num, int kernel);                                                                           // 三角滤波右边线
void Coordinate_transformation_rightup(int pt0_in[][2], int in_num, int pt0_out[][2]);


/*辅助计算*/
float Image_ab_value(float a, float b);                                        // 求浮点型的绝对值
float Image_Getk(int16 derta_column, int16 derta_row);                         // 简单计算直线斜率
float Image_Getb(int16 example_column, int16 example_row, float k);            // 简单计算截距
int abs_int(int a, int b);                                                     // 求两整型绝对值
int min(int a, int b);                                                         // 求两整型最小值
int Image_LeftGrowDirection(uint8 end, uint8 Direction);                       // 计算左边线中生长某方向的总个数
int Image_RightGrowDirection(uint8 end, uint8 Direction);                      // 计算右边线中生长某方向的总个数
uint8 Image_Scan_Row(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_row);       // 扫某行的黑白跳变点（斑马线判断）
uint8 Image_Scan_Column(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_column); // 扫某列的黑白跳变点（斑马线判断）
void Image_pointsleft(uint8 x1, uint8 y1, uint8 x2, uint8 y2);                 // 两点坐标求斜率和截距
void Image_pointsright(uint8 x1, uint8 y1, uint8 x2, uint8 y2);                // 两点坐标求斜率和截距（存的数组不一样）
float Imgae_SlopeLeft(uint8 begin, uint8 end);                                 // 最小二乘法求左边线斜率
void Image_CountLeftKB_L(uint8 start, uint8 end);
float mySqrt(float x); // 计算一个浮点数的平方根

void run_cicrle_right_in(void);


/*运行*/
void Image_Run(void); // 图像处理主函数

extern uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH];
extern uint8 Image_Use_Robert[IMAGE_HEIGHT][IMAGE_WIDTH]; // 全局声明用于处理的图像数组
void compressimage();
// void halve_image(unsigned char *p_in, unsigned char *p_out, unsigned char row, unsigned char col);
// void CannyEdgeTest(uint8 org[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 lowThr); // CANNY边缘检测代码，目前不准备使用

typedef struct img
{
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t step; // 走过路径的长度
} image_t;

enum track_type_e
{
    TRACK_LEFT_MID,
    TRACK_LEFT,
    TRACK_RIGHT,
    TRACK_RIGHT_MID,
    TRACK_NONE,
};
extern enum track_type_e track_type;
/*这个得放在车库的.c现在还没建成*/

#define LLL 60

extern int16_t garage_cnt;

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
void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value);                                          // 两点画线
void draw_line2(float pt0[2], float pt1[2], float pts_out[][2], int *num, float dist);                        // 逆透视等距采样
void SplicingArray(float pt0[][2], int num1, float pt1[][2], int num2, float pt_out[][2], int *num, uint8 x); // 数组拼接
void test(void);
void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel);
void Pespective(int pts_in[][2], int int_num, float pts_out[][2]);
void Get_Midline(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num);
float Get_err1(int pts_in[][2], int num);
float LineRession(int pts_in[][2], int num);
void Get_Midline2(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num);
void Find_Borderline_Second(void);
void SplicingArray_int(int pt0[][2], int num1, int pt1[][2], int num2, int pt_out[][2], uint8 x);
float Get_Mid_Cross(void);
void Pespective_xy(int x_in, int y_in, float *x_out, float *y_out);
void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);
void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);
void Image_CheckState(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r);
void Cross_Drawline_plus(int in_put_l[][2], int in_put_num_l, int in_put_lnew[][2], int in_put_num_lnew,
                         int in_put_r[][2], int in_put_r_num, int in_put_rnew[][2], int in_put_r_numnew);
float Cross_Drawline(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_r_num);
void test_new(void);

void Get_guaidian(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_r_num);
// W矩阵参数（原图转化成逆透视后图像的参数）
// 60*80
// #define a11 (-4.3801f)
// #define a12 (-0.0293f)
// #define a13 (347.5647f)
// #define a21 (0.0782f)
// #define a22 (3.2241f)
// #define a23 (-476.1463f)
// #define a31 (0.0070f)
// #define a32 (-0.1864f)
// #define a33 (1.0f)

// #define a11 (-0.238288f)
// #define a12 (0.210738f)
// #define a13 (-15.705617f)
// #define a21 (0.005936f)
// #define a22 (0.010945f)
// #define a23 (-8.492773f)
// #define a31 (0.000212f)
// #define a32 (0.002450f)
// #define a33 (-0.433058f)//效果较好

// #define a11 (-0.304968f)
// #define a12 (0.321069f)
// #define a13 (-16.977831f)
// #define a21 (0.000000f)
// #define a22 (0.032146f)
// #define a23 (-12.074271f)
// #define a31 (0.0000f)
// #define a32 (0.004067f)
// #define a33 (-0.527594f)

// #define a11 (0.494521f)
// #define a12 (-0.235086f)
// #define a13 (-3.302742f)
// #define a21 (0.023865f)
// #define a22 (0.004361f)
// #define a23 (3.304021f)
// #define a31 (0.000249f)
// #define a32 (-0.002777f)
// #define a33 (0.412018f)

// #define a11 (-1.9446f)
// #define a12 (-0.0768f)
// #define a13 (152.9928f)
// #define a21 (-0.0901f)
// #define a22 (0.4470f)
// #define a23 (-190.8998f)
// #define a31 (-0.0030f)
// #define a32 (-0.0846f)
// #define a33 (1.0f)

// #define a11 (-2.2450f)
// #define a12 (-8.5371f)
// #define a13 (252.9928f)
// #define a21 (-0.0901f)
// #define a22 (0.4470f)
// #define a23 (-190.8998f)
// #define a31 (-0.0030f)
// #define a32 (-0.0846f)
// #define a33 (1.0f)

// #define a11 (-1.9850f)
// #define a12 (-6.8451f)
// #define a13 (232.9928f)
// #define a21 (-0.0901f)
// #define a22 (0.4470f)
// #define a23 (-190.8998f)
// #define a31 (-0.0030f)
// #define a32 (-0.0636f)
// #define a33 (1.0f)//best

// #define a11 (2.4483f)
// #define a12 (9.9016f)
// #define a13 (-187.3382f)
// #define a21 (-0.0240f)
// #define a22 (-0.9154f)
// #define a23 (308.7282f)
// #define a31 (-0.0062f)
// #define a32 (0.1221f)

// #define a11 (-2.2450f)
// #define a12 (-8.5371f)
// #define a13 (252.9928f)
// #define a21 (-0.0901f)
// #define a22 (0.4470f)
// #define a23 (-190.8998f)
// #define a31 (-0.0030f)
// #define a32 (-0.0846f)
// #define a33 (1.0f)

#define a11 (-5.5988f)
#define a12 (-27.9734f)
#define a13 (709.0200f)
#define a21 (0.1837f)
#define a22 (0.7491f)
#define a23 (-648.4869f)
#define a31 (0.0194f)
#define a32 (-0.3464f)
#define a33 (1.0f)

// #define a11 (-1.9850f)
// #define a12 (-6.8451f)
// #define a13 (232.9928f)
// #define a21 (-0.0901f)
// #define a22 (0.4470f)
// #define a23 (-190.8998f)
// #define a31 (-0.0030f)
// #define a32 (-0.0636f)
// #define a33 (1.0f)

#define getx(u, v) (a11 * (u) + a12 * (v) + a13)
#define gety(u, v) (a21 * (u) + a22 * (v) + a23)
#define getw(u, v) (a31 * (u) + a32 * (v) + a33)

// D矩阵参数
/*这些宏定义都是给60*80的矩阵*/
#define b11 (-0.4928f)
#define b12 (-0.4038f)
#define b13 (47.5991f)

#define b21 (0.0208f)

#define b22 (-0.0466f)

#define b23 (-14.1659f)

#define b31 (0.0003f)
#define b32 (-0.0052f)
#define b33 (-0.0556f)

#define getx_b(u, v) (b11 * (u) + b12 * (v) + b13)
#define gety_b(u, v) (b21 * (u) + b22 * (v) + b23)
#define getw_b(u, v) (b31 * (u) + b32 * (v) + b33)

void ImagePerspective_Init(void);

void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist);

void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void find_corners(void);
void cross_farline(void); // 寻远线
void check_cross(void);
void run_cross(void);
void run_cross_b(void);
void run_cross_c(void);
void run_cross_d(void);
#define POINTS_MAX_LEN (150) // 边线点最多的情况——>num

// 逆透视补线数组
extern float left_line[POINTS_MAX_LEN][2];
extern float right_line[POINTS_MAX_LEN][2];
extern int left_num, right_num;
// 拼接数组
extern float splicing_leftline[POINTS_MAX_LEN][2];
extern float splicing_rightline[POINTS_MAX_LEN][2];
extern int splicing_leftline_num, splicing_rightline_num;
// 拼接数组平移中线
extern float splicing_leftline_center[POINTS_MAX_LEN][2];
extern float splicing_rightline_center[POINTS_MAX_LEN][2];
extern int splicing_leftline_center_num, splicing_rightline_center_num;

// 左右边丢线
extern uint8 loseline0;
extern uint8 loseline1;

extern int x0_first, y0_first, x1_first, y1_first;
extern int x1, y1;
extern int x2, y2;

extern int begin_x0, begin_y0; // 找线偏移点
extern int begin_x1, begin_y1; // 找线偏移点

extern uint8 Image_Use[120][160];
extern uint8 Image_Use_Robert[120][160];
extern uint8 touch_boundary0;    // 左边线走到图像边界
extern uint8 touch_boundary1;    // 右边线走到图像边界
extern uint8 touch_boundary_up0; // 左边线走到图像左边界
extern uint8 touch_boundary_up1; // 右边线走到图像右边界

extern float xielv_left_y_to_end, xielv_right_y_to_end; // 在逆透视后得坐标系建得斜率

// 原图左右边线
extern int ipts0[POINTS_MAX_LEN][2];
extern int ipts1[POINTS_MAX_LEN][2];
extern int ipts0_num, ipts1_num;
// 变换后左右边线
extern float rpts0[POINTS_MAX_LEN][2];
extern float rpts1[POINTS_MAX_LEN][2];
extern int rpts0_num, rpts1_num;
// 变换后左右边线+滤波
extern float rpts0b[POINTS_MAX_LEN][2];
extern float rpts1b[POINTS_MAX_LEN][2];
extern int rpts0b_num, rpts1b_num;
// 变换后左右边线+等距采样
extern float rpts0s[POINTS_MAX_LEN][2];
extern float rpts1s[POINTS_MAX_LEN][2];
extern int rpts0s_num, rpts1s_num;
// 左右边线局部角度变化率
extern float rpts0a[POINTS_MAX_LEN];
extern float rpts1a[POINTS_MAX_LEN];
extern int rpts0a_num, rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
extern float rpts0an[POINTS_MAX_LEN];
extern float rpts1an[POINTS_MAX_LEN];
extern int rpts0an_num, rpts1an_num;
// 左/右中线
extern float rptsc0[POINTS_MAX_LEN][2];
extern float rptsc1[POINTS_MAX_LEN][2];
extern int rptsc0_num, rptsc1_num;
// 归一化中线
extern float rptsn[POINTS_MAX_LEN][2];
extern int rptsn_num;

// Y角点
extern int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
extern bool Ypt0_found, Ypt1_found;
// L角点
extern int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
extern bool Lpt0_found, Lpt1_found;
// 内L角点
extern int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
extern bool N_Lpt0_found, N_Lpt1_found;

extern int N_Xfound_num; // 面向赛道编程，双内L计数
// 长直道
extern bool is_straight0, is_straight1;
// 弯道
extern bool is_turn0, is_turn1;

extern enum track_type_e track_type; // 当前巡线模式

extern float error[1];
extern float ave_error; // ave_error名为平均偏差，实际按正态分布算
// 远预锚点位置
extern int aim_idx[1];

// 计算远锚点偏差值
extern float dx[1];
extern float dy[1];
extern float dn[1];
// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩

extern float pure_angle;

#define L_CROSS 80 // 十字模式中存储左边线坐标的个数为80
void Arc_Check(int pts_in[][2], int pts_num /*,int8 *flag*/);
uint8_t RoundaboutGetArc(uint8_t imageSide[120][2], uint8_t status, uint8_t num, uint8_t *index);
void FarCorners_Find_Left(int pts_in[][2], int pts_num);
void FarCorners_Find_Right(int pts_in[][2], int pts_num);
void LongLine_Check(int pts_in[][2], int pts_num);
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
    5.前面几个点的生长方向为4，就判断为第二状态

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