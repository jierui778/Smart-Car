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
Ŀǰ���������⣺1. ���뻷��״̬�ж��Ƿ�Ҫȡ�ұ��ߣ� ����״�����ұ��߿��ܻᶪ���Թ�������ʼ�������ɨ50�У�
2. ���ߺ�Ѳ�����ߣ�Ȼ��ֱ�ӽ��ұ������Ƶ�ʱ�򣬻���һ��ֱ�߳��������棨����ֱ�ߣ�
3. ���뻷����ԭ���Ĳ���ֱ�߻���ţ�ͬʱ�Թ�����������������̫�ߣ�������������غ�
4. �����뻷����״̬ʱ�������ߣ������߶��߻��߸����Ƚ��٣���Ѳ���ߣ������2�����⣩��������û���ߣ���Ѳ˫����
5. ����ʼ״̬ʱ���¹յ㣨L�������Ϲյ㣨Բ�������ҵ������������¹յ�ΪԲ���յ㣬���Ϲյ�Ϊ��L�ǵ��ʱ�����ʱ����ܹյ���жϻ��������
��������Ҫ����ͬ����µ��ҹյ㺯���ֿ�����Ȼ���Ѵ�����Ŀǰû�зֿ���
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
//    l_flag=flag;//fjw�ӵģ�����״̬�жϣ�����ȶ���,ÿ�θ���״̬������״̬������һ��״̬
//    FarBorderLine_Enable = 1;
//    NearBorderLine_Enable = 1;//��ʹ�ܣ���ȻԶ��ɨ����
//    NearCorners_Find_Left(ipts0,ipts0_num,CornersLeft_Point,&Near_Lpt0_Found);
//    NearCorners_Find_Left_Again();//�ҽ��յ㣬Again��ǿ�ӵ��жϣ���x�������ֵ����ʵ�������û���⣬�ڵ�һ��L��Բ���յ㶼���ң�

//    FarCorners_Find_Left(Far_ipts0,Far_ipts0_num,FarCornersLeft_Point,&Far_Lpt0_Found);
//    FarCorners_Find_Left_Again();//Զ�յ��жϣ���Ҫ���жϵ�L�յ��

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
//    /*������߹յ���Ϊ�������֣���L��Բ������L����������ж��е��Ѹ㣨����5��*/

//    if((circle_type == CIRCLE_NONE && Near_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found && Far_Lpt0_Found
//        && (ipts1[ipts1_num-1][1] < 40) ) )
//        /*�ǻ���״̬ + ���£��Ϲյ��ҵ� + �ұ������յ�û�ҵ� + ��ֱ�����һ����y������ͼ���ϰ�*/
//    {
//        flag=CIRCLE_LEFT_FOUND;//����������ȷ�������ǵ�һ��
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
//            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//����ֱ��
//            BorderLine_Find();//�ҽ��ߣ�ҪѲֱ��
//            // ips200_draw_line(CornersLeft_Point[0],CornersLeft_Point[1],FarCornersLeft_Point[0],FarCornersLeft_Point[1],RGB565_RED);
//            Arc_Point_Get(ipts0, ipts0_num,ArcLeft_Point,&NearIs_Arc0);//��������Ƿ��ҵ�
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
