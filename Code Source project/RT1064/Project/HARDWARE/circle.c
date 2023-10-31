#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"

enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0;

int Fix_Point[2] = {158, 80};
int Fin_Point2[2] = {2, 10};
int Fix_Point3[2] = {1, 100};
int Fix_Point4[2] = {80, 10};
int cross_state = 0;

// 考虑到十字转圆环的时候必定出现单边丢线(单边近角点找不到,只能看到圆弧远线)
// 针对双边不丢线的情况考虑的话比较稳妥,但是如果单边丢线的话,圆环状态机就会出现一系列问题,考虑解决方案
// 弯道转十字思路:先丢左线后有线
void Circle_Check(void)
{
    if ((circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found && touch_boundary0 && !touch_boundary1 && !Near_Lpt1_Found && !Far_Lpt1_Found)) //(非圆环模式,左近角点+远线圆弧+右边长直线)或者(非圆环模式,远线圆弧+右边长直线)
    {
        circle_type = CIRCLE_LEFT_FOUND; // 发现左圆环
        circle_num++;
        ips200_draw_line(0, 0, 100, 100, RGB565_BLUE);
    }

    else if ((circle_type == CIRCLE_NONE && Near_Lpt1_Found && Far_Lpt1_Found && touch_boundary1 && !touch_boundary0 && !Near_Lpt0_Found && !Far_Lpt0_Found)) //(非圆环模式,右近角点+远线圆弧+左边长直线)或者(非圆环模式,)
    //
    {
        circle_type = CIRCLE_RIGHT_FOUND; // 发现右圆环
        circle_num++;
        ips200_draw_line(0, 0, 100, 100, RGB565_RED);
        
    }
}

void Circle_Run(void)
{
    // Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);
    Line_Add(&img_raw, CornersRight_Point, FarCornersRight_Point, 0);
    BorderLine_Find(); // 寻找边线
    if (circle_type == CIRCLE_LEFT_FOUND)
    {
        track_type = TRACK_RIGHT; // 圆环初期先循右线,防止过早靠近圆环导致转弯后车身姿态不稳
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND)
    {
        track_type = TRACK_LEFT; // 圆环初期先循左线,防止过早靠近圆环导致转弯后车身姿态不稳
    }
    // 转弯进入环岛后,左远角点会向右边移动,可以采用阻塞对角度进行短时间固定,对边线数组采用截断处理
    //     // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // ?????????????????,??????
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
    // int count11 = 0;
    // // ???????????????,????????,??????
    // switch (circle_type)
    // {
    // case CIRCLE_LEFT_FOUND:
    // {
    //     //            FarCorners_Find_Left_New();
    //     Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0); //??????????????
    //     BorderLine_Find();                                              //???????
    //     break;
    // }
    // case CIRCLE_LEFT_IN1:
    // {
    //     // FarCorners_Find_Left_Again();
    //     Line_Add(&img_raw, Fix_Point3, FarCornersLeft_Point, 0);
    //     BorderLine_Find(); //???????
    //     run_straight();
    //     break;
    // }
    // case CIRCLE_LEFT_IN:
    // {
    //     NearCorners_Find_Left(ipts0, ipts0_num, CornersLeft_Point, &Near_Lpt0_Found);
    //     // NearCorners_Find_Left_Again();
    //     // FarCorners_Find_Left_Again();
    //     Line_Add(&img_raw, Fix_Point3, CornersLeft_Point, 0); //?????????????????????
    //     /* ----- ?????????????????????????????????(&img_raw,Fix_Point3,CornersLeft_Point,255);???????????*/
    //     Line_Add(&img_raw, Fix_Point, FarCornersLeft_Point, 0); //
    //     BorderLine_Find();                                      //???????
    //     run_left();

    //     // count11++;
    //     NearBorderLine_Enable = 1; //????????????
    //     FarBorderLine_Enable = 1;
    //     // BorderLine_Find();//?????α????????????????????????track_check??????????track_check??????????棬???????????
    //     break;
    // }
    // case CIRCLE_LEFT_RUN:
    // {
    //     Line_Add(&img_raw, Fix_Point, FarCornersLeft_Point, 0); //
    //     BorderLine_Find();                                      //???????
    //     Track_Run();

    //     break; //?????????????
    // }
    // case CIRCLE_LEFT_OUT: //???????б????????????????????????????????
    // {
    //     Line_Add(&img_raw, CornersLeft_Point, Fin_Point2, 0); //?????????????????????
    //     BorderLine_Find();                                    //???????
    //     if (track_type != TRACK_LEFT)
    //     {
    //         Track_Check(); //?????????????TRACK_LEFT??CIRCLE_LEFT_OUT??????
    //         Track_Run();
    //     }
    //     break;
    // }
    // case CIRCLE_LEFT_END:
    // {
    //     Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0); //?????????????????
    //     BorderLine_Find();                                              //??????????????????????????????????cross???????
    //     break;
    // }
    // default:
    // {
    //     break;
    // }
    // }

    // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // ?????????????Χ??,??????
    // {
    //     track_type = TRACK_RIGHT;
    // }
    // else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] < 40)
    // {
    //     track_type = TRACK_RIGHT;
    // }

    // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] > 40)
    // {
    //     track_type = TRACK_LEFT;
    // }
    // else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] > 40)
    // {
    //     track_type = TRACK_LEFT;
    // }

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

    // // ???????????????,????????,??????
    // switch (circle_type)
    // {
    //     case CIRCLE_LEFT_FOUND:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Left,FarCorners_Find_Left,0);//左边两个拐点连线
    //         break;
    //     }
    //     case CIRCLE_LEFT_IN:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point,0);//左上角和右下角（固定点）补线
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
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point2,0);//左上角（固定点）和右下角补线
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
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//左上角拐点和右上角拐点补线
    //         BorderLine_Find();//左右扫线，扫线成直道（这里不开使能是因为没有cross就不开）
    //         break;
    //     }
    //     default:
    //     {
    //         break;
    //     }
    // }

    // switch (circle_type)
    // {
    //     case CIRCLE_RIGHT_FOUND:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Right,0);
    //         break;
    //     }
    //     case CIRCLE_RIGHT_IN:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//?????????????????????
    //         NearBorderLine_Enable = 1;//????????????
    //         BorderLine_Find();//?????α????????????????????????track_check??????????track_check??????????棬???????????
    //         break;
    //     }
    //     case CIRCLE_LEFT_RUN:
    //     {
    //         if(track_type != TRACK_LEFT)
    //         {
    //             Track_Check();//???????
    //             Track_Run();
    //         }
    //         break;//?????????????
    //     }
    //     case CIRCLE_LEFT_OUT://???????б????????????????????????????????
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point4,0);//?????????????????????
    //         BorderLine_Find();//???????
    //         if(track_type != TRACK_LEFT)
    //         {
    //             Track_Check();//?????????????TRACK_LEFT??CIRCLE_LEFT_OUT??????
    //             Track_Run();
    //         }
    //         break;
    //     }
    //     case CIRCLE_LEFT_END:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//???????????????????????
    //         BorderLine_Find();//??????????????????????????????????cross???????
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
    //     circle_type = CIRCLE_RIGHT_OUT; // ????????,?????????????,??????????
    // }

    // if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_DIS && !Near_Lpt1_Found && !Near_Lpt0_Found) // ??????????????????????????????,????????
    // {
    //     circle_type = CIRCLE_NONE;
    //     Encoder_Int_Clear(); // ????????????
    // }
    // ips200_show_int(160, 20, count11, 4);
}
