#include "zf_common_headfile.h"
#ifndef _IMAGE_H
#define _IMAGE_H

#define IN
#define OUT
#define IN_OUT

#define WHITE 255
#define BLACK 0
#define IMAGE_WIDTH 160  // 用于处理的图像高度
#define IMAGE_HEIGHT 120 // 用于处理的图像宽度
#define MID_LINE 80      // 中线为x=80

#define POINTS_MAX_LEN (120)      // 边线点最多的情况——>num
#define POINTS_MIN_LEN (40)       // 边线点最少的情况——>num
#define PRIMEVAL_HEIGHT MT9V03X_H // 原始图像高度
#define PRIMEVAL_WIDTH MT9V03X_W  // 原始图像宽度

#define L_CROSS 80 // 十字模式中存储左边线坐标的个数为80

#define Sobel_Gx(addr, y, x) (addr[UP][RR] + 2 * addr[y][RR] + addr[DN][RR] - (addr[UP][LL] + 2 * addr[y][LL] + addr[DN][LL]))
#define Sobel_Gy(addr, y, x) (addr[UP][LL] + 2 * addr[UP][x] + addr[UP][RR] - (addr[DN][LL] + 2 * addr[DN][x] + addr[DN][RR]))
#define Sobel_G(addr, y, x) (abs(Sobel_Gx(addr, y, x)) + abs(Sobel_Gy(addr, y, x)))
// 转向矩阵

#define AT AT_IMAGE
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

typedef struct img
{
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t step; // 走过路径的长度
} image_t;

enum track_type_e
{
    TRACK_LEFT,
    TRACK_RIGHT,
    TRACK_BOTH,
};
extern enum track_type_e track_type; // 当前巡线模式
/*这个得放在车库的.c现在还没建成*/
extern uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH];        // 全局声明原图像数组
extern uint8 Image_Use_Robert[IMAGE_HEIGHT][IMAGE_WIDTH]; // 全局声明用于处理的图像数组
/*图像处理*/
void Image_Compress();                                                                                                       // 对原始图像进行压缩
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height);                                                          // 优化大津法获取阈值
void Image_Sobel(uint8 Image_in[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t Image_out[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 Threshold); // 全局sobel方案
void Image_Binarization(unsigned char threshold, uint8 (*Image_Use)[IMAGE_WIDTH]);                                           // 全局二值化方案

extern image_t img_raw;
void Left_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
void FarBorderLine_Find(void);

void FarBorderline_Find(void); //
void MidLine_Get(void);        // W矩阵参数（原图转化成逆透视后图像的参数）

// 左右边丢线
extern uint8 loseline0;
extern uint8 loseline1;

extern int Far_Lpt0_Found, Far_Lpt1_Found;
extern int Near_Lpt0_Found, Near_Lpt1_Found;

extern int x0_first, y0_first, x1_first, y1_first;

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

extern int Lpt0_id, Lpt1_id;         // 近线L角点id
extern int Far_Lpt0_id, Far_Lpt1_id; // 远线L角点id

extern int CornersLeft_Point[2];
extern int CornersRight_Point[2];
extern int FarCornersLeft_Point[2];
extern int FarCornersRight_Point[2];

// 长直道
extern int Is_straight0, Is_straight1;
// 弯道
extern bool is_turn0, is_turn1;

extern int Far_ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
extern int Far_ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
extern float Err[5];                     // 中线误差
// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩

void Arc_Rec(int pts_in[][2], int pts_num, int pts_out[2]);
void NearCorners_Find_Left(int pts_in[][2], int pts_num, int pts_out[2], int *flag);
void NearCorners_Find_Right(int pts_in[][2], int pts_num, int pts_out[2], int *flag);
void Straight_Rec(int pts_in[][2], int pts_num, int pts_out[2]);
void FarCorners_Find_Left(int pts_in[][2], int pts_num, int pts_out[2], int *flag);
void FarCorners_Find_Right(int pts_in[][2], int pts_num, int pts_out[2], int *flag);

void BorderLine_Find(void);

#endif
