#include "cross.h"
#include "image.h"
#include "circle.h"
#include "encoder.h"
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

void Cross_Run(void)
{
    if (cross_type == CROSS_FOUND)
    {
        a = 1;
        if (ipts0_num < 40 || ipts1_num < 40) // 左右边线一边点数小于40且双远近角点存在,进入十字
        {
            cross_type = CROSS_IN; // 进入十字
            touch_boundary0 = 0;
            touch_boundary1 = 0;
            Near_Lpt0_Found = 0;
            Near_Lpt1_Found = 0;
            // ips200_show_int(200, 200, 6, 1);
        }
    }
    if (cross_type == CROSS_IN)
    {
        Encoder_Int_Enable(); // 开启编码器积分
        a = 2;
        if ((Far_ipts0[5][1] > 70 && Far_ipts1[5][1] > 70)) // 远边线最下面的一个y坐标大于70,跳出十字模式
        {
            cross_type = CROSS_OUT; // 出圆环
            ips200_show_int(200, 200, 7, 1);
        }
        touch_boundary1 = 0;
        touch_boundary0 = 0;
        Far_Lpt1_Found = 0;
        Far_Lpt0_Found = 0;  // 清零标志位
        Encoder_Int_Clear(); // 清零编码器积分
    }
    // ips200_show_uint(200, 250, a, 3);
    // ips200_show_uint(200, 270, Far_ipts0[5][1], 3);
    // ips200_show_uint(200, 290, Far_ipts1[5][1], 3);
    // ips200_draw_line(0, 0, Far_ipts0[5][0], Far_ipts0[5][1], RGB565_BLUE);
    // ips200_draw_line(0, 0, Far_ipts1[5][0], Far_ipts1[5][1], RGB565_BLUE);
    // ips200_show_int(200, 120, touch_boundary0, 1);
    // ips200_show_int(200, 140, touch_boundary1, 1);
}
