#include "circle.h"
#include "image.h"
#include "garage.h"
#include "encoder.h"

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // ��¼Բ������

enum circle_type_e circle_type = CIRCLE_NONE;

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
    else
    {
        circle_type = CIRCLE_NONE;
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

    if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] < 40) // Բ����͹����ﵽ��Χ��,ѭ��Բ�뻷
    {
        track_type = TRACK_RIGHT;
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] < 40)
    {
        track_type = TRACK_RIGHT;
    }

    if (circle_type == CIRCLE_LEFT_FOUND && CornersRight_Point[1] > 40)
    {
        track_type = TRACK_LEFT;
    }
    else if (circle_type == CIRCLE_RIGHT_FOUND && CornersLeft_Point[1] > 40)
    {
        track_type = TRACK_LEFT;
    }

    if (Far_Lpt0_Found) // ���Բ��������㵽��һ����ֵ��,Զ�ǵ����
    {
        LeftLine_Found = 0;
        LeftLine_Loss = 0;
        circle_type = CIRCLE_LEFT_IN; // ������Բ��
        Encoder_Int_Enable();         // ��������������
    }
    else if (Far_Lpt1_Found) // �ұ�Բ��������㵽��һ����ֵ��,Զ�ǵ����
    {
        RightLine_Found = 0;
        RightLine_Loss = 0;
        circle_type = CIRCLE_RIGHT_IN; // ������Բ��
        Encoder_Int_Enable();          // ��������������
    }

    // ��Բ���ȶ��ߺ�����,����ѭ��Բ,��ѭ��Բ
    if (circle_type == CIRCLE_LEFT_RUN && Near_Lpt1_Found) // Բ��ģʽ,��Բ�ܷ��ֽǵ�,׼����Բ��
    {
        circle_type = CIRCLE_LEFT_OUT;
    }
    else if (circle_type == CIRCLE_RIGHT_RUN && Near_Lpt0_Found)
    {
        circle_type = CIRCLE_RIGHT_OUT; // ׼����Բ��,�ǵ㰴ָ��б�ʲ���,�л�ѭ�߷���
    }

    if ((Encoder_L_Dis + Encoder_R_Dis) / 2 > CIRCLE_DIS && !Near_Lpt1_Found && !Near_Lpt0_Found) // ���������ִ��ھ�����ֵ��˫�����޽ǵ�,�˳�Բ��ģʽ
    {
        circle_type = CIRCLE_NONE;
        Encoder_Int_Clear(); // �رձ���������
    }
}
