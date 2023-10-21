#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"
enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // 记录圆环个数

void Circle_Check(void)
{
    if (circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found) // 单边长直线且另外一边为角点+圆弧
    // 非车库模式，非圆环模式且找到左角点，无右角点，左线迷宫触碰到边沿，右边为长直线
    {
        circle_type = CIRCLE_LEFT_FOUND; // 枚举状态置为找到左圆环
        circle_num++;
    }
    else if (circle_type == CIRCLE_NONE && Near_Lpt1_Found && Far_Lpt1_Found)
    // 条件与左圆环模式相反
    {
        circle_type = CIRCLE_RIGHT_FOUND; // 枚举状态为找到右圆环
        circle_num++;
    }
}

void Circle_Run(void)
{
    if (circle_type == CIRCLE_LEFT_FOUND)
    {
        track_type = TRACK_RIGHT; // 进左圆环前，先巡右线
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND)
    {
        track_type = TRACK_LEFT; // 进右圆环前，先巡右线
    }
    Encoder_Int_Enable(); // 开启编码器积分

    if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // 圆弧外凸坐标达到范围内,循内圆入环
    {
        track_type = TRACK_RIGHT;
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] < 40)
    {
        track_type = TRACK_RIGHT;
    }

    if (1) // 进入圆环,内园必丢线,循外圆
    {
    }
    else if (1) //
    {
    }

    if (Near_Lpt1_Found && (Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_ENCODER_THRES) // 发现单边有角点且编码器积分到达阈值,准备出圆环
    {
    }
    else if (1)
    {
    }
    Encoder_Int_Clear(); // 清零编码器积分

    // if (ipts1_num > 30 && LeftLine_Loss > 0)
    // {
    //     LeftLine_Found++;
    //     if (LeftLine_Found > 1 && x0_first > 2)
    //     { // x0_first>3  意思为防止大圆环过早循左线，导致抖动
    //         circle_type = CIRCLE_LEFT_IN;
    //         LeftLine_Loss = 0; // 清零标志位
    //         LeftLine_Found = 0;
    //         // circle_encoder = current_encoder; // 入环记录位置
    //     }
    // }
    // else if (circle_type == CIRCLE_LEFT_IN)
    // {
    //     track_type = TRACK_LEFT; // 15cm摄像头看不到内线,默认巡左
    // }
}
