#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"


enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0;

int Fix_Point[2] = {158,80}; // ?????????
int Fin_Point2[2] = {2,10};//?????????
int Fix_Point3[2]={1,100};//?????????
int Fix_Point4[2]={80,10};//????????
int cross_state=0;
int ll_flag=0,l_flag=0,flag=0;

/*2023/10/29
目前遇到的问题：1. 进入环岛状态判断是否要取右边线？ 极限状况下右边线可能会丢（迷宫法的起始点最多能扫50行）
2. 补线后巡单边线，然后直接将右边线左移的时候，会有一段直线出现在下面（干扰直线）
3. 进入环岛后，原来的补的直线会干扰；同时迷宫法的限制行数不能太高，否则生长点会重合
4. 在左入环岛的状态时，补完线，如果左边丢线或者个数比较少，就巡右线（会出现2的问题）；如果左边没丢线，就巡双边线
5. 在起始状态时左下拐点（L）和左上拐点（圆弧）能找到，但是在左下拐点为圆弧拐点，左上拐点为倒L角点的时候，这个时候可能拐点的判断会出现问题
——除非要将不同情况下的找拐点函数分开，不然很难处理。（目前没有分开）
*/
//void Circle_Check(void)
//{
//    // if (circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found) // ?????????????????????+???
//    // // ??????????????????????????????????????????????????????????
//    // {
//    //     circle_type = CIRCLE_LEFT_FOUND; // ????????????????
//    //     circle_num++;
//    // }
//    // else if (circle_type == CIRCLE_NONE && Near_Lpt1_Found && Far_Lpt1_Found)
//    // // ???????????????
//    // {
//    //     circle_type = CIRCLE_RIGHT_FOUND; // ??????????????
//    //     circle_num++;
//    // }
//    // else
//    // {
//    //     circle_type = CIRCLE_NONE;
//    // }
//    /*2023/10/26*/
//    ll_flag=l_flag;
//    l_flag=flag;//fjw加的，三个状态判断，提高稳定性,每次更新状态会把这次状态赋给上一次状态
//    FarBorderLine_Enable = 1;
//    NearBorderLine_Enable = 1;//开使能，不然远线扫不到
//    NearCorners_Find_Left(ipts0,ipts0_num,CornersLeft_Point,&Near_Lpt0_Found);
//    NearCorners_Find_Left_Again();//找近拐点，Again是强加的判断，求x坐标最大值（其实这个函数没问题，在第一个L和圆弧拐点都能找）

//    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);
//    FarCorners_Find_Left_Again();//远拐点判断，主要是判断倒L拐点的

//    if(circle_type == CIRCLE_NONE && Near_Lpt1_Found && !Near_Lpt0_Found && !Far_Lpt0_Found && loseline0 == 0 && touch_boundary1 ==1 && CornersRight_Point[1] > 80)
//    {
//        flag = CIRCLE_RIGHT_FOUND;
//    }

//    else if(circle_type == CIRCLE_RIGHT_FOUND  && Far_Lpt1_Found &&  !Near_Lpt0_Found && !Far_Lpt0_Found && touch_boundary1 ==1 && loseline1 == 1)
//    {
//        flag = CIRCLE_RIGHT_IN1;
//    }
//    else if((circle_type == CIRCLE_RIGHT_IN1 && ArcRight_Point[1] > 50 && ArcRight_Point[1] < 60 )    || (circle_type == CIRCLE_RIGHT_IN1 && loseline1 == 1 && loseline0 == 0))
//    {
//        flag = CIRCLE_RIGHT_IN;
//    }
//    else if(circle_type == CIRCLE_RIGHT_IN && loseline0 == 0 && loseline1 ==1)
//    {
//        flag = CIRCLE_RIGHT_RUN;
//    }
//    /*这里左边拐点因为会有三种：正L，圆弧，倒L，所以这个判断有点难搞（问题5）*/

