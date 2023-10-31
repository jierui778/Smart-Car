#include "image.h"
#include "mymath.h"
#include "control.h"
#include "stdlib.h"
#include "cross.h"
#include "circle.h"
#include "garage.h"
#include "ramp.h"
#include "math.h" //后期处理掉

enum track_type_e track_type = TRACK_BOTH;
image_t img_raw = DEF_IMAGE(NULL, IMAGE_WIDTH, IMAGE_HEIGHT);
uint8 Image_Use[IMAGE_HEIGHT][IMAGE_WIDTH];
uint8 Image_Use_Robert[IMAGE_HEIGHT][IMAGE_WIDTH];
const int dir_front[4][2] = {{0, -1},
                             {1, 0},
                             {0, 1},
                             {-1, 0}};

const int dir_frontleft[4][2] = {{-1, -1},
                                 {1, -1},
                                 {1, 1},
                                 {-1, 1}};
const int dir_frontright[4][2] = {{1, -1},
                                  {1, 1},
                                  {-1, 1},
                                  {-1, -1}};


float percent_k;
int Shift= 82;
uint8 Image_Use_Robert[120][160]; // sobel二值化图像

int Last_Lpt0_id=0, Last_Lpt1_id=0; // 上一次的角点id
int Last_Far_Lpt0_id=0, Last_Far_Lpt1_id=0;

int Last_CornersLeft_Point[2] = {0};
int Last_FarCornersLeft_Point[2] = {0};

const float Weight[80] = // 偏差数组
    {
        80.0, 79.2, 78.4, 77.6, 76.8, 76.0, 75.2, 74.4, 73.6, 72.8, 72.0, 71.2, 70.4, 69.6, 68.8, 68.0, 67.2, 66.4, 65.6, 64.8, 64.0, 63.2, 62.4, 61.6, 60.8, 60.0, 59.2, 58.4, 57.6, 56.8, 56.0, 55.2, 54.4, 53.6, 52.8, 52.0, 51.2, 50.4, 49.6, 48.8, 48.0, 47.2, 46.4, 45.6, 44.8, 44.0, 43.2, 42.4, 41.6, 40.8, 40.0, 39.2, 38.4, 37.6, 36.8, 36.0, 35.2, 34.4, 33.6,
        32.8, 32.0, 31.2, 30.4, 29.6, 28.8, 28.0, 27.2, 26.4, 25.6, 24.8, 24.0, 23.2, 22.4, 21.6, 20.8, 20.0, 19.2, 18.4, 17.6, 16.8};

uint8 State; // 状态机
uint8 mid_line_num = 0;
float Finnal_err;    // 最终误差
float err, last_err; // 全局变量的误差
float mid_row_first; // 中线行的起点
// flash参数统一定义
float begin_x = 5;   // 起始点距离图像中心的左右偏移量	8
float begin_y = 118; // 起始点距离图像底部的上下偏移量 120高度：35;100高	58
/*beginy值越小，初始的生长点与上框越近*/

float block_size = 7; // 自适应阈值的block大小
float clip_value = 2; // 自适应阈值的阈值裁减量
int NearBorderLine_Enable = 1;
int FarBorderLine_Enable = 0; // 开启远近线的标志位//默认只开启近线,不开启远线

<<<<<<< HEAD
float thres = 120;                                  // 二值化阈值，主要用于找起始点(边线使用自适应阈值，不使用该阈值)
float line_blur_kernel = 9;                         // 边线三角滤波核的大小
float pixel_per_meter = 102;                        // 俯视图中，每个像素对应的长度 (厘米*10^3)/像素个数 调大意味着平移距离增大 原：102
float sample_dist = 0.022;                          // 边线等距采样的间距 动态调整，使其每两点距离刚好为1cm 为0.02时10个点间隔约为9cm
float angle_dist = 0.2;                             // 计算边线转角时，三个计算点的距离
float far_rate = 0.5;                               //
float aim_distance_flash = 0.68;                    // 预锚点长度
float aim_dist[5] = {0.68, 0.78, 0.88, 0.98, 1.08}; // 多个预瞄点长度，间隔5cm，34~54个点，用于速度模糊控制，而不是偏差角计算！
=======
// 左右边丢线
uint8 loseline0;              // 左边线丢失标志位
uint8 loseline1;              // 右边线丢失标志位
int ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int ipts0_num;                // 存放边线像素点个数(左)
int ipts1_num;                // 存放边线像素点个数(右)
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5

int Far_ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int Far_ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int Far_ipts0_num;                // 存放边线像素点个数(左)
int Far_ipts1_num;                // 存放边线像素点个数(右)

int Lpt0_id, Lpt1_id;         // 近线L角点id
int Far_Lpt0_id, Far_Lpt1_id; // 远线L角点id

int Is_straight0 = 0, Is_straight1 = 0;     // 直道标志位
int Far_Arc0_Found = 0, Far_Arc1_Found = 0; // 圆弧标志位
int Near_Arc0_Found = 0, Near_Arc1_Found = 0;
int Is_Bend0 = 0, Is_Bend1 = 0; // 弯道标志位

int Far_Lpt0_Found = 0, Far_Lpt1_Found = 0;   // 远线L角点标志位
int Near_Lpt0_Found = 0, Near_Lpt1_Found = 0; // 近线角点标志位

int NearIs_Arc0, NearIs_Arc = 0;

uint8 touch_boundary0; // 左边线走到图像左边界
uint8 touch_boundary1; // 右边线走到图像右边界

uint8 touch_boundary_up0; // 左边线走到图像上边界
uint8 touch_boundary_up1; // 右边线走到图像上边界

int begin_x0, begin_y0; // 找线偏移点
int begin_x1, begin_y1; // 找线偏移点

int x0_first, y0_first, x1_first, y1_first; // 寻找跳变点坐标

int x1, y1;
int x2, y2;

int mid_line[150][2] = {0};

int CornersLeft_Point[2] = {0};
int CornersRight_Point[2] = {0}; // 近角点坐标
int FarCornersLeft_Point[2] = {0};
int FarCornersRight_Point[2] = {0}; // 远角点坐标
int ArcLeft_Point[2] = {0};
int ArcRight_Point[2] = {0}; // 近角点坐标

float Err[5] = {0}; // 中线误差数组

void test(void) // 测试函数
{
    int test[100][2] = {0};
    int test2 = 0;
    int test3[2] = {0, 0};
    int test5;
    Image_Compress();                                                    // 压缩图像
    int TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT); // 优化大津法获取动态阈值
    Image_Sobel(Image_Use, Image_Use_Robert, TH);                        // 全局Sobel得二值图(方案二) 2.8ms
    img_raw.data = *Image_Use_Robert;                                    // 传入sobel边沿检测图像
    // 寻找左右边线
    if (NearBorderLine_Enable)
    {
        BorderLine_Find(); // 寻找近边线
    }
    NearCorners_Find_Left(ipts0, ipts0_num, CornersLeft_Point, &Near_Lpt0_Found);   // 寻找近角点
    NearCorners_Find_Right(ipts1, ipts1_num, CornersRight_Point, &Near_Lpt1_Found); // 近角点正常
    ips200_draw_line(0, 0, CornersLeft_Point[0], CornersLeft_Point[1], RGB565_BLUE);
    ips200_draw_line(0, 0, CornersRight_Point[0], CornersRight_Point[1], RGB565_RED);
    // ips200_show_int(50, 200, loseline0, 2);
    // ips200_show_int(50, 220, FarCornersLeft_Point[1], 2);
    // ips200_show_float(40,160,Finnal_err+12,3,3);
    if (Near_Lpt0_Found || Near_Lpt1_Found) // 如果近边线有角点
    {
        FarBorderLine_Enable = 1;
    }
    Finnal_err = 0;
    err = 0;
    if (FarBorderLine_Enable) // 开启
    {
        FarBorderline_Find(); // 寻找远边线
        if (1)  // 如果左边线有角点
        {
            FarCorners_Find_Left(Far_ipts0, Far_ipts0_num, FarCornersLeft_Point, &Far_Lpt0_Found);
        }
        if (1)
        {
            FarCorners_Find_Right(Far_ipts1, Far_ipts1_num, FarCornersRight_Point, &Far_Lpt1_Found);
        }
    }
    // LongStarightLine_Rec(ipts1, ipts1_num, 66, &Is_straight1);
    // int test666 = RoundaboutGetArc(ipts0, 1, 10, &Is_straight0);
	Finnal_err = Finnal_err * percent_k;
    // ips200_show_uint(160, 160, 1, 1);
    // Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);


    // ips200_show_int(160,80, loseline1, 1);
    // ips200_show_int(160,40, loseline0, 1);
    // ips200_draw_line(80, 60, CornersLeft_Point[0], CornersLeft_Point[1], RGB565_RED);
    // // ips200_draw_line(80, 60, CornersRight_Point[0], CornersRight_Point[1], RGB565_RED);
    // ips200_draw_line(80, 60, FarCornersLeft_Point[0], FarCornersLeft_Point[1], RGB565_YELLOW);
    // ips200_show_uint(10,200,CornersLeft_Point[0],3);
    // ips200_show_uint(10,220,CornersLeft_Point[1],3);
    // ips200_show_uint(10,240,FarCornersLeft_Point[0],3);
    // ips200_show_uint(10,260,FarCornersLeft_Point[1],3);
    // // ips200_draw_line(80, 60, FarCornersRight_Point[0], FarCornersRight_Point[1], RGB565_GREEN);

    // // ips200_show_int(20, 200, Near_Lpt0_Found, 1);
    // // ips200_show_int(20, 220, Near_Lpt1_Found, 1);
    // // ips200_show_int(20, 240, Far_Lpt0_Found, 1);
    // // ips200_show_int(20, 260, Far_Lpt1_Found, 1);
    // // ips200_show_int(20, 280, touch_boundary0, 1);
    // // ips200_show_int(20, 300, touch_boundary1, 1);
    // ips200_show_int(80,200,Lpt0_id,3);
    // ips200_show_int(80,220,Far_Lpt0_id,3);
    // // ips200_show_int(80, 200, loseline0, 1);
    // // ips200_show_int(80, 220, loseline1, 1);
    // // ips200_show_int(160, 200, NearBorderLine_Enable, 1);
    // // ips200_show_int(160, 240, FarBorderLine_Enable, 1);

    // // Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);
    // // Line_Add(&img_raw, CornersRight_Point, FarCornersRight_Point, 0);

    // ips200_show_int(160, 20, State, 3);
    // ips200_show_int(160, 20, ipts1_num, 3);

    if (1)
    {
        Cross_Check();
    }
    if (cross_type != CROSS_NONE)
    {
        Cross_Run();
        // ips200_draw_line(0,0,120,160,RGB565_BLUE);
    }


    // if (cross_type == CROSS_NONE)
    // {
    //     Circle_Check();
    // }
    // if (circle_type != CIRCLE_NONE)
    // {
    //     Circle_Run();
    //     // ips200_draw_line(0,0,120,160,RGB565_BLUE);
    // }

    Track_Check();
    Track_Run();
    // Finnal_err = Err_Handle(1);

    // else if(track_type == TRACK_RIGHT)
    // {
    //     mid_line_num = ipts1_num;
    //     track_rightline(ipts1,ipts1_num,mid_line, (int)round(0.2/0.022),120 *0.39/2);
    // }
    // else if(track_type == TRACK_BOTH)
    // {
    //     mid_line_num = (ipts0_num > ipts1_num) ? ipts1_num : ipts0_num;//mid_line_num选择较小的数
    //     track_leftline(ipts0,ipts0_num,mid_line, (int)round(0.2/0.022),120 *0.39/2);
    //     track_rightline(ipts1,ipts1_num,mid_line, (int)round(0.2/0.022),120 *0.39/2);
    // }

     for (uint8 i = 0; i < mid_line_num; i++)
     {
         if(mid_line[i][0] > 2 && mid_line[i][0] < 158 && mid_line[i][1] > 2 && mid_line[i][1] < 118)
         {
             ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_RED);
         }
         // ips200_draw_line(160,0,mid_line[i][0],mid_line[i][1],RGB565_BLUE);
     }
    // ips200_show_uint(0,120,mid_line_num,3);
    // MidLine_Get(ipts0, ipts0_num, ipts1, ipts1_num, test, 2);
    // NearCorners_Find_Left(ipts0, ipts0_num, test, &test2);
    // NearCorners_Find_Right(ipts1, ipts1_num, test3, &test5);//近角点正常
    // Features_Find();  // 寻找特征点

    // int test = Is_Straight(ipts0, ipts0_num, 100);
    // test = Is_Straight(ipts0, ipts0_num, sample_dist);
    // Straight_Rec(ipts1, ipts1_num);

    // for (int i = 0; i < ipts0_num; i++)
    // {
    //     ips200_draw_point(ipts0[i][0] + 3, ipts0[i][1], RGB565_RED);
    // }
    // for (int i = 0; i < ipts1_num; i++)
    // {
    //     ips200_draw_point(ipts1[i][0] - 3, ipts1[i][1], RGB565_RED);
    // }

    // for (int i = 0; i < ipts0_num; i++)
    // {
    //     ips200_draw_line(0, 0, ipts0[i][0] + 5, ipts0[i][1], RGB565_BLUE);
    // }
    // for (int i = 0; i < Far_ipts0_num; i++)
    // {
    //     ips200_draw_line(0, 0, Far_ipts0[i][0], Far_ipts0[i][1], RGB565_BLUE);
    // }
    // for (int i = 0; i < Far_ipts1_num; i++)
    // {
    //     ips200_draw_line(160, 0, Far_ipts1[i][0], Far_ipts1[i][1], RGB565_RED);
    // }
    // for (int i = 0; i < ipts0_num; i++)
    // {
    //     ips200_draw_line(0,0,ipts0[i][0] , ipts0[i][1], RGB565_BLUE);
    // }
    // for (int i = 0; i < ipts1_num; i++)
    // {
    //     ips200_draw_line(160,0,ipts1[i][0] , ipts1[i][1], RGB565_RED);
    // }

}

/**
 * @brief 寻找远左拐点,判断条件过头
 *
 * @param pts_in 左远线数组
 * @param pts_num 左边线数组长度
 * @param pts_out 拐点坐标
 * @param flag 标志位
 */
void FarCorners_Find_Left(int pts_in[][2], int pts_num, int pts_out[2], int *flag)
{

    int Is_Corner = 0; // 角点判断标志位
    for (int i = 10; i < pts_num - 10; i++)
    {
        if ((pts_in[i][0] - pts_in[i - 2][0] > 0 && pts_in[i][0] - pts_in[i - 4][0] > 0 && pts_in[i][0] - pts_in[i - 6][0] > 0 &&
             pts_in[i][0] - pts_in[i - 8][0] > 0 && pts_in[i][0] - pts_in[i - 10][0] > 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 &&
             pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0) ||
            (pts_in[i][0] - pts_in[i - 2][0] > 0 && pts_in[i][0] - pts_in[i - 4][0] > 0 &&
             pts_in[i][0] - pts_in[i - 6][0] > 0 && pts_in[i][0] - pts_in[i - 8][0] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0 &&
             pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 &&
             pts_in[i][1] - pts_in[i + 8][1] > 0 &&
             pts_in[i][1] - pts_in[i + 10][1] > 0)) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i + 1][1] || pts_in[i][1] == pts_in[i - 1][1])
            {
                continue;
            }
            Far_Lpt0_id = i; // 记录拐点的下标
            // if(i>pts_num-20)
            // {
            //     i = pts_num / 2;
            // }
            pts_out[0] = pts_in[Far_Lpt0_id][0];
            pts_out[1] = pts_in[Far_Lpt0_id][1]; // 找到拐点,传出拐点坐标

            *flag = 1;                 // 拐点标志位置为一
            // 标志位为1
            break;
        }
        else
        {
            *flag = 0;
            // pts_out[0] = 0;
            // pts_out[1] = 0;
        }
    }
}

/**
 * @brief 寻右远拐点
 *
 * @param pts_in 右远线数组
 * @param pts_num 右远线数组长度
 * @param pts_out 拐点坐标
 * @param flag 标志位
 */
void FarCorners_Find_Right(int pts_in[][2], int pts_num, int pts_out[2], int *flag)
{

    int Is_Corner = 0; // 角点判断标志位

    for (int i = 10; i < pts_num - 10; i++)
    {
        // if(pts_in[i][0] - pts_in[i-1][0] >0)
        //     continue;
        if ((pts_in[i][0] - pts_in[i - 1][0] < 0 && pts_in[i][0] - pts_in[i - 2][0] < 0 && pts_in[i][0] - pts_in[i - 3][0] < 0 &&
             pts_in[i][0] - pts_in[i - 4][0] < 0 && pts_in[i][0] - pts_in[i - 5][0] < 0 && pts_in[i][1] - pts_in[i + 1][1] > 0 &&
             pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 3][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 &&
             pts_in[i][1] - pts_in[i + 5][1] > 0) ||
            (pts_in[i][0] - pts_in[i - 1][0] < 0 && pts_in[i][0] - pts_in[i - 2][0] < 0 &&
             pts_in[i][0] - pts_in[i - 3][0] < 0 && pts_in[i][0] - pts_in[i - 4][0] < 0 && pts_in[i][1] - pts_in[i + 1][1] > 0 &&
             pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 3][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 &&
             pts_in[i][1] - pts_in[i + 5][1] > 0)) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i - 1][1] || pts_in[i][1] == pts_in[i + 1][1] /**|| (pts_in[i][0] - pts_in[i - 2][0] <= 0 && pts_in[i][0] - pts_in[i - 4][0] <= 0 & pts_in[i][0] - pts_in[i - 6][0] <= 0 && pts_in[i][0] - pts_in[i + 4][1] <= 0 && (pts_in[i][0] - pts_in[i + 6][0] <= 0))*/)
            {
                continue;
            }
            Far_Lpt1_id = i;
            pts_out[0] = pts_in[Far_Lpt1_id][0];
            pts_out[1] = pts_in[Far_Lpt1_id][1];

            *flag = 1;
            break;
        }
        else
        {
            *flag = 0;
        }
    }
    // ips200_draw_line(0, 0, pts_out[0], pts_out[1], RGB565_RED);
    // ips200_show_int(100, 200, *flag, 1);
}

