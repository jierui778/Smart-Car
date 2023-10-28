#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"

enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // ??????????

int Fix_Point[2] = {158,80}; // 右下角固定点
int Fin_Point2[2] = {2,10};//左上角固定点
int Fix_Point3[2]={1,100};//左下角固定点
int Fix_Point4[2]={80,10};//中上固定点
int cross_state=0;
void Circle_Check(void)
{
    // if (circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found) // ?????????????????????+???
    // // ??????????????????????????????????????????????????????????
    // {
    //     circle_type = CIRCLE_LEFT_FOUND; // ????????????????
    //     circle_num++;
    // }
    // else if (circle_type == CIRCLE_NONE && Near_Lpt1_Found && Far_Lpt1_Found)
    // // ???????????????
    // {
    //     circle_type = CIRCLE_RIGHT_FOUND; // ??????????????
    //     circle_num++;
    // }
    // else
    // {
    //     circle_type = CIRCLE_NONE;
    // }
    /*2023/10/26*/
    if(circle_type == CIRCLE_NONE && Near_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found)//左环岛，只有左边两个拐点能找到
    {
        circle_type = CIRCLE_LEFT_FOUND;
        cross_state = 1;
        circle_num++;
    }
    else if(circle_type == CIRCLE_LEFT_FOUND && !Near_Lpt0_Found&& !Near_Lpt1_Found && !Far_Lpt1_Found)
    {
        circle_type = CIRCLE_LEFT_IN1;
        cross_state = 2;
    }
    else if(circle_type == CIRCLE_LEFT_IN1 && loseline0 == 0 && Far_Lpt0_Found && ipts0[Lpt0_id][1] > 35)
    {
        circle_type = CIRCLE_LEFT_IN;
        cross_state = 3;
    }
    else if(circle_type == CIRCLE_LEFT_IN  && loseline0 ==1 && Far_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found)
    {
        circle_type = CIRCLE_LEFT_RUN;
        cross_state = 4;
    }
    else if(circle_type == CIRCLE_LEFT_OUT)
    {
        cross_state = 5;
        circle_type = CIRCLE_NONE;
    }
}

