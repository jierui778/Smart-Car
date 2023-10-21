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
void Cross_Check(void)//得考虑斜入十字的情况
{
    //双边确定十字
    // bool Xfound = Lpt0_found && Lpt1_found;
    if (cross_type == CROSS_NONE && ((touch_boundary0&&Near_Lpt0_Found&&Far_Lpt0_Found)||(touch_boundary1&&Near_Lpt1_Found&&Far_Lpt1_Found)))//双边迷宫巡线到达左右边界且双远近角点存在,正入十字
    {
        cross_type = CROSS_FOUND;
        cross_num ++;//记录圆环个数
    }
}

/**
 * @brief 十字运行函数
 *
 */
void Cross_Run(void)
{
    int a = 0;
    if(cross_type == CROSS_FOUND)
    {
        a = 1;
        if ((ipts0_num < 40 || ipts1_num < 40) && touch_boundary1 && touch_boundary0 && ((Near_Lpt0_Found && Far_Lpt0_Found) || (Near_Lpt1_Found && Far_Lpt1_Found)))
        {
            cross_type = CROSS_IN; // 进入圆环

            }
    }
    if(cross_type == CROSS_IN)
    {
        a = 2;
        if ((Far_ipts0[5][1] > 60 && Far_ipts1[5][1] > 60))
        {
            cross_type = CROSS_OUT;//出圆环
        }
        touch_boundary1 = 0;
        touch_boundary0 = 0;
    }
    if(cross_type == CROSS_OUT)
    {
        a = 3;
    }
    ips200_show_uint(200, 250, a, 3);
    ips200_show_uint(200, 270, Far_ipts0[5][1], 3);
    ips200_show_uint(200, 290, Far_ipts1[5][1], 3);
    ips200_draw_line(0, 0, Far_ipts0[5][0], Far_ipts0[5][1], RGB565_BLUE);
    ips200_draw_line(0, 0, Far_ipts1[5][0], Far_ipts1[5][1], RGB565_BLUE);
}
