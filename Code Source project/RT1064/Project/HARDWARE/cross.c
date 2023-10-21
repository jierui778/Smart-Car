#include "cross.h"
#include "image.h"
#include "circle.h"
enum cross_type_e cross_type = CROSS_NONE;
int cross_num = 0;

/*十字检测思路：
双边迷宫巡线到达左右边界
是否需要再向上巡线，二次验证双远角点的存在

/**
 * @brief 十字检测函数
*/
void Cross_Check(void) // 得考虑斜入十字的情况
{
    // 双边确定十字
    //  bool Xfound = Lpt0_found && Lpt1_found;
    if (cross_type == CROSS_NONE && ((touch_boundary0 && Near_Lpt0_Found && Far_Lpt0_Found) || (touch_boundary1 && Near_Lpt1_Found && Far_Lpt1_Found))) // 双边迷宫巡线到达左右边界且双远近角点存在,正入十字
    {
        cross_type = CROSS_FOUND;
        cross_num++; // 记录圆环个数
    }
}

uint8 Straight_State = 0;
/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 *
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */
void Image_CheckState(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r)
{
    int a = 0;
    if (cross_type == CROSS_FOUND)
    {
        a = 1;
        if ((ipts0_num < 40 || ipts1_num < 40) && touch_boundary1 && touch_boundary0 && ((Near_Lpt0_Found && Far_Lpt0_Found) || (Near_Lpt1_Found && Far_Lpt1_Found))) // 左右边线一边点数小于40且双远近角点存在,进入十字
        {
            cross_type = CROSS_IN; // 进入十字
        }
    }
    if (cross_type == CROSS_IN)
    {
        a = 2;
        if ((Far_ipts0[5][1] > 50 && Far_ipts1[5][1] > 50))
        {
            cross_type = CROSS_OUT; // 出圆环
        }
        touch_boundary1 = 0;
        touch_boundary0 = 0;
        Far_Lpt0_Found = 0;
        Far_Lpt0_Found = 0;//清零标志位
    }
    // ips200_show_uint(200, 250, a, 3);
    // ips200_show_uint(200, 270, Far_ipts0[5][1], 3);
    // ips200_show_uint(200, 290, Far_ipts1[5][1], 3);
    // ips200_draw_line(0, 0, Far_ipts0[5][0], Far_ipts0[5][1], RGB565_BLUE);
    // ips200_draw_line(0, 0, Far_ipts1[5][0], Far_ipts1[5][1], RGB565_BLUE);
    // ips200_show_int(200, 120, touch_boundary0, 1);
    // ips200_show_int(200, 140, touch_boundary1, 1);
}
