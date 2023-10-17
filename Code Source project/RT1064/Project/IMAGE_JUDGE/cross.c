#include "cross.h"
int circle_num = 0;

// 入环出环偏差放大倍数
float circle_in = 1.05, circle_out = 1.0;

// 但人为放置在在CPU0后，就可以了
// 原图左右边线
int far_circle_ipts0[LLLL][2];
int far_circle_ipts1[LLLL][2];
int far_circle_ipts0_num, far_circle_ipts1_num;

// 变换后左右边线
float far_circle_rpts0[LLLL][2];
float far_circle_rpts1[LLLL][2];
int far_circle_rpts0_num, far_circle_rpts1_num;

// 变换后左右边线+滤波
float far_circle_rpts0b[LLLL][2];
float far_circle_rpts1b[LLLL][2];
int far_circle_rpts0b_num, far_circle_rpts1b_num;

// 变换后左右边线+等距采样
float far_circle_rpts0s[LLLL][2];
float far_circle_rpts1s[LLLL][2];
int far_circle_rpts0s_num, far_circle_rpts1s_num;

// 左右边线局部角度变化率
float far_circle_rpts0a[LLLL];
float far_circle_rpts1a[LLLL];
int far_circle_rpts0a_num, far_circle_rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
float far_circle_rpts0an[LLLL];
float far_circle_rpts1an[LLLL];
int far_circle_rpts0an_num, far_circle_rpts1an_num;

enum cross_type_e cross_type = CROSS_NONE;//初始化时为非十字模式

void Cross_Check(void)
{
    if (circle_type == CIRCLE_NONE && Lpt0_found && Lpt1_found && touch_boundary0 && touch_boundary1)//未进入圆环状态，且左右角点找到，且迷宫巡线触碰到边框
    {
        cross_type = CROSS_FOUND;//枚举状态为找到十字
        circle_num++;//十字计数
    }



}

void Cross_Run(void)
{
    if (cross_type == CROSS_FOUND)//如果枚举状态为找到十字
    {

    }
    //找到近处线，跳出十字状态
    if (rpts1s_num < 20 && rpts0s_num < 20)
    {

    }
}




/**
 * @brief 十字补线
 *
 */
void Cross_DrawLine(void)
{


}



