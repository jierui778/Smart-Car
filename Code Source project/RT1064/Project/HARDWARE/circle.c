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

// ���ǵ�ʮ��תԲ����ʱ��ض����ֵ��߶���(���߽��ǵ��Ҳ���,ֻ�ܿ���Բ��Զ��)
// ���˫�߲����ߵ�������ǵĻ��Ƚ�����,����������߶��ߵĻ�,Բ��״̬���ͻ����һϵ������,���ǽ������
// ���תʮ��˼·:�ȶ����ߺ�����
void Circle_Check(void)
{
    if ((circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found && touch_boundary0 && !touch_boundary1 && !Near_Lpt1_Found && !Far_Lpt1_Found)) //(��Բ��ģʽ,����ǵ�+Զ��Բ��+�ұ߳�ֱ��)����(��Բ��ģʽ,Զ��Բ��+�ұ߳�ֱ��)
    {
        circle_type = CIRCLE_LEFT_FOUND; // ������Բ��
        circle_num++;
        ips200_draw_line(0, 0, 100, 100, RGB565_BLUE);
    }

    else if ((circle_type == CIRCLE_NONE && Near_Lpt1_Found && Far_Lpt1_Found && touch_boundary1 && !touch_boundary0 && !Near_Lpt0_Found && !Far_Lpt0_Found)) //(��Բ��ģʽ,�ҽ��ǵ�+Զ��Բ��+��߳�ֱ��)����(��Բ��ģʽ,)
    //
    {
        circle_type = CIRCLE_RIGHT_FOUND; // ������Բ��
        circle_num++;
        ips200_draw_line(0, 0, 100, 100, RGB565_RED);
        
    }
}

void Circle_Run(void)
{
    // Line_Add(&img_raw, CornersLeft_Point, FarCornersLeft_Point, 0);
    Line_Add(&img_raw, CornersRight_Point, FarCornersRight_Point, 0);
    BorderLine_Find(); // Ѱ�ұ���
    if (circle_type == CIRCLE_LEFT_FOUND)
    {
        track_type = TRACK_RIGHT; // Բ��������ѭ����,��ֹ���翿��Բ������ת�������̬����
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND)
    {
        track_type = TRACK_LEFT; // Բ��������ѭ����,��ֹ���翿��Բ������ת�������̬����
    }
    // ת����뻷����,��Զ�ǵ�����ұ��ƶ�,���Բ��������ԽǶȽ��ж�ʱ��̶�,�Ա���������ýضϴ���
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
    //     // BorderLine_Find();//?????��????????????????????????track_check??????????track_check??????????��???????????
    //     break;
    // }
    // case CIRCLE_LEFT_RUN:
    // {
    //     Line_Add(&img_raw, Fix_Point, FarCornersLeft_Point, 0); //
    //     BorderLine_Find();                                      //???????
    //     Track_Run();

    //     break; //?????????????
    // }
    // case CIRCLE_LEFT_OUT: //???????��????????????????????????????????
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

    // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // ?????????????��??,??????
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
    //         Line_Add(&img_raw,NearCorners_Find_Left,FarCorners_Find_Left,0);//��������յ�����
    //         break;
    //     }
    //     case CIRCLE_LEFT_IN:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point,0);//���ϽǺ����½ǣ��̶��㣩����
    //         NearBorderLine_Enable = 1;//������ɨ��ʹ��
    //         BorderLine_Find();//��Ѱһ�α��ߣ��ҵ����ٲ��ߣ������ٴ���track_check�ˣ���Ϊ�ոպ�track_check�ڴ˺����ĺ��棬������˫״̬��
    //         break;
    //     }
    //     case CIRCLE_LEFT_RUN:
    //     {
    //         if(track_type != TRACK_LEFT)
    //         {
    //             Track_Check();//���״̬��
    //             Track_Run();
    //         }
    //         break;//��ȥ�Ժ�ͻ�һֱɨ��
    //     }
    //     case CIRCLE_LEFT_OUT://����Ҫ��б�ʲ��߻���ֱ�ӹ̶���Ҳ���ԣ������õĹ̶���
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point2,0);//���Ͻǣ��̶��㣩�����½ǲ���
    //         BorderLine_Find();//����ɨ��
    //         if(track_type != TRACK_LEFT)
    //         {
    //             Track_Check();//���״̬����ͬʱ��TRACK_LEFT��CIRCLE_LEFT_OUT����״̬
    //             Track_Run();
    //         }
    //         break;
    //     }
    //     case CIRCLE_LEFT_END:
    //     {
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//���Ͻǹյ�����Ͻǹյ㲹��
    //         BorderLine_Find();//����ɨ�ߣ�ɨ�߳�ֱ�������ﲻ��ʹ������Ϊû��cross�Ͳ�����
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
    //         BorderLine_Find();//?????��????????????????????????track_check??????????track_check??????????��???????????
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
    //     case CIRCLE_LEFT_OUT://???????��????????????????????????????????
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