void Circle_Run(void)
{
//     // if (circle_type == CIRCLE_LEFT_FOUND)
//     // {
//     //     track_type = TRACK_RIGHT; // ?????????????????
//     // }
//     // else if (circle_type == CIRCLE_RIGHT_FOUND)
//     // {
//     //     track_type = TRACK_LEFT; // ?????????????????
//     // }

//     // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // ?????????????Χ??,??????
//     // {
//     //     track_type = TRACK_RIGHT;
//     // }
//     // else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] < 40)
//     // {
//     //     track_type = TRACK_RIGHT;
//     // }

//     // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] > 40)
//     // {
//     //     track_type = TRACK_LEFT;
//     // }
//     // else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] > 40)
//     // {
//     //     track_type = TRACK_LEFT;
//     // }

    // if (Far_Lpt0_Found) // ???????????????????????,???????
    // {
    //     LeftLine_Found = 0;
    //     LeftLine_Loss = 0;
    //     circle_type = CIRCLE_LEFT_IN; // ?????????
    //     Encoder_Int_Enable();         // ??????????????
    // }
    // else if (Far_Lpt1_Found) // ???????????????????????,???????
    // {
    //     RightLine_Found = 0;
    //     RightLine_Loss = 0;
    //     circle_type = CIRCLE_RIGHT_IN; // ?????????
    //     Encoder_Int_Enable();          // ??????????????
    // }
    int count11=0;
    // ???????????????,????????,??????
    switch (circle_type)
    {
        case CIRCLE_LEFT_FOUND:
        {
            FarCorners_Find_Left_New();
            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//左边两个拐点连线
            BorderLine_Find();//左右扫线
            break;
        }
        case CIRCLE_LEFT_IN1:
        {
            FarCorners_Find_Left_Again();
            Line_Add(&img_raw,Fix_Point3,FarCornersLeft_Point,0);
            BorderLine_Find();//左右扫线
            run_straight();
            break;
        }
        case CIRCLE_LEFT_IN:
        {
            NearCorners_Find_Left(ipts0, ipts0_num, CornersLeft_Point, &Near_Lpt0_Found);
            NearCorners_Find_Left_Again();
            FarCorners_Find_Left_Again();
            Line_Add(&img_raw,Fix_Point3,CornersLeft_Point,0);//左上角和右下角（固定点）补线
            /* ----- 如果补线会影响扫线的话，就在找拐点前面加一个(&img_raw,Fix_Point3,CornersLeft_Point,255);，将直线变成白色*/
            Line_Add(&img_raw,Fix_Point,FarCornersLeft_Point,0);//
            BorderLine_Find();//左右扫线
            run_left();
            
            // count11++;
            NearBorderLine_Enable = 1;//开启近扫线使能
            FarBorderLine_Enable = 1;
            // BorderLine_Find();//再寻一次边线，找到后再补线，不用再次用track_check了，因为刚刚好track_check在此函数的后面，这里用双状态机
            break;
        }
        case CIRCLE_LEFT_RUN:
        {
            Line_Add(&img_raw,Fix_Point,FarCornersLeft_Point,0);//
            BorderLine_Find();//左右扫线
            Track_Run();
            
            break;//进去以后就会一直扫了
        }
        case CIRCLE_LEFT_OUT://这里要按斜率补线或者直接固定点也可以，这里用的固定点
        {
            Line_Add(&img_raw,CornersLeft_Point,Fin_Point2,0);//左上角（固定点）和右下角补线
            BorderLine_Find();//左右扫线
            if(track_type != TRACK_LEFT)
            {
                Track_Check();//检测状态机，同时开TRACK_LEFT和CIRCLE_LEFT_OUT两个状态
                Track_Run();
            }
            break;
        }
        case CIRCLE_LEFT_END:
        {
            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//左上角拐点和右上角拐点补线
            BorderLine_Find();//左右扫线，扫线成直道（这里不开使能是因为没有cross就不开）
            break;
        }
        default:
        {
            break;
        }
    }

    // switch (circle_type)
    // {
    //     case CIRCLE_RIGHT_FOUND:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Right,0);
    //         break;
    //     }
    //     case CIRCLE_RIGHT_IN:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//左上角和右下角（固定点）补线
    //         NearBorderLine_Enable = 1;//开启近扫线使能
    //         BorderLine_Find();//再寻一次边线，找到后再补线，不用再次用track_check了，因为刚刚好track_check在此函数的后面，这里用双状态机
    //         break;
    //     }
    //     case CIRCLE_LEFT_RUN:
    //     {
    //         if(track_type != TRACK_LEFT)
    //         {
    //             Track_Check();//检测状态机
    //             Track_Run();
    //         }
    //         break;//进去以后就会一直扫了
    //     }
    //     case CIRCLE_LEFT_OUT://这里要按斜率补线或者直接固定点也可以，这里用的固定点
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point4,0);//左上角（固定点）和右下角补线
    //         BorderLine_Find();//左右扫线
    //         if(track_type != TRACK_LEFT)
    //         {
    //             Track_Check();//检测状态机，同时开TRACK_LEFT和CIRCLE_LEFT_OUT两个状态
    //             Track_Run();
    //         }
    //         break;
    //     }
    //     case CIRCLE_LEFT_END:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//左上角拐点和左下角（固定点）补线
    //         BorderLine_Find();//左右扫线，扫线成直道（这里不开使能是因为没有cross就不开）
    //         break;
    //     }
    //     default:
    //     {
    //         break;
    //     }
    // }

    // if (circle_type == CIRCLE_LEFT_RUN && Near_Lpt1_Found) // ?????,??????????,????????
    // {
    //     circle_type = CIRCLE_LEFT_OUT;
    // }
    // else if (circle_type == CIRCLE_RIGHT_RUN && Near_Lpt0_Found)
    // {
    //     circle_type = CIRCLE_RIGHT_OUT; // ????????,??????б?????,?л???????
    // }

    // if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_DIS && !Near_Lpt1_Found && !Near_Lpt0_Found) // ??????????????????????????????,????????
    // {
    //     circle_type = CIRCLE_NONE;
    //     Encoder_Int_Clear(); // ????????????
    // }
    ips200_show_int(160,20,count11,4);
}