//    if((circle_type == CIRCLE_NONE && Near_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found && Far_Lpt0_Found
//        && (ipts1[ipts1_num-1][1] < 40) ) )
//        /*非环岛状态 + 左下，上拐点找到 + 右边两个拐点没找到 + 右直线最后一个点y坐标在图像上半*/
//    {
//        flag=CIRCLE_LEFT_FOUND;//检验三次再确定，这是第一次
//    }
//    // else if(circle_type == CIRCLE_LEFT_FOUND && !Near_Lpt0_Found&& !Near_Lpt1_Found && !Far_Lpt1_Found && Far_Lpt0_Found && (!loseline0 && !loseline1)
//    //  && ipts0[Lpt0_id][1] > 80
//    // )
//    // {
//    //     circle_type = CIRCLE_LEFT_IN1;
//    //     cross_state = 2;
//    // }
//    // if(circle_type == CIRCLE_LEFT_IN1   && ((ipts0[Lpt0_id][1] > 47 && ipts0[Lpt0_id][1] < 60)||ipts0[Lpt0_id][0]< 20))
//    if(circle_type == CIRCLE_LEFT_FOUND  && Far_Lpt0_Found && (FarCornersLeft_Point[1] > 40 )&&( FarCornersLeft_Point[0] <80)
//    )
//    {
//        flag = CIRCLE_LEFT_IN;
//    }
//    // else if(circle_type == CIRCLE_LEFT_IN  && (loseline0 ==1 && loseline1 == 0) && (ipts1[0][0] - ipts1[ipts1_num - 1][0]) > (IMAGE_WIDTH/2) )
//    // {
//    //     flag = CIRCLE_LEFT_RUN;
//    // }
//    // else if (circle_type  == CIRCLE_LEFT_RUN && loseline0 == 1 && Near_Lpt1_Found == 1 && !Near_Lpt0_Found)
//    // {
//    //     circle_type = CIRCLE_LEFT_OUT;
//    // }
//    // else if(circle_type == CIRCLE_LEFT_OUT)
//    // {
//    //     cross_state = 5;
//    //     circle_type = CIRCLE_NONE;
//    // }
//    if(ll_flag==l_flag  && l_flag==flag &&flag!=circle_type)
//    {
//        circle_type=flag;
//    }
//
//
//    if( circle_type == CIRCLE_RIGHT_FOUND)
//    {
//        cross_state = 1;
//        circle_num++;
//    }
//    else if(circle_type == CIRCLE_RIGHT_IN1)
//    {
//        cross_state = 2;
//    }

//    else if(circle_type == CIRCLE_RIGHT_IN)
//    {
//        cross_state = 3;
//    }
//    else if(circle_type == CIRCLE_LEFT_RUN)
//    {
//        cross_state = 4;
//    }
//    else if(circle_type == CIRCLE_NONE)
//    {
//        cross_state = 5;
//    }
//
//}

//void Circle_Run(void)
//{
//     // if (circle_type == CIRCLE_LEFT_FOUND)
//     // {
//     //     track_type = TRACK_RIGHT; // ?????????????????
//     // }
//     // else if (circle_type == CIRCLE_RIGHT_FOUND)
//     // {
//     //     track_type = TRACK_LEFT; // ?????????????????
//     // }

    //     // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] > 40)
    //     // {
    //     //     track_type = TRACK_LEFT;
    //     // }
    //     // else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] > 40)
    //     // {
    //     //     track_type = TRACK_LEFT;
    //     // }

