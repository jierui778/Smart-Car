#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"

enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // ??????????

int Fix_Point[2] = {158,80}; // ���½ǹ̶���
int Fin_Point2[2] = {2,10};//���Ͻǹ̶���
int Fix_Point3[2]={1,100};//���½ǹ̶���
int Fix_Point4[2]={80,10};//���Ϲ̶���
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
    if(circle_type == CIRCLE_NONE && Near_Lpt0_Found && !Near_Lpt1_Found && !Far_Lpt1_Found)//�󻷵���ֻ����������յ����ҵ�
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

//     // if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // ?????????????��??,??????
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
            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//��������յ�����
            BorderLine_Find();//����ɨ��
            break;
        }
        case CIRCLE_LEFT_IN1:
        {
            FarCorners_Find_Left_Again();
            Line_Add(&img_raw,Fix_Point3,FarCornersLeft_Point,0);
            BorderLine_Find();//����ɨ��
            run_straight();
            break;
        }
        case CIRCLE_LEFT_IN:
        {
            NearCorners_Find_Left(ipts0, ipts0_num, CornersLeft_Point, &Near_Lpt0_Found);
            NearCorners_Find_Left_Again();
            FarCorners_Find_Left_Again();
            Line_Add(&img_raw,Fix_Point3,CornersLeft_Point,0);//���ϽǺ����½ǣ��̶��㣩����
            /* ----- ������߻�Ӱ��ɨ�ߵĻ��������ҹյ�ǰ���һ��(&img_raw,Fix_Point3,CornersLeft_Point,255);����ֱ�߱�ɰ�ɫ*/
            Line_Add(&img_raw,Fix_Point,FarCornersLeft_Point,0);//
            BorderLine_Find();//����ɨ��
            run_left();
            
            // count11++;
            NearBorderLine_Enable = 1;//������ɨ��ʹ��
            FarBorderLine_Enable = 1;
            // BorderLine_Find();//��Ѱһ�α��ߣ��ҵ����ٲ��ߣ������ٴ���track_check�ˣ���Ϊ�ոպ�track_check�ڴ˺����ĺ��棬������˫״̬��
            break;
        }
        case CIRCLE_LEFT_RUN:
        {
            Line_Add(&img_raw,Fix_Point,FarCornersLeft_Point,0);//
            BorderLine_Find();//����ɨ��
            Track_Run();
            
            break;//��ȥ�Ժ�ͻ�һֱɨ��
        }
        case CIRCLE_LEFT_OUT://����Ҫ��б�ʲ��߻���ֱ�ӹ̶���Ҳ���ԣ������õĹ̶���
        {
            Line_Add(&img_raw,CornersLeft_Point,Fin_Point2,0);//���Ͻǣ��̶��㣩�����½ǲ���
            BorderLine_Find();//����ɨ��
            if(track_type != TRACK_LEFT)
            {
                Track_Check();//���״̬����ͬʱ��TRACK_LEFT��CIRCLE_LEFT_OUT����״̬
                Track_Run();
            }
            break;
        }
        case CIRCLE_LEFT_END:
        {
            Line_Add(&img_raw,CornersLeft_Point,FarCornersLeft_Point,0);//���Ͻǹյ�����Ͻǹյ㲹��
            BorderLine_Find();//����ɨ�ߣ�ɨ�߳�ֱ�������ﲻ��ʹ������Ϊû��cross�Ͳ�����
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
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//���ϽǺ����½ǣ��̶��㣩����
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
    //         Line_Add(&img_raw,NearCorners_Find_Right,Fix_Point4,0);//���Ͻǣ��̶��㣩�����½ǲ���
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
    //         Line_Add(&img_raw,NearCorners_Find_Right,FarCorners_Find_Left,0);//���Ͻǹյ�����½ǣ��̶��㣩����
    //         BorderLine_Find();//����ɨ�ߣ�ɨ�߳�ֱ�������ﲻ��ʹ������Ϊû��cross�Ͳ�����
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
    //     circle_type = CIRCLE_RIGHT_OUT; // ????????,??????��?????,?��???????
    // }

    // if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_DIS && !Near_Lpt1_Found && !Near_Lpt0_Found) // ??????????????????????????????,????????
    // {
    //     circle_type = CIRCLE_NONE;
    //     Encoder_Int_Clear(); // ????????????
    // }
    ips200_show_int(160,20,count11,4);
}