void FarBorderline_Find(void)
{
    uint8 uthres = 1;
    if (loseline1) // 近处丢线,采用静态起始点
    {
        CornersRight_Point[0] = 158;
        CornersRight_Point[1] = 100;
    }
    if (loseline0)
    {
        CornersLeft_Point[0] = 1;
        CornersLeft_Point[1] = 100;
    }
    // ips200_draw_line(0, 0, CornersLeft_Point[0], CornersLeft_Point[1], RGB565_RED);
    ips200_show_uint(200, 200, Far_ipts0_num, 2);

    if (1)
    {

        x0_first = CornersLeft_Point[0] + 10;
        y0_first = CornersLeft_Point[1] - 10;

        Far_ipts0_num = sizeof(Far_ipts0) / sizeof(Far_ipts0[0]); // 求数组的长度
        // 扫底下五行，寻找跳变点
        for (; y0_first > 0; y0_first--) // 从所选的行，向上扫5次，每次从中间向左线扫
        {
            if (AT_IMAGE(&img_raw, x0_first, y0_first) < uthres) // 如果扫到黑点（灰度值为0），就从该点开始扫线
            {
                goto out1; // 开始扫左线
            }
        }
        // 如果扫不到的话，判定左边的底边丢线
    out1: // 从起始点开始执行扫线
    {
        // if (AT_IMAGE(&img_raw, x0_first+1, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, Far_ipts0, &Far_ipts0_num); // 开始跑迷宫
        // else
        // 	Far_ipts0_num = 0;//如果不是的话，就不用跑了，求得的number记为0
    }
    }

    if (1)
    {
        // 寻右边线
        // x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

        // 标记种子起始点(后续元素处理要用到)
        // ips200_show_int(50, 200, 666, 2);
        x1_first = CornersRight_Point[0] - 10;
        y1_first = CornersRight_Point[1] - 10;
        ;

        Far_ipts1_num = sizeof(Far_ipts1) / sizeof(Far_ipts1[0]);
        for (; y1_first > 15; y1_first--)
        {
            if (AT_IMAGE(&img_raw, x1_first, y1_first) < uthres)
            {
                goto out2;
            }
        }
    out2:
    {
        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, Far_ipts1, &Far_ipts1_num);
    }
    }
}

/**
 * @brief 获取中线（从中间向两边扫）
 *
 * @param mode
 */
void Center_edge()
{

    int Left_Edge[150][2] = {0}; // 实际上只会用到120个，怕越界
    int Right_Edge[150][2] = {0};
    int x, y;
    int i;
    int uthres = 1;
    y = 0;
    for (i = 0; i < 118; i++)
    {
        for (x = 80; x > 0; x--)
        {
            if ((AT_IMAGE(&img_raw, x + 1, i) > uthres) && (AT_IMAGE(&img_raw, x, i) < uthres))
            {
                Left_Edge[i][0] = x;
                Left_Edge[i][1] = i;
                break;
            }
        }
    }
    y = 0;
    for (i = 0; i < 118; i++)
    {
        for (x = 80; x < 159; x++)
        {
            if ((AT_IMAGE(&img_raw, x + 1, i) > uthres) && (AT_IMAGE(&img_raw, x, i) < uthres))
            {
                Right_Edge[i][0] = x;
                Right_Edge[i][1] = i;
                break;
            }
        }
    }

    for (i = 30; i < 118; i++)
    {
        mid_line[i][0] = (Left_Edge[i][0] + Right_Edge[i][0]) / 2;
        mid_line[i][1] = (Left_Edge[i][1] + Right_Edge[i][1]) / 2;
    }
}

void Features_Find(void)
{
    if (touch_boundary0) // 触碰到左边界
    {
        //        Arc_Rec(ipts0, ipts0_num); // 检测左边线是否为圆弧
    }
    else // 长直道一定不会触碰到边界
    {
        //        Straight_Rec(ipts0, ipts0_num); // 检测左边线是否为直线
    }
    if (touch_boundary1) // 迷宫触碰到右边界
    {
        //        Arc_Rec(ipts1, ipts1_num); // 检测右边线是否为圆弧
    }
    else
    {
        //        Straight_Rec(ipts1, ipts1_num); // 检测右边线是否为直线
    }
}

/**
 * @brief 寻找左边线角点
 *
 * @param pts_in 输入边线数组
 * @param pts_num 输入边线数组长度
 * @param pts_out 输出角点坐标
 * @param flag 角点判断标志位
 */
void NearCorners_Find_Left(int pts_in[][2], int pts_num, int pts_out[2], int *flag)
{
    int Is_Corner = 0; // 角点判断标志位
    int Is_Arc = 0;    // 圆环判断标志位
    // pts_out[0] = 0;
    // pts_out[1] = 0;
    for (int i = 5; i < pts_num - 5; i++)
    {
        // 圆弧
        if (
            pts_in[i][0] - pts_in[i - 1][0] >= 0 && pts_in[i][0] - pts_in[i - 2][0] >= 0 && pts_in[i][0] - pts_in[i - 3][0] >= 0 &&
            pts_in[i][0] - pts_in[i - 4][0] >= 0 && pts_in[i][0] - pts_in[i - 5][0] >= 0 && pts_in[i][0] - pts_in[i - 6][0] >= 0 && pts_in[i][0] - pts_in[i + 1][0] >= 0 && pts_in[i][0] - pts_in[i + 2][0] > 0 && pts_in[i][0] - pts_in[i + 3][0] >= 0 && pts_in[i][0] - pts_in[i + 4][0] >= 0 && pts_in[i][0] - pts_in[i + 5][0] > 0 && pts_in[i][0] - pts_in[i + 6][0] >= 0 && my_abs(pts_in[i][1] - pts_in[i + 1][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 2][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 3][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 4][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 5][1]) < 5) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i + 1][1])
            {
                continue;
            }
            else
            {
                Lpt0_id = i;
                pts_out[0] = pts_in[i][0];
                pts_out[1] = pts_in[i][1];
                Lpt0_id = i;
                *flag = 1;
                break;
            }
        }
        else
        {
            *flag = 0;
        }
    }
}
/**
 * @brief 寻找右边线角点
 *
 * @param pts_in 输入边线数组
 * @param pts_num 输入边线数组长度
 * @param pts_out 输出角点坐标
 * @param flag 角点判断标志位
 */
void NearCorners_Find_Right(int pts_in[][2], int pts_num, int pts_out[2], int *flag)
{
    int Is_Corner = 0; // 角点判断标志位
    int Is_Arc = 0;    // 圆环判断标志位
    for (int i = 5; i < pts_num - 5; i++)
    {
        // 圆弧
        if (
            pts_in[i][0] - pts_in[i - 1][0] <= 0 && pts_in[i][0] - pts_in[i - 2][0] <= 0 && pts_in[i][0] - pts_in[i - 3][0] <= 0 &&
            pts_in[i][0] - pts_in[i - 4][0] <= 0 && pts_in[i][0] - pts_in[i - 5][0] <= 0 && pts_in[i][0] - pts_in[i - 6][0] <= 0 && pts_in[i][0] - pts_in[i + 1][0] <= 0 && pts_in[i][0] - pts_in[i + 2][0] < 0 && pts_in[i][0] - pts_in[i + 3][0] <= 0 && pts_in[i][0] - pts_in[i + 4][0] <= 0 && pts_in[i][0] - pts_in[i + 5][0] < 0 && pts_in[i][0] - pts_in[i + 6][0] <= 0 && my_abs(pts_in[i][1] - pts_in[i + 1][1]) < 8 && my_abs(pts_in[i][1] - pts_in[i + 2][1]) < 8 && my_abs(pts_in[i][1] - pts_in[i + 3][1]) < 8 && my_abs(pts_in[i][1] - pts_in[i + 4][1]) < 8 && my_abs(pts_in[i][1] - pts_in[i + 5][1]) < 8) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i + 1][1])
            {
                continue;
            }
            else
            {
                Lpt1_id = i;
                pts_out[0] = pts_in[i][0];
                pts_out[1] = pts_in[i][1];
                *flag = 1;
                break;
            }
        }
        else
        {
            *flag = 0;
        }
    }
}
/**
 * @brief 圆弧检测,取出圆弧的极边界坐标(几何特征,左圆弧先增后减,右圆弧先减后增,且相邻几个点的坐标一般不相同,呈一定的增长/减少趋势)
 *
 * @param pts_in 边线坐标数组
 * @param pts_num 边线坐标数组长度
 * @param pts_out 极边界坐标数组
 */
void Arc_Point_Get(int pts_in[][2], int pts_num, int pts_out[2])
{
    // int Is_Arc = 0; // 圆环判断标志位
    for (int i = 10; i < pts_num - 10; i++)
    {
        // 圆弧b
        if (pts_in[i][0] - pts_in[i - 2][0] <= 0 && pts_in[i][0] - pts_in[i - 4][0] <= 0 && pts_in[i][0] - pts_in[i - 6][0] <= 0 && pts_in[i][0] - pts_in[i - 8][0] <= 0 && pts_in[i][0] - pts_in[i - 10][0] <= 0 && pts_in[i][0] - pts_in[i + 2][0] <= 0 && pts_in[i][0] - pts_in[i + 4][0] <= 0 && pts_in[i][0] - pts_in[i + 6][0] <= 0 && pts_in[i][0] - pts_in[i + 8][0] <= 0 && pts_in[i][0] - pts_in[i + 10][0] <= 0) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][0] == pts_in[i + 1][0])
            {
                continue;
            }
            pts_out[0] = ipts0[i][0];
            pts_out[1] = ipts0[i][1];

            break;
        }
        else
        {
        }
    }


    ips200_show_int(160, 220, pts_num, 2);
    ips200_show_int(160, 160, *flag, 1);
}
/**
 * @brief 检测单边是否为长直线
 *
 * @param pts_in 边界数组
 * @param pts_num 数组长度
 * @param flag 标志位
 */
void Straight_Rec(int pts_in[][2], int pts_num, int *flag)
{
    int Is_Straight = 0; // 长直道判断标志位
    for (int i = 0; i < pts_num; i++)
    {
        if (my_abs((pts_in[i][0] - pts_in[i - 5][0]) - (pts_in[i + 5][0] - pts_in[i][0])) < 3 && my_abs((pts_in[i][0] - pts_in[i - 10][0]) - (pts_in[i][0] - pts_in[i + 10][0])) && my_abs((pts_in[i][1] - pts_in[i - 5][1]) - (pts_in[i + 5][1] - pts_in[i][1])) < 3 && my_abs((pts_in[i][1] - pts_in[i - 10][1]) - (pts_in[i][1] - pts_in[i + 10][1])))
        {
            Is_Straight++;
        }
        else
        {
            Is_Straight--;
        }
    }
    if (Is_Straight > 50) // 大于阈值认为是长直线
    {
        *flag = 1;
    }
    else
    {
        *flag = 0;
    }
}

#define MID_LINE 80 // 定义中线,图像宽为160,所以中线y=80
#define PROS_NUM 20 //  前瞻点数

/**
 * @brief
 *
 * @param pts_in
 * @param pts_num
 * @return float
 */
float Err_Get(int pts_in[][2], int pts_num)
{
    float Err = 0;
    for (int i = 0; i < pts_num; i++)
    {
        Err += MID_LINE - pts_in[i][0];
    }
    return Err;
}

/**
 * @brief 压缩图像为我们需要的大小，这里我们采用不压缩，即直接对原图进行处理
 * @param uint8 (*InImg)[IMGW] 输入图像地址
 * @param uint8 (*OutImg)[IMGW] 输出图像地址
 */
void Image_Compress(void)
{
    uint8_t div_h, div_w;
    uint32_t temp_h = 0;
    uint32_t temp_w = 0;
    uint32_t row_start = 0;
    uint32_t lin_start = 0;

    div_h = PRIMEVAL_HEIGHT / IMAGE_HEIGHT;
    div_w = PRIMEVAL_WIDTH / IMAGE_WIDTH;

    // 从中心取图像
    if (IMAGE_HEIGHT * div_h != PRIMEVAL_HEIGHT)
    {
        row_start = (PRIMEVAL_HEIGHT - IMAGE_HEIGHT * div_h) / 2;
        temp_h = row_start;
    }
    if (IMAGE_WIDTH * div_w != PRIMEVAL_WIDTH)
    {
        lin_start = (PRIMEVAL_WIDTH - IMAGE_WIDTH * div_w) / 2;
    }
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        temp_w = lin_start;
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            Image_Use[i][j] = mt9v03x_image[temp_h][temp_w];
            temp_w += div_w;
        }
        temp_h += div_h;
    }
}
/**
 * @brief GitHub copilot优化版本大津法
 *
 * @param image
 * @param Width
 * @param Height
 * @return uint8
 */
uint8 OSTU_GetThreshold(uint8 *image, uint16 Width, uint16 Height)
{
    uint8 HistGram[257] = {0}; // 将数组大小改为 257
    uint16 x, y;
    int16 Y;
    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;
    int16 MinValue, MaxValue;
    uint8 Threshold = 0;
    uint8 *data = image;
    for (y = 0; y < Height; y++)
    {
        for (x = 0; x < Width; x++)
        {
            HistGram[data[y * Width + x]]++;
        }
    }
    HistGram[255] = 0; // 将像素值为 255 的像素单独处理

    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++)
        ;
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MaxValue] == 0; MaxValue--)
        ;

    if (MaxValue == MinValue)
    {
        return MaxValue;
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;
    }
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
        PixelBack = PixelBack + HistGram[Y];
        PixelFore = Amount - PixelBack;
        OmegaBack = (double)PixelBack / Amount;
        OmegaFore = (double)PixelFore / Amount;
        PixelIntegralBack += HistGram[Y] * Y;
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;
        MicroBack = (double)PixelIntegralBack / PixelBack;
        MicroFore = (double)PixelIntegralFore / PixelFore;
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
        if (Sigma > SigmaB)
        {
            SigmaB = Sigma;
            Threshold = Y;
        }
    }
    return Threshold;
}
/**
 * @brief 全局sobel方案
 *
 * @param Image_in 输入图像
 * @param Image_out 输出图像
 * @param Threshold 阈值
 */
