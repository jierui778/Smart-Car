#include "garage.h"

// enum garage_type_e garage_type = GARAGE_NONE;//初始化车库类型为非车库模式，debug
enum garage_type_e garage_type = GARAGE_OUT_RIGHT;//初始化车库类型为右出库，比赛要求
uint8_t Garage_Cnt = 0;//车库计数器，记录第几个车库


void Garage_Check(void)
{
    if()

}


/**
 * @brief 发车放车库中心，距离出赛道一个手掌的距离
 *
 */
void Garage_Out(void)
{
        if (garage_type == GARAGE_OUT_RIGHT)
    {

    }

}


void Garage_In(void)
{
    int32 Bottom_black_point_num = 0;//记录底部黑线的点数
    for (int j = IMAGE_HEIGHT - 2; j > IAMGE_WIDTH - 10; j--)
    {
        for (int i = 0; i < UCOL; i++)
        {
            if (Image_Use_Robert[j][i] == 0) // 底部两行黑点数            一般正常最多不超过100，斑马线大概300，阈值garage_thres可以设置120(Sobel) 440(Ostu)
            {
                Bottom_black_point_num++;//累计斑马线的黑点数
            }
        }
    }
    if (garage_type == GARAGE_FOUND_RIGHT && Garage_Cnt == 1 && Bottom_black_point_num > ZEBRA_THRES)
    {
        garage_type = GARAGE_IN_RIGHT;//右入库
    }
    else if (garage_type == GARAGE_FOUND_LEFT && Garage_Cnt == 1 && Bottom_black_point_num > ZEBRA_THRES)
    {
        garage_type = GARAGE_IN_LEFT;//左入库
    }

}