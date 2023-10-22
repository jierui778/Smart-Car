#include "cross.h"
#include "image.h"
#include "circle.h"
enum cross_type_e cross_type = CROSS_NONE;
int cross_num;



/*十字检测思路：
双边迷宫巡线到达左右边界
是否需要再向上巡线，二次验证双远角点的存在


/**
 * @brief 十字检测函数
*/
void Cross_Check(void)
{
    //双边确定十字
    // bool Xfound = Lpt0_found && Lpt1_found;
    if (cross_type == CROSS_NONE && circle_type == CIRCLE_NONE && touch_boundary0 && touch_boundary1)
    {
        // cross_encoder = Z.all_length ;
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
    
    switch (cross_type)
    {
        /*进入十字：状态1 补线*/
        case CROSS_BEGIN:
        {
            if (touch_boundary0 == 1 && touch_boundary1 == 1 && (ipts0[ipts0_num - 1][1] > 50 || ipts1[ipts1_num - 1][1] > 50))
            {
                cross_type = CROSS_IN;
            }
            run_cross_b();
            break;
        }

        /*十字中：状态2*/
        case CROSS_IN:
        {
            if ((loseline0 == 1 && loseline1 == 1 ))
            {
                cross_type = CROSS_OUT;
            }
            run_cross_c();
            break;
        }

        case CROSS_OUT:
        {
            run_cross_d();
            break;
        }

        case CROSS_FOUND:
        {
            if((loseline0 == 0 && loseline1 == 0 && (ipts1[ipts1_num - 1][0] - ipts0[ipts0_num - 3][0]) > (IMAGE_WIDTH - 10)) && (my_abs(ipts0[ipts0_num - 3][1] - ipts1[ipts1_num - 1][1]) < 10))
            {
                cross_type=CROSS_BEGIN;
            }
            break;
        }

        case CROSS_NONE:
        {
            break;
        }
    }
}