void Image_Sobel(uint8 Image_in[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t Image_out[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 Threshold)
{
    uint8_t i, j;
    uint8_t UP, DN, LL, RR;
    if (Threshold == 0) // 观察每点梯度值
    {
        for (i = 1; i < IMAGE_HEIGHT - 1; i++)
        {
            DN = i + 1;
            UP = i - 1;
            for (j = 1; j < IMAGE_WIDTH - 1; j++)
            {
                RR = j + 1;
                LL = j - 1;
                Image_out[i][j] = Sobel_G(Image_in, i, j);
            }
        }
    }
    else // 根据梯度值二值化
    {
        for (i = 1; i < IMAGE_HEIGHT - 1; i++)
        {
            DN = i + 1;
            UP = i - 1;
            for (j = 1; j < IMAGE_WIDTH - 1; j++)
            {
                RR = j + 1;
                LL = j - 1;
                Image_out[i][j] = (Sobel_G(Image_in, i, j) >= Threshold ? 0 : 255); // 修改的tft二值化图库函数：黑0；白1
            }
        }
    }
}

/**
 * @brief 将输入的灰度图像转化为二值化图像，弃用
 * @param Threshold 图像阈值(实际上阈值需要进行计算，而不是直接赋值)
 */
void Image_Binarization(uint8 threshold, uint8 (*Image)[IMAGE_WIDTH])
{
    uint32 i, j;
    uint16 temp = 0;

    for (j = 0; j < IMAGE_HEIGHT; j++)
    {
        for (i = 0; i < IMAGE_WIDTH; i++)
        {
            temp = *(Image[0] + j * IMAGE_WIDTH + i);                              // 读取像素点
            if (j == 0 || j == IMAGE_HEIGHT - 1 || i == 0 || i == IMAGE_WIDTH - 1) // 大津法加一个黑框
            {
                //                *(Image[0] + j * IMAGE_WIDTH + i) = 0;
            }
            else
            {
                if (temp >= threshold)
                    *(Image[0] + j * IMAGE_WIDTH + i) = 255;
                else
                    *(Image[0] + j * IMAGE_WIDTH + i) = 0;
            }
        }
    }
}

<<<<<<< HEAD
/**
 * @brief 计算一个整数的绝对值
 *
 * @param value 需要计算绝对值的整数
 * @return int 返回value的绝对值
 */
int my_abs(int value)
{
    if (value >= 0)
        return value;
    else
        return -value;
}

/**
 * @brief 限制一个整数的范围在 [a, b] 之间
 *
 * @param x 待限制的整数
 * @param a 范围下限
 * @param b 范围上限
 * @return int16 限制后的整数
 */
int16 limit_a_b(int16 x, int a, int b)
{
    if (x < a)
        x = a;
    if (x > b)
        x = b;
    return x;
}

/**
 * @brief 比较两个整数的大小，返回其中较小的一个
 *
 * @param a 整数a
 * @param b 整数b
 * @return int 返回a和b中较小的一个
 */
int min(int a, int b)
{
    return a < b ? a : b;
}

/**
 * @brief 获取图像左侧起始生长点--寻找起始点函数，从第56行开始数
 * @input start_line 开始找起始点的行
 *
 * @return unsigned char start_row 将输入的起始行的值返回，因为加了黑框肯定会找得到，没必要关注找没找得到
 * start_row 一般默认是110-119内（119是最低的行），无论怎么选都是可以找到起始生长点的
    一般建议118-117就行，不然最下面没有的话数组很容易越界啥的
*  这里返回star_row是为了在八邻域里面确定起始点，且可以操作
 */
uint8 left_point;     // 记录第一个关键点的列坐标，定义为全局变量，方便后面的函数调用
uint8 Left_Find_Flag; // 左线起始点找到标志
uint8 Image_Get_LeftPoint(uint8 start_row)
{
    for (left_point = (IMAGE_WIDTH / 2); left_point > 3; left_point--)
    {
        if ((Image_Use_Robert[start_row][left_point] == 255) && (Image_Use_Robert[start_row][left_point - 1] == 0) && (Image_Use_Robert[start_row][left_point - 2] == 0))
        {
            left_point -= 1;
            Left_Find_Flag = 1; //
            break;
        }
    }
    if (!Left_Find_Flag)
    {
        left_point = 2;
    }
    return start_row;
}

uint8 Image_Get_LeftPoint2(uint8 start_row)
{
    for (left_point = (IMAGE_WIDTH / 2); left_point > 3; left_point--)
    {
        if ((Image_Use_Robert[start_row][left_point] == 255) && (Image_Use_Robert[start_row][left_point - 1] == 0))
        {
            left_point -= 1;
            Left_Find_Flag = 1; //
            break;
        }
    }
    if (!Left_Find_Flag)
    {
        left_point = 2;
    }
    return start_row;
}

/**
 * @brief 获取图像右侧起始生长点--寻找起始点函数，从第start_row行开始数
 * @input start_line 开始找起始点的行
 *
 * @return unsigned char start_row 将输入的起始行的值返回
 * @explaination 由于最右边两行画了黑框，正常情况下是肯定能扫描出来的，也就是一定能扫得到在157列坐标
 */
uint8 right_point;     // 记录第一个关键点的列坐标
uint8 Right_Find_Flag; // 右线起始点找到标志
uint8 Image_Get_RightPoint(uint8 start_row)
{
    for (right_point = (IMAGE_WIDTH / 2); right_point < (IMAGE_WIDTH - 2); right_point++) // 扫到
    {
        if ((Image_Use_Robert[start_row][right_point] == 255) && (Image_Use_Robert[start_row][right_point + 1] == 0) && (Image_Use_Robert[start_row][right_point + 2] == 0)) // 这里指针变量不能直接和值比较，需要解地址
        {
            right_point = right_point + 1;
            Right_Find_Flag = 1;
            break; // 这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
    if (!Right_Find_Flag) // 如果找不到
    {
        right_point = IMAGE_WIDTH - 3; // 取边界点，这里的点就是白点了
    }
    return start_row;
}

uint8 Image_Get_RightPoint2(uint8 start_row)
{
    for (right_point = (IMAGE_WIDTH / 2); right_point < (IMAGE_WIDTH - 2); right_point++) // 扫到
    {
        if ((Image_Use_Robert[start_row][right_point] == 255) && (Image_Use_Robert[start_row][right_point + 1] == 0)) // 这里指针变量不能直接和值比较，需要解地址
        {
            right_point = right_point + 1;
            Right_Find_Flag = 1;
            break; // 这里不能直接return 会有报错，就用break跳出循环，然后在最外面return即可
        }
    }
    if (!Right_Find_Flag) // 如果找不到
    {
        right_point = IMAGE_WIDTH - 3; // 取边界点，这里的点就是白点了
    }
    return start_row;
}

/**
 * @brief 滤波函数，将图像中部分噪声去除
 *
 *
 * @return 无
 */
void Image_Filter(void)
{
    uint16 i, j;
    uint32 num = 0;
    for (i = 1; i < IMAGE_HEIGHT - 1; i++)
    {
        for (j = 1; j < IMAGE_WIDTH - 1; j++)
        {
            // 统计8个方向的像素值
            num = Image_Use_Robert[i - 1][j - 1] + Image_Use_Robert[i - 1][j] + Image_Use_Robert[i - 1][j + 1] + Image_Use_Robert[i][j - 1] + Image_Use_Robert[i][j + 1] + Image_Use_Robert[i + 1][j - 1] + Image_Use_Robert[i + 1][j] + Image_Use_Robert[i + 1][j + 1];

            if (num >= THRESHOLD_MAX && Image_Use_Robert[i][j] == 0) // 如果黑点四周的8个点只有2个黑点
            {
                Image_Use_Robert[i][j] = 255;
            }
            if (num <= THRESHOLD_MIN && Image_Use_Robert[i][j] == 255) // 如果白点周围只有2个白点
            {
                Image_Use_Robert[i][j] = 0; // 过滤成黑
            }
        }
    }
}

/**
 * @brief 绘制一个矩形，将图片的边缘变为黑色
 * @example Image_DrawRectangle();
 * @explain 该函数将图片的边缘变为黑色，以便后面的边缘提取,
 * 给图像绘黑边，不然八邻域会出错，位置一定要放在显示的前面
 */
void Image_DrawRectangle(void)
{
    uint8 i = 0;
    for (i = 0; i < IMAGE_HEIGHT; i++)
    {
        Image_Use[i][0] = 0;
        Image_Use[i][1] = 0;
        Image_Use[i][IMAGE_WIDTH - 1] = 0;
        Image_Use[i][IMAGE_WIDTH - 2] = 0;
    }
    for (i = 0; i < IMAGE_WIDTH; i++)
    {
        Image_Use[0][i] = 0;
        Image_Use[1][i] = 0;
    }
}

/**
 * @brief 通过两个点的坐标计算直线斜率和截距，并将结果存储在全局变量left_line中
 *
 * @param x1 第一个点的x坐标
 * @param y1 第一个点的y坐标
 * @param x2 第二个点的x坐标
 * @param y2 第二个点的y坐标
 */
void Image_pointsleft(uint8 x1, uint8 y1, uint8 x2, uint8 y2)
{
    float two_points_k, two_points_b;
    two_points_k = (float)((y2 - y1) / (x2 - x1));
    two_points_b = (float)((y1 * x2 - x1 * y2) / (x2 - x1));
    left_line.k = two_points_k;
    left_line.b = two_points_b;
}
/**
 * @brief 通过两个点的坐标计算直线斜率和截距，并将结果存储在全局变量right_line中
 *
 * @param x1 第一个点的x坐标
 * @param y1 第一个点的y坐标
 * @param x2 第二个点的x坐标
 * @param y2 第二个点的y坐标
 */
void Image_pointsright(uint8 x1, uint8 y1, uint8 x2, uint8 y2)
{
    float two_points_k, two_points_b;
    two_points_k = (float)((y2 - y1) / (x2 - x1));
    two_points_b = (float)((y1 * x2 - x1 * y2) / (x2 - x1));
    right_line.k = two_points_k;
    right_line.b = two_points_b;
}

/**
 * @brief 计算通过一组点的直线的斜率
 *
 * 此函数计算通过一组点的直线的斜率,接受线的起点和终点以及线上点的y坐标数组作为输入,返回直线的斜率
 *
 * @param begin 线的起点
 * @param end 线的终点
 * @return 通过一组点的直线的斜率
 */
float Imgae_SlopeLeft(uint8 begin, uint8 end)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16 i = 0;
    float result = 0;
    static float resultlast = 0;  // 记录上次结果，用来比对
    for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）

        for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）
        {
            xsum += Left[i].row;                     // 对x坐标自增（行坐标）
            ysum += Left[i].column;                  // 对y坐标自增（列坐标）
            xysum += Left[i].row * (Left[i].column); // xy坐标乘积自增
            x2sum += Left[i].row * Left[i].row;      // x坐标平方自增
        }
    if ((end - begin) * x2sum - xsum * xsum) // 这个在求斜率中会作为分母，因此需要判断是否为0
    {
        result = ((end - begin) * xysum - xsum * ysum) / ((end - begin) * x2sum - xsum * xsum);
        resultlast = result;
    }
    else
    {
        result = resultlast;
    }
    return result;
}
/**
 * @brief 计算通过一组点的直线的斜率
 * @param begin 线的起点
 * @param end 线的终点
 * @return 通过一组点的直线的斜率
 */
/**
 * @brief 计算右边车道线的斜率
 *
 * @param begin 起点下标
 * @param end 终点下标
 * @return float 斜率值
 */
float Imgae_SlopeRight(uint8 begin, uint8 end)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16 i = 0;
    float result = 0;
    static float resultlast = 0; // 记录上次结果，用来比对

    for (i = begin; i < end; i++) // 从起点开始向终点自增（这个应该指的是下标）
    {
        xsum += Right[i].row;                      // 对x坐标自增（行坐标）
        ysum += Right[i].column;                   // 对y坐标自增（列坐标）
        xysum += Right[i].row * (Right[i].column); // xy坐标乘积自增
        x2sum += Right[i].row * Right[i].row;      // x坐标平方自增
    }
    if ((end - begin) * x2sum - xsum * xsum) // 这个在求斜率中会作为分母，因此需要判断是否为0
    {
        result = ((end - begin) * xysum - xsum * ysum) / ((end - begin) * x2sum - xsum * xsum);
        resultlast = result;
    }
    else
    {
        result = resultlast;
    }
    return result;
}

/**
 * @brief 计算斜率和截距,配合上面的最小二乘法使用（求的是左边线的）
 * @param begin 线的起点
 * @param end 线的终点
 * @return 通过一组点的直线的斜率,通过全局变量
 * @example Image_CountLeftKB(1,45) 得到的方程为column=k*row+b;
 */
float slope_rate_l, intercept_l;
void Image_CountLeftKB_L(uint8 start, uint8 end)
{
    uint16 i, num = 0;
    uint16 xsum = 0, ysum = 0;
    float y_average, x_average;
    num = 0;
    xsum = 0;
    ysum = 0;
    y_average = 0;
    x_average = 0;
    for (i = start; i < end; i++) // 计算xy坐标的总值（行列坐标）
    {
        xsum += Left[i].row;    // 行坐标累加
        ysum += Left[i].column; // 列坐标累加
        num++;
    }
    // 计算x,y坐标的平均值
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);
    }
    slope_rate_l = Imgae_SlopeLeft(start, end);         // 计算斜率
    intercept_l = y_average - (slope_rate_l)*x_average; // 计算截距
}
/**
 * @brief 计算斜率和截距,配合上面的最小二乘法使用（求的是右边线的）
 * @brief 计算斜率和截距,配合上面的最小二乘法使用
 *
 *
 * @param begin 线的起点
 * @param end 线的终点
 * @param border 线上点的y坐标数组
 * @return 通过一组点的直线的斜率,通过指针来返回斜率和截距
 * @example Image_CountRightKB_R(1,45)
 */
float slope_rate_r, intercept_r;
void Image_CountRightKB_R(uint8 start, uint8 end)
{
    uint16 i, num = 0;
    uint16 xsum = 0, ysum = 0;
    float y_average, x_average;
    num = 0;
    xsum = 0;
    ysum = 0;
    y_average = 0;
    x_average = 0;
    for (i = start; i < end; i++) // 计算xy坐标的总值（行列坐标）
    {
        xsum += Right[i].row;    // 行坐标累加
        ysum += Right[i].column; // 列坐标累加
        num++;
    }
    // 计算x,y坐标的平均值
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);
    }
    slope_rate_r = Imgae_SlopeRight(start, end);        // 计算斜率
    intercept_r = y_average - (slope_rate_r)*x_average; // 计算截距
}

uint8 Left_Count, Right_Count;         // 记录左右边界点的个数
uint8 grow_left, grow_right;           // 记录左右边界在八邻域时寻点的相对位置
uint8 Left_Max = 200, Right_Max = 200; // 左右边界搜点时允许最大的搜点量
uint8 Boundary_search_end = 30;        // 搜寻行数的最高行

uint8 Gather_flag;
uint8 Gather_row[50]; // 两线会聚数组，用来存储行坐标
uint8 Gather_Count;   // 计数
/**
 * @brief 八邻域寻边界
 *
 * @param uint8 必须为经过边框处理Draw_Frame后的图像才可以进行八领域扫线
 * @return 无
 * @exception 1.我默认左上角为（0，0）
 * 2.巡线原理就是找到第一个点，然后找附近的8个点，再选择下一个点来作为衍生，将得到的点记录即可得到边界
 * 3.这里是从第start_row行的点开始寻找，因为要留一行做起始八邻域点，还有边界两行已经改成黑了
 * 4.生长方向表和坐标表（可以随便定，这里也是一种方法）
 * row-1 col-1      row-1 col       row-1 col+1         7   0   6
 * row   col-1      row   col       row   col+1         4   *   5
 * row+1 col-1      row+1 col       row+1 col+1         3   1   2
 */

void Image_Get_neighborhoods(uint8 (*Image_Use)[IMAGE_WIDTH])
{
    Left_Count = 0;
    Right_Count = 0;
    uint16 cur_row, cur_col;            // 当前行列
    uint8 Pixel_Change;                 // 灰白像素变化标志位——恢复原来的bug
    uint8 Pixel_Count_r, Pixel_Count_l; // 计算变化对应的下标值
    uint8 Pixel_row, Pixel_col;         // 记录变化像素点的坐标（右坐标）
    if (Image_Get_LeftPoint2(117))
    {
        Left[0].row = Image_Get_LeftPoint(117);
        Left[0].column = left_point - 1;
        Left[0].flag = 1;
        Left[0].grow = 2;      // 初始生长方向为2
        cur_row = Left[0].row; // 返回起始的行数
        cur_col = left_point;  // 这是全局变量，可以调用
        Left_Count++;
        while (Left_Max--) // 找140个
        {
            // 一 寻点生长
            // 0黑1白
            if (Image_Use_Robert[cur_row + 1][cur_col] == BLACK && Image_Use_Robert[cur_row + 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 0;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 1;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col - 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 2;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col] == WHITE)
            {
                if (Image_Use_Robert[cur_row][cur_col + 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col + 1] == WHITE &&
                    Image_Use_Robert[cur_row + 1][cur_col + 1] == WHITE)
                /*生长方向选择为6（正左），详情解析在Right里面*/
                {
                    Left[Left_Count].row = cur_row;
                    Left[Left_Count].column = cur_col + 1;
                    Left[Left_Count].flag = 1;
                    Left[Left_Count].grow = 6;
                    cur_row = Left[Left_Count].row;
                    cur_col = Left[Left_Count].column;
                    Left_Count++;
                }
                else
                {
                    if (cur_col != 2) // 处理分界点问题
                    {
                        Left[Left_Count].row = cur_row - 1;
                        Left[Left_Count].column = cur_col - 1;
                        Left[Left_Count].flag = 1;
                        Left[Left_Count].grow = 3;
                        cur_row = Left[Left_Count].row;
                        cur_col = Left[Left_Count].column; // 更新中心坐标点
                        Left_Count++;
                    }
                    else
                    {
                        Image_Use_Robert[cur_row - 1][cur_col] = BLACK; // 将跳变的白点变成黑色，下次就找得到了
                        Pixel_row = cur_row - 1;
                        Pixel_col = cur_col;
                        Pixel_Count_l = Left_Count; // 记录此时的计数值
                        Pixel_Change = 1;           // 1为变成了黑色
                    }
                }
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col] == BLACK && Image_Use_Robert[cur_row - 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 4;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 5;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col + 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 6;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 7;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else
            {
                break;
            }
            // 二 1.检验越界
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            // 2 黑框分点的还原
            if ((Left_Count - Pixel_Count_l) == 1)
            {
                Image_Use_Robert[Pixel_row][Pixel_col] = WHITE;
            }
        }
    }
    // 采取左右对称，后面好写
    /*
    5	4	3
    6		2
    7	0	1
    */
    if (Image_Get_RightPoint2(117))
    {
        Right[0].row = Image_Get_RightPoint(117);
        Right[0].column = right_point + 1;
        Right[0].flag = 1;
        Right[0].grow = 2;
        cur_row = Right[0].row;
        cur_col = right_point;
        Right_Count++;
        while (Right_Max--)
        {
            // 0黑1白
            if (Image_Use_Robert[cur_row + 1][cur_col] == BLACK && Image_Use_Robert[cur_row + 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 0;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 1;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col + 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 2;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col + 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col] == WHITE)
            {
                /*
                5 7检测为白，6为黑——检测竖直状态拐点问题（十字路口扫线扫不上去的问题）
                注：放在这里的话扫线速度更快（因为都是else if结构，提高算法的速度）
                */
                if (Image_Use_Robert[cur_row][cur_col - 1] == BLACK && Image_Use_Robert[cur_row - 1][cur_col - 1] == WHITE &&
                    Image_Use_Robert[cur_row + 1][cur_col - 1] == WHITE)
                /*生长方向选择为6（正左）*/
                {
                    Right[Right_Count].row = cur_row;
                    Right[Right_Count].column = cur_col - 1;
                    Right[Right_Count].flag = 1;
                    Right[Right_Count].grow = 6;
                    cur_row = Right[Right_Count].row;
                    cur_col = Right[Right_Count].column;
                    Right_Count++;
                }
                else // 提高优先级90°巡线判断的优先级
                {
                    if (cur_col != 157) // 处理分界点问题：当4黑 5白 6黑 7白 的时候，由于（右边）是逆时针扫描，就会有bug
                    {
                        Right[Right_Count].row = cur_row - 1;
                        Right[Right_Count].column = cur_col + 1;
                        Right[Right_Count].flag = 1;
                        Right[Right_Count].grow = 3;
                        cur_row = Right[Right_Count].row;
                        cur_col = Right[Right_Count].column;
                        Right_Count++;
                    }
                    else
                    {
                        Image_Use_Robert[cur_row - 1][cur_col] = BLACK; // 将跳变的白点变成黑色，下次就找得到了
                        Pixel_row = cur_row - 1;
                        Pixel_col = cur_col;
                        Pixel_Count_r = Right_Count; // 记录此时的计数值
                        Pixel_Change = 1;            // 1为变成了黑色
                    }
                }
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col] == BLACK && Image_Use_Robert[cur_row - 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 4;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row - 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 5;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row][cur_col - 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 6;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use_Robert[cur_row + 1][cur_col - 1] == BLACK && Image_Use_Robert[cur_row + 1][cur_col] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 7;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else
                break;
            // 下面是巡线以后其他的处理
            // 1  越界处理
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            // 2 变化灰度的还原的处理（因为不处理的话下次就还是会扫错）
            if ((Right_Count - Pixel_Count_r) == 1) // 确保是变黑的下一次就变白（记录次数，防止误扫）
            {
                Image_Use_Robert[Pixel_row][Pixel_col] = WHITE; // 一般都是白->黑->白
            }
            //			//下面是巡线以后其他的处理
            //            //一 越界处理
            //			if(cur_row<=2||cur_row>=(IMAGE_WIDTH-3)||cur_col<=2||cur_col>=(IMAGE_WIDTH-3))
            //			{
            //				break;
            //			}
            //            //以下是待测试的代码
            //            //二 连续3次都是同一个点就不再寻找
            //            if(Right[Right_Count].row==Right[Right_Count-1].row&&Right[Right_Count].column==Right[Right_Count-1].column&&
            //                Right[Right_Count-1].row==Right[Right_Count-2].row&&Right[Right_Count-1].column==Right[Right_Count-2].column)
            //            {
            //                break;
            //            }
            //            //三 当扫到的点多个在同一行时，只保留最后一个点（这个目前不写，因为处理的是压缩后的图像，点多一点也没关系，如果需要的话后期再改进）
            //            //四 当左右线会聚时退出循环，并记录此时的行数（判断坡道）
            //            if((Left[Left_Count-1].row==Right[Right_Count-1].row)&&(Left[Left_Count-1].column==Right[Right_Count-1].column||
            //            Right[Right_Count-1].column-Left[Left_Count-1].column<=3))
            //            //判断条件：行相等，列也相等or列相差在3以内
            //            {
            //                Gather_row[Gather_Count]=Left[Left_Count-1].row;
            //                Gather_flag=1;
            //                break;
            //            }
            //            else
            //            {
            //                Gather_flag=0;
            //            }
        }
    }
}

void Image_Get_neighborhoods2(uint8 (*Image_Use)[IMAGE_WIDTH])
{
    Left_Count = 0;
    Right_Count = 0;
    uint16 cur_row, cur_col;            // 当前行列
    uint8 Pixel_Change;                 // 灰白像素变化标志位——恢复原来的bug
    uint8 Pixel_Count_r, Pixel_Count_l; // 计算变化对应的下标值
    uint8 Pixel_row, Pixel_col;         // 记录变化像素点的坐标（右坐标）
    if (Image_Get_LeftPoint(117))
    {
        Left[0].row = Image_Get_LeftPoint(117);
        Left[0].column = left_point - 1;
        Left[0].flag = 1;
        Left[0].grow = 2;      // 初始生长方向为2
        cur_row = Left[0].row; // 返回起始的行数
        cur_col = left_point;  // 这是全局变量，可以调用
        Left_Count++;
        while (Left_Max--) // 找140个
        {
            // 一 寻点生长
            // 0黑1白
            if (Image_Use[cur_row + 1][cur_col] == BLACK && Image_Use[cur_row + 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 0;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col - 1] == BLACK && Image_Use[cur_row][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 1;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row][cur_col - 1] == BLACK && Image_Use[cur_row - 1][cur_col - 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 2;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col - 1] == BLACK && Image_Use[cur_row - 1][cur_col] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col - 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 3;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col] == BLACK && Image_Use[cur_row - 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 4;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col + 1] == BLACK && Image_Use[cur_row][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row - 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 5;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row][cur_col + 1] == BLACK && Image_Use[cur_row + 1][cur_col + 1] == WHITE)
            {
                Left[Left_Count].row = cur_row;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 6;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col + 1] == BLACK && Image_Use[cur_row + 1][cur_col] == WHITE)
            {
                Left[Left_Count].row = cur_row + 1;
                Left[Left_Count].column = cur_col + 1;
                Left[Left_Count].flag = 1;
                Left[Left_Count].grow = 7;
                cur_row = Left[Left_Count].row;
                cur_col = Left[Left_Count].column; // 更新中心坐标点
                Left_Count++;
            }
            else
            {
                break;
            }
            // 二 1.检验越界
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            // 2 黑框分点的还原
            if ((Left_Count - Pixel_Count_l) == 1)
            {
                Image_Use[Pixel_row][Pixel_col] = WHITE;
            }
        }
    }
    // 采取左右对称，后面好写
    /*
    5	4	3
    6		2
    7	0	1
    */
    if (Image_Get_RightPoint(117))
    {
        Right[0].row = Image_Get_RightPoint(117);
        Right[0].column = right_point + 1;
        Right[0].flag = 1;
        Right[0].grow = 2;
        cur_row = Right[0].row;
        cur_col = right_point;
        Right_Count++;
        while (Right_Max--)
        {
            // 0黑1白
            if (Image_Use[cur_row + 1][cur_col] == BLACK && Image_Use[cur_row + 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 0;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col + 1] == BLACK && Image_Use[cur_row][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 1;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row][cur_col + 1] == BLACK && Image_Use[cur_row - 1][cur_col + 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 2;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col + 1] == BLACK && Image_Use[cur_row - 1][cur_col] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col + 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 3;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col] == BLACK && Image_Use[cur_row - 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 4;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row - 1][cur_col - 1] == BLACK && Image_Use[cur_row][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row - 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 5;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row][cur_col - 1] == BLACK && Image_Use[cur_row + 1][cur_col - 1] == WHITE)
            {
                Right[Right_Count].row = cur_row;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 6;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else if (Image_Use[cur_row + 1][cur_col - 1] == BLACK && Image_Use[cur_row + 1][cur_col] == WHITE)
            {
                Right[Right_Count].row = cur_row + 1;
                Right[Right_Count].column = cur_col - 1;
                Right[Right_Count].flag = 1;
                Right[Right_Count].grow = 7;
                cur_row = Right[Right_Count].row;
                cur_col = Right[Right_Count].column;
                Right_Count++;
            }
            else
                break;
            // 下面是巡线以后其他的处理
            // 1  越界处理
            if (cur_row < 2 || cur_row > IMAGE_HEIGHT - 2 || cur_col < 1 || cur_col > IMAGE_WIDTH - 2)
            {
                break;
            }
            //			//下面是巡线以后其他的处理
            //            //一 越界处理
            //			if(cur_row<=2||cur_row>=(IMAGE_WIDTH-3)||cur_col<=2||cur_col>=(IMAGE_WIDTH-3))
            //			{
            //				break;
            //			}
            //            //以下是待测试的代码
            // 二 连续3次都是同一个点就不再寻找
            if (Right[Right_Count].row == Right[Right_Count - 1].row && Right[Right_Count].column == Right[Right_Count - 1].column &&
                Right[Right_Count - 1].row == Right[Right_Count - 2].row && Right[Right_Count - 1].column == Right[Right_Count - 2].column)
            {
                break;
            }
            //            //四 当左右线会聚时退出循环，并记录此时的行数（判断坡道）
            //            if((Left[Left_Count-1].row==Right[Right_Count-1].row)&&(Left[Left_Count-1].column==Right[Right_Count-1].column||
            //            Right[Right_Count-1].column-Left[Left_Count-1].column<=3))
            //            //判断条件：行相等，列也相等or列相差在3以内
            //            {
            //                Gather_row[Gather_Count]=Left[Left_Count-1].row;
            //                Gather_flag=1;
            //                break;
            //            }
            //            else
            //            {
            //                Gather_flag=0;
            //            }
        }
    }
}
uint8 Mid_Count;
/**
 * @brief 取中线函数（有bug，待验证）
 *
 * @param uint8 无
 * @example Get_Midpoint(void)
 */
