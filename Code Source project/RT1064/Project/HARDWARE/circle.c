#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"

enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // ??????????

int Fix_Point[2] = {158,118}; // 右下角固定点
int Fin_Point2[2] = {2,2};//左上角固定点
int Fix_Point3[2]={2,118};//左下角固定点
int Fix_Point4[2]={80,2};//中上固定点
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
    if(circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found)//左环岛，只有左边两个拐点能找到
    {
        circle_type = CIRCLE_LEFT_FOUND;
        circle_num++;
    }
    else if(circle_type == CIRCLE_LEFT_FOUND && !Near_Lpt0_Found && Far_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found)
    {
        circle_type = CIRCLE_LEFT_IN;
    }
    else if(circle_type == CIRCLE_LEFT_IN && track_type == TRACK_RIGHT)
    {
        circle_type = CIRCLE_LEFT_RUN;
    }
    else
    {
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

    // ???????????????,????????,??????
    switch (circle_type)
    {
        case CIRCLE_LEFT_FOUND:
        {
            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//左边两个拐点连线
            break;
        }
        case CIRCLE_LEFT_IN:
        {
            Line_Add(&img_raw,CornersLeft_Point,Fix_Point,0);//左上角和右下角（固定点）补线
            NearBorderLine_Enable = 1;//开启近扫线使能
            BorderLine_Find();//再寻一次边线，找到后再补线，不用再次用track_check了，因为刚刚好track_check在此函数的后面，这里用双状态机
            break;
        }
        case CIRCLE_LEFT_RUN:
        {
            if(track_type != TRACK_LEFT)
            {
                Track_Check();//检测状态机
                Track_Run();
            }
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
            Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//左上角拐点和右上角拐点补线
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

    if (circle_type == CIRCLE_LEFT_RUN && Near_Lpt1_Found) // ?????,??????????,????????
    {
        circle_type = CIRCLE_LEFT_OUT;
    }
    else if (circle_type == CIRCLE_RIGHT_RUN && Near_Lpt0_Found)
    {
        circle_type = CIRCLE_RIGHT_OUT; // ????????,??????б?????,?л???????
    }

    // if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_DIS && !Near_Lpt1_Found && !Near_Lpt0_Found) // ??????????????????????????????,????????
    // {
    //     circle_type = CIRCLE_NONE;
    //     Encoder_Int_Clear(); // ????????????
    // }
}
