#include "garage.h"
// enum garage_type_e garage_type = GARAGE_NONE;//初始化车库类型为非车库模式，debug
enum garage_type_e garage_type = GARAGE_OUT_RIGHT;//初始化车库类型为右出库，比赛要求
uint8_t Garage_Cnt = 0;//车库计数器，记录第几个车库，暂时用不上





/**
 * @brief 发车放车库中心，距离出赛道一个手掌的距离
 *
 */
void Garage_Out(void)
{
    if (garage_type == GARAGE_OUT_RIGHT)
    {
        //巡右线，枚举赋予状态

    }
    else if(garage_type == GARAGE_OUT_LEFT)
    {
        //巡左线，枚举赋予状态

    }

    if (Garage_Distance > GARAGE_DISTANCE_THRES) // 积分距离大于出库阈值
    {
        garage_type = GARAGE_NONE; // 非车库模式,进入直道
        Garage_Distance = 0;       // 清零
    }
}

/**
 * @brief 车库检测
 *
 */
void Garage_Check(void)
{
    if ((garage_type == GARAGE_FOUND_RIGHT||GARAGE_FOUND_LEFT)&&Zebra_Threshold_Count > ZEBRA_THRES) // 右边/左边找到车库且找到的黑点数大于斑马线黑点数阈值，则认为车库在即，进入车库模式
    {
        if(garage_type == GARAGE_FOUND_RIGHT)//右边发现车库
        {
            //相关执行函数

        }
        else if(garage_type == GARAGE_FOUND_LEFT)
        {
            //执行函数
        }
    }
}
/**
 * @brief 进入车库
 *
 */
void Garage_In(void)
{

    // int32 Bottom_black_point_num = 0;//记录底部黑线的点数
    // for (int j = IMAGE_HEIGHT - 2; j > IAMGE_WIDTH - 10; j--)
    // {
    //     for (int i = 0; i < UCOL; i++)
    //     {
    //         if (Image_Use_Robert[j][i] == 0) // 底部两行黑点数            一般正常最多不超过100，斑马线大概300，阈值garage_thres可以设置120(Sobel) 440(Ostu)
    //         {
    //             Bottom_black_point_num++;//累计斑马线的黑点数
    //         }
    //     }
    // }
    // if (garage_type == GARAGE_FOUND_RIGHT && Garage_Cnt == 1 && Bottom_black_point_num > ZEBRA_THRES)
    // {
    //     garage_type = GARAGE_IN_RIGHT;//右入库
    // }
    // else if (garage_type == GARAGE_FOUND_LEFT && Garage_Cnt == 1 && Bottom_black_point_num > ZEBRA_THRES)
    // {
    //     garage_type = GARAGE_IN_LEFT;//左入库
    // }



}