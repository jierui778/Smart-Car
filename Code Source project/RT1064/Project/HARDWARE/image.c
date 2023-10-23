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

uint8 Image_Use_Robert[120][160]; // sobel二值化图像

uint8 mid_line_num=0;

int mid_row_first; // 中线行的起点
// flash参数统一定义
float begin_x = 5;   // 起始点距离图像中心的左右偏移量	8
float begin_y = 118; // 起始点距离图像底部的上下偏移量 120高度：35;100高	58
/*beginy值越小，初始的生长点与上框越近*/

float block_size = 7; // 自适应阈值的block大小
float clip_value = 2; // 自适应阈值的阈值裁减量
int NearBorderLine_Enable = 1;
int FarBorderLine_Enable = 0; // 开启远近线的标志位//默认只开启近线,不开启远线

// 左右边丢线
uint8 loseline0;
uint8 loseline1;
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

int Far_Lpt0_Found = 0, Far_Lpt1_Found = 0;   // 远线L角点标志位
int Near_Lpt0_Found = 0, Near_Lpt1_Found = 0; // 近线角点标志位

uint8 touch_boundary0; // 左边线走到图像左边界
uint8 touch_boundary1; // 右边线走到图像右边界

uint8 touch_boundary_up0; // 左边线走到图像上边界
uint8 touch_boundary_up1; // 右边线走到图像上边界

int begin_x0, begin_y0; // 找线偏移点
int begin_x1, begin_y1; // 找线偏移点

int x0_first, y0_first, x1_first, y1_first; // 左右边线第一个点的坐标

int x1, y1;
int x2, y2;

int CornersLeft_Point[2] = {0};
int CornersRight_Point[2] = {0}; // 近角点坐标
int FarCornersLeft_Point[2] = {0};
int FarCornersRight_Point[2] = {0}; // 远角点坐标

float Err[5] = {0}; // 中线误差数组


float err,last_err;
float Finnal_err;

void test(void)
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

    ips200_show_int(50, 200, FarCornersLeft_Point[0], 2);
    ips200_show_int(50, 220, FarCornersLeft_Point[1], 2);
    if (Near_Lpt0_Found || Near_Lpt1_Found) // 如果近边线有角点
    {
        FarBorderLine_Enable = 1;
    }
    if (FarBorderLine_Enable) // 开启
    {
        FarBorderline_Find(); // 寻找远边线

        if (Near_Lpt0_Found) // 如果左边线有角点
        {
            FarCorners_Find_Left(Far_ipts0, Far_ipts0_num, FarCornersLeft_Point, &Far_Lpt0_Found);
        }
        if (Near_Lpt1_Found)
        {
            FarCorners_Find_Right(Far_ipts1, Far_ipts1_num, FarCornersRight_Point, &Far_Lpt1_Found);
        }
    }

    ips200_show_float(120, 140, Finnal_err, 5, 5);
    ips200_show_uint(40,120,mid_line_num,3);
    ips200_show_uint(40,140,ipts0_num,3);
    ips200_show_uint(40,160,ipts1_num,3);

    ips200_show_int(20, 200, Near_Lpt0_Found, 1);
    ips200_show_int(20, 220, Near_Lpt1_Found, 1);
    ips200_show_int(20, 240, Far_Lpt0_Found, 1);
    ips200_show_int(20, 260, Far_Lpt1_Found, 1);
    ips200_show_int(20, 280, touch_boundary0, 1);
    ips200_show_int(20, 300, touch_boundary1, 1);
    ips200_draw_line(80, 60, CornersLeft_Point[0], CornersLeft_Point[1], RGB565_RED);
    ips200_draw_line(80, 60, CornersRight_Point[0], CornersRight_Point[1], RGB565_RED);
    ips200_draw_line(80, 60, FarCornersLeft_Point[0], FarCornersLeft_Point[1], RGB565_BLUE);
    ips200_draw_line(80, 60, FarCornersRight_Point[0], FarCornersRight_Point[1], RGB565_RED);

    ips200_show_int(80, 200, loseline0, 1);
    ips200_show_int(80, 220, loseline1, 1);
    ips200_show_int(160, 200, NearBorderLine_Enable, 1);
    ips200_show_int(160, 240, FarBorderLine_Enable, 1);
    

    // ips200_show_uint(0,120,Lpt0_id,3);
    // ips200_show_uint(0,140,Lpt1_id,3);
    // Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);
    // Line_Add(&img_raw, CornersRight_Point, FarCornersRight_Point, 0);

    // ips200_show_int(100, 100, test, 3);

    
    Cross_Check();
    Cross_Run(ipts0,ipts0_num,ipts1,ipts1_num);
    Track_Check();
    Track_Run();
    ips200_show_float(140, 120, Finnal_err, 3,3);
    ips200_show_float(140, 160, atan(1/Finnal_err), 3,3);
    // ips200_draw_line(0,0,CornersLeft_Point[0],CornersLeft_Point[1],RGB565_RED);
    // ips200_draw_line(160,0,CornersRight_Point[0],CornersRight_Point[1],RGB565_BLUE);
    
    ips200_show_uint(0,160,Far_Lpt0_id,3);
    ips200_show_uint(0,180,Far_Lpt1_id,3);

    

    for (int i = 0; i < ipts0_num; i++)
    {
        ips200_draw_point(ipts0[i][0] + 5, ipts0[i][1], RGB565_RED);
    }
    for (int i = 0; i < Far_ipts0_num; i++)
    {
        ips200_draw_point(Far_ipts0[i][0] + 5, Far_ipts0[i][1], RGB565_RED);
    }
}


