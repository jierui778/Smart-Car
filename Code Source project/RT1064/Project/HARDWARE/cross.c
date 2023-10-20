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
void Cross_Check(void)//得考虑斜入十字的情况
{
    //双边确定十字
    // bool Xfound = Lpt0_found && Lpt1_found;
    if ((cross_type == CROSS_NONE && circle_type == CIRCLE_NONE && touch_boundary0 && touch_boundary1&&Near_Lpt0_Found && Far_Lpt1_Found&&Far_Lpt0_Found && Near_Lpt1_Found))//双边迷宫巡线到达左右边界且双远近角点存在,正入十字
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
    if(cross_type == CROSS_FOUND)
    {
        
    }

}
