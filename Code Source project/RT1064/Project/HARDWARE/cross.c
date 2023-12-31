#include "cross.h"
#include "image.h"
#include "circle.h"
#include "encoder.h"
enum cross_type_e cross_type = CROSS_NONE;
static int cross_num = 0; // 记录圆环个数
static int a;
/*十字检测思路：
双边迷宫巡线到达左右边界
是否需要再向上巡线，二次验证双远角点的存在
**/

void Cross_Check(void) // 得考虑斜入十字的情况
{
    // 双边确定十字
    //  bool Xfound = Lpt0_found && Lpt1_found;
    if (cross_type == CROSS_NONE && touch_boundary0 && Near_Lpt0_Found && Far_Lpt0_Found && !Near_Lpt1_Found && loseline1) // 左边双角点,右边近角点丢失,斜入左十字,进入十字模式
    {
        cross_type = CROSS_HALF_LEFT_FOUND;
        
        cross_num++; // 记录圆环个数
        a = 1;
    }
    else if (cross_type == CROSS_NONE && touch_boundary1 && Near_Lpt1_Found && Far_Lpt1_Found && !Near_Lpt0_Found && loseline0) // 右边双角点,左边近角点丢失,斜入右十字,进入十字模式
    {
        cross_type = CROSS_HALF_RIGHT_FOUND;
        cross_num++; // 记录圆环个数
        a = 2;
    }
    else if (cross_type == CROSS_NONE && touch_boundary0 && Near_Lpt0_Found && Far_Lpt0_Found && touch_boundary1 && Near_Lpt1_Found && Far_Lpt1_Found) // 双边双角点,进入十字模式
    {
        cross_type = CROSS_DOUBLLE_FOUND;

        cross_num++; // 记录圆环个数
        a = 3;
    }
    // Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);
    // Line_Add(&img_raw, CornersRight_Point, FarCornersRight_Point, 0);
    // BorderLine_Find(); // 寻找边线
    if(a == 3)
    {
        Finnal_err = run_straight();
    }
    Center_edge();//从中间向左右两边扫线

    Finnal_err = Err_Handle(1); // 求出最终误差，选择模式3——返回角度偏差
}

int count = 0;
/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 *
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */
void Cross_Run(void)
{
    // FarCornersLeft_Point[0] = Far_ipts0[Far_ipts0_num -20][0];   
    // FarCornersLeft_Point[1] = Far_ipts0[Far_ipts0_num -20][1]; 
    // FarCornersRight_Point[0] = Far_ipts1[Far_ipts1_num -20][0];
    // FarCornersRight_Point[1] = Far_ipts1[Far_ipts1_num -20][1];
    Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);
    Line_Add(&img_raw, CornersRight_Point, FarCornersRight_Point, 0);
    BorderLine_Find(); // 寻找边线
    if (cross_type == CROSS_HALF_LEFT_FOUND)
    {
        if ((loseline0 == 0 && loseline1 == 0 && (ipts1[ipts1_num - 1][0] - ipts0[ipts0_num - 3][0]) > (IMAGE_WIDTH - 20)))
        {
            cross_type = CROSS_IN_LEFT; // 左边近线丢失,循左边远线
            Encoder_Int_Enable();       // 开启编码器积分
            a = 11;
            NearBorderLine_Enable = 0; // 关闭近边线
        }
    }
    else if (cross_type == CROSS_HALF_RIGHT_FOUND) // 斜入左十字
    {
        if (ipts0_num < 40) // 左边线一边点数小于40,进入十字
        {
            cross_type = CROSS_IN_LEFT; // 左边近线丢失,循左边远线
            Encoder_Int_Enable();
            a = 12;
            NearBorderLine_Enable = 0; // 关闭近边线
        }
    }

    else if (cross_type == CROSS_DOUBLLE_FOUND) // 斜入左十字
    {
        if (ipts0_num < 40 && ipts1_num < 40) // 左边线一边点数小于40,进入十字
        {
            cross_type = CROSS_IN_DOUBLE; // 左边近线丢失,循左边远线
            Encoder_Int_Enable();
            a = 13;

            NearBorderLine_Enable = 0; // 关闭近边线
        }
    }

    if ((CROSS_IN_LEFT && Far_ipts0[1][1] > 60 && Far_ipts0[1][0] < 40) || (CROSS_IN_RIGHT && Far_ipts1[1][1] > 60 && Far_ipts1[1][0] > 80)) // 远边线最下面的一个y坐标大于70,跳出十字模式
    {
        cross_type = CROSS_OUT;    // 出十字
        NearBorderLine_Enable = 1; // 重新开启近边线
        FarBorderLine_Enable = 0;  // 关闭远线
        cross_type = CROSS_NONE;
        a = 66;
        Encoder_Int_Clear(); // 清除编码器积分
    }

    Center_edge();
    Finnal_err = Err_Handle(1);
    if(a == 13)
    {
        Finnal_err = run_straight();
    }
    ips200_show_uint(0,120,a,3);
}