/**
 * @brief 寻找远左拐点
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
        if ((pts_in[i][0] - pts_in[i - 1][0] > 0 && pts_in[i][0] - pts_in[i - 2][0] > 0 && pts_in[i][0] - pts_in[i - 3][0] > 0 &&
             pts_in[i][0] - pts_in[i - 4][0] > 0 && pts_in[i][0] - pts_in[i - 5][0] > 0 && pts_in[i][1] - pts_in[i + 1][1] > 0 &&
             pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 3][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 &&
             pts_in[i][1] - pts_in[i + 5][1] > 0) ||
            (pts_in[i][0] - pts_in[i - 1][0] > 0 && pts_in[i][0] - pts_in[i - 2][0] > 0 &&
             pts_in[i][0] - pts_in[i - 3][0] > 0 && pts_in[i][0] - pts_in[i - 4][0] > 0 && pts_in[i][1] - pts_in[i + 1][1] > 0 &&
             pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 3][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 &&
             pts_in[i][1] - pts_in[i + 5][1] > 0)) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i - 1][1] || (pts_in[i][0] - pts_in[i - 2][0] >= 0 && pts_in[i][0] - pts_in[i - 4][0] >= 0 & pts_in[i][0] - pts_in[i - 6][0] >= 0 && pts_in[i][0] - pts_in[i + 4][0] >= 0 && (pts_in[i][0] - pts_in[i + 6][0] >= 0)))
            {
                continue;
            }
            pts_out[0] = pts_in[i][0];
            pts_out[1] = pts_in[i][1];
            Far_Lpt0_id = i; // 记录拐点的下标
            *flag = 1;       // 标志位为1
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
        if ((pts_in[i][0] - pts_in[i - 2][0] < 0 && pts_in[i][0] - pts_in[i - 4][0] < 0 && pts_in[i][0] - pts_in[i - 6][0] < 0 && pts_in[i][0] - pts_in[i - 8][0] < 0 && pts_in[i][0] - pts_in[i - 10][0] < 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0) || (pts_in[i][0] - pts_in[i - 2][0] < 0 && pts_in[i][0] - pts_in[i - 4][0] < 0 && pts_in[i][0] - pts_in[i - 6][0] < 0 && pts_in[i][0] - pts_in[i - 8][0] < 0 && pts_in[i][1] - pts_in[i - 10][1] < 0 && pts_in[i][1] - pts_in[i + 2][1] > 0 && pts_in[i][1] - pts_in[i + 4][1] > 0 && pts_in[i][1] - pts_in[i + 6][1] > 0 && pts_in[i][1] - pts_in[i + 8][1] > 0 && pts_in[i][1] - pts_in[i + 10][1] > 0)) // 感觉可以加条件进行二次强判断
        {
            if (pts_in[i][1] == pts_in[i - 1][1] || (pts_in[i][0] - pts_in[i - 2][0] <= 0 && pts_in[i][0] - pts_in[i - 4][0] <= 0 & pts_in[i][0] - pts_in[i - 6][0] <= 0 && pts_in[i][0] - pts_in[i + 4][1] <= 0 && (pts_in[i][0] - pts_in[i + 6][0] <= 0)))
            {
                continue;
            }
            pts_out[0] = pts_in[i][0];
            pts_out[1] = pts_in[i][1];
            Far_Lpt1_id = i;
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
        CornersRight_Point[0] = 155;
        CornersRight_Point[1] = 100;
    }
    if (loseline0)
    {
        CornersLeft_Point[0] = 5;
        CornersLeft_Point[1] = 100;
    }
    ips200_draw_line(0, 0, CornersLeft_Point[0], CornersLeft_Point[1], RGB565_RED);
    ips200_show_uint(200, 200, Far_ipts0_num, 2);

    if (1)
    {

        x0_first = CornersLeft_Point[0];
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
        x1_first = CornersRight_Point[0];
        y1_first = CornersRight_Point[1] - 10;
        

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
 * @brief 获取中线
 *
 * @param mode 1.95-35行加权求误差    2. 中线斜率求做误差
 */