void Get_Midpoint(void)
{
    uint8 left_c, right_c;
    uint8 i; // 中间值
    left_c = Left_Count;
    right_c = Right_Count; // 存入左右计数值
    for (i = 0; i < left_c; i--)
    {
        mid[i].row = (Left[i].row + Right[i].row) / 2;
        mid[i].column = (Left[i].column + Right[i].column) / 2;
    }
}

/**
 * @brief 十字补线函数
 *
 * @param uint8 已经经过八邻域扫线的图像Image_Use
 * @example Image_FillCross(Image_Use)
 * @explanation  处理十字不难，只需要找到4个拐点，然后补线即可，找到4个拐点的处理方法：1.判断多个点的生长方向 2.多个点构成的斜率的急剧变化 这里用方法1
 */
// 分别设置左上，左下，右上，右下四个拐点坐标
struct coordinate left_high;
struct coordinate left_low;
struct coordinate right_high;
struct coordinate right_low;
struct coordinate last_right_low; // 定义有拐点的上次坐标
void Image_FillCross(uint8 (*Image_Use)[IMAGE_WIDTH])
{
    uint8 i, j;             // 中间变量
    uint8 l_ready, r_ready; // 定义左右线扫完的标志位

    // 先扫左拐点，从左下角开始扫，左右线的计数值不一定相等
    // 两种写法，一种生长方向由5-->2，另外一种直接判断多个2（因为不能保证斜线一直是5），这里用方法一，后面可能会改，觉得这个不太行
    // 判断是否进入十字，或者未进入十字
    //    if(Image_Use[IMAGE_HEIGHT-4][4]==BLACK&&Image_Use[IMAGE_HEIGHT-4][6]==BLACK&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-6]==BLACK&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-4]==BLACK)

    for (i = 0; i < Left_Count; i++) // 实际上理想情况下是i不会=left_count，因为找到左下拐点时肯定还有很多点没找
    {
        /*plan2：生长方向优化*/
        if ((Left[i - 1].grow == 5 || Left[i - 4].grow == 5) && (Left[i - 2].grow == 5 || Left[i - 5].grow == 5) && (Left[i - 3].grow == 5 || Left[i - 6].grow == 5) && (Left[i + 1].grow != 5 || Left[i + 4].grow != 5) && (Left[i + 2].grow != 5 || Left[i + 5].grow != 5) && (Left[i + 3].grow != 5 || Left[i + 6].grow != 5))
        {
            left_low.row = Left[i].row;
            left_low.column = Left[i].column;
            left_low.index = i;
            left_low.flag = 1;
            break;
        }
    }
    // 找的是左上拐点，这里可以不放在同一个for循环，因为肯定是先找到左下拐点，然后才能找到左拐点
    //        for(i=0;i<Left_Count;i++)
    //        {
    //            //图片上方的有效点比较少，判断就简单一点，不然找不到，注意生长方向就行
    ////            if((Left[i-1].grow==6||Left[i-4].grow==6)&&(Left[i-2].grow==6||Left[i-5].grow==6)&&(Left[i-3].grow==6||Left[i-6].grow==6)&&
    ////				(Left[i+1].grow!=6||Left[i+4].grow!=6)&&(Left[i+2].grow!=6||Left[i+5].grow!=6)&&(Left[i+3].grow!=6||Left[i+6].grow!=6))
    //			// if(Left[i].grow!=5&&(Left[i-1].grow!=5||Left[i-4].grow!=5)&&(Left[i-2].grow!=5||Left[i-5].grow!=5)&&
    //			// 	(Left[i+1].grow==5||Left[i+4].grow==5)&&(Left[i+2].grow==5||Left[i+5].grow==5))
    //            // {
    //            //     left_high.row=Left[i].row;
    //            //     left_high.column=Left[i].column;
    //			// 	left_high.index=i;
    //            //     break;
    //            // }
    //            /*plan3 斜率最大法*/
    //            // if(Image_Getk(Left[i].column,Left[i].row-120)>)
    //        }
    //        //左右对称，生长方向一样，就不用改了
    for (i = 0; i < Right_Count; i++)
    {
        /*取点1*/
        //            if((Right[i-1].grow==5||Right[i-4].grow==5)&&(Right[i-2].grow==5||Right[i-5].grow==5)&&(Right[i-3].grow==5||Right[i-6].grow==5)
        //				&&(Right[i+1].grow==2||Right[i+4].grow==2)&&(Right[i+2].grow==2||Right[i+5].grow==2)&&(Right[i+3].grow==2||Right[i+6].grow==2)
        //			&&Right[i].row<(IMAGE_HEIGHT/2)&&my_abs(Right[i].row-Right[i+4].row)<3)
        /*取点2*/
        if ((Right[i - 1].grow == 5 || Right[i - 2].grow == 5) && (Right[i].column < Right[i - 1].column) && (Right[i + 1].grow == 2) && Right[i].row > (IMAGE_HEIGHT / 2) && ((Right[i - 3].row - Right[i].row) >= 1))
        /*取点3*/
        //			if(Right[i-1].grow==5&&(Right[i-2].grow==5||Right[i-3].grow==5)&&Right[i-4].grow==5&&Right[i].grow==5&&Right[i+1].grow!=5)
        {

            right_low.row = Right[i].row;
            right_low.column = Right[i].column;
            right_low.index = i;
            break;
        }
    }
    for (i = 0; i < Right_Count; i++)
    {
        if (Right[i].grow != 5 && (Right[i - 1].grow != 5 || Right[i - 4].grow != 5) && (Right[i - 2].grow != 5 || Right[i - 5].grow != 5) &&
            (Right[i + 1].grow == 5 || Right[i + 4].grow == 5) && (Right[i + 2].grow == 5 || Right[i + 5].grow == 5))
        {
            right_high.row = Right[i].row;
            right_high.column = Right[i].column;
            right_high.index = i;
            break;
        }
    }
    // 左边线补线
    if (left_low.index != 0) // 判断是否为0
    {
        left_line.k = Image_Getk((left_low.column - Left[left_low.index - 3].column), (left_low.row - Left[left_low.index - 3].row));
        left_line.b = Image_Getb(left_low.column, left_low.row, left_line.k);
        // 求出左线斜率和截距：column=k*row+b（十字的k一般都是1或-1）
        // 补线有两种方法：一种把全部的点都补上，一种只补两个拐点拐点，这里用第二种，后面再用基础扫线，扫出补线后的边线

        for (i = left_low.row; i > 10; i--) // 从左下拐点开始向上补线
        {
            int new_column_l = (int)(left_line.k * i + left_line.b);
            if (new_column_l > 0)
            {
                Image_Use[i][new_column_l] = BLACK; // 从拐点向上补线
                ips200_draw_line(0, 0, i, new_column_l, RGB565_RED);
            }
            else
            {
                break;
            }
        }
        // 记录上一次的斜率和截距
        last_left_line.k = left_line.k;
        last_left_line.b = left_line.b;
        // 记录完后清零
        left_low.index = 0;
        left_line.k = 0;
        left_line.b = 0;
    }
    else // 如果找不到的话（下标为0）
    {
        left_line.k = last_left_line.k; // 就用上次的斜率和截距
        left_line.b = last_left_line.b;
        for (i = left_low.row; i > 10; i--) // 从左下拐点开始向上补线
        {
            int new_column_l = (int)(left_line.k * i + left_line.b);
            if (new_column_l > 0)
            {
                Image_Use[i][new_column_l] = BLACK; // 从拐点向上补线
                ips200_draw_line(0, 0, i, new_column_l, RGB565_RED);
            }
            else
            {
                break;
            }
        }
    }
    // 右边线补线
    if (right_low.index != 0)
    {
        right_line.k = Image_Getk((right_low.column - Right[right_low.index - 3].column), (right_low.row - Right[right_low.index - 3].row));
        //			right_line.k=1.0;
        right_line.b = Image_Getb(right_low.column, right_low.row, right_line.k);
        for (i = right_low.row; i > 10; i--) // 从左下拐点开始向上补线
        {
            int new_column_r = (int)(right_line.k * i + right_line.b);
            if (new_column_r > 0)
            {
                Image_Use[i][new_column_r] = BLACK; // 从拐点向上补线
                ips200_draw_line(0, 0, new_column_r, i, RGB565_BLUE);
            }
            else
                break;
        }
        last_right_line.k = right_line.k;
        last_right_line.b = right_line.b;
        // 记录完后清零
        //            right_low.index=0;
        //            right_line.k=0;
        //            right_line.b=0;
    }
    else // 如果找不到的话（下标为0）
    {
        right_line.k = last_right_line.k; // 就用上次的斜率和截距
        right_line.b = last_right_line.b;
        for (i = right_low.row; i > 10; i--) // 从左下拐点开始向上补线
        {
            int new_column_r = (int)(right_line.k * i + right_line.b);
            if (new_column_r > 0)
            {
                Image_Use[i][new_column_r] = BLACK; // 从拐点向上补线
                ips200_draw_line(0, 0, i, new_column_r, RGB565_BLUE);
            }
            else
            {
                break;
            }
        }
    }
    //		Image_CountLeftKB_L(right_low.index,right_low.index-3);//求出右线的斜率和截距slope_rate_r
    //		right_line.k=slope_rate_r;
    //		right_line.b=intercept_r;//左线补线
    //		slope_rate_r=0.0;
    //		intercept_r=0.0;
    //        //补线有两种方法：一种把全部的点都补上，一种只补两个拐点拐点，这里用第二种，后面再用基础扫线，扫出补线后的边线
    //        for(i=right_low.row;i<IMAGE_HEIGHT-1;i++)//从左下拐点开始向上补线
    //        {
    //            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=BLACK;//补线,这里传入的k是浮点型,后面要转换
    //			//补线方程：column=k*row+b
    //            if(i<=IMAGE_HEIGHT-10)
    //            {
    //                r_ready=1;//左线补完了
    //                break;
    //            }
    //        }
    //        Image_pointsright(right_high.row,right_high.column,right_low.row,right_low.column);//求出右线的斜率和截距
    //        for(i=right_low.row;i<IMAGE_HEIGHT-1;i++)//从右下拐点开始补线
    //        {
    //            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=BLACK;//补线
    //            if(i>=IMAGE_HEIGHT-4)
    //            {
    //                r_ready=1;//右线补完了
    //                break;
    //            }
    //        }
    //
    //    /*如果未进十字的补线判断完成，那么接下来就要进入十字中途的补线，这里和前面的最大区别就是左下角和右下角是不是白色*/
    //		/*用的sobel就判断不出来左下角是不是黑色了*/
    ////    if(l_ready&&r_ready&&Image_Use[IMAGE_HEIGHT-4][4]==WHITE&&Image_Use[IMAGE_HEIGHT-4][6]==WHITE&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-6]==WHITE&&Image_Use[IMAGE_HEIGHT-4][IMAGE_WIDTH-4]==WHITE)//等左右线补完，只要补完就说明有过，那么就顺便看左下角和右小角是否为白色
    //
    //        //此时只有左上拐点和右上拐点，因为左下角和右下角都是白色，所以只需要找左上拐点和右上拐点，取直线就往拐点往上扫
    //        for(i=0;i<Left_Count;i++)
    //        {
    //            if(Left[i].grow==5&&Left[i-2].grow==5&&Left[i-5].grow==5
    //            &&Left[i-2].grow==6&&Left[i-4].grow==6&&Left[i-6].grow==6)
    //            {
    //                left_high.row=Left[i].row;
    //                left_high.column=Left[i].column;
    //                break;
    //            }
    //        }
    //        for(i=0;i<Right_Count;i++)
    //        {
    //            if(Right[i].grow==5&&Right[i-2].grow==5&&Right[i-5].grow==5
    //            &&Right[i-2].grow==6&&Right[i-4].grow==6&&Right[i-6].grow==6)
    //            {
    //                right_high.row=Right[i].row;
    //                right_high.column=Right[i].column;
    //                break;
    //            }
    //        }
    //        //求出拐点后开始补线,这里求斜率有三种方法:1.直接用两点(拐点和左下角)求斜率 2.用最小二乘法求斜率(用拐点往上的点) 3.用最小二乘法求斜率，但是只用拐点附近的点
    //        Image_pointsleft(left_high.row,left_high.column,IMAGE_HEIGHT-4,4);//求出左线的斜率和截距
    //        for(i=IMAGE_HEIGHT-4;i>4;i--)
    //        {
    //            Image_Use[i][(int)(left_line.k)*i+(int)(left_line.b)]=BLACK;//补线
    //            if(i<=left_high.row-1)
    //            {
    //                l_ready=1;//左线补完了
    //                break;
    //            }
    //        }
    //        Image_pointsright(right_high.row,right_high.column,IMAGE_HEIGHT-4,IMAGE_WIDTH-4);//求出右线的斜率和截距
    //        for(i=IMAGE_HEIGHT-4;i>4;i--)
    //        {
    //            Image_Use[i][(int)(right_line.k)*i+(int)(right_line.b)]=BLACK;//补线
    //            if(i<=right_high.row-1)
    //            {
    //                r_ready=1;//右线补完了
    //                break;
    //            }
    //        }
}

/*下面是直道函数的辅助函数*/
/**
 * @brief 获取图像的k值
 *
 * @param int16 derta_column,derta_row，坐标差值，别算反
 * @return float k值
 */
