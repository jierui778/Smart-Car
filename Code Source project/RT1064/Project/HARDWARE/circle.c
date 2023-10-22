#include "circle.h"
#include "image.h"
#include "garage.h"

enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // 记录圆环个数

void Circle_Check(void)
{
    if (garage_type == GARAGE_NONE && circle_type == CIRCLE_NONE && Lpt0_found && !Lpt1_found && is_straight1 && touch_boundary0)
    // 非车库模式，非圆环模式且找到左角点，无右角点，左线迷宫触碰到边沿，右边为长直线
    {
        circle_type = CIRCLE_LEFT_FOUND; // 枚举状态置为找到左圆环
        circle_num++;
    }
    if (circle_type == CIRCLE_NONE && Lpt1_found && !Lpt0_found && is_straight0 && touch_boundary1)
    // 条件与左圆环模式相反
    {
        circle_type = CIRCLE_RIGHT_FOUND; // 枚举状态为找到右圆环
        circle_num++;
    }

    if( ipts0_num>100 && ipts0[ipts0_num-1][1]<40 && touch_boundary1==1 && ipts1[ipts1_num-1][0]>140 && circle_type==CIRCLE_NONE)
    {
        circle_type = CIRCLE_RIGHT_FOUND; // 枚举状态置为找到左圆环
        circle_num++;
    }
    if( ipts1_num>100 && ipts1[ipts1_num-1][1]<40 && touch_boundary0==1 && ipts0[ipts0_num-1][0]>140 && circle_type==CIRCLE_NONE)
    {
        circle_type = CIRCLE_LEFT_FOUND;
    }
}

void Circle_Run(void)
{
    //   if (circle_type == CIRCLE_LEFT_IN)
    //   {
    //       track_type = TRACK_RIGHT;//进左圆环的时候先巡内圆，即左线
    //   }
    //   else if(circle_type == CIRCLE_RIGHT_IN)
    //   //先丢左线后有线
    //   if (rpts0s_num < 20 /*0.4 / sample_dist*/)//左边线点数少于20，预判其即将丢失，准备巡右线（外圆）
    //   {
    //       LeftLine_Loss++;
    //   }

    //   if (rpts0s_num > 30 && LeftLine_Loss > 0)
    //   {
    //       LeftLine_Found++;
    //       if (LeftLine_Found > 1 && x0_first > 2)
    //       { // x0_first>3  意思为防止大圆环过早循左线，导致抖动
    //           circle_type = CIRCLE_LEFT_IN;
    //           LeftLine_Loss = 0; // 清零标志位
    //           LeftLine_Found = 0;
    //           // circle_encoder = current_encoder; // 入环记录位置
    //       }
    //   }
    //   else if (circle_type == CIRCLE_LEFT_IN)
    //   {
    //       track_type = TRACK_LEFT; // 15cm摄像头看不到内线,默认巡左
    //   }
    switch (circle_type)
    {
        case CIRCLE_RIGHT_FOUND:
        {
            if (touch_boundary1 == 1 && touch_boundary0 == 0 && ipts1[ipts1_num - 1][1] < 50)
            {
                circle_type = CIRCLE_RIGHT_IN;
            }
            break;
        }
        case CIRCLE_RIGHT_IN:
        {
            run_cicrle_right_in();
            if(ipts0_num>100 && ipts0[ipts0_num-1][1]<40 && touch_boundary1==1 && ipts1[ipts1_num-1][0]>140)
            {
                circle_type = CIRCLE_RIGHT_FOUND;
            }
        }
    }
}