float Center_edge(uint8 mode)
{
    
    int Left_Edge[150][2]={0}; // 实际上只会用到120个，怕越界
    int Right_Edge[150][2]={0};
    int x,y;
    int i;
    int uthres=1;
    y=0;
    for(i = 0;i< 118; i++)
    {
        for(x=80;x>0;x--)
        {
            if((AT_IMAGE(&img_raw, x+1, i) > uthres)&&(AT_IMAGE(&img_raw, x, i) < uthres))
            {
                Left_Edge[i][0]=x;
                Left_Edge[i][1]=i;
                break;
            }
        }
    }
    y=0;
    for(i=0;i<118;i++)
    {
        for(x=80;x<159;x++)
        {
            if((AT_IMAGE(&img_raw, x+1, i) > uthres)&&(AT_IMAGE(&img_raw, x, i) < uthres))
            {
                Right_Edge[i][0]=x;
                Right_Edge[i][1]=i;
                break;
            }
        }
    }
    
    
    int Mid_line[150][2]={0};
    int Mid_line_num =0;
    /*显示函数，可以去掉*/
    for(i=0;i<118;i++)
    {
        ips200_draw_point(Mid_line[i][0],Mid_line[i][1],RGB565_RED);
    }

    
    switch (mode)
    {
        case 1:
        {
            err=0.0;
            for(int row = 95; row > 35; row--)//只取中线的35至95行
            {
                float k = 0.01; // 比例系数
                float mapped_row = (row - 35) / 60.0; // 线性映射到0-1之间
                err += (float)(k * mapped_row * ((Left_Edge[row][0] + Right_Edge[row][1])/2 - 80)/3.5);
                Mid_line_num++;
            }
            break;
        }
        case 2:
        {
            for (i = 30; i < 118; i++)
            {
                Mid_line[i][0] = (Left_Edge[i][0] + Right_Edge[i][0]) / 2;
                Mid_line[i][1] = (Left_Edge[i][1] + Right_Edge[i][1]) / 2;
            }
            for(i=0;i<118;i++)
            {
                ips200_draw_point(Mid_line[i][0],Mid_line[i][1],RGB565_RED);
            }
            err = LineRession(Mid_line, 118);

            err = atan(err);//返回角度
            break;
        }
    }
    /*对高处的直线作截断处理*/

    
    return err;
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
            if (pts_in[pts_num - 5][1] - pts_in[i][1]>10)
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
            if (pts_in[pts_num - 5][1] - pts_in[i][1] > 10)
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
 * @brief 圆弧检测,取出圆弧的极边界坐标
 *
 * @param pts_in 边线坐标数组
 * @param pts_num 边线坐标数组长度
 * @param pts_out 极边界坐标数组
 */
void Arc_Point_Get(int pts_in[][2], int pts_num, int pts_out[2])
{
    int Is_Arc = 0; // 圆环判断标志位
    for (int i = 0; i < pts_num; i++)
    {
        // 圆弧
        if (ipts0[i][0] - ipts0[i - 1][0] >= 0 && ipts0[i][0] - ipts0[i - 2][0] >= 0 && ipts0[i][0] - ipts0[i - 3][0] >= 0 && ipts0[i][0] - ipts0[i + 1][0] >= 0 && ipts0[i][0] - ipts0[i + 2][0] > 0 && ipts0[i][0] - ipts0[i + 3][0] >= 0) // 感觉可以加条件进行二次强判断
        {
            pts_out[0] = ipts0[i][0];
            pts_out[1] = ipts0[i][1];
            break;
        }
        else
        {
        }
    }
}

/**
 * @brief
 *
 * @param pts_in
 * @param pts_num
 * @param pts_out
 */
void Straight_Rec(int pts_in[][2], int pts_num)
{
    int Is_Straight = 0; // 长直道判断标志位
    for (int i = 0; i < pts_num; i++)
    {
        if (pts_in[i][0] - pts_in[i - 1][0] == pts_in[i + 1][0] - pts_in[i][0] && pts_in[i][0] - pts_in[i - 5][0] == pts_in[i + 5][0] - pts_in[i][0])
        {
            Is_Straight++;
        }
    }
}

#define MID_LINE 80 // 定义中线,图像宽为160,所以中线y=80
#define PROS_NUM 20 //  前瞻点数
const uint8 Weight[IMAGE_HEIGHT] =
    {
        0,
}; // 定义误差权重
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

// const int dir_front[4][2] = {{0, -1},
//                              {1, 0},
//                              {0, 1},
//                              {-1, 0}};
// const int dir_frontleft[4][2] = {{-1, -1},
//                                  {1, -1},
//                                  {1, 1},
//                                  {-1, 1}};
// const int dir_frontright[4][2] = {{1, -1},
//                                   {1, 1},
//                                   {-1, 1},
//                                   {-1, -1}};

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
        if ((x == 2 && y < img->height - 50) || x == img->width - 2 || y == 2 || (y == 20 && step > 19)) // 30修改后能扫线
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
    if (step < 20 && touch_boundary0)
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
        if ((x == img->width - 2 && y < img->height - 50) || x == 1 || y == 1 || (y == 20 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线，直到边线个数达到最大为止
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
    if (step < 20 && touch_boundary1)
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

float err, last_err;

float run_left_mid(void)
{
    int mid_line[150][2]; // 中线
    uint8 i;
    mid_line_num = ipts0_num; // 中线赋值
    for (i = 0; i < mid_line_num; i++)
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;//直接取平均（前面有个更好的函数，但是试过不行，后面再优化）
    }
    last_err = err;
    err = LineRession(mid_line, mid_line_num - 1);
    ips200_show_uint(0, 160, mid_line_num, 3);
    /*三 误差简单滤波*/
    err=0.8*err+last_err*0.2;
    return err;

}

float run_left(void)
{
    /*一 求中线*/
    int mid_line[150][2]; // 中线
    mid_line_num = 0;
    if(loseline0 == 1 || (loseline0 == 0 && ipts0_num < 50))//情况1：左边完全丢线
    {
        mid_line_num = ipts1_num; // 中线赋值
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = ipts1[i][0] / 2;
            mid_line[i][1] = ipts1[i][1];
        }
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
        }
    }
    else if(loseline0 ==  0 && ipts0_num > 50)
    {
        mid_line_num = ipts0_num;
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
            mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;//直接取平均（前面有个更好的函数，但是试过不行，后面再优化）
        }
    }
    
    for(uint8 i=0; i < mid_line_num ; i++)
    {
        ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    }

    err = Center_edge(2);//选择模式2：中线斜率误差
   
    return err;
}

