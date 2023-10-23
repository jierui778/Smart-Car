#include "cross.h"
#include "image.h"
#include "circle.h"
#include "encoder.h"

enum cross_type_e cross_type = CROSS_NONE;
int cross_num = 0; // 记录圆环个数
int a;
/*十字检测思路：
双边迷宫巡线到达左右边界
是否需要再向上巡线，二次验证双远角点的存在

*/

/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 *
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */

void Cross_Check(void) // 得考虑斜入十字的情况
{
    // 双边确定十字
    //  bool Xfound = Lpt0_found && Lpt1_found;
    if (cross_type == CROSS_NONE && touch_boundary0 && Near_Lpt0_Found && Far_Lpt0_Found && !Near_Lpt1_Found) // 左边双角点,右边近角点丢失,斜入左十字,进入十字模式
    {
        cross_type = CROSS_HALF_LEFT_FOUND;
        Cross_Drawline_Found_Left();
        cross_num++; // 记录圆环个数
        a = 1;
    }
    else if (cross_type == CROSS_NONE && touch_boundary1 && Near_Lpt1_Found && Far_Lpt1_Found && !Near_Lpt0_Found) // 右边双角点,左边近角点丢失,斜入右十字,进入十字模式
    {
        cross_type = CROSS_HALF_RIGHT_FOUND;
        Cross_Drawline_Found_Right();
        cross_num++; // 记录圆环个数
        a = 2;
    }
    else if (cross_type == CROSS_NONE && touch_boundary0 && Near_Lpt0_Found && Far_Lpt0_Found && touch_boundary1 && Near_Lpt1_Found && Far_Lpt1_Found) // 双边双角点,进入十字模式
    {
        cross_type = CROSS_DOUBLLE_FOUND;
        Cross_Drawline_Found_Double();
        cross_num++; // 记录圆环个数
        a = 3;
    }

    else 
    {
        cross_type = CROSS_NONE;
    }
    Finnal_err = Center_edge(1); // 求出最终误差
}


int count=0;
/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 *
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */
void Cross_Run(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r)
{
    if(cross_type==CROSS_HALF_LEFT_FOUND)
    {  
        if ((loseline0 == 0 && loseline1 == 0 && (in_put_r[in_put_num_r - 1][0] - in_put_l[in_put_num_l - 3][0]) > (IMAGE_WIDTH - 20)))
        {
            cross_type = CROSS_IN_LEFT; // 左边近线丢失,循左边远线
            Cross_Drawline_In_Left();
            Encoder_Int_Enable();
            a = 12;
            NearBorderLine_Enable = 0; // 关闭近边线
        }
    }

    else if(cross_type == CROSS_HALF_RIGHT_FOUND)
    {
        if(loseline0 ==0 && loseline1 ==0 &&in_put_r[in_put_num_r-1][0] - in_put_l[in_put_num_l - 3][0] > (IMAGE_WIDTH - 20))
        {
            cross_type = CROSS_IN_RIGHT;
            Cross_Drawline_Found_Right();
            a = 12 ;
            NearBorderLine_Enable = 0; // 关闭近边线
        }
    }
    else if (cross_type == CROSS_DOUBLLE_FOUND) // 斜入左十字
    {
        if (loseline0 && loseline1) // 左边线一边点数小于40,进入十字
        {
            cross_type = CROSS_IN_DOUBLE; // 左边近线丢失,循左边远线
            Encoder_Int_Enable();
            Cross_Drawline_In_Double();
            a = 13;
            NearBorderLine_Enable = 0; // 关闭近边线
        }
    }

    
    if (((cross_type == CROSS_IN_LEFT && Far_ipts0[1][1] > 60 && Far_ipts0[1][0] < 40) || (cross_type == CROSS_IN_RIGHT && Far_ipts1[1][1] > 60 && Far_ipts1[1][0] > 80)
        ||(cross_type == CROSS_IN_DOUBLE && Far_ipts0[1][1] > 60 && Far_ipts0[1][0] < 40 && Far_ipts1[1][1] > 60 && Far_ipts1[1][0] > 80)) && loseline0 && loseline1) // 远边线最下面的一个y坐标大于70,跳出十字模式
    {
        cross_type = CROSS_OUT;    // 出十字
        NearBorderLine_Enable = 1; // 重新开启近边线
        FarBorderLine_Enable = 0;  // 关闭远线
        cross_type = CROSS_NONE;

        a=15;

        Encoder_Int_Clear(); // 清除编码器积分
    }

    Finnal_err = Center_edge(1); // 求出最终误差

    ips200_show_uint(0,120,a,3);
    ips200_show_uint(200, 230, Far_ipts0[1][0], 2);
    ips200_show_uint(200, 250, Far_ipts0[1][1], 2);
    ips200_show_uint(200, 270, a, 2);
}