float Image_Getk(int16 derta_column, int16 derta_row)
{
    return (float)(derta_column / derta_row);
}
/*下面是直道函数的辅助函数*/
/**
 * @brief 获取图像的b值
 *
 * @param int16 example_row,example_column, k
 * @return float k值
 */
float Image_Getb(int16 example_column, int16 example_row, float k)
{
    return (float)(example_column - k * example_row);
}
/**
 * @brief 计算两个浮点数之间的绝对值差
 *
 * @param a 第一个浮点数
 * @param b 第二个浮点数
 * @return float 两个浮点数之间的绝对值差
 */
float Image_ab_value(float a, float b)
{
    if (a > b)
    {
        return a - b;
    }
    else
    {
        return b - a;
    }
}
float k_buff[5]; // 存放5个斜率值
/**
@brief 判断左直道函数，如果为直道，会通过返回值用来判断是否为直道
@param 二值化后的图像，实际上用不上
@return 1，则存在左直道，0就没有
@example Image_Stretch(void)
@note 将直线分为2个地方，看上下斜率相差是否相近，近的话就判断为直道
*/
uint8 Image_Stretch_Left(void)
{
    // uint8 i;
    // float k_left_low,k_left_high;//左下线斜率，左上线斜率
    // float k_all;//多断小斜率叠加
    // k_all=0.0;
    // for(i=0;i<5;i++)//求左线下10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Left[i].column,Left[i+5].column,(Left[i+5].row-Left[i].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_left_low=k_all/5;
    // k_all=0.0;
    // for(i=0;i<5;i++)//求左线上10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Left[40+i].column,Left[i+45].column,(Left[i+45].row-Left[i+40].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_left_high=k_all/5;
    // if(Image_ab_value(k_left_high,k_left_low)<LINE_K)//看直线的斜率是否符合要求
    // {
    //     return 1;
    // }
    // else
    // {
    //     return 0;
    // }
    return 1;
}

/**
@brief 判断右直道函数，如果为直道，通过返回值用来判断是否为直道
@param 二值化后的图像
@return 1，则存在右直道，0就没有
@example Image_Stretch_Right(void)
@note 将直线分为2个地方，看上下斜率相差是否相近，近的话就判断为直道
*/
uint8 Image_Stretch_Right(void)
{
    // uint8 i;
    // float k_right_low,k_right_high,k_all;
    // k_all=0.0;
    // for(i=0;i<5;i++)//求右线下10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Right[i].column,Right[i+5].column,(Right[i+5].row-Right[i].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_right_low=k_all/5;
    // k_all=0.0;
    // for(i=0;i<5;i++)//求右线上10个点的平均斜率
    // {
    //     k_buff[i]=Image_Getk(Right[40+i].column,Right[i+45].column,(Right[i+45].row-Right[i+40].row));//x是行坐标,y是列坐标
    // }
    // for(i=0;i<5;i++)
    // {
    //     k_all+=k_buff[i];
    // }
    // k_right_high=k_all/5;
    // if(Image_ab_value(k_right_high,k_right_low)<LINE_K)//看直线的斜率是否符合要求
    // {
    //     return 1;
    // }
    // else
    // {
    //     return 0;
    // }
    return 1;
}
float Image_B(uint8 point_row, uint8 point_column, float k)
{
    float b;
    b = k * point_row - point_column;
    return b;
}
/**
 * @brief 图像运行函数
 *
 * @param void
 * @example Image_Run(Image_Use)
 * @explanation  无
 */
// 图像处理的函数都放在这里，这样就避免了定义问题
void Image_Run(void)
{
    uint8 i, j;
    uint8 TH;
    //	i=Image_Get_LeftFlag(117);
    //	j=Image_Get_Rightflag(117);
    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
    //	Image_Binarization(TH,Image_Use);
    Image_DrawRectangle();
    Image_Filter();
    Image_Sobel(Image_Use, Image_Use_Robert, TH);                // 全局Sobel得二值图(方案二) 2.8ms
    ips200_displayimage03x((uint8 *)Image_Use_Robert, 160, 120); // pidMotor1Speed
    //	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count

    Image_Get_neighborhoods(Image_Use_Robert);
    //
    Image_FillCross(Image_Use_Robert);
    //	tft180_show_int(3,120,points_l[l_count-1][0],3);left_lineleft_line.b
    //	Image_Get_neighborhoods(100,Image_Use);left_high.index
    ips200_draw_line(0, 0, left_low.column, left_low.row, RGB565_RED);
    ips200_draw_line(0, 0, right_low.column, right_low.row, RGB565_BLUE);
    //    ips200_draw_line(60,80,left_high.column,left_low.row,RGB565_RED);
    //	ips200_draw_line(0,0,right_high.column,right_high.row,RGB565_BLUE);
    //	new_column
    ips200_show_int(3, 140, left_low.column, 3);
    ips200_show_int(3, 160, left_low.index, 3);
    ips200_show_int(3, 180, Left[left_low.index - 3].row, 3);
    ips200_show_int(3, 200, Left[left_low.index - 3].column, 3);

    ips200_show_int(43, 120, right_low.row, 3);
    ips200_show_int(43, 140, right_low.column, 3);
    ips200_show_int(43, 160, right_low.index, 3);
    ips200_show_int(43, 180, Right[right_low.index - 3].row, 3);
    ips200_show_int(43, 200, Right[right_low.index - 3].column, 3);

    ips200_show_float(3, 220, right_line.k, 4, 4);
    ips200_show_float(3, 240, right_line.b, 4, 4);

    //	Get_Midpoint();
    for (i = 0; i < Left_Count; i++)
    {
        ips200_draw_point(Left[i].column, Left[i].row, RGB565_RED);
        ips200_draw_point((Left[i].column) + 1, Left[i].row, RGB565_RED);
        //		ips200_draw_line(30,40,Left[i].column,Left[i].row,RGB565_RED);
    }
    for (i = 0; i < Right_Count; i++)
    {
        ips200_draw_point(Right[i].column, Right[i].row, RGB565_GREEN);
        ips200_draw_point((Right[i].column) + 1, Right[i].row, RGB565_GREEN);
    }

    //	if(Left_Count>195)
    //	{
    //		check();
    //	}
    //	for(i=0;i<Left_Count;i++)
    //	{
    //		tft180_draw_point((Left[i].column)/2+1,Left[i].row/2,RGB565_RED);
    //	}
    //	for(i=0;i<Left_Count;i++)
    //	{
    //		tft180_draw_point((Left[i].column)/2-1,Left[i].row/2,RGB565_RED);
    //	}
    //	for(i=0;i<Right_Count;i++)
    //	{
    //		tft180_draw_point(Right[i].column,Right[i].row,RGB565_RED);
    //	}

    Image_DrawRectangle();
    //	tft180_draw_line(0,0,start_point_Left[0],start_point_Left[1],RGB565_RED);//行坐标l_countl_count
    tft180_show_int(3, 80, left_point, 3);
    Image_Get_neighborhoods(Image_Use);
    //	tft180_show_int(3,120,points_l[l_count-1][0],3);
    //	tft180_draw_line(0,0,cur_col,cur_row,RGB565_RED);//行坐标l_countl_count
    //	Get_Midpoint();
    for (i = 0; i < Left_Count; i++)
    {
        tft180_draw_point(Left[i].column, Left[i].row, RGB565_BLUE);
    }
    for (i = 0; i < Left_Count; i++)
    {
        tft180_draw_point(Left[i].column + 1, Left[i].row, RGB565_BLUE);
    }
    for (i = 0; i < Left_Count; i++)
    {
        tft180_draw_point(Left[i].column - 1, Left[i].row, RGB565_BLUE);
    }
    for (i = 0; i < Right_Count; i++)
    {
        tft180_draw_point(Right[i].column, Right[i].row, RGB565_RED);
    }
}

/**
 * @brief 坡道判断函数
 *
 * @param void
 * @example Image_Run()
 * @explanation  判断左右线相交的行坐标不断增加，从而判断出为坡道
 * （想过用坡道的拐点左右的斜率来判断坡道，但是应该找不到）
 */
void Image_Ramp(void)
{
    uint8 i;                        // 中间变量
    uint8 Ramp_flag, Last_Rampflag; // 坡道标志位，以及上一个状态标志位
    if (Gather_flag == 1)           // 找到坡道
    {
        for (i = 0; i < Gather_Count; i++)
        {
            if ((Gather_row[i] > Gather_row[i + 1]) && (Gather_row[i + 1] > Gather_row[i + 2])) // 表示正在靠近坡道，最高行不断上升
            {
                Last_Rampflag = Ramp_flag; // 记录上一个状态
                Ramp_flag = 1;             // 标志位为1，表示还没上坡道，还在坡道附近的直道
            }
            else if ((Gather_row[i] < Gather_row[i + 1]) && (Gather_row[i + 1] < Gather_row[i + 2])) // 最高行变小，表示已经上坡道了
            {
                Last_Rampflag = Ramp_flag;
                Ramp_flag = 2; // 标志位为2，表示已经上坡道了，此时需要加速
                // Control_SpeedUp();//小车加速（电控的代码）
            }
            else
            {
                Last_Rampflag = Ramp_flag;
                Ramp_flag = 0;
            }
        }
    }
    if (!Gather_flag && !Ramp_flag && Last_Rampflag == 2) // 判断为下坡状态
    {
        // Control_SpeedDown();//小车减速（电控的代码）
    }
}

/**
 * @brief 计算两个整数的绝对值之差
 *
 * @param a 整数a
 * @param b 整数b
 * @return int 返回a和b的绝对值之差
 */
int abs_int(int a, int b)
{
    int diff = a - b;
    if (diff < 0)
    {
        return -diff;
    }
    else
    {
        return diff;
    }
}

/**
 * @brief 计算左侧图像中左边线生长方向与给定方向相同的数量
 *
 * @param Direction 给定的生长方向
 * @return uint8 相同方向的数量
 */
int Image_LeftGrowDirection(uint8 end, uint8 Direction)
{
    uint8 i, direction_count;
    for (i = 0; i < end; i++)
    {
        if (Left[i].grow == Direction) // 如果方向一致的话，就计数自增
        {
            direction_count++;
        }
    }
    return direction_count;
}
/**
 * @brief 计算右侧图像中右边线生长方向与给定方向相同的数量
 *
 * @param Direction 给定的生长方向
 * @return uint8 相同方向的数量
 */
int Image_RightGrowDirection(uint8 end, uint8 Direction)
{
    uint8 i, direction_count;
    for (i = 0; i < Right_Count; i++)
    {
        if (Right[i].grow == Direction) // 如果方向一致的话，就计数自增
        {
            direction_count++;
        }
    }
    return direction_count;
}
/**
 * @brief 左环岛判断并补线函数
 *
 * @param Image_Use
 * @example Image_LeftRound(Image_Use)
 * @explanation  右边线为直线，左边线较为弯曲，将整个环岛分为几个状态，所以搞个状态机
 * 1. 未进环岛，左边补线：直走
 * 2. 未进环岛，前面补线：左转
 * 3. 进入环岛，不用补线：一直左转
 * 4. 离开环岛，右边需要补线：左转
 * 5. 离开环岛，不用补线：一直左转
 * 6. 离开环岛，左边需要补线：直走（和状态1一样）
 */
struct coordinate Left_first_point;  // 左环岛的第一个拐点
struct coordinate Left_second_point; // 左环岛的第二个拐点
struct coordinate Right_first_point; // 右拐点
void Image_LeftRound(uint8 (*Image_Use)[IMAGE_WIDTH])
{
    uint8 LeftRound_State = 0;      // 左环岛状态1-5
    uint8 Last_LeftRound_State = 0; // 上一个状态
    uint8 i;                        // 中间变量
    uint8 Left_first_point_index;   // 记录左拐点在左边线数组的下标
    uint8 Left_second_point_index;
    uint8 Right_first_point_index;
    uint8 Left_third_point_index;
    uint8 index_temp; // 中间变量
    left_line.k = 0.0;
    left_line.b = 0.0;         // 清零
    uint8 BLACK_count;         // 计算黑点值
    if (Image_Stretch_Right()) // 先看右边是不是直道，右边直道只能判断1，2，5，6
    {
        for (i = 0; i < Left_Count; i++)
        {
            if (Left[i].grow == 5 && Left[i - 2].grow == 5 && Left[i - 5].grow == 5 && Left[i + 2].grow != 5 && Left[i + 3].grow != 5 && Left[i + 4].grow != 5 && Left[i + 5].grow != 5)
            // 通过判断生长方向是否还是为右上来判断
            {
                Left_first_point.row = Left[i].row;
                Left_first_point.column = Left[i].column; // 记录左环岛第一个拐点坐标
                Left_first_point_index = i;
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 1;
                break;
            }
            // 状态2的标志判断
            if (LeftRound_State == 1 && Last_LeftRound_State == 0 && Left[i].grow == 4 && Left[i + 1].grow == 4 && Left[i + 3].grow == 4) // 只是用来判断是否进入状态2的函数
            {
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 2; // 进入状态2
            }
            // 状态2的拐点判断
            if (LeftRound_State == 2 && Last_LeftRound_State == 1 && Left[i].grow == 5 && Left[i + 2].grow == 5 && Left[i - 2].grow != 5 && Left[i - 2].grow != 4) // 找到左边第二个拐点
            {
                Left_second_point_index = i; // 记录下标，其实只需要记录下标就行？
                break;
            }
            // 状态5的标志判断：1.右侧拐点消失（或右侧拐点太低了） 2. 左线生长方向向上 3.右线基本生长方向左上5
            // 4. 左右线相交  5.和状态3差不多，唯一不同的就是右线的生长方向为直线（可以求斜率来判断）
            // 这里来的做法是统计5方向的个数，如果大于3/4就认为是状态5（也可以判断斜率的直线）
            if (LeftRound_State == 4 && Last_LeftRound_State == 3 && (Image_RightGrowDirection(Right_Count, 5) >= ((Right_Count / 4) * 3)))
            {
                Last_LeftRound_State = LeftRound_State;
                LeftRound_State = 5; // 进入状态5
            }
        }
        // 状态1的补线
        if (Last_LeftRound_State == 0 && LeftRound_State == 1) // 通过状态机进行不同的补线，这里因为LeftRound_State取值，就要==1，不能直接写LeftRound_State
        {
            left_line.k = 0.0;
            left_line.b = 0.0; // 清零
            Image_pointsleft(Left[Left_first_point_index - 2].row, Left[Left_first_point_index - 2].column, Left[Left_first_point_index - 5].row, Left[Left_first_point_index - 5].column);
            // 求出左补线的斜率和截距
            for (i = Left[Left_first_point_index - 2].row; i > IMAGE_HEIGHT - 10; i--) // 从左下拐点，向上开始补线
            {
                if (Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] == BLACK) // 如果要变黑的点本来就是黑点
                {
                    BLACK_count++;
                }
                else
                {
                    Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] = BLACK; // column=k*row+b，这里补线方程和下面不一样
                }
                if (i <= IMAGE_HEIGHT - 13 || BLACK_count >= 3)
                {
                    Left_first_point_index = 0; // 补完下标索引清零
                    BLACK_count = 0;
                    break; // 左线补完了
                }
            }
        }
        // 状态2的补线
        if (Last_LeftRound_State == 1 && LeftRound_State == 2 && Left_second_point_index != 0) // 补线
        {
            left_line.k = 0.0;
            left_line.b = 0.0; // 补线前先清零
            Image_pointsleft(Left[Left_second_point_index].column, Left[Left_second_point_index].row,
                             Left[Left_second_point_index - 3].column, Left[Left_second_point_index - 3].row); // 这里补线是要跨过宽度的，因此需要变换直线
            for (i = Left[Left_second_point_index].column; i < 96; i++)                                        // 从左上拐点开始补线，向右补
            {
                if (Image_Use[(int)(left_line.k) * i + (int)(left_line.b)][i] == BLACK) // 如果要变黑的点本来就是黑点
                {
                    BLACK_count++; // 是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[(int)(left_line.k) * i + (int)(left_line.b)][i] = BLACK; // 如果是白色的话就变为黑点
                }
                // 这里的直线方程为:row=k*column+b
                if ((i >= IMAGE_WIDTH - 4) || (BLACK_count >= 3)) // 防止赛道上出现噪点
                {
                    Left_second_point_index = 0; // 补完下标索引清零
                    BLACK_count = 0;             // 记得清零
                    break;                       // 左线补完了
                }
            }
        }
    }
    /*状态3不用补线*/
    // 状态3的标志判断：1. 左边线和右边线会相交 2.左右边线起始点的列坐标相差较大 3.左线的生长方向基本都是正上（4）
    // 4.右线的列坐标的起始和终点相差较大
    else if (LeftRound_State == 2 && Last_LeftRound_State == 1 && (abs_int(Right[0].column, Left[0].column) > 80) && (Image_LeftGrowDirection(Left_Count, 4) >= (Left_Count / 2)) && (abs_int(Right[0].column, Right[Right_Count - 1].column) > 80))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 3; // 进入状态3，状态3就不用补线了，拐点也不用搞了
    }
    // 状态4的标志判断
    // 标志判断：1.左线的生长方向正上占比较少 2.右线的起点和终点列坐标相差较小 3.右线起始的列坐标和和终点的列坐标相差较小
    else if (LeftRound_State == 3 && Last_LeftRound_State == 2 && (Image_LeftGrowDirection(Left_Count, 4) <= (Left_Count / 4)) && (abs_int(Right[0].column, Right[Right_Count - 1].column) < 20))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 4; // 进入状态4
    }
    // 状态6的标志判断
    // 1. 右线的起始点和终止点的列坐标间隔较小   2. 左线向上生长方向的比例下降 3.左线的起点和终止点的列坐标间隔较大
    else if (LeftRound_State == 5 && Last_LeftRound_State == 4 && (abs_int(Left[0].column, Left[Left_Count - 1].column) > 80) && (abs_int(Right[0].column, Right[Right_Count - 1].column) < 20) && (Image_LeftGrowDirection(Left_Count, 4) <= (Left_Count / 4)))
    {
        Last_LeftRound_State = LeftRound_State;
        LeftRound_State = 6; // 进入状态6
    }
    /*下面是补线操作*/
    // 状态4的补线：先找右拐点，然后取拐点后退做线来补线
    if (LeftRound_State == 4 && Last_LeftRound_State == 3)
    {
        for (i = 0; i < Right_Count; i++)
        {
            if (Left[i].grow == 5 && (Left[i - 3].grow == 5 || Left[i - 2].grow == 5) && (Left[i - 4].grow == 5 || Left[i - 5].grow == 5) && (Left[i + 1].grow == 3 || Left[i + 2].grow == 3) && (Left[i + 3].grow == 3 || Left[i + 4].grow == 3))
            // 这里判断条件放松了，避免找不到，后期可以再改
            {
                Right_first_point_index = i; // 记录下标
                break;
            }
        }
        // 状态4补线：从右拐点向左补线，直线方程要变换
        if (Right_first_point_index != 0) // 确认一下找没找到拐点
        {
            right_line.k = 0.0;
            right_line.b = 0.0; // 清零
            Image_pointsright(Right[Right_first_point_index].column, Right[Right_first_point_index].row,
                              Right[Right_first_point_index - 3].column, Right[Right_first_point_index - 3].row); // 这里补线是要跨过宽度的，因此需要变换直线
            for (i = Right[Right_first_point_index].column; i > 0; i--)                                           // 从右下拐点开始补线，向左补
            {
                if (Image_Use[(int)(right_line.k) * i + (int)(right_line.b)][i] == BLACK) // 如果要变黑的点本来就是黑点
                {
                    BLACK_count++; // 是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[(int)(right_line.k) * i + (int)(right_line.b)][i] = BLACK; // 如果是白色的话就变为黑点
                }
                // 这里的直线方程为:row=k*column+b，列的点比较多
                if ((i <= IMAGE_WIDTH - 4) || (BLACK_count >= 3)) // 防止赛道上出现噪点
                {
                    Right_first_point_index = 0; // 补完下标索引清零
                    BLACK_count = 0;             // 记得清零
                    break;                       // 右线补完了
                }
            }
        }
    }
    // 下面是状态5的补线：
    if (LeftRound_State == 5 && Last_LeftRound_State == 4)
    {
        // 好像不用补，直接扫线就行
    }
    // 下面是状态6的补线：先找左拐点，然后取左拐点的右上点补线
    if (LeftRound_State == 6 && Last_LeftRound_State == 5)
    {
        for (i = 0; i < Left_Count; i++)
        {
            /*防函数报错*/
            // if(Image_ab_value((Image_Getk(Left[i].column,Left[i-3].column,abs_int(Left[i-3].row,Left[i].row))),
            // Image_Getk(Left[i].column,Left[i+3].column,abs_int(Left[i+3].row,Left[i].row)))>0.4)
            // {
            //     Left_third_point_index=i;//记录下标
            //     break;
            // }
        }
        // 补线是沿着行向下补，故直线方程为column=row*k+b
        if (Left_third_point_index != 0)
        {
            left_line.k = 0.0;
            left_line.b = 0.0; // 清零
            Image_pointsleft(Left[Left_third_point_index].row, Left[Left_third_point_index].column,
                             Left[Left_third_point_index + 3].row, Left[Left_third_point_index + 3].column); // 向下补线
            for (i = Left[Left_third_point_index].row; i < 56; i++)                                          // 从左下拐点开始补线，向左下补
            {
                if (Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] == BLACK) // 如果要变黑的点本来就是黑点
                {
                    BLACK_count++; // 是黑点的话就说明补超过右边线了，这里就不补了
                }
                else
                {
                    Image_Use[i][(int)(left_line.k) * i + (int)(left_line.b)] = BLACK; // 如果是白色的话就变为黑点
                }
                // 这里的直线方程为:row=k*column+b，列的点比较多
                if ((i >= IMAGE_HEIGHT - 4) || (BLACK_count >= 3)) // 防止赛道上出现噪点
                {
                    Left_third_point_index = 0; // 补完下标索引清零
                    BLACK_count = 0;            // 记得清零
                    break;                      // 左线补完了
                }
            }
        }
    }
}

