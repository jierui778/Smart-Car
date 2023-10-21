#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"
enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // ��¼Բ������

void Circle_Check(void)
{
    if (circle_type == CIRCLE_NONE && Near_Lpt0_Found && Far_Lpt0_Found) // ���߳�ֱ��������һ��Ϊ�ǵ�+Բ��
    // �ǳ���ģʽ����Բ��ģʽ���ҵ���ǵ㣬���ҽǵ㣬�����Թ����������أ��ұ�Ϊ��ֱ��
    {
        circle_type = CIRCLE_LEFT_FOUND; // ö��״̬��Ϊ�ҵ���Բ��
        circle_num++;
    }
    else if (circle_type == CIRCLE_NONE && Near_Lpt1_Found && Far_Lpt1_Found)
    // ��������Բ��ģʽ�෴
    {
        circle_type = CIRCLE_RIGHT_FOUND; // ö��״̬Ϊ�ҵ���Բ��
        circle_num++;
    }
}

void Circle_Run(void)
{
    if (circle_type == CIRCLE_LEFT_FOUND)
    {
        track_type = TRACK_RIGHT; // ����Բ��ǰ����Ѳ����
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND)
    {
        track_type = TRACK_LEFT; // ����Բ��ǰ����Ѳ����
    }
    Encoder_Int_Enable(); // ��������������

    if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // Բ����͹����ﵽ��Χ��,ѭ��Բ�뻷
    {
        track_type = TRACK_RIGHT;
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] < 40)
    {
        track_type = TRACK_RIGHT;
    }

    if (1) // ����Բ��,��԰�ض���,ѭ��Բ
    {
    }
    else if (1) //
    {
    }

    if (Near_Lpt1_Found && (Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_ENCODER_THRES) // ���ֵ����нǵ��ұ��������ֵ�����ֵ,׼����Բ��
    {
    }
    else if (1)
    {
    }
    Encoder_Int_Clear(); // �������������

    // if (ipts1_num > 30 && LeftLine_Loss > 0)
    // {
    //     LeftLine_Found++;
    //     if (LeftLine_Found > 1 && x0_first > 2)
    //     { // x0_first>3  ��˼Ϊ��ֹ��Բ������ѭ���ߣ����¶���
    //         circle_type = CIRCLE_LEFT_IN;
    //         LeftLine_Loss = 0; // �����־λ
    //         LeftLine_Found = 0;
    //         // circle_encoder = current_encoder; // �뻷��¼λ��
    //     }
    // }
    // else if (circle_type == CIRCLE_LEFT_IN)
    // {
    //     track_type = TRACK_LEFT; // 15cm����ͷ����������,Ĭ��Ѳ��
    // }
}