//   // if (Far_Lpt0_Found) // ???????????????????????,???????
//   // {
//   //     LeftLine_Found = 0;
//   //     LeftLine_Loss = 0;
//   //     circle_type = CIRCLE_LEFT_IN; // ?????????
//   //     Encoder_Int_Enable();         // ??????????????
//   // }
//   // else if (Far_Lpt1_Found) // ???????????????????????,???????
//   // {
//   //     RightLine_Found = 0;
//   //     RightLine_Loss = 0;
//   //     circle_type = CIRCLE_RIGHT_IN; // ?????????
//   //     Encoder_Int_Enable();          // ??????????????
//   // }
//   int count11=0;
//   // ???????????????,????????,??????
//   switch (circle_type)
//   {
//       case CIRCLE_LEFT_FOUND:
//       {
//            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//画出直线
//            BorderLine_Find();//找近线，要巡直道
//            // ips200_draw_line(CornersLeft_Point[0],CornersLeft_Point[1],FarCornersLeft_Point[0],FarCornersLeft_Point[1],RGB565_RED);
//            Arc_Point_Get(ipts0, ipts0_num,ArcLeft_Point,&NearIs_Arc0);//左近弧线是否找到
//            FarCorners_Find_Left_Again();
//            break;
//       }
//    //    case CIRCLE_LEFT_IN1:
//    //    {
//    //        FarCorners_Find_Left_Again();
//    //        Line_Add(&img_raw,Fix_Point3,FarCornersLeft_Point,0);
//    //        BorderLine_Find();//???????
//    //        run_straight();
//
//    //        break;
//    //    }
//       case CIRCLE_LEFT_IN:
//       {
//           NearCorners_Find_Left(ipts0, ipts0_num, CornersLeft_Point, &Near_Lpt0_Found);
//           NearCorners_Find_Left_Again();
//           FarCorners_Find_Left_Again();
//           Line_Add(&img_raw,Fix_Point3,CornersLeft_Point,0);//?????????????????????
//           /* ----- ?????????????????????????????????(&img_raw,Fix_Point3,CornersLeft_Point,255);???????????*/
//           Line_Add(&img_raw,Fix_Point,FarCornersLeft_Point,0);//
//           BorderLine_Find();//???????
//           run_left();
//
//           // count11++;
//           NearBorderLine_Enable = 1;//????????????
//           FarBorderLine_Enable = 1;
//           // BorderLine_Find();//???????????????????????????????track_check??????????track_check???????????????????????
//           break;
//       }
//       case CIRCLE_LEFT_RUN:
//       {
//           BorderLine_Find();//???????
//           run_left();
//
//           break;//?????????????
//       }
//       // case CIRCLE_LEFT_OUT://?????????????????????????????????????????
//       // {
//       //     Line_Add(&img_raw,CornersLeft_Point,Fin_Point2,0);//?????????????????????
//       //     BorderLine_Find();//???????
//       //     if(track_type != TRACK_LEFT)
//       //     {
//       //         Track_Check();//?????????????TRACK_LEFT??CIRCLE_LEFT_OUT??????
//       //         Track_Run();
//       //     }
//       //     break;
//       // }
//       case CIRCLE_LEFT_OUT:
//       {
//           if(Near_Lpt1_Found == 1)
//           {
//               run_left();
//           }
//           break;
//       }
//       case CIRCLE_LEFT_END:
//       {
//           Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//?????????????????
//           // BorderLine_Find();//??????????????????????????????????cross???????
//           NearCorners_Find_Right(ipts1,ipts1_num,CornersRight_Point,&Near_Lpt1_Found);
//           break;
//       }
//
//       default:
//       {
//           break;
//       }
//   }

//   // switch (circle_type)
//   // {
//   //     case CIRCLE_RIGHT_FOUND:
//   //     {
//   //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Right,0);
//   //         break;
//   //     }
//   //     case CIRCLE_RIGHT_IN:
//   //     {
//   //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//?????????????????????
//   //         NearBorderLine_Enable = 1;//????????????
//   //         BorderLine_Find();//???????????????????????????????track_check??????????track_check???????????????????????
//   //         break;
//   //     }
//   //     case CIRCLE_LEFT_RUN:
//   //     {
//   //         if(track_type != TRACK_LEFT)
//   //         {
//   //             Track_Check();//???????
//   //             Track_Run();
//   //         }
//   //         break;//?????????????
//   //     }
//   //     case CIRCLE_LEFT_OUT://?????????????????????????????????????????
//   //     {
//   //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point4,0);//?????????????????????
//   //         BorderLine_Find();//???????
//   //         if(track_type != TRACK_LEFT)
//   //         {
//   //             Track_Check();//?????????????TRACK_LEFT??CIRCLE_LEFT_OUT??????
//   //             Track_Run();
//   //         }
//   //         break;
//   //     }
//   //     case CIRCLE_LEFT_END:
//   //     {
//   //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//???????????????????????
//   //         BorderLine_Find();//??????????????????????????????????cross???????
//   //         break;
//   //     }
//   //     default:
//   //     {
//   //         break;
//   //     }
//   // }

//   // if (circle_type == CIRCLE_LEFT_RUN && Near_Lpt1_Found) // ?????,??????????,????????
//   // {
//   //     circle_type = CIRCLE_LEFT_OUT;
//   // }
//   // else if (circle_type == CIRCLE_RIGHT_RUN && Near_Lpt0_Found)
//   // {
//   //     circle_type = CIRCLE_RIGHT_OUT; // ????????,?????????????,??????????
//   // }

//   // if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_DIS && !Near_Lpt1_Found && !Near_Lpt0_Found) // ??????????????????????????????,????????
//   // {
//   //     circle_type = CIRCLE_NONE;
//   //     Encoder_Int_Clear(); // ????????????
//   // }
//   ips200_show_int(160,0,cross_state,3);
//}