/**
 * @brief 扫描60行中的某一行的黑白跳变的函数（检测斑马线）
 *
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */
uint8 Image_Scan_Row(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_row)
{
    uint8 i;                     // 中间变量
    uint8 BLACK_WHITE_count = 0; // 黑白跳变的计数
    for (i = 0; i < IMAGE_WIDTH - 1; i++)
    {
        if (Image_Use[target_row][i] != Image_Use[target_row][i + 1]) // 如果不相等的话，就说明是黑白跳变
        {
            BLACK_WHITE_count++;
        }
    }
    return BLACK_WHITE_count;
}

/**
 * @brief 扫描100行中的某一行的黑白跳变的函数（检测斑马线）
 *
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 *
 * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
 * @return uint8 返回黑白跳变的次数
 */

// 限幅函数
int clip(int x, int low, int up)
{
    return x > up ? up : x < low ? low
                                 : x;
}

/*************************************************************************
 *  函数名称：void blur_points();
 *  功能说明：点集三角滤波
 *  参数说明：输入边线数组，边线总个数，输出边线数组，点集范围
 *  函数返回：无
 *  修改时间：2022年10月4日
 *  备    注：
 * 例如：kernel = 5
 *  xi = (0*xi-3 + 1*xi-2 + 2*xi-1 + 3*xi + 2*xi+1 + 1*xi+2 + 0*xi+3)/9
 *  yi 同理
 *************************************************************************/

void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++)
        {
            pts_out[i][0] += (float)pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
            pts_out[i][1] += (float)pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}
/**
 * @brief 左线的三角滤波
 *
 * @param int num 左线的点数总个数 int kernel 内核大小
 * @return 无，输出值为边线Left_New数组
 * @example void Image_blur_points_Left(Left_Count,5);
 */
void Image_blur_points_Left(int num, int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        Left_New[i].row = Left_New[i].column = 0;
        for (int j = -half; j <= half; j++)
        {
            Left_New[i].row += (float)Left[clip(i + j, 0, num - 1)].row * (half + 1 - abs(j));
            Left_New[i].column += (float)Left[clip(i + j, 0, num - 1)].column * (half + 1 - abs(j));
        }
        Left_New[i].row /= (2 * half + 2) * (half + 1) / 2;
        Left_New[i].column /= (2 * half + 2) * (half + 1) / 2;
        Left_New[i].grow = Left[i].grow;
    }
}
/**
 * @brief 右线的三角滤波
 *
 * @param int num 右线的点数总个数 int kernel 内核大小
 * @return 无
 * @example void Image_blur_points_Left(Left_Count,5);
 */
void Image_blur_points_Right(int num, int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        Right_New[i].row = Right_New[i].column = 0;
        for (int j = -half; j <= half; j++)
        {
            Right_New[i].row += (float)Right[clip(i + j, 0, num - 1)].row * (half + 1 - abs(j));
            Right_New[i].column += (float)Right[clip(i + j, 0, num - 1)].column * (half + 1 - abs(j));
        }
        Right_New[i].row /= (2 * half + 2) * (half + 1) / 2;
        Right_New[i].column /= (2 * half + 2) * (half + 1) / 2;
        Right_New[i].grow = Right[i].grow;
    }
}

uint8 Image_Scan_Column(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_column)
{
    uint8 i;
    uint8 BLACK_WHITE_count = 0; // 黑白跳变的计数
    uint8 black_white_count = 0; // 黑白跳变的计数
    for (i = 0; i < IMAGE_HEIGHT; i++)
    {
        if (Image_Use[i][target_column] != Image_Use[i + 1][target_column]) // 如果不相等的话，就说明是黑白跳变
        {
            black_white_count++;
        }
    }
    return black_white_count;
}

uint8 touch_boundary0; // 左边线走到图像左边界
uint8 touch_boundary1; // 右边线走到图像右边界

uint8 touch_boundary_up0; // 左边线走到图像上边界
uint8 touch_boundary_up1; // 右边线走到图像上边界

#define ROAD_WIDTH (0.39)    // 赛道宽度45cm 适时调整 注意：应用方案三时情况特殊为负数-0.40,正常0.43
#define POINTS_MAX_LEN (150) // 边线点最多的情况——>num

image_t img_raw = DEF_IMAGE(NULL, UCOL, UROW);

// 左右边丢线
uint8 loseline0;
uint8 loseline1;
int ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int ipts0_num;                // 存放边线像素点个数(左)
int ipts1_num;                // 存放边线像素点个数(右)

// 逆透视变换后左右边线
float rpts0[POINTS_MAX_LEN][2];
float rpts1[POINTS_MAX_LEN][2];
int rpts0_num, rpts1_num;

// 逆透视变换后左右边线再三角滤波后的边线数组
float rpts0b[POINTS_MAX_LEN][2];
float rpts1b[POINTS_MAX_LEN][2];
int rpts0b_num, rpts1b_num;

// 逆透视变换后左右边线再三角滤波后再等距采样的数组
float rpts0s[POINTS_MAX_LEN][2];
float rpts1s[POINTS_MAX_LEN][2];
int rpts0s_num, rpts1s_num;
int rpts0an_num, rpts1an_num;
int rpts0_num, rpts1_num;
int x0_first, y0_first, x1_first, y1_first; // 左右边线第一个点的坐标

int x1, y1;
int x2, y2;

int Mid_line[POINTS_MAX_LEN][2]; // 定义中线数组
int mid_num;                     // 中线数组点数

int rpts0a_num, rpts1a_num;

float rpts0a[POINTS_MAX_LEN];
float rpts1a[POINTS_MAX_LEN];

// 左右边线局部角度变化率+非极大抑制
float rpts0an[POINTS_MAX_LEN];
float rpts1an[POINTS_MAX_LEN];

int rptsc0_num, rptsc1_num;
// SOBEL二值化图像

void test(void)
{
    //    int th;
    uint8 i;
    Image_Compress();
    int TH;
    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
    //    Image_Binarization(TH, Image_Use);
    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms

    img_raw.data = *Image_Use_Robert;

    Find_Borderline();

    Pespective(ipts0, ipts0_num, rpts0);
    rpts0_num = ipts0_num;

    Pespective(ipts1, ipts1_num, rpts1);
    rpts1_num = ipts1_num;

    // 三角滤波
    blur_points(rpts0, rpts0_num, rpts0b, (int)round(line_blur_kernel));
    rpts0b_num = rpts0_num;
    blur_points(rpts1, rpts1_num, rpts1b, (int)round(line_blur_kernel));
    rpts1b_num = rpts1_num;

    // 边线等距采样，num为逆透视后实际距离点，两点间距离为0.02*102=2cm
    rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]); // 求数组的长度 即等距采样后边线点个数
    resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
    rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
    resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);


    // 边线局部角度变化率 round():四舍五入
    local_angle_points(rpts0s, rpts0s_num, rpts0a, (int)round(angle_dist / sample_dist));
    rpts0a_num = rpts0s_num;
    local_angle_points(rpts1s, rpts1s_num, rpts1a, (int)round(angle_dist / sample_dist));
    rpts1a_num = rpts1s_num;
    // 角度变化率非极大抑制
    nms_angle(rpts0a, rpts0a_num, rpts0an, (int)round(angle_dist / sample_dist) * 2 + 1);
    rpts0an_num = rpts0a_num;
    nms_angle(rpts1a, rpts1a_num, rpts1an, (int)round(angle_dist / sample_dist) * 2 + 1);
    rpts1an_num = rpts1a_num;

    for (i = 0; i < ipts1_num; i++)
    {
        ips200_draw_line(0, 0, ipts0[i][0], ipts0[i][1] + 200, RGB565_RED);
    }
    for (i = 0; i < ipts1_num; i++)
    {
        ips200_draw_line(0, 0, ipts1[i][0], ipts1[i][1] + 200, RGB565_BLUE);
    }

    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            ips200_draw_point(i, j, RGB565_BLACK); // 0xffff=255区域内显示白点
        }
    }

    for (int i = 0; i < rpts0s_num; i++) // 显示左边线
    {
        uint16 x, y;

        //[60][80]rpts0s rpts0s_num ipts0_num

        x = func_limit_ab(rpts0s[i][0], 0, 200);
        y = func_limit_ab(rpts0s[i][1], 0, 199);

        ips200_draw_point(x, 200 - y, RGB565_GREEN); // 左线为绿色 不知道为什么改成-x/2+50就能正常先显示

        // tft180_draw_point( x/2 +50 -1 , 99-y/2 ,  RGB565_GREEN);
        // tft180_draw_point( x/2 +50 +1, 99-y/2 ,  RGB565_GREEN);
        // tft180_draw_point( x/2 +50 , 99-y/2 ,  RGB565_GREEN);
        /*
        x = func_limit_ab (ipts0[i][0] , -99,99);
        y = func_limit_ab (ipts0[i][1] , 0,  199);
        tft180_draw_point( x , y ,  RGB565_GREEN);
        */
        /*
        x = func_limit_ab(rpts0s[i][0] , 0,80);
        y = func_limit_ab(rpts0s[i][1] , 0,60);
        tft180_draw_point(x, y,  0x0000);//左线为绿色
        */
    }

    for (int i = 0; i < rpts0_num; i++) // 显示右边线
    {
        uint16 x, y;

        //[60][80]rpts1s rpts1s_num

        x = func_limit_ab(rpts1s[i][0], 0, 200);
        y = func_limit_ab(rpts1s[i][1], 0, 199);

        ips200_draw_point(x, 200 - y, RGB565_YELLOW);
        // tft180_draw_point( x/2 +50 -1 , 99-y/2 ,  RGB565_YELLOW);
        // tft180_draw_point( x/2 +50 +1, 99-y/2 ,  RGB565_YELLOW);
        // tft180_draw_point( x/2 +50 , 99-y/2 ,  RGB565_YELLOW);
        /*
        x = func_limit_ab (ipts0[i][0] , -99,99);
        y = func_limit_ab (ipts0[i][1] , 0,  199);
        tft180_draw_point( x , y ,  RGB565_GREEN);

        x = func_limit_ab(rpts1s[i][0] , 0,80);
        y = func_limit_ab(rpts1s[i][1] , 0,60);
        tft180_draw_point(x, y,  0x0000);//右线为黄色
        */
    }

    // ips200_show_int(3,140,ipts0_num,3);

    // ips200_show_int(3,160,loseline0,3);
    // ips200_show_int(3,180,*num,3);
    //	tft180_draw_line(0,0,ipts0[20-1][1],ipts0[20-1][0],RGB565_RED);
    //	tft180_show_int(3,100,ipts0[20-1][1],4);

    //	bluetooth_ch9141_send_image((const uint8 *)Image_Use_Robert, 19200);
    //    Get_Midline(ipts0,ipts0_num,ipts1,ipts1_num);
}

/**
 * @brief 获取左右车道线的中线
 *
 * @param pts_l 左车道线点集
 * @param pts_l_num 左车道线点数
 * @param pts_r 右车道线点集
 * @param pts_r_num 右车道线点数
 */
/*
    2023/10/08优化（待测试）：1.判定弯道的丢线问题 2.同时舍弃掉过高以上的中线
修改以后测试成功，不过还是要继续优化
*/
void Get_Midline(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num)
{
    int i;
    /*丢线判定*/
    if (loseline0 == 1) // 左边丢线
    {
        mid_num = pts_r_num; // 中线点数取右边的点数
        for (i = 0; i < mid_num; i++)
        {
            Mid_line[i][0] = pts_r[i][0] / 2;
            Mid_line[i][1] = pts_r[i][1];
            if (Mid_line[i][0] <= 40) // 如果中线生长到2/3
                break;
        }
    }
    /*右边丢线*/
    else if (loseline1 == 1)
    {
        mid_num = pts_l_num;
        for (i = 0; i < mid_num; i++)
        {
            Mid_line[i][0] = (IMAGE_WIDTH - pts_l[i][0]) / 2 + pts_l[i][0];
            Mid_line[i][1] = pts_l[i][1];
            if (Mid_line[i][1] <= 45) // 如果中线生长到2/3
                break;
        }
    }
    /*两边都不丢线*/
    else
    {
        /*取两个边线中最小的一个做数组*/
        if (pts_l_num > pts_r_num)
        {
            mid_num = pts_r_num;
        }
        else
            mid_num = pts_l_num;
        /*取中线*/
        for (i = 0; i < mid_num; i++)
        {
            Mid_line[i][0] = (pts_l[i][0] + pts_r[i][0]) / 2;
            Mid_line[i][1] = (pts_l[i][1] + pts_r[i][1]) / 2;
            if (Mid_line[i][0] <= 40) // 如果中线生长到2/3
                break;
        }
    }
    /*显示*/
    for (i = 0; i < ipts0_num; i++)
    {
        ips200_draw_point(Mid_line[i][0], Mid_line[i][1], RGB565_RED);
        ips200_show_uint(3, 140, Mid_line[i][0], 3);
        ips200_show_uint(3, 160, Mid_line[i][1], 3);
    }
    for (i = 0; i < ipts1_num; i++)
    {
        ips200_draw_line(0, 0, pts_r[i][0], pts_r[i][1], RGB565_RED);
    }
    ips200_show_uint(3, 180, loseline0, 3);
    ips200_show_uint(3, 200, loseline1, 3);
}

void Get_Midline2(int pts_l[][2], int pts_l_num, int pts_r[][2], int pts_r_num)
{
    int i, mid_num;
    /*取两个边线中最小的一个做数组*/
    if (pts_l_num > pts_r_num)
    {
        mid_num = pts_r_num;
    }
    else
    {
        mid_num = pts_l_num;
    }
    /*取中线*/
    for (i = 0; i < mid_num; i++)
    {
        Mid_line[i][0] = (pts_l[i][0] + pts_r[i][0]) / 2;
        Mid_line[i][1] = (pts_l[i][1] + pts_r[i][1]) / 2;
    }
    /*显示*/
    for (i = 0; i < ipts0_num; i++)
    {
        if (Mid_line[i][0] <= 60)
        {
            break;
        }
        ips200_draw_point(Mid_line[i][0], Mid_line[i][1], RGB565_RED);
    }
}

#define TARGET_ANGEL // 目标机械中值对应的角度

/**
 * @brief 计算给定中线的偏差（只适用于直道）
 *
 * @param pts_in 给定点集
 * @param num 点集中点的数量
 * @return float 误差值，返回给舵机
 */
float Get_err1(int pts_in[][2], int num)
{
    float k, err, last_err;
    k = LineRession(Mid_line, mid_num); // 对中线线性回归，先对中线进行处理了，所以不会超过80行
    err = -k;                           // 符合舵机的斜率，左负右正
    err = err * 0.8 + last_err * 0.2;   // 滤波
    last_err = err;                     // 更新上一次的偏差值
    return err;
}

void Find_Borderline(void)
=======
// float Image_ab_value(float a, float b)
// {
//     if (a > b)
//     {
//         return a - b;
//     }
//     else
//     {
//         return b - a;
//     }
// }

// void Find_Borderline_Second(void)
// {
//     int x1, y1;
//     int x2, y2;
//     uint8 uthres = 1;
//     if (touch_boundary0 == 1)
//     {
//         // 迷宫巡线是否走到左右边界
//         // touch_boundary00 = 0; // 清零
//         // touch_boundary11 = 0;

