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

// 左右边丢线
uint8 loseline0;              // 左边线丢失标志位
uint8 loseline1;              // 右边线丢失标志位
int ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int ipts0_num;                // 存放边线像素点个数(左)
int ipts1_num;                // 存放边线像素点个数(右)

int Far_ipts0[POINTS_MAX_LEN][2]; // 存放边线数据（左）
int Far_ipts1[POINTS_MAX_LEN][2]; // 存放边线数据（右）
int Far_ipts0_num;                // 存放边线像素点个数(左)
int Far_ipts1_num;                // 存放边线像素点个数(右)

int Lpt0_id, Lpt1_id;         // 近线L角点id
int Far_Lpt0_id, Far_Lpt1_id; // 远线L角点id

int Is_straight0, Is_straight1; // 直道标志位
int Is_Arc0, Is_Arc1;           // 圆弧标志位
int Is_Bend0, Is_Bend1;         // 弯道标志位

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
            pts_out[0] = pts_in[i][0];
            pts_out[1] = pts_in[i][1];
            Lpt0_id = i;
            *flag = 1;
            break;
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
            pts_in[i][0] - pts_in[i - 4][0] <= 0 && pts_in[i][0] - pts_in[i - 5][0] <= 0 && pts_in[i][0] - pts_in[i - 6][0] <= 0 && pts_in[i][0] - pts_in[i + 1][0] <= 0 && pts_in[i][0] - pts_in[i + 2][0] < 0 && pts_in[i][0] - pts_in[i + 3][0] <= 0 && pts_in[i][0] - pts_in[i + 4][0] <= 0 && pts_in[i][0] - pts_in[i + 5][0] < 0 && pts_in[i][0] - pts_in[i + 6][0] <= 0 && my_abs(pts_in[i][1] - pts_in[i + 1][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 2][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 3][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 4][1]) < 5 && my_abs(pts_in[i][1] - pts_in[i + 5][1]) < 5) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i + 1][1])
            {
                continue;
            }
            pts_out[0] = pts_in[i][0];
            pts_out[1] = pts_in[i][1];
            Lpt1_id = i;
            *flag = 1;
            break;
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
void Arc_Point_Get(int pts_in[][2], int pts_num, int pts_out[2], int *flag)
{
    // int Is_Arc = 0; // 圆环判断标志位
    for (int i = 10; i < pts_num - 10; i++)
    {
        // 圆弧b
        if (pts_in[i][0] - pts_in[i - 2][0] >= 0 && pts_in[i][0] - pts_in[i - 4][0] >= 0 && pts_in[i][0] - pts_in[i - 6][0] >= 0 && pts_in[i][0] - pts_in[i - 8][0] >= 0 && pts_in[i][0] - pts_in[i - 10][0] >= 0 && pts_in[i][0] - pts_in[i + 2][0] >= 0 && pts_in[i][0] - pts_in[i + 4][0] >= 0 && pts_in[i][0] - pts_in[i + 6][0] >= 0 && pts_in[i][0] - pts_in[i + 8][0] >= 0 && pts_in[i][0] - pts_in[i + 10][0] >= 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][0] == pts_in[i + 1][0] || pts_in[i][1] == pts_in[i + 1][1])
            {
                continue;
            }
            pts_out[0] = ipts0[i][0];
            pts_out[1] = ipts0[i][1];
            ips200_show_int(160, 180, pts_out[0], 2);
            ips200_show_int(160, 200, pts_out[1], 2);
            ips200_draw_line(0, 0, pts_out[0], pts_out[1], RGB565_BLUE);
            *flag = 1;
            break;
        }
        else
        {
            *flag = 0;
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
    /*保证step步数小于Num   保证x（列）坐标>0同时小于宽度-1   保证y（行）坐标大于0小于高度-1*/
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
        //=======添加部分=======（限制条件）
        /*  当扫点的列坐标到左黑框边界且行坐标小于20    列坐标到右边的黑框边界  行坐标为1   行坐标为88的同时步数已经大于19*/
        if ((x == 2 && y < img->height - 60) || x == img->width - 2 || y == 2 || (y == 10 && step > 19)) // 30修改后能扫线
        {
            if (x == 2 /*|| x== img->width - 2*/) // 限制迷宫巡线的左边界
                touch_boundary0 = 1;              // 左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
            if (y == 2)                           // 限制迷宫巡线的高度
                touch_boundary_up0 = 1;           // 走到顶边，判断坡道or障碍
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
    uint8 i;
    zf_assert(img && img->data);
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1);
    //       int half = block_size / 2;        //上交方案
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
        if ((x == img->width - 2 && y < img->height - 60) || x == 1 || y == 1 || (y == 10 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线，直到边线个数达到最大为止
        {
            if (x == img->width - 2 /*|| x==1*/) // 限制迷宫巡线的右边界
            {
                touch_boundary1 = 1; // 右边界是因为到最右边才停下来的，触碰到最右边/最左边，可能是环岛，十字等，
            }
            if (y == 1) // 限制迷宫巡线的高度
            {
                touch_boundary_up1 = 1; // 走到顶边，判断坡道
            }
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
            }

        }
        ips200_show_int(160,120,Left_Eastest,4);
    }
}


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
