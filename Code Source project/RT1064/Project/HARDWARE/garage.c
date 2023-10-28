#include "garage.h"
#include "image.h"
#include "encoder.h"

enum garage_type_e garage_type = GARAGE_NONE; // 枚举类型全局定义
int Garage_cnt = 0;
/**
 * @brief 检查车库
 *
 */
void Garage_Check(void)
{
    if (Near_Lpt0_Found && Far_Lpt0_Found && touch_boundary0 && !touch_boundary1)//有左近角点，右近角点，触碰左边界，无触碰右边界
    {
        garage_type = GARAGE_FOUND_LEFT;
        Garage_cnt++;
    }

    else if (Near_Lpt1_Found && Far_Lpt1_Found && touch_boundary1 && !touch_boundary0)//有右近角点，左近角点，触碰右边界，无触碰左边界
    {
        garage_type = GARAGE_FOUND_RIGHT;
        Garage_cnt++;
    }
}
/**
 * @brief 入库
 *
 */
void Garage_Run(void)
{
    if (garage_type == GARAGE_FOUND_LEFT)
    {
        if (FarCornersLeft_Point[1] > 50 && loseline0)//左近线丢线且左远角点y坐标大于50
        {
            garage_type = GARAGE_IN_LEFT;
            Encoder_Int_Enable();
        }
    }
    else if (garage_type == GARAGE_FOUND_RIGHT)
    {
        if (FarCornersRight_Point[1] > 50 && loseline1)//右近线丢线且右远角点y坐标大于50
        {
            garage_type = GARAGE_IN_LEFT;
            Encoder_Int_Enable();
        }
    }
    if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > GARAGE_DIS)//编码器距离大于阈值退出车库模式
    {
        garage_type = GARAGE_STOP;
        Encoder_Int_Clear();
    }
}
/**
 * @brief 出库
 *
 */
void Garage_Out(void)
{
    if (garage_type == GARAGE_OUT_LEFT && Near_Lpt0_Found && Near_Lpt1_Found && touch_boundary0 && touch_boundary1)
    {
        garage_type == GARAGE_OUTTING_LEFT;
        Encoder_Int_Enable();
    }
    else if (garage_type == GARAGE_OUT_RIGHT && Near_Lpt0_Found && Near_Lpt1_Found && touch_boundary0 && touch_boundary1)
    {
        garage_type == GARAGE_OUTTING_RIGHT;
        Encoder_Int_Enable();
    }
    if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > GARAGE_DIS)
    {
        garage_type = GARAGE_NONE;
        Encoder_Int_Clear();
    }
}