//         // // 迷宫巡线是否走到上边界
//         // touch_boundary_up00 = 0; // 清零
//         // touch_boundary_up11 = 0;

//         // // 底边扫线防止丢线 注意：由于sobel边缘检测特殊性（黑框），此丢线标志仅适用Ostu方案
//         // loseline00 = 0;
//         // loseline11 = 0;

//         /*添加*/
//         //	begin_y=Image_Get_LeftPoint(117);ipts0_num

//         //    uint8 uthres = ostu();
//         // 寻左边线
//         x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
//         int TH;

//         // 标记种子起始点(后续元素处理要用到)
//         // x0_first = x1;
//         // y0_first = ipts0[ipts0_num-1][1]-5;

//         // ipts00_num = sizeof(ipts00) / sizeof(ipts00[0]); // 求数组的长度
//         // // 扫底下五行，寻找跳变点
//         // for (; y0_first >20; y0_first--)//从所选的行，向上扫5次，每次从中间向左线扫
//         // {
//         //     for (; x0_first > 0; x0_first--)//在选的每行中，从中间向左线扫
//         //         if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)//如果扫到黑点（灰度值为0），就从该点开始扫线
//         //             goto out1;//开始扫左线
//         //     x0_first = img_raw.width / 2 - begin_x;//每次每一行扫完，都把x0_first归位
//         // }
//         x0_first = 10;
//         y0_first = ipts0[ipts1_num - 0][1] - 5;

//         Far_ipts0_num = sizeof(Far_ipts0) / sizeof(Far_ipts0[0]); // 求数组的长度
//         // 扫底下五行，寻找跳变点
//         for (; y0_first > 10; y0_first--) // 从所选的行，向上扫5次，每次从中间向左线扫
//         {
//             if (AT_IMAGE(&img_raw, x0_first, y0_first) < uthres) // 如果扫到黑点（灰度值为0），就从该点开始扫线
//             {
//                 goto out1; // 开始扫左线
//             }
//         }
//         // 如果扫不到的话，判定左边的底边丢线
//         // loseline00 = 1; // 底边丢线
//     out1: // 从起始点开始执行扫线
//     {
//         // if (AT_IMAGE(&img_raw, x0_first+1, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
//         Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, Far_ipts0, &Far_ipts0_num); // 开始跑迷宫
//         // else
//         // 	ipts00_num = 0;//如果不是的话，就不用跑了，求得的number记为0
//     }
//     }
//     if (touch_boundary1 == 1)
//     {
//         // 寻右边线
//         x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

//         // 标记种子起始点(后续元素处理要用到)
//         x1_first = 149;
//         y1_first = ipts1[ipts1_num - 1][1] - 5;
//         ;

//         Far_ipts1_num = sizeof(Far_ipts1) / sizeof(Far_ipts1[0]);
//         for (; y1_first > 10; y1_first--)
//         {
//             if (AT_IMAGE(&img_raw, x1_first, y1_first) < uthres)
//             {
//                 goto out2;
//             }
//         }
//         // loseline11 = 1; // 底边丢线
//     out2:
//     {
//         Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, Far_ipts1, &Far_ipts1_num);
//     }
//     }
// }

// /**
//  * @brief 扫描100行中的某一行的黑白跳变的函数（检测斑马线）
//  *
//  * @param uint8(*Image_Use)[IMAGE_WIDTH] 二值化后的图像 target_row：目标行
//  * @return uint8 返回黑白跳变的次数
//  */
// uint8 Image_Scan_Column(uint8 (*Image_Use)[IMAGE_WIDTH], uint8 target_column)
// {
//     uint8 i;
//     uint8 black_white_count = 0; // 黑白跳变的计数
//     for (i = 0; i < IMAGE_HEIGHT; i++)
//     {
//         if (Image_Use[i][target_column] != Image_Use[i + 1][target_column]) // 如果不相等的话，就说明是黑白跳变
//         {
//             black_white_count++;
//         }
//     }
//     return black_white_count;
// }

// float begin_x = 8;  // 起始点距离图像中心的左右偏移量
// float begin_y = 58; // 起始点距离图像底部的上下偏移量 120高度：35;100高

// float block_size = 7; // 自适应阈值的block大小
// float clip_value = 2; // 自适应阈值的阈值裁减量
//  SOBEL二值化图像
void BorderLine_Find(void)
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
{
    // 迷宫巡线是否走到左右边界
    touch_boundary0 = 0; // 清零
    touch_boundary1 = 0;

    // 迷宫巡线是否走到上边界
    touch_boundary_up0 = 0; // 清零
    touch_boundary_up1 = 0;

    // 底边扫线防止丢线 注意：由于sobel边缘检测特殊性（黑框），此丢线标志仅适用Ostu方案
    loseline0 = 0;
    loseline1 = 0;

    uint8 uthres = 1;
    //    uint8 uthres = ostu();
    // 寻左边线
    x1 = img_raw.width / 2 - begin_x, y1 = begin_y;

    // 标记种子起始点(后续元素处理要用到)
    x0_first = x1;
    y0_first = y1;

    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]); // 求数组的长度
    // 扫底下五行，寻找跳变点
    for (; y0_first > begin_y - 5; y0_first--)
    {
        for (; x0_first > 0; x0_first--)
            if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)
                goto out1;
        x0_first = img_raw.width / 2 - begin_x;
    }
    loseline0 = 1; // 底边丢线
out1:
{
    if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres)
        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);
    else
        ipts0_num = 0;
}

    // 寻右边线
    x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

    // 标记种子起始点(后续元素处理要用到)
    x1_first = x2;
    y1_first = y2;

    ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
    for (; y1_first > begin_y - 5; y1_first--)
    {
        for (; x1_first < img_raw.width - 1; x1_first++)
            if (AT_IMAGE(&img_raw, x1_first + 1, y1_first) < uthres)
                goto out2;
        x1_first = img_raw.width / 2 + begin_x;
    }
    loseline1 = 1; // 底边丢线
out2:
{
    if (AT_IMAGE(&img_raw, x1_first, y1_first) >= uthres)
        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts1, &ipts1_num);
    else
        ipts1_num = 0;
}
}

#define AT AT_IMAGE

void Left_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
{
    zf_assert(img && img->data); // 不满足则退出执行
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1); // 保证block_size为奇数
    //    int half = block_size / 2;                       //上交方案
    int half = 0;                                                                                                      // 方案二
    int step = 0, dir = 0, turn = 0;                                                                                   // step表示前进的步数；dir通过改变索引改变当前小人朝向的方向
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4) // 保证block不出界
    {
        int local_thres = 1;
        //        int local_thres;
        // 自适应二值化
        /*
            for (int dy = -half; dy <= half; dy++)  // for循环用来计算block区域的像素值之和（自适应阈值）
            {
                for (int dx = -half; dx <= half; dx++)
                {
                    local_thres += AT(img, x + dx, y + dy);
                }
            }
            local_thres /= block_size * block_size;
            local_thres -= clip_value;   // (x,y)点block区域内的阈值
        */
        int current_value = AT(img, x, y);                                                   // 当前像素点灰度值
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);             // 正前方像素点灰度值 （dir=0 下；dir=1 右；dir=2 上；dir=3 左）
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]); // 左前方像素点灰度值 （dir=0左下；dir=1 右下；dir=2 右上；dir=3 左上 ）
<<<<<<< HEAD
        //=======添加部分=======
        if ((x == 1 && y < img->height - 10) || x == img->width - 2 || y == 1 || (y == 100 && step > 19))
        {
            if (x == 1 /*|| x== img->width - 2*/)
                touch_boundary0 = 1; // 左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
            if (y == 1)
                touch_boundary_up0 = 1; // 走到顶边，判断坡道or障碍
=======
        //=======添加部分=======（限制条件）
        /*  当扫点的列坐标到左黑框边界且行坐标小于20    列坐标到右边的黑框边界  行坐标为1   行坐标为88的同时步数已经大于19*/
        if ((x == 2 && y < img->height - 50) || x == img->width - 2 || y == 2 || (y == 10 && step > 19)) // 30修改后能扫线
        {
            if (x == 2 /*|| x== img->width - 2*/) // 限制迷宫巡线的左边界
                touch_boundary0 = 1;              // 左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
            if (y == 2)                           // 限制迷宫巡线的高度
                touch_boundary_up0 = 1;           // 走到顶边，判断坡道or障碍
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
            break;
        }
        //=======添加部分=======
        if (front_value < local_thres) // 前进方向像素为黑色
        {
            dir = (dir + 1) % 4; // 遇到前方为黑色需要右转一次
            turn++;
        }
        else if (frontleft_value < local_thres) // 前方像素为白色，且左前方像素为黑色
        {

            x += dir_front[dir][0];
            y += dir_front[dir][1];
            if (x > 2) // 判断起始点
            {
                pts[step][0] = x; // 用来存放边线坐标信息
                pts[step][1] = y;
                step++;
                turn = 0;
            }

            // AT(img,x,y) = RGB565_GREEN;
        }
        else // 前方为白色，左前方为白色（墙角）
        {
            x += dir_frontleft[dir][0]; // 遇到墙角要斜着走
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4; // 遇到墙角要左转一次
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            // AT(img,x,y) = RGB565_GREEN;
        }
    }
    // 丢线标志，否则由于sobel特殊性会一直往上巡线
    if (step < 10 && touch_boundary0)
        loseline0 = 1;
    // 记录边线数目
    *num = step;
}

void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
{
    zf_assert(img && img->data);
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1);
    //    int half = block_size / 2;        //上交方案
    int half = 0; // 方案二
    int step = 0, dir = 0, turn = 0;
    while (step < *num && 0 < x && x < img->width - 1 && half < y && y < img->height - half - 1 && turn < 4)
    {
        int local_thres = 1;
        //        int local_thres;
        // 自适应二值化
        /*
                for (int dy = -half; dy <= half; dy++) {
                    for (int dx = -half; dx <= half; dx++) {
                        local_thres += AT(img, x + dx, y + dy);
                    }
                }
                local_thres /= block_size * block_size;
                local_thres -= clip_value;
        */
        int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
        //=======添加部分=======
        if ((x == img->width - 2 && y < img->height - 50) || x == 1 || y == 1 || (y == 10 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线，直到边线个数达到最大为止
        {
<<<<<<< HEAD

            if (x == img->width - 2 /*|| x==1*/)
                touch_boundary1 = 1; // 右边界是因为到最右边才停下来的，触碰到最右边，可能是环岛，十字等，
            if (y == 1)
                touch_boundary_up1 = 1; // 走到顶边，判断坡道or障碍

=======
            if (x == img->width - 2 /*|| x==1*/) // 限制迷宫巡线的右边界
            {
                touch_boundary1 = 1; // 右边界是因为到最右边才停下来的，触碰到最右边/最左边，可能是环岛，十字等，
            }
            if (y == 1) // 限制迷宫巡线的高度
            {
                touch_boundary_up1 = 1; // 走到顶边，判断坡道
            }
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
            break;
        }
        //=======添加部分=======
        if (front_value < local_thres)
        {
            dir = (dir + 3) % 4;
            turn++;
        }
        else if (frontright_value < local_thres)
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            if (x < 158)
            {
                pts[step][0] = x;
                pts[step][1] = y;
                step++;
                turn = 0;
            }
            // AT(img,x,y) = RGB565_YELLOW;
        }
        else
        {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            // AT(img,x,y) = RGB565_YELLOW;
        }
    }
    // 丢线标志，否则由于sobel特殊性会一直往上巡线
    if (step < 10 && touch_boundary1)
        loseline1 = 1;
    // 记录边线数目
    *num = step;
<<<<<<< HEAD
}
// 补线 原图

/**
 * @brief 两点求直线，直线颜色为value
 *
 * @param img image_t类型的指针img
 * @param pt0[2] 两个点的坐标
 * @return 无（主要的作用是在图上画出直线）
 * @explanation 函数首先计算出pt0到pt1的水平和垂直方向的差值dx和dy。如果dx的绝对值大于dy的绝对值，
 * 那么函数就从pt0[0]开始遍历到pt1[0]，每次增加1或减少1，取决于dx的正负。在遍历的过程中，函数计算出当前点的纵坐标y，
 * 并将value赋值给img图像上的对应像素点。计算y的方法是：y = pt0[1] + (x - pt0[0]) * dy / dx，其中x是当前遍历到的横坐标。
 * 如果dx的绝对值小于等于dy的绝对值，那么函数就从pt0[1]开始遍历到pt1[1]，每次增加1或减少1，取决于dy的正负。在遍历的过程中，
 * 函数计算出当前点的横坐标x，并将value赋值给img图像上的对应像素点。计算x的方法是：x = pt0[0] + (y - pt0[1]) * dx / dy，
 * 其中y是当前遍历到的纵坐标。
 */
void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value)
{
    int dx = pt1[0] - pt0[0]; // 求出x,y坐标的差值
    int dy = pt1[1] - pt0[1];
    if (abs(dx) > abs(dy)) // 如果x的差值大于y的差值，那么直线形式就是y=kx+b
    {
        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1))
        {
            int y = pt0[1] + (x - pt0[0]) * dy / dx;                                  // y = 左线横坐标 + x遍历差值占总差值比例 * y方向差值
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; // （x，y）坐标像素（不超出边界）赋值
        }
    }
    else // 如果y的差值大于x的差值，那么直线形式就是x=ky+b
    {
        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1))
        {
            int x = pt0[0] + (y - pt0[1]) * dx / dy;
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    }
=======
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
}

/**
 * @brief 线性回归求斜率
 *
 * @param pts_in 二维数组
 * @param num 数组长度
 * @return float 返回斜率
 */
float LineRession(int pts_in[][2], int num)
{
    float slope;
    // int num = sizeof(pts_in) / sizeof(pts_in[0]); // 求数组的长度
    int i = 0, SumX = 0, SumY = 0, SumLines = 0;
    float SumUp = 0, SumDown = 0, avrX = 0, avrY = 0, A;
    SumLines = pts_in[0][1] - pts_in[num][1]; // pts_in[0][1] 为开始行， //pts_in[num][1] 结束行 //SumLines

    for (i = 0; i < num; i++)
    {
        SumY += pts_in[i][1];
        SumX += pts_in[i][0]; // 对边界X,Y求和
    }
    avrY = SumY / SumLines; // Y的平均值
    avrX = SumX / SumLines; // X的平均值
    SumUp = 0;
    SumDown = 0;
    for (i = 0; i < num; i++)
    {
        SumUp += (pts_in[i][0] - avrX) * (pts_in[i][1] - avrY);
        SumDown += (pts_in[i][1] - avrY) * (pts_in[i][1] - avrY);
    }
    if (SumDown == 0)
        slope = 0;
    else
        slope = (SumUp / SumDown);
    A = (SumX - slope * SumY) / SumLines; // 截距
    return slope;                         // 返回斜率
}

/*左转运行函数*/
float run_left(void)
{

    /*一 求中线*/
    if (loseline0 == 1 || (loseline0 == 0 && ipts0_num < 50)) // 情况1：左边完全丢线
    {
        mid_line_num = ipts1_num; // 中线赋值
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = ipts1[i][0] / 2;
            mid_line[i][1] = ipts1[i][1];
            // ips200_draw_line(0,120,mid_line[i][0],mid_line[i][1],RGB565_BLUE);
        }
    }
    else
    {
        mid_line_num = ipts0_num;
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
            mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2; // 直接取平均（前面有个更好的函数，但是试过不行，后面再优化）
        }
    }

    //入左环岛左圆弧中线偏移
    // if(circle_type == CIRCLE_LEFT_IN )
    // {
    //     mid_line_num = ipts0_num - Lpt0_id;
    //     for(uint8 i =0; i<mid_line_num; i++)
    //     {
    //         mid_line[i][0] = ipts0[i+Lpt0_id][0] + 35;
    //         mid_line[i][1] = ipts0[i+Lpt0_id][1];
    //     }

    // }
    if(circle_type == CIRCLE_LEFT_IN )
    {
        mid_line_num = ipts1_num ;
        for(uint8 i =0; i<mid_line_num; i++)
        {
            mid_line[i][0] = ipts1[i][0] - 67;
            mid_line[i][1] = ipts1[i][1];
        }

    }

    //出左环岛右拐点边线偏移
    else if(circle_type == CIRCLE_LEFT_OUT)
    {
        mid_line_num =  Lpt1_id;
        for(uint8 i =0; i<mid_line_num; i++)
        {
            mid_line[i][0] = ipts1[i][0] - 55;
            mid_line[i][1] = ipts1[i][1];
        }
    }

    /*左环岛中误差增大*/
    else if( circle_type == CIRCLE_LEFT_IN)
    {
        percent_k = 1.3;//增大误差
    }

    else if(circle_type == CIRCLE_LEFT_RUN)
    {
        percent_k = 1.6;//增大误差
    }
    else
    {
        percent_k = 1;
    }
    err = Err_Handle(1);//选择模式2：中线斜率误差

    return err*1.7;
}

/*误差处理计算：三种模式1.95-35行加权求误差    2. 中线斜率求做误差   3. 中线斜率求角度*/
// float Err_Handle(uint8 mode)
// {
//     float QuanZhong[10]={0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1,0.01};
//     int count=0;
//     int sum=0;
//     int distance[2][10];//距离存储值

//     while(count < 10)
//     {
//         int i,j=0;
//         while(ipts0[i][1] < ipts1[j][1])
//         {
//             j++;//如果右边线的点坐标高于左边线,右边取高一点的点
//         }
//         while(ipts0[i][1] > ipts1[j][1])
//         {
//             i++;//如果左边线的坐标高于右边线，左边线取高一点点
//         }
//         distance[0][count] = (ipts1[i][0] - ipts0[j][0]);//左右线的距离
//         sum += distance[0][count];

//         distance[1][count] = ((ipts0[i][0] + ipts1[j][0]) /2 - 80);//中线的偏差
//         count++;
//     }
// }
//     count = 0;

//     float err = 0.0;

//     for(int i=0;i < 10; i++)
//     {
//         err += (float)(distance[1][i] * distance[0][i]/sum);

//     }
//     ips200_show_uint(160,20,sum,3);
//     return err;
// }

// float Err_Handle(uint8 mode)
// {
//     uint8 count;
//     int i,j=0;
//     int temp;
//     for(temp=0;temp>10;temp++)//常规误差计算
//     {
//         while(ipts0[i][1] < ipts1[j][1])
//         {
//             j++;//如果右边线的点坐标高于左边线,右边取高一点的点
//         }
//         while(ipts0[i][1] > ipts1[j][1])
//         {
//             i++;//如果左边线的坐标高于右边线，左边线取高一点点
//         }
//         err+=(80-((ipts0[i][0]+ipts1[j][0])/2));
//         count++;
//     }