void Cross_Drawline_In_Left(void)
{
    int i;
    float k_left, b_left;
    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);

    k_left=(float)(Far_ipts0[Far_Lpt0_id][1]-118)/(Far_ipts0[Far_Lpt0_id][0]-2);
    b_left=Far_ipts0[Far_Lpt0_id][1]*k_left-Far_ipts0[Far_Lpt0_id][0];//将拐点坐标和左下右下相连，求出斜率
    k_left = (1 / k_left);
    b_left = k_left * (-b_left);

    for (i = Far_ipts0[Far_Lpt0_id][1]; i < 115; i++)
    {
        int new_column_l = (int)(k_left * i + b_left);
        if (new_column_l > 0 && a == 11)
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
}

void Cross_Drawline_In_Right(void)
{
    int i;
    float k_right, b_right;
    FarCorners_Find_Right(Far_ipts1,Far_ipts1_num,FarCornersRight_Point,&Far_Lpt1_Found);

    k_right=(float)(Far_ipts1[Far_Lpt1_id][1]-118)/(Far_ipts1[Far_Lpt1_id][0]-157);
    b_right=Far_ipts1[Far_Lpt1_id][1]*k_right-Far_ipts1[Far_Lpt1_id][0];//将拐点坐标和左下右下相连，求出斜率
    
    k_right = (1 / k_right);
    b_right = k_right * (-b_right);

    for(i = Far_ipts1[Far_Lpt1_id][1]; i <115 ; i++)
    {
        int new_column_r = (int)(k_right * i + b_right);
        if (new_column_r > 0 && a == 12)
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }
}

/**
 * @brief 十字状态d的运行函数
 * 
 */
void Cross_Drawline_In_Double(void)
{
    float k_r,k_l,b_r,b_l;//定义左右边线斜率和截距
    float k_l_line,k_r_line;//定义左右中线斜率

    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);
    FarCorners_Find_Right(Far_ipts1,Far_ipts1_num,FarCornersRight_Point,&Far_Lpt1_Found);

    k_l=(float)(Far_ipts0[Far_Lpt0_id][1]-118)/(Far_ipts0[Far_Lpt0_id][0]-2);
    b_l=Far_ipts0[Far_Lpt0_id][1] - k_l * Far_ipts0[Far_Lpt0_id][0];//将拐点坐标和左下右下相连，求出斜率
    //直线方程为 row=k*column+b，但是补线的时候要转化为column=k*row+b（其实后面也没用补线，直接求斜率即可）
    k_l = (1 / k_l);
    b_l = k_l * (-b_l);

    k_r = (float)(FarCornersRight_Point[1] - 118) / (FarCornersRight_Point[0] - 158);
    b_r = FarCornersRight_Point[1] - k_r * FarCornersRight_Point[0];

        
    k_r=(1/k_r);
    b_r=-b_r*k_r;
    int i;
    /*这里是补线，补线后再次扫一次左右边线
        但是直接用斜率我觉得也是可以的，这部分可以去掉*/
    for (i = Far_ipts0[Lpt0_id][1]; i < 118; i++)
    {
        int new_column_l = (int)(k_l * i + b_l);
        if (new_column_l > 0 )
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
    for (i = Far_ipts1[Far_Lpt0_id][1]; i <118; i++)
    {
        int new_column_r = (int)(k_r * i + b_r);
        if (new_column_r > 0 )
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }

}



/**
 * @brief 找到双十字的运行函数
 * 
 */
void Cross_Double_Found(void)
{
    int mid_line[150][2]; // 中线数组，150个防止溢出
    mid_line_num = 0;//中线数组的计数值
    uint8 i;
    if (ipts0_num > ipts1_num)
    {
        mid_line_num = Lpt0_id; // 中线个数赋值
    }
    else
    {
        mid_line_num = Lpt1_id; // 中线个数赋值
    }
    for (i = 0; i < mid_line_num; i++)//截断处理，从第一个点取到左右边线的最近的一个拐点的下标
    {
        mid_line[i][0] = (ipts1[i][0] + ipts0[i][0]) / 2;
        mid_line[i][1] = (ipts1[i][1] + ipts0[i][1]) / 2;
    }
    /*显示函数，可以去掉*/
    for (i = 0; i < mid_line_num; i++)
    {
        ips200_draw_point(mid_line[i][0], mid_line[i][1], RGB565_GREEN);
    }

    last_err = err;//存储上次的误差

    err = LineRession(mid_line, mid_line_num - 1);//中线求出误差

    err = 0.8 * err + 0.2 * last_err;//简单滤波处理

    Finnal_err = err;//直接用全局变量，不返回误差
}

