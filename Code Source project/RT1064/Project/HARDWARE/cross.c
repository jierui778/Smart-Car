#include "cross.h"
#include "image.h"
#include "circle.h"

enum cross_type_e cross_type = CROSS_NONE;
int cross_num = 0;



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
void Cross_Check(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r)//得考虑斜入十字的情况
{
    //双边确定十字
    // bool Xfound = Lpt0_found && Lpt1_found;
    // if (cross_type == CROSS_NONE && ((touch_boundary0&&Near_Lpt0_Found&&Far_Lpt0_Found)||(touch_boundary1&&Near_Lpt1_Found&&Far_Lpt1_Found)))//双边迷宫巡线到达左右边界且双远近角点存在,正入十字
    // {
    //     cross_type = CROSS_FOUND;
    //     cross_num ++;//记录圆环个数
    // }
    if ((loseline0 == 0 && loseline1 == 0 && (in_put_r[in_put_num_r - 1][0] - in_put_l[in_put_num_l - 3][0]) > (IMAGE_WIDTH - 20))&&cross_type==CROSS_FOUND)
    {
        cross_type=CROSS_BEGIN;
        Finnal_err=run_cross_begin();
    }

    /*十字路口中：左边线不丢线 右边线不丢线 左边线丢线和右边线丢线的列坐标相差较小，行坐标相差较少*/
    if (touch_boundary0 == 1 && touch_boundary1 == 1 && cross_type == CROSS_BEGIN  && (ipts0[ipts0_num - 1][1] > 50 || ipts1[ipts1_num - 1][1] > 50))
    {
        cross_type = CROSS_IN;
        Finnal_err=run_cross_in();
    }

    /*十字状态末：丢线判断*/
    if ((loseline0 == 1 && loseline1 == 1 )&& cross_type == CROSS_IN)
    {
        cross_type = CROSS_OUT;
        Finnal_err=run_cross_out();
    }

    /*左右能找到线：退出十字状态*/
    if(loseline0 ==0 && loseline1 ==0 && cross_type == CROSS_OUT)
    {
        cross_type = CROSS_NONE;
    }
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
    if(cross_type==CROSS_FOUND)
    {  
        if ((loseline0 == 0 && loseline1 == 0 && (in_put_r[in_put_num_r - 1][0] - in_put_l[in_put_num_l - 3][0]) > (IMAGE_WIDTH - 20)))
        {
            cross_type=CROSS_BEGIN;
        }
        
    }
    

	// ips200_draw_line(in_put_l[in_put_num_l-1][0],in_put_l[in_put_num_l-1][1],in_put_r[in_put_num_r-1][0],in_put_r[in_put_num_r-1][1],RGB565_RED);
	// ips200_show_uint(3,200,Cross_State_b,2);
	// ips200_show_uint(3,220,Cross_State_c,2);
	// ips200_show_uint(3,240,Cross_State_d,2);
	// ips200_show_uint(3,260,Straight_State,2);
	// ips200_show_uint(3,280,Left_Turn,2);
	// ips200_show_uint(3,300,Left_Turn_Mid,2);
	// ips200_show_uint(43,200,in_put_num_l,3);
	// ips200_show_uint(43,220,in_put_num_r,3);
	// ips200_show_uint(43,240,ipts0_num,3);
	// ips200_show_uint(43,260,ipts1_num,3);
	// ips200_show_uint(43,280,ipts0[ipts0_num-1][1],2);
	// ips200_show_uint(43,300,ipts1[ipts1_num-1][1],2);
	
	// ips200_show_uint(83,200,touch_boundary0,3);
	// ips200_show_uint(83,220,touch_boundary1,3);
	// ips200_show_uint(83,240,loseline0,3);
	// ips200_show_uint(83,260,loseline1,3);
	
}

/*十字状态begin处理：返回的斜率为row=k*column+b*/
float run_cross_begin(void)
{
    float k_left,k_right;

    NearCorners_Find_Left(ipts0,ipts0_num,CornersLeft_Point,&Near_Lpt0_Found);
    NearCorners_Find_Right(ipts1,ipts1_num,CornersRight_Point,&Near_Lpt1_Found);//求出左下和右下的拐点

    

    k_left = LineRession(ipts0,Lpt0_id);//左边线拟合，求出斜率，截断处理
    k_right = LineRession(ipts1,Lpt1_id);//右边线拟合，求出斜率

    

    ips200_show_float(40,120,k_left,3,3);
    ips200_show_float(40,140,k_right,3,3);
    last_err=err;//误差赋值给上一次的误差

    err=k_left*0.5+k_right*0.5;//左右边线斜率平均值
    
    err=err*0.7+last_err*0.3;//上一次的斜率和这一次的斜率加权

    return err;
}


/*十字状态in处理：返回的斜率为row=k*column+b*/
float run_cross_in(void)
{
    float k_left,k_right;
    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);
    FarCorners_Find_Right(Far_ipts1,Far_ipts1_num,FarCornersRight_Point,&Far_Lpt1_Found);//求出左上和右上的拐点

    NearCorners_Find_Left(ipts0,ipts0_num,CornersLeft_Point,&Near_Lpt0_Found);
    NearCorners_Find_Right(ipts1,ipts1_num,CornersRight_Point,&Near_Lpt1_Found);//求出左下和右下的拐点

    k_left = (FarCornersLeft_Point[1]-CornersLeft_Point[1])/(FarCornersLeft_Point[0]-CornersLeft_Point[0]);//左边线拟合，求出斜率，截断处理

    k_right = (FarCornersRight_Point[1]-CornersRight_Point[1])/(FarCornersRight_Point[0]-CornersRight_Point[0]);//右边线拟合，求出斜率

    last_err=err;//误差赋值给上一次的误差

    err=k_left*0.5+k_right*0.5;//左右边线斜率平均值

    err=err*0.7+last_err*0.3;//上一次的斜率和这一次的斜率加权

    return err;
}


/*十字状态out处理：返回的斜率为row=k*column+b*/
float run_cross_out(void)
{
    float k_left,k_right;

    Find_Borderline_Third();//开启三次扫线

    FarCorners_Find_Left(Extra_ipts0,Extra_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);
    FarCorners_Find_Right(Extra_ipts1,Extra_ipts1_num,FarCornersRight_Point,&Far_Lpt1_Found);//求出左上和右上的拐点

    
    k_left = (FarCornersLeft_Point[1]-118)/(FarCornersLeft_Point[0]-2);//左边线拟合，求出斜率，截断处理

    k_right = (FarCornersRight_Point[1]-118)/(FarCornersRight_Point[0]-157);//右边线拟合，求出斜率


    last_err=err;//误差赋值给上一次的误差

    err=k_left*0.5+k_right*0.5;//左右边线斜率平均值
    
    err=err*0.7+last_err*0.3;//上一次的斜率和这一次的斜率加权

    return err;
}