//     err = err / count;

//     return err;
// }

// float midline(uint8 type)
// {
//     float Err_staright = 0;
//     int i = 20, j = 20;
//     // 从第20个点开始取，让点都在同一行
//     while (ipts0[i][1] < ipts1[j][1])
//     {
//         j++;
//     }
//     while (ipts0[i][1] > ipts1[j][1])
//     {
//         i++;
//     }
//     int max_num = ipts0_num - i > ipts1_num - j ? ipts1_num - j : ipts0_num - i; // 避免在处理两个数组时越界
//     max_num = max_num > 10 ? 10 : max_num;
//     float d = ipts1[j][0] - ipts0[i][0]; // 计算第一个相同起始行列坐标的差值
//     float sum = 0, num = 0;              // 比较个数，
//     for (int k = 0; k < max_num; k++)    // 一般maxnum都是10行
//     {
//         while (ipts0[i][1] < ipts1[j][1])
//         {
//             j++;
//         }
//         while (ipts0[i][1] > ipts1[j][1])
//         {
//             i++;
//         }
//         // 调节到同一行上去
//         sum += ((ipts1[j][0] + ipts0[i][0]) / 2) * ((ipts1[j][0] - ipts0[i][0]) / d);
//         // 加权求值，其中d为第一行列坐标的值，根据10行值的长度与第一行的做一个长度上的加权，简单来说就是中线*长度的权
//         num += (ipts1[j][0] - ipts0[i][0]) / d; // 对加权值进行累加
//         i++;
//     }
//     Err_staright = (sum / num); // 加权总值进行加权平均
//     return Err_staright;        // 返回误差值
// }

float Err_Handle(uint8 mode)
{
    uint8 count=0;
    int i,j=0;
    int y_start,y_end;//起始y行和结束y行
    y_start = 15;//前瞻起始行
    y_end = 53;//前瞻结束行
    int y_all = my_abs(y_start-y_end);

    last_err = err;
    for( int a =y_start; a < y_end ;a++)//常规误差计算
    {
        // if(mid_line[a][1] > 60 && mid_line[a][1] < 100)
        // {

            err += (80-(mid_line[a][0])) * (y_all - my_abs(a-y_start))/y_all;
            count++;
        // }

        //err的前者为x坐标与中线的坐标差值，后者为行数的权重
    }
    // for(int a=10; a<20;a++)
    // {
    //     if(mid_line[a][1]!=mid_line[a+1][1])
    //     {
    //         err += (80-(mid_line[a][0]));
    //         count++;
    //     }
    // }
    err = err / count;
    // if(track_type != TRACK_BOTH)
    // {
    //     err = err * 1.5;
    // }
    ips200_show_uint(160,60,count,3);
    if(err > 90 || err < -90)
    err = last_err*0.5 + err*0.5;
    return err;
}

/*右转运行函数*/
float run_right(void)
{
    /*一 求中线*/
    mid_line_num = 0;
    /*情况1：右边线不完全丢线且个数较多*/
    if (touch_boundary1 == 1 && ipts0[ipts0_num - 1][0] > 100 && ipts0_num > 100 && ipts1_num > 40)
    {
        mid_line_num = ipts1_num; // 中线赋值
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
            mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
        }
    }
    /*情况2 ：右边完全丢线或者点的个数较少*/
    // if (ipts0_num > 80 && ipts0[ipts0_num - 1][0] > 100 && (loseline1 == 1 || ipts1_num < 40))
    else
    {
        mid_line_num = ipts0_num; // 中线赋值
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = ipts0[i][0] / 2 + Shift; // 左线坐标偏移
            mid_line[i][1] = ipts0[i][1];
        }
    }
    /*显示函数，可以去掉*/
    // for (uint8 i = 0; i < mid_line_num; i++)
    // {
    //     ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    // }

    err = Err_Handle(1); // 选择模式3：中线斜率角度误差

    return err * 1.7;
}

/*直道运行函数*/
float run_straight(void)
{
    if(circle_type == CIRCLE_LEFT_FOUND)
    {
        Center_edge();
    }
    mid_line_num = (ipts0_num > ipts1_num) ? ipts1_num : ipts0_num;
    // int count = 0;
    for (uint8 i = 0; i < mid_line_num; i++)
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
        // if(mid_line[i][1] < 50 && mid_line[i][1] == mid_line[i-1][1])
        // {
        //     break;
        // }
    }

    if (mid_line_num == 0 && loseline0 == 1 && loseline1 == 1)
    {
        mid_line_num = (Far_ipts0_num > Far_ipts1_num) ? Far_ipts1_num : Far_ipts0_num;
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = (Far_ipts1[i][0] + Far_ipts0[i][0]) / 2;
            mid_line[i][1] = (Far_ipts1[i][1] + Far_ipts0[i][1]) / 2;
        }
    }
    err = Err_Handle(1); // 选择模式3：中线斜率角度误差


    // else if(circle_type == CIRCLE_LEFT_FOUND)
    // {
    //     for(int a = ipts1[0][1]; a < 80 ; a++)
    //     {

    //     }
    // }


    err = Err_Handle(1);//选择模式3：中线斜率角度误差
    /*发现左环岛 ：削弱直道偏移作用*/
    if(circle_type == CIRCLE_LEFT_FOUND)
    {
        percent_k = 0.6;
    }
    else
    {
        percent_k = 1;
    }
    return err;
}
/*Track检测函数*/
void Track_Check(void)
{
    // if (cross_type == CROSS_NONE)
    // {
        if ((ipts0_num > 100 && ipts0[ipts0_num - 1][0] > (IMAGE_WIDTH / 2) && (loseline1 == 1 || ipts1_num <30) && ipts0[ipts0_num - 1][1] > 30))
        {
            track_type = TRACK_RIGHT;
            cross_type = CROSS_NONE;
        }
        else if ((ipts1_num > 100 && ipts1[ipts1_num - 1][0] < 60 && (loseline0 == 1 || ipts0_num <30) && ipts1[ipts1_num - 1][1] > 30 && ipts0[ipts0_num - 1][1] > (IMAGE_HEIGHT / 2)) || (loseline0 == 1 && ipts1_num > 80 && ipts1[ipts1_num - 1][0] < 60))
        {
            track_type = TRACK_LEFT;
            cross_type = CROSS_NONE;
        }
        else if (ipts1_num > 60 && ipts0_num > 60 && ipts0[ipts0_num - 1][1] < 60 && ipts1[ipts1_num - 1][1] < 60 && !loseline0 && !loseline1)
        {
            track_type = TRACK_BOTH;
            cross_type = CROSS_NONE;
        }
    // }

    // if(circle_type == CIRCLE_LEFT_IN || circle_type == CIRCLE_LEFT_RUN)//在状态3和4选择左进入环岛
    // {
    //     // BorderLine_Find();
    //     track_type = TRACK_LEFT;
    // }
    // else if(circle_type == CIRCLE_LEFT_FOUND  &&  loseline1 == 1)//如果找到左环岛入直道，且右边丢线：巡左线
    // {
    //     track_type = TRACK_LEFT;
    // }
    // else if(circle_type == CIRCLE_LEFT_FOUND && !Near_Lpt0_Found)//状态1的左丢拐角点
    // {

    // }

}

/*Track运行函数*/
void Track_Run(void)
{
    switch (track_type)
    {
    case TRACK_LEFT:
    {
        Finnal_err = run_left();
        State = 1;
        break;
    }
    case TRACK_RIGHT:
    {
        Finnal_err = run_right();
        State = 2;
        break;
    }
    case TRACK_BOTH:
    {
        Finnal_err = run_straight();
        State = 3;
        break;
    }
    default:
    {
        break;
    }
    }
}

void Line_Add(image_t *img, int pts0_in[2], int pts1_in[2], int8 value)
{
    int dx = pts1_in[0] - pts0_in[0];
    int dy = pts1_in[1] - pts0_in[1];
    if (abs(dx) > abs(dy))
    {
        for (int x = pts0_in[0]; x != pts1_in[0]; x += (dx > 0 ? 1 : -1))
        {
            int y = pts0_in[1] + (x - pts0_in[0]) * dy / dx;                          // y = 左线横坐标 + x遍历差值占总差值比例 * y方向差值
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; // （x，y）坐标像素（不超出边界）赋值
        }
    }
    else
    {
        for (int y = pts0_in[1]; y != pts1_in[1]; y += (dy > 0 ? 1 : -1))
        {
            int x = pts0_in[0] + (y - pts0_in[1]) * dx / dy;
            AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    }
}

float mySqrt(float x)
{
    float a = x;
    unsigned int i = *(unsigned int *)&x;
    i = (i + 0x3f76cf62) >> 1;
    x = *(float *)&i;
    x = (x + a / x) * 0.5;
    return x;
}

void track_leftline(int pts_in[][2], int num, int pts_out[][2], int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = mySqrt(dx * dx + dy * dy);
        // float dn = sqrt(dx * dx + dy * dy);
        dx /= dn; // cosθ
        dy /= dn; // sinθ
        pts_out[i][0] = pts_in[i][0] + dy * dist;
        pts_out[i][1] = pts_in[i][1] - dx * dist;
    }
}
void track_rightline(int pts_in[][2], int num, int pts_out[][2], int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = mySqrt(dx * dx + dy * dy);
        // float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] - dy * dist;
        pts_out[i][1] = pts_in[i][1] + dx * dist;
    }
}

/*返回值为误差*/
float Line_Shifting(void)
{
    int left_shift[80][2] = {0};
    int right_shift[80][2] = {0};
    for (int i = 0; i < 80; i++)
    {
        left_shift[i][0] = (ipts0[i][0] + Weight[i]);
        left_shift[i][1] = ipts0[i][1];
        ips200_draw_point(left_shift[i][0], left_shift[i][1], RGB565_RED);

        right_shift[i][0] = (ipts1[i][0] - Weight[i]);
        right_shift[i][1] = ipts1[i][1];
        ips200_draw_point(right_shift[i][0], right_shift[i][1], RGB565_RED);
    }

    for (int i = 0; i < 80; i++)
    {
        mid_line[i][0] = (left_shift[i][0] + right_shift[i][0]) / 2;
        mid_line[i][1] = (left_shift[i][1] + right_shift[i][1]) / 2;
    }
    last_err = err;
    for (int a = 0; a < 80; a++) // 常规误差计算
    {
        err += (80 - (mid_line[a][0])) * ((80 - a) / 80);
        // err的前者为x坐标与中线的坐标差值，后者为行数的权重
    }

    return err;
}

/**
 * @brief 透视变换函数，将提取出来的两条边线进行透视变换
 *
 * @param pts_in 二维数组，表示输入的点集，每个点有两个坐标值
 * @param int_num 整型，表示输入的点集中点的数量
 * @param pts_out 二维数组，表示输出的点集，每个点有两个坐标值
 */
void Pespective(int pts_in[][2], int int_num, float pts_out[][2])
// 带入提取出来的两条边线，得到透视变换后的两条边线
{
    float x, y, w;
    for (int16_t i = 0; i < int_num; i++)
    {
        x = getx((pts_in[i][0]), (pts_in[i][1]));
        y = gety((pts_in[i][0]), (pts_in[i][1]));
        w = getw((pts_in[i][0]), (pts_in[i][1]));
        pts_out[i][0] = x / w;
        pts_out[i][1] = y / w;
    }
}
/*判断思路:单边x/y坐标单调增/减,且相邻点的坐标差值相等,大于一定阈值可以认为是直道*/
/**
 * @brief  判断是否为直道
 *
 * @param pts_in 二维数组，表示输入的点集，每个点有两个坐标值
 * @param pts_num 整型，表示输入的点集中点的数量
 * @param thres 整型，表示阈值
 * @param flag 整型指针，表示是否为直道的标志位
 */
void LongStarightLine_Rec(int pts_in[][2], int pts_num, int thres, int *flag)
{
    int Is_Straight = 0;
    for (int i = 10; i < pts_num - 10; i++)
    {
<<<<<<< HEAD
        pt0_out[i][0] = IMAGE_HEIGHT - pt0_in[i][0] - 1;
        pt0_out[i][1] = IMAGE_HEIGHT - pt0_in[i][1] - 1;
    }
}

/*坐标复原，找完十字以后坐标就和以前一样*/
void Coordinate_restore_left(int16 pt0_in[][2], int16 in_num, int16 pt0_out[][2])
{
    uint16 i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][0] = IMAGE_HEIGHT - pt0_in[i][0] - 1;
    }
}

void Coordinate_restore_right(int16 pt0_in[][2], int16 in_num, int16 pt0_out[][2])
{
    uint16 i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][0] = IMAGE_HEIGHT - pt0_in[i][0] - 1;
        pt0_out[i][1] = IMAGE_HEIGHT - pt0_in[i][1] - 1;
    }
}

/*************************************************************************
 *  函数名称：void nms_angle();
 *  功能说明：角度变化率非极大抑制
 *  参数说明：输入角度变化率数组，角度变换数组总个数，抑制后的角度变化数组，点集奇数核
 *  函数返回：无
 *  修改时间：2022年11月25日
 *  备    注：上交方案：kernel = 21，保存极大值的数组，即一段范围内斜率变换最大的值，非极大值均设为零
 *************************************************************************/
void nms_angle(float angle_in[], int num, float angle_out[], int kernel)
{
    zf_assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++)
    {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++)
        {
            // fabs():求绝对值
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i]))
            {
                angle_out[i] = 0;
                break;
=======
        if (my_abs((pts_in[i + 5][1] - pts_in[i][1]) - (pts_in[i][1] - pts_in[i - 5][1])) < 2 && my_abs((pts_in[i + 10][1] - pts_in[i][1]) - (pts_in[i][1] - pts_in[i - 10][1])) < 2 /*&& my_abs((pts_in[i + 6][1] - pts_in[i][1]) - (pts_in[i][1] - pts_in[i - 6][1])) < 3*/ && my_abs(my_abs(pts_in[i + 5][0] - pts_in[i][0]) - my_abs(pts_in[i][0] - pts_in[i - 5][0])) < 2 && my_abs(my_abs(pts_in[i + 10][0] - pts_in[i][0]) - my_abs(pts_in[i][0] - pts_in[i - 10][0])) < 3 && pts_in[i][1] >= pts_in[i + 2][1] && pts_in[i][1] >= pts_in[i + 4][1] & pts_in[i][1] >= pts_in[i + 6][1] && pts_in[i][1] <= pts_in[i - 2][1] && pts_in[i][1] <= pts_in[i - 4][1] & pts_in[i][1] <= pts_in[i - 6][1])
        {
            Is_Straight++;
        }
    }
    if (Is_Straight > thres) // 判断是否为直道
    {
        *flag = 1;
    }
    else
    {
        *flag = 0;
    }
    ips200_show_uint(160, 160, Is_Straight, 3);
}

void NearCorners_Find_Left_Again(void)
{
    int Left_Eastest = 0;
    if(Near_Lpt0_Found == 0 && circle_type == CIRCLE_LEFT_IN)//二次寻找
    {
        for(int i=0; i<ipts0_num; i++)
        {
            if(ipts0[i][0] > Left_Eastest)
            {
                Left_Eastest =ipts0[i][0];
                Last_Lpt0_id = Lpt0_id;
                Last_CornersLeft_Point[0] = CornersLeft_Point[0];
                Last_CornersLeft_Point[1] = CornersLeft_Point[1];
                Lpt0_id = i;
                CornersLeft_Point[0] = ipts0[Lpt0_id][0];
                CornersLeft_Point[1] = ipts0[Lpt0_id][1];
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
            }

        }
        ips200_show_int(160,120,Left_Eastest,4);
    }
}

<<<<<<< HEAD
/*************************************************************************
 *  函数名称：void local_angle_points();
 *  功能说明：点集局部角度变化率
 *  参数说明：输入边线数组，边线点个数，输出角度变换率数组，两点间固定距离
 *  函数返回：无
 *  修改时间：2022年11月23日
 *  备    注：计算该点前后两点连线的斜率作为该点切线斜率的近似
 *************************************************************************/
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++)
    {
        if (i <= 0 || i >= num - 1)
        {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = mySqrt(dx1 * dx1 + dy1 * dy1); // 此点与前一点间距离
        // float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);//此点与前一点间距离
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = mySqrt(dx2 * dx2 + dy2 * dy2); // 此点与后一点间距离
        // float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);//此点与后一点间距离
        float c1 = dx1 / dn1; // cosθ1
        float s1 = dy1 / dn1; // sinθ1
        float c2 = dx2 / dn2; // cosθ2
        float s2 = dy2 / dn2; // sinθ2
        // 化简展开得到角公式(k1-k2)/(1+k1k2),即tan的差角公式==>得出两点间角度变化==>曲线斜率变化
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1); // atan2f(y,x):返回以弧度表示的y/x反正切
    }
}
=======

void FarCorners_Find_Left_Again(void)
{
    // int Far_high[120] = {0};
    int Far_Highest = 0;
    int Far_Eastest = 0;


        for(int i=0;i<Far_ipts0_num ; i++)
        {
            if((Far_ipts0[i][0] < Far_ipts0[i+1][0]) && (Far_ipts0[i][0] > Far_ipts0[i-1][0]) && (Far_ipts0[i][1] > Far_ipts0[i+1][1])
                &&(Far_ipts0[i][0] < Far_ipts0[i+2][0]) && (Far_ipts0[i][0] > Far_ipts0[i-2][0]) &&(Far_ipts0[i][1] > Far_ipts0[i+2][1])
                && ((Far_ipts0[i][1] == Far_ipts0[i-1][1])||(Far_ipts0[i][1] == Far_ipts0[i-2][1])||(Far_ipts0[i][1] == Far_ipts0[i-3][1]))
                && ((Far_ipts0[i][1] - Far_ipts0[0][1])<3))
            {
                Last_Far_Lpt0_id = Far_Lpt0_id;
                Last_FarCornersLeft_Point[0] = FarCornersLeft_Point[0];
                Last_FarCornersLeft_Point[1] = FarCornersLeft_Point[1];
                Far_Lpt0_id = i;
                FarCornersLeft_Point[0] = Far_ipts0[i][0];
                FarCornersLeft_Point[1] = Far_ipts0[i][1];
            }
            // if(Far_ipts0[i][1] == Far_ipts0[i+1][1])
            // {
            //     Far_high[Far_ipts0[i+1][1]] ++;
            // }
        }
        ips200_show_uint(160,60,Far_Highest,4);
        // for(int i=0;i<120;i++)
        // {
        //     if(Far_high[i] > Far_Highest)
        //     {
        //         Far_Highest = Far_high[i];
        //     }
        //     ips200_show_int(0,160,Far_Highest,3);
        // }


}
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