float run_right_mid(void)
{
    int mid_line[150][2]; // 中线
    mid_line_num = 0;
    uint8 i;
    
    
    last_err = err;
    err = LineRession(mid_line, mid_line_num - 1);
    ips200_show_uint(0, 160, mid_line_num, 3);
    /*三 误差简单滤波*/
    err = 0.8 * err + last_err * 0.2;
    
    return err;
}


float run_right(void)
{
    /*一 求中线*/
    int mid_line[150][2]; // 中线
    mid_line_num = 0;
    if( (loseline1 == 0 && ipts1_num > 50))
    {
        mid_line_num = ipts1_num; // 中线赋值
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
            mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
        }
    }
    else
    {
        mid_line_num = ipts0_num; // 中线赋值
        for (uint8 i = 0; i < mid_line_num; i++)
        {
            mid_line[i][0] = ipts0[i][0] / 2+ 80;
            mid_line[i][1] = ipts0[i][1];
        }
    }
    
    for(uint8 i=0; i < mid_line_num ; i++)
    {
        ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    }
    err = Center_edge(2);//选择模式2：中线斜率误差
    return err;
}
float run_straight(void)
{
    int mid_line[150][2];
    mid_line_num = (ipts0_num > ipts1_num) ? ipts1_num : ipts0_num;
    for(uint8 i =0; i< mid_line_num ;i++)
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
    }
    err = Center_edge(2);//选择模式2：中线斜率误差
    for(uint8 i=0; i < mid_line_num ; i++)
    {
        ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    }

    return err;
}
void Track_Check(void)
{
    if(ipts1_num > 100 && ipts1[ipts1_num - 1][0] < (IMAGE_WIDTH/2) && (loseline0 ==1 || touch_boundary0 == 1))
    {
        track_type = TRACK_RIGHT;
    }
    else if ( ipts0_num > 100 && ipts0[ipts0_num -1][0] < (IMAGE_WIDTH/2) &&(loseline0 ==1 || touch_boundary0 ==1))
    {
        track_type = TRACK_LEFT;
    }
    else if( ipts1_num > 120 && ipts0_num > 120 && ipts0[ipts0_num-1][1] < (IMAGE_HEIGHT/2) && ipts1[ipts1_num-1][1]<(IMAGE_HEIGHT/2) && !loseline0 && !loseline1)
    {
        track_type = TRACK_BOTH;
    }
}

void Track_Run(void)
{
    switch (track_type)
    {
        case TRACK_LEFT:
        {
            Finnal_err=run_left();
            break;
        }
        case TRACK_RIGHT:
        {
            Finnal_err=run_right();
            break;
        }
        case TRACK_BOTH:
        {
            Finnal_err=run_straight();
            break;
        }
        default:
            break;
    }
}
