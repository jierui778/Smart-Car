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

uint8 Straight_State = 0;
/**
 * @brief 检查图像状态，判断当前是否处于十字路口、直道、左弯道或右弯道
 *
 * @param in_put_l 左边线的坐标数组
 * @param in_put_num_l 左边线的坐标数量
 * @param in_put_r 右边线的坐标数组
 * @param in_put_r 右边线的坐标数量
 */
void Image_CheckState(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r)
{
    /*十字路口前：左右不丢线 左边线丢线和右边线丢线的列坐标相差较大，行坐标相差较少*/
    if ((loseline0 == 0 && loseline1 == 0 && (in_put_r[in_put_num_r - 1][0] - in_put_l[in_put_num_l - 3][0]) > (IMAGE_WIDTH - 10)) && (my_abs(in_put_l[in_put_num_l - 3][1] - in_put_r[in_put_num_r - 1][1]) < 10) && Cross_State_d == 0)
    {
        cross_type=CROSS_BEGIN;
        turn_type=TURN_NONE;
        Straight_State = 0;
        Left_Turn_Mid = 0;
    }
    /*十字路口中：左边线不丢线 右边线不丢线 左边线丢线和右边线丢线的列坐标相差较小，行坐标相差较少*/
    if (touch_boundary0 == 1 && touch_boundary1 == 1 && cross_type == CROSS_BEGIN  && (ipts0[ipts0_num - 1][1] > 50 || ipts1[ipts1_num - 1][1] > 50))
    {
        cross_type = CROSS_IN;
        turn_type=TURN_NONE;
        Straight_State = 0;
        
    }
    /*十字状态末：丢线判断*/
    if ((loseline0 == 1 && loseline1 == 1 )&& cross_type == CROSS_IN)
    {
        cross_type = CROSS_OUT;
        turn_type=TURN_NONE;
        Straight_State = 0;
    }

    /*直道：左右线找到的点超过100，左右线的最后一个点的行坐标在30以内*/
    if (ipts0_num > 80 && ipts1_num > 80 && ipts0[ipts0_num - 1][1] < 30 && ipts1[ipts1_num - 1][1] < 30)
    {
        
        Straight_State = 1;
        turn_type = TURN_NONE;
        cross_type = CROSS_NONE;
    }

    /*左弯道中：左边不丢线，右边也不丢线，左边到边界，左右点数差异大*/
    if(touch_boundary0==1&&in_put_num_r>80&&cross_type != CROSS_IN)
    {
        turn_type = TURN_LEFT_MID;
        cross_type = CROSS_NONE;
        Straight_State = 0;
    }
    /*左弯道后：左丢线，右边不丢线，右边线最大点距离左边小于50个的单位*/
    if (loseline0 == 1 && in_put_num_r > 80 && in_put_r[in_put_num_r - 1][0] < 60)
    {
        turn_type = TURN_LEFT;
        cross_type = CROSS_NONE;
        Straight_State = 0;
    }

    /*右弯道中：右丢线，左边不丢线，左边线最大点距离右边小于50个的单位*/
    if (touch_boundary1 == 1 && in_put_l[in_put_num_l - 1][0] > 120 && in_put_num_l > 100)
    {
        turn_type = TURN_RIGHT_MID;
        cross_type = CROSS_NONE;
        Straight_State = 0;
    }
    /*右弯道后 ：右边找不到线，左边不丢线，左边点的个数大于100，左线最后一个点在较右处*/
    if (loseline1 == 1 && in_put_l[in_put_num_l - 1][0] > 120 && in_put_num_l > 100 && Right_Turn_Mid == 1)
    {
        turn_type = TURN_MID;
        cross_type = CROSS_NONE;
        Straight_State = 0;
    }

    if(touch_boundary0==1 &&ipts0_num>100&&ipts0[ipts0_num-1][1]<30)
    {
        Huandao_a=1;
    }
	// ips200_draw_line(in_put_l[in_put_num_l-1][0],in_put_l[in_put_num_l-1][1],in_put_r[in_put_num_r-1][0],in_put_r[in_put_num_r-1][1],RGB565_RED);
	ips200_show_uint(3,200,Cross_State_b,2);
	ips200_show_uint(3,220,Cross_State_c,2);
	ips200_show_uint(3,240,Cross_State_d,2);
	ips200_show_uint(3,260,Straight_State,2);
	ips200_show_uint(3,280,Left_Turn,2);
	ips200_show_uint(3,300,Left_Turn_Mid,2);
	ips200_show_uint(43,200,in_put_num_l,3);
	ips200_show_uint(43,220,in_put_num_r,3);
	ips200_show_uint(43,240,ipts0_num,3);
	ips200_show_uint(43,260,ipts1_num,3);
	ips200_show_uint(43,280,ipts0[ipts0_num-1][1],2);
	ips200_show_uint(43,300,ipts1[ipts1_num-1][1],2);
	
	ips200_show_uint(83,200,touch_boundary0,3);
	ips200_show_uint(83,220,touch_boundary1,3);
	ips200_show_uint(83,240,loseline0,3);
	ips200_show_uint(83,260,loseline1,3);
	
}

/*左下线坐标变换，用于求拐点，原始坐标原点为左上角，变换后为左下角*/
void Coordinate_transformation_left(int pt0_in[][2], int in_num, int pt0_out[][2])
{
    int i;
    for (i = 0; i < in_num; i++)
    {
        pt0_out[i][1] = IMAGE_HEIGHT - pt0_in[i][1] - 1;//y坐标倒置，x坐标保持不变
        pt0_out[i][0] = pt0_in[i][0];
    }
}