/**
 * @brief 十字状态c的运行函数
 * 
 */
void Cross_In_Double(void)
{
    FarBorderline_Find();//寻远线
    Cross_Drawline_In_Double();//直接返回误差（函数里面会有滤波）
}


void Cross_Drawline_Found_Left(void)
{
    uint16 i;
    float k_left,b_left;

    NearCorners_Find_Left(ipts0,ipts0_num,CornersLeft_Point,&Near_Lpt0_Found);

    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);

    k_left = (float)(ipts0[Lpt0_id][1] - Far_ipts0[Far_Lpt0_id][1]) / (ipts0[Lpt0_id][0] - Far_ipts0[Far_Lpt0_id][0]);
    b_left = ipts0[Lpt0_id][1] - k_left * ipts0[Lpt0_id][0];

    k_left = (1 / k_left);     // 新斜率取倒数
    b_left = -b_left * k_left; // 新截距取相反数

    for (i = ipts0[Lpt0_id][1]; i > 10; i--)
    {
        int new_column_l = (int)(k_left * i + b_left);
        if (new_column_l > 0 && a == 1)
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
}

void Cross_Drawline_Found_Right(void)
{
    uint16 i;
    float k_right,b_right;

    NearCorners_Find_Right(ipts1,ipts1_num,CornersRight_Point,&Near_Lpt1_Found);
    FarCorners_Find_Right(Far_ipts1,Far_ipts1_num,FarCornersRight_Point,&Far_Lpt1_Found);

    k_right = (float)(ipts1[Lpt1_id][1] - Far_ipts1[Far_Lpt1_id][1]) / (ipts1[Lpt1_id][0] - Far_ipts1[Far_Lpt1_id][0]);
    b_right = ipts1[Lpt1_id][1] - k_right * ipts1[Lpt1_id][0];

    k_right = (1 / k_right);      // 新斜率取倒数
    b_right = -b_right * k_right; // 新截距取相反数

    for (i = ipts1[Lpt1_id][1]; i > 10; i--)
    {
        int new_column_r = (int)(k_right * i + b_right);
        if (new_column_r > 0 && a == 2)
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }
}

void Cross_Drawline_Found_Double(void)
{
    uint16 i;
    uint16 left_index, right_index; // 左右拐点的坐标
    uint16 left_highest = 0, right_highest = 0;
    uint16 right_up_highest_index;
    uint16 left_up_highest_index;
    float k_left, k_right;
    float b_left, b_right;
    float k_left_low, k_right_low;
    
    NearCorners_Find_Left(ipts0,ipts0_num,CornersLeft_Point,&Near_Lpt0_Found);
    NearCorners_Find_Right(ipts1,ipts1_num,CornersRight_Point,&Near_Lpt1_Found);

    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);
    FarCorners_Find_Right(Far_ipts1,Far_ipts1_num,FarCornersRight_Point,&Far_Lpt1_Found);

    /*三 求直线方程*/
    k_left = (float)(ipts0[Lpt0_id][1] - Far_ipts0[Far_Lpt0_id][1]) / (ipts0[Lpt0_id][0] - Far_ipts0[Far_Lpt0_id][0]);

    b_left = ipts0[Lpt0_id][1] - k_left * ipts0[Lpt0_id][0];

    // 求得的直线方程是row=column*k+b，实际上应该是column=row*k+b
    k_right = (float)(ipts1[Lpt1_id][1] - Far_ipts1[Far_Lpt1_id][1]) / (ipts1[Lpt1_id][0] - Far_ipts1[Far_Lpt1_id][0]);

    b_right = ipts1[Lpt1_id][1] - k_right * ipts1[Lpt1_id][0];

    k_left = (1 / k_left);     // 新斜率取倒数
    b_left = -b_left * k_left; // 新截距取相反数

    k_right = (1 / k_right);      // 新斜率取倒数
    b_right = -b_right * k_right; // 新截距取相反数

    // 新直线方程为 column=k*row+b
    /*四 补线*/
    for (i = ipts0[Lpt0_id][1]; i > 10; i--)
    {
        int new_column_l = (int)(k_left * i + b_left);
        if (new_column_l > 0 && a == 3)
        {
            Image_Use_Robert[i][new_column_l] = BLACK;
        }
    }
    for (i = ipts1[Lpt1_id][1]; i > 10; i--)
    {
        int new_column_r = (int)(k_right * i + b_right);
        if (new_column_r > 0 && a == 3)
        {
            Image_Use_Robert[i][new_column_r] = BLACK;
        }
    }

}



