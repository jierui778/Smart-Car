#include "circle.h"
#include "image.h"
#include "garage.h"

enum circle_type_e circle_type = CIRCLE_NONE;

int LeftLine_Loss = 0, RightLine_Loss = 0;
int LeftLine_Found = 0, RightLine_Found = 0;

int circle_num = 0; // ��¼Բ������

void Circle_Check(void)
{
    if (1)
    // �ǳ���ģʽ����Բ��ģʽ���ҵ���ǵ㣬���ҽǵ㣬�����Թ����������أ��ұ�Ϊ��ֱ��
    {
        circle_type = CIRCLE_LEFT_FOUND; // ö��״̬��Ϊ�ҵ���Բ��
        circle_num++;
    }
    if (1)
    // ��������Բ��ģʽ�෴
    {
        circle_type = CIRCLE_RIGHT_FOUND; // ö��״̬Ϊ�ҵ���Բ��
        circle_num++;
    }
}

void Circle_Run(void)
{
    //   if (circle_type == CIRCLE_LEFT_IN)
    //   {
    //       track_type = TRACK_RIGHT;//����Բ����ʱ����Ѳ��Բ��������
    //   }
    //   else if(circle_type == CIRCLE_RIGHT_IN)
    //   //�ȶ����ߺ�����
    //   if (rpts0s_num < 20 /*0.4 / sample_dist*/)//����ߵ�������20��Ԥ���伴����ʧ��׼��Ѳ���ߣ���Բ��
    //   {
    //       LeftLine_Loss++;
    //   }

    //   if (rpts0s_num > 30 && LeftLine_Loss > 0)
    //   {
    //       LeftLine_Found++;
    //       if (LeftLine_Found > 1 && x0_first > 2)
    //       { // x0_first>3  ��˼Ϊ��ֹ��Բ������ѭ���ߣ����¶���
    //           circle_type = CIRCLE_LEFT_IN;
    //           LeftLine_Loss = 0; // �����־λ
    //           LeftLine_Found = 0;
    //           // circle_encoder = current_encoder; // �뻷��¼λ��
    //       }
    //   }
    //   else if (circle_type == CIRCLE_LEFT_IN)
    //   {
    //       track_type = TRACK_LEFT; // 15cm����ͷ����������,Ĭ��Ѳ��
    //   }
}
