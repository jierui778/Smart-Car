/*
 * garage.c
 *
 *  Created on: 2023��1��12��
 *      Author: HQC
 */
#include "garage.h"

// // ���¶���Ϊ����ѰԶ���趨
// //int g_far_ipts0[LLL][2];           //����Ѳ��
// //int g_far_ipts1[LLL][2];
// //int g_far_ipts0_num, g_far_ipts1_num;
// //
// //float g_far_rpts0[LLL][2];         //��͸��
// //float g_far_rpts1[LLL][2];
// //int g_far_rpts0_num, g_far_rpts1_num;
// //
// //float g_far_rpts0b[LLL][2];        //��͸���˲�
// //float g_far_rpts1b[LLL][2];
// //int g_far_rpts0b_num, g_far_rpts1b_num;
// //
// //float g_far_rpts0s[LLL][2];        //��͸�ӵȾ����
// //float g_far_rpts1s[LLL][2];
// //int g_far_rpts0s_num, g_far_rpts1s_num;
// //
// //
// //
// //
// //float g_far_rpts0a[LLL];           //ÿ��ĽǶȣ���û�õ�
// //float g_far_rpts1a[LLL];
// //int g_far_rpts0a_num, g_far_rpts1a_num;
// //
// //float g_far_rpts0an[LLL];          //�Ƕȼ���ֵ���ƣ�����û�õ�
// //float g_far_rpts1an[LLL];
// //int g_far_rpts0an_num, g_far_rpts1an_num;

// // ����Զ��L�ǵ�
// int g_far_Lpt0_rpts0s_id, g_far_Lpt1_rpts1s_id;
// bool g_far_Lpt0_found, g_far_Lpt1_found;
// // ����Զ����L�ǵ�
// int g_far_N_Lpt0_rpts0s_id, g_far_N_Lpt1_rpts1s_id;
// bool g_far_N_Lpt0_found, g_far_N_Lpt1_found;

// int g_not_have_line = 0;

// int zebra_cross_flag_begin = 0;
// int zebra_cross_flag0[30];
// int zebra_cross_flag0_num = 0;
// int zebra_cross_flag1[30];
// int zebra_cross_flag1_num = 0;

// float (*garage_rpts)[2];
// int garage_rpts_num;

enum garage_type_e garage_type = GARAGE_NONE; // ��ʼ��Ϊ������� ����״̬ΪNONE
int Zibra_Thres = 0;
// uint8 touch_boundary0; // ������ߵ�ͼ����߽�
// uint8 touch_boundary1; // �ұ����ߵ�ͼ���ұ߽�
void Garage_Check(void)//�ҵ��Ͻǵ�������Ͻǵ�һ���ж�
{
    if (1&& Zibra_Thres > ZIBRA_THRES)
    {
        garage_type = GARAGE_LEFT_FOUND;
    }
    else if (1&& Zibra_Thres > ZIBRA_THRES)
    {
        garage_type = GARAGE_RIGHT_FOUND;
    }
}

void Garage_Run(void)
{
}
// // ����������ֹ�ظ����������
// int64_t garage_encoder = -10000;

// // ����ת����
// bool out_dir = 0;

// // ��¼�ڼ�������
// int16_t garage_cnt = 0;

// //uint8 r_garage_check_supple()
// //{
// //    int G_x1,G_y1;
// //    Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &G_x1 , &G_y1 );
// //
// //    //��L�յ�����ɨ�ߣ��������
// //    for (; G_y1 > 0; G_y1--) if (AT_IMAGE(&img_raw, G_x1, G_y1-1) < thres) break;
// //    if (AT_IMAGE(&img_raw, G_x1, G_y1) >= thres)
// //      Right_Adaptive_Threshold(&img_raw, block_size, clip_value, G_x1, G_y1, g_far_ipts1, &g_far_ipts1_num);
// //    else g_far_ipts1_num = 0;
// //    //�Ա���͸�ӱ任
// //    Pespective(g_far_ipts1,g_far_ipts1_num ,  g_far_rpts1);
// //    g_far_rpts1_num = g_far_ipts1_num;
// //    //���������˲�
// //    blur_points(g_far_rpts1,g_far_rpts1_num,g_far_rpts1b,(int) round(line_blur_kernel));
// //    g_far_rpts1b_num = g_far_rpts1_num;
// //    // ���ߵȾ����
// //    g_far_rpts1s_num = sizeof(g_far_rpts1s) / sizeof(g_far_rpts1s[0]);
// //    resample_points(g_far_rpts1b, g_far_rpts1b_num, g_far_rpts1s, &g_far_rpts1s_num, sample_dist * pixel_per_meter);
// //    // ���߾ֲ��Ƕȱ仯��
// //    local_angle_points(g_far_rpts1s, g_far_rpts1s_num, g_far_rpts1a, (int) round(angle_dist / sample_dist));
// //    g_far_rpts1a_num = g_far_rpts1s_num;
// //    // �Ƕȱ仯�ʷǼ�������
// //    nms_angle(g_far_rpts1a, g_far_rpts1a_num, g_far_rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //    g_far_rpts1an_num = g_far_rpts1a_num;
// //
// //    //Զ�ǵ�ʶ��(ȷ�ϳ���ؼ�)
// //    g_far_Lpt1_found = g_far_N_Lpt1_found = false; //��ʼ��Ϊ0����ֹ�ϴα�־λ����
// //    for (int i = 1; i < g_far_rpts1s_num; i++) {
// //        if (g_far_rpts1an[i] == 0) continue;
// //        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, g_far_rpts1s_num - 1);
// //        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, g_far_rpts1s_num - 1);
// //        float conf = fabs(g_far_rpts1a[i]) - (fabs(g_far_rpts1a[im1]) + fabs(g_far_rpts1a[ip1])) / 2;
// //
// //        if (g_far_Lpt1_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {
// //            g_far_Lpt1_rpts1s_id = i;
// //            g_far_Lpt1_found = true;
// //
// //            return g_far_Lpt1_found;
// //
// ////            if(g_far_rpts1a[g_far_Lpt1_rpts1s_id] > 0)              //��һ��L����������ҹյģ�������L�㣨>0��� <0�ҹգ�
// ////            {
// ////                g_far_N_Lpt1_rpts1s_id = g_far_Lpt1_rpts1s_id;
// ////                g_far_N_Lpt1_found = true;
// ////                g_far_Lpt1_found = false;
// ////            }
// ////            if(g_far_Lpt1_found)
// ////                break;//һ����⵽�ڶ���L�ǵ㣬��������ѭ��
// //        }
// //    }
// ////    return g_far_Lpt1_found;
// //}
// //uint8 l_garage_check_supple()
// //{
// //    int G_x0,G_y0;
// //    Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &G_x0 , &G_y0 );
// //
// //    //��L�յ�����ɨ�ߣ��������
// //    for (; G_y0 > 0; G_y0--) if (AT_IMAGE(&img_raw, G_x0, G_y0-1) < thres) break;
// //    if (AT_IMAGE(&img_raw, G_x0, G_y0) >= thres)
// //      Left_Adaptive_Threshold(&img_raw, block_size, clip_value, G_x0, G_y0, g_far_ipts0, &g_far_ipts0_num);
// //    else g_far_ipts0_num = 0;
// //    //�Ա���͸�ӱ任
// //    Pespective(g_far_ipts0,g_far_ipts0_num ,  g_far_rpts0);
// //    g_far_rpts0_num = g_far_ipts0_num;
// //    //���������˲�
// //    blur_points(g_far_rpts0,g_far_rpts0_num,g_far_rpts0b,(int) round(line_blur_kernel));
// //    g_far_rpts0b_num = g_far_rpts0_num;
// //    // ���ߵȾ����
// //    g_far_rpts0s_num = sizeof(g_far_rpts0s) / sizeof(g_far_rpts0s[0]);
// //    resample_points(g_far_rpts0b, g_far_rpts0b_num, g_far_rpts0s, &g_far_rpts0s_num, sample_dist * pixel_per_meter);
// //    // ���߾ֲ��Ƕȱ仯��
// //    local_angle_points(g_far_rpts0s, g_far_rpts0s_num, g_far_rpts0a, (int) round(angle_dist / sample_dist));
// //    g_far_rpts0a_num = g_far_rpts0s_num;
// //    // �Ƕȱ仯�ʷǼ�������
// //    nms_angle(g_far_rpts0a, g_far_rpts0a_num, g_far_rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //    g_far_rpts0an_num = g_far_rpts0a_num;
// //
// //    //Զ�ǵ�ʶ��(ȷ�ϳ���ؼ�)
// //    g_far_Lpt0_found = g_far_N_Lpt0_found = false; //��ʼ��Ϊ0����ֹ�ϴα�־λ����
// //    for (int i = 1; i < g_far_rpts0s_num; i++) {
// //        if (g_far_rpts0an[i] == 0) continue;
// //        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, g_far_rpts0s_num - 1);
// //        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, g_far_rpts0s_num - 1);
// //        float conf = fabs(g_far_rpts0a[i]) - (fabs(g_far_rpts0a[im1]) + fabs(g_far_rpts0a[ip1])) / 2;
// //
// //        if (g_far_Lpt0_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {
// //            g_far_Lpt0_rpts0s_id = i;
// //            g_far_Lpt0_found = true;
// //
// //            return g_far_Lpt0_found;
// ////            if(g_far_rpts0a[g_far_Lpt0_rpts0s_id] > 0)              //��һ��L�����������յģ�������L��
// ////            {
// ////                g_far_N_Lpt0_rpts0s_id = g_far_Lpt0_rpts0s_id;
// ////                g_far_N_Lpt0_found = true;
// ////                g_far_Lpt0_found = false;
// ////            }
// ////            if(g_far_Lpt0_found)
// ////                break;//һ����⵽�ڶ���L�ǵ㣬��������ѭ��
// //        }
// //    }
// ////    return g_far_Lpt0_found;
// //
// //}

// void check_garage(void )
// {
//     //Sobel��Ե����ж�
//     //��·���߹�����
// //    if(choose_Binarization == 0)//sobel��⳵��
// //    {
//         if( Lpt0_found && N_Lpt1_found  && touch_boundary0 && ! touch_boundary1)
//         {
//             garage_type = GARAGE_FOUND_LEFT;
//             garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
//             garage_cnt ++ ;
//             aim_distance = 0.3;
//     //        Z.Stop = 1;                                 //ͣ��
//         }

//         if(Lpt1_found && N_Lpt0_found  && touch_boundary1 && ! touch_boundary0)
//         {
//             garage_type = GARAGE_FOUND_RIGHT;
//             garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
//             garage_cnt ++;
//             aim_distance = 0.3;
//     //        Z.Stop = 1;                                 //ͣ��
//         }
// //    }
// //    else if(choose_Binarization == 1)
// //    {
// //        // ���ݵ�ǰ��L�ǵ����������ȡ��(��)���ߣ����а����ߵ�Ѱ��
// //        if (Lpt0_found && !Lpt1_found) {
// //            // ����ǵ㣬�������������Ұ�����
// //            garage_rpts = rptsc1;
// //            garage_rpts_num = rptsc1_num;
// //        } else if (Lpt1_found && !Lpt0_found) {
// //            // ���ҽǵ㣬�������������Ұ�����
// //            garage_rpts = rptsc0;
// //            garage_rpts_num = rptsc0_num;
// //        } else {
// //            // �����������
// //            garage_rpts_num = 0;
// //        }
// //        int pt[2];
// //        // û�����ҵ����߾��˳�
// //        if (garage_rpts_num == 0) return;
// //        // ��������һ����Χ���Ұ�����
// //        for (int i = 10; i < MIN(80, garage_rpts_num); i++) {
// //            Pespective_anti(garage_rpts[i][0],garage_rpts[i][1],pt[0],pt[1]);//���任��ԭͼ�߶�
// //            if(pt[0] < 1 || pt[0] > 78 || pt[1] < 1 || pt[1] > 58) return;//����ͼ��Χ������
// //
// //            zebra_cross_flag_begin = AT_IMAGE(&img_raw, pt[0], pt[1]) > thres;
// //
// //            memset(zebra_cross_flag0, 0, sizeof(zebra_cross_flag0));//��̬����ռ�
// //            //�������ߺڰ�����
// //            zebra_cross_flag0_num = 0;
// //            for (int x = pt[0] - 1; x >= MAX(0, pt[0] - 50); x--) {
// //                if (zebra_cross_flag_begin == 0) { // even white, odd black
// //                    if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current even, current white
// //                        zebra_cross_flag0[zebra_cross_flag0_num]++;
// //                    } else if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current even, current black
// //                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
// //                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current odd, current white
// //                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
// //                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current odd, current black
// //                        zebra_cross_flag0[zebra_cross_flag0_num]++;
// //                    }
// //                } else { // even black, odd white
// //                    if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current even, current white
// //                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
// //                    } else if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current even, current black
// //                        zebra_cross_flag0[zebra_cross_flag0_num]++;
// //                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current odd, current white
// //                        zebra_cross_flag0[zebra_cross_flag0_num]++;
// //                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current odd, current black
// //                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
// //                    }
// //                }
// //            }
// //            //�������ߺڰ�����
// //            memset(zebra_cross_flag1, 0, sizeof(zebra_cross_flag1));
// //            zebra_cross_flag1_num = 0;
// //            for (int x = pt[0] + 1; x <= MIN(img_raw.width - 1, pt[0] + 50); x++) {
// //                if (zebra_cross_flag_begin == 0) { // even white, odd black
// //                    if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current even, current white
// //                        zebra_cross_flag1[zebra_cross_flag1_num]++;
// //                    } else if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current even, current black
// //                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
// //                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current odd, current white
// //                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
// //                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current odd, current black
// //                        zebra_cross_flag1[zebra_cross_flag1_num]++;
// //                    }
// //                } else { // even black, odd white
// //                    if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current even, current white
// //                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
// //                    } else if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current even, current black
// //                        zebra_cross_flag1[zebra_cross_flag1_num]++;
// //                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) > thres) { // current odd, current white
// //                        zebra_cross_flag1[zebra_cross_flag1_num]++;
// //                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, pt[1]) < thres) { // current odd, current black
// //                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
// //                    }
// //                }
// //            }
// //            // �ж������������ֵ������ʶ�������
// //            int i0 = 1;
// //            for (; i0 < zebra_cross_flag0_num - 1; i0++) {
// //                if (zebra_cross_flag0[i0] < 2 || zebra_cross_flag0[i0] >= 20 || abs(zebra_cross_flag0[i0 + 1] - zebra_cross_flag0[i0]) >= 10) break;
// //            }
// //            if(i0 > 6)            garage_type = GARAGE_FOUND_LEFT;
// //
// //            int i1 = 1;
// //            for (; i1 < zebra_cross_flag1_num - 1; i1++) {
// //                if (zebra_cross_flag1[i1] < 2 || zebra_cross_flag1[i1] >= 20 || abs(zebra_cross_flag1[i1 + 1] - zebra_cross_flag1[i1]) >= 10) break;
// //            }
// //            if(i1 > 6)            garage_type = GARAGE_FOUND_RIGHT;
// //
// //        }
// //    }

// //    //Ostu������,����18�컪������������̣��ж�һ��ֱ����һ�����н�L���ԶL�����ж�Ϊ����
// //    if(Lpt1_found && Z.all_length - garage_encoder > ENCODER_PER_METER * 1 && ! is_straight1 && Lpt1_rpts1s_id < 45 && touch_boundary1 && ! touch_boundary0)
// //    {
// //        //�˴����Լ�һ������Զ��˫L�ǵ��ж�
// //        int temp1 = r_garage_check_supple();
// //        if(temp1)
// //        {
// //            garage_type = GARAGE_FOUND_RIGHT;
// //            garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
// //            garage_cnt ++ ;
// //        }
// //    }
// //
// //    if(Lpt0_found && Z.all_length - garage_encoder > ENCODER_PER_METER * 1 && ! is_straight0 && Lpt0_rpts0s_id < 45 && touch_boundary0 && ! touch_boundary1)
// //    {
// //        //�˴����Լ�һ������Զ��˫L�ǵ��ж�
// //        int temp2 = l_garage_check_supple();
// //        if(temp2)
// //        {
// //            garage_type = GARAGE_FOUND_LEFT;
// //            garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
// //            garage_cnt ++;
// //        }
// //    }

// }

// void run_garage(void )
// {

//     int bottom_black_point_num = 0;

//     for(int j = UROW-2 ;j > UROW-10 ;j --)
//     {
//        for(int i =0 ;i < UCOL;i++)
//         {
//             if(Image_Use_Robert[j][i] == 0)                //�ײ����кڵ���            һ��������಻����100�������ߴ��300����ֵgarage_thres��������120(Sobel) 440(Ostu)
//             {
//                 bottom_black_point_num ++;
//             }

//         }
//     }

//     //���ݲ�ͬԤ����ѡ���������ֵ
//     uint16 garage_thres;
//     if(choose_Binarization == 0) //Sobel
//         garage_thres = 120;
//     else if(choose_Binarization == 1)
//         garage_thres = 440;

//     //·���󳵿�
//      if(garage_type == GARAGE_FOUND_LEFT && garage_cnt ==1 && bottom_black_point_num > garage_thres)
//      {
//          garage_type = GARAGE_GO_LEFT;

//      }

// //     else if(garage_type == GARAGE_PASS_LEFT)
// //     {
// //         garage_farline_right();             //ѭ�ұߵ�Զ��
// //
// //
// //         if ( rpts0s_num < 16) { g_not_have_line++; }               //��ߵ�������ʧ
// //         if (g_not_have_line > 2 && rpts1s_num > 20 && rpts0s_num > 20
// //                 && bottom_black_point_num <garage_thres) {           //Ȼ���������߶����֣��ҵײ�û�к��ߣ�����͹�ȥ��
// //             garage_type = GARAGE_NONE;
// //             g_not_have_line = 0;
// //         }
// //
// //     }

//      //  ���󳵿�
// //      else if(garage_type == GARAGE_FOUND_LEFT && garage_cnt ==2)
//       if((/*garage_type == GARAGE_FOUND_LEFT ||*/ garage_type == GARAGE_GO_LEFT) && garage_cnt ==1)
//      {
//           Z.integral_angle_start_flag = 1;
// //         aim_distance = 0.2;

//          if( ( N_Lpt1_found && N_Lpt0_found )  || (Z.all_length - garage_encoder > ENCODER_PER_METER * 0.6 && CarAngle.Turn_Angle < -2.5))
// //         if( ( N_Lpt1_found || N_Lpt0_found )  || Z.all_length - garage_encoder > ENCODER_PER_METER * 1.2)
//                             //�����˳�����,���߿��������0.5M
//          {
//              garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
//              garage_type = GARAGE_IN_LEFT;
//              Z.integral_angle_start_flag = 0;
//          }

//      }

//      else if(garage_type == GARAGE_IN_LEFT  && Z.all_length - garage_encoder > ENCODER_PER_METER * 0.2 )
//      {
//          Z.Stop = 1;                                 //ͣ��
//          garage_type = GARAGE_STOP;

//          tft180_clear();

//      }

// //    //  ���󳵿�
// //     else if(garage_type == GARAGE_FOUND_LEFT && garage_cnt ==2)
// //    {
// //
// //        if( ( N_Lpt1_found && N_Lpt0_found  ) || Z.all_length - garage_encoder > ENCODER_PER_METER * 0.6
// //                || (Z.all_length - garage_encoder > ENCODER_PER_METER * 0.4 && ( N_Lpt1_found || N_Lpt0_found ))
// //
// //        )                    //�����˳�����,���߿��������0.5M
// //        {
// //            //garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
// //            garage_type = GARAGE_IN_LEFT;
// //
// //            aim_distance = 0.3;
// //        }
// //
// //    }
// //
// //    else if(garage_type == GARAGE_IN_LEFT /* && Z.all_length - garage_encoder > ENCODER_PER_METER * Z.stop_all_dis*/ )
// //    {
// //        Z.Stop = 1;                                 //ͣ��
// //        garage_type = GARAGE_STOP;
// //
// //        ips200_clear(0x07E0);
// //
// //    }

//      //·���ҳ��� ����
// //    else if(garage_type == GARAGE_FOUND_RIGHT  && garage_cnt ==1 )
// //     else if(garage_type == GARAGE_FOUND_RIGHT  && garage_cnt ==100 )
// //    {
// //        garage_type = GARAGE_PASS_RIGHT;
// //
// //
// //
// //    }
//       if(garage_type == GARAGE_FOUND_RIGHT && garage_cnt ==1 && bottom_black_point_num > garage_thres)
//       {
//           garage_type = GARAGE_GO_RIGHT;

//       }

// //    else if(garage_type == GARAGE_PASS_RIGHT)
// //    {
// //        garage_farline_left();             //ѭ��ߵ�Զ��
// //
// //
// //        if ( rpts1s_num < 16) { g_not_have_line++; }               //�ұߵ�������ʧ
// //        if (g_not_have_line > 2 && rpts1s_num > 30 && rpts0s_num > 30
// //                && bottom_black_point_num <garage_thres)
// //        {           //Ȼ���������߶����֣��ҵײ�û�к��ߣ�����͹�ȥ��
// //            garage_type = GARAGE_NONE;
// //            g_not_have_line = 0;
// //
// //        }
// //
// //    }

//     //���ҳ���
// //    else if(garage_type == GARAGE_FOUND_RIGHT  && garage_cnt ==2 )
//     if((/*garage_type == GARAGE_FOUND_RIGHT ||*/ garage_type == GARAGE_GO_RIGHT) && garage_cnt == 1 )
//     {
// //        if ( rpts1s_num < 16) { g_not_have_line++; }               //�ұߵ�������ʧ
// //        if (g_not_have_line > 2 && rpts1s_num > 30 && rpts0s_num > 30
// //                && bottom_black_point_num <garage_thres)
// //        {           //Ȼ���������߶����֣��ҵײ�û�к��ߣ�����͹�ȥ��
// //            garage_type = GARAGE_GO_RIGHT;
// //            g_not_have_line = 0;
// //
// //        }
//         Z.integral_angle_start_flag = 1;
// //        aim_distance = 0.2;

//         if( (N_Lpt1_found && N_Lpt0_found  ) || ( Z.all_length - garage_encoder > ENCODER_PER_METER * 0.6 && CarAngle.Turn_Angle > 2.5))         //�����˳�����
//     //        if(   (N_Lpt1_found || N_Lpt0_found  )  || Z.all_length - garage_encoder > ENCODER_PER_METER * 1.2  )                    //�����˳�����
//         {
//             Z.integral_angle_start_flag = 0;
//             garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
//             garage_type = GARAGE_IN_RIGHT;
//         }
// //        if((N_Lpt1_found && N_Lpt0_found  )|| Z.all_length - garage_encoder > ENCODER_PER_METER * 0.2)
// //        {
// //            garage_encoder = Z.all_length;                   //�˿̵ĳ���·�̱��������
// //            garage_type = GARAGE_IN_RIGHT;
// //        }
//     }

//     else if(garage_type == GARAGE_IN_RIGHT  && Z.all_length - garage_encoder > ENCODER_PER_METER * 0.2 )
//     {
//         Z.Stop = 1;                                 //ͣ��
//         garage_type = GARAGE_STOP;

//     }

//     if (garage_type == GARAGE_FOUND_LEFT || garage_type == GARAGE_IN_LEFT || garage_type == GARAGE_PASS_RIGHT ) {
//         track_type = TRACK_LEFT;

//     } else if (garage_type == GARAGE_FOUND_RIGHT || garage_type == GARAGE_IN_RIGHT || garage_type == GARAGE_PASS_LEFT) {

//         track_type = TRACK_RIGHT;
//     }

// }

// //����ʱ�ų������ģ����������һ�����ƾ��뼴��
// void out_garage(void )
// {
//     Z.integral_angle_start_flag = 1;
//     //ƫ���ǽǶȸ����жϳ���
//     if( !Z.start_left_or_right)
//     {
// //        if(Z.all_length < ENCODER_PER_METER*0.2)
// //            Z.mpu6050.expect_yaw = 0;
// //        else
// //            Z.mpu6050.expect_yaw = -3.0;
//         if(Z.all_length > ENCODER_PER_METER*0.6 ||  CarAngle.Turn_Angle < -2.5)
//         {
//             garage_type = GARAGE_NONE;
//             Z.integral_angle_start_flag = 0;
// //            Z.Stop = 1;
//         }
//     }
//     else
//     {
// //        if(Z.all_length < ENCODER_PER_METER*0.2)
// //            Z.mpu6050.expect_yaw = 0;
// //        else
// //            Z.mpu6050.expect_yaw = 3.0;
//         if(Z.all_length > ENCODER_PER_METER*0.6 ||  CarAngle.Turn_Angle > 2.5)
//         {
//             garage_type = GARAGE_NONE;
//             Z.integral_angle_start_flag = 0;
// //            Z.Stop = 1;
//         }
//     }

//     //�涨��������
// //    if( !Z.start_left_or_right)
// //    {
// //        track_type = TRACK_LEFT;
// //
// //    }else {
// //        track_type = TRACK_RIGHT;
// //    }

// }

// /****************************
// void garage_farline_right(void )
// {

//         int  L_x1 , L_y1;

//         if ( N_Lpt1_found && Lpt0_found  )
//           Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &L_x1 , &L_y1 );  //���ݽ�����L��,�ҵ�Զ�����ұ���
//         //������͸�ӵ�L�����˻ص�60*80��ͼ������
//         else
//         {
//              L_x1 = 40 + 18;
//              L_y1 = 25 + 15;

//          }

//         int     begin_x1 = clip(  L_x1 -18, 30, 78 );      //�����ͼ��һ�㲻��ƫ�������޶�Զ����ʼ�ĵ����Ұ벿��
//         int     begin_y1 = clip(L_y1 -10,2, 58 );     //��ʼ��

//         int  far_x1 =0 , far_y1 = 0;                  //Ѱ�ҵ��ĵ�

//         image_t img_raw =  {.data=Image_Use_Robert,          .width=UCOL, .height=UROW, .step=UCOL};

//         uint8 thres = 1;

//         //�ұ�

//         g_far_ipts1_num = sizeof(g_far_ipts1) / sizeof(g_far_ipts1[0]);

//                     //�ȴ��������ң���Ϊ�˷�ֹԶ����ʼ���Ǻڵ�
//         for (; begin_y1 >0; begin_y1--) {
//             //�Ұױ��
//             if (AT_IMAGE(&img_raw, begin_x1, begin_y1) >=thres) {

//                 break;
//             }
//         }

//         //�ڴ���������

//         for (; begin_x1 <UCOL -1; begin_x1++) {
//             //�Ұױ��
//             if (AT_IMAGE(&img_raw, begin_x1, begin_y1) <thres) {
//                 far_x1 = begin_x1;
//                 far_y1 = begin_y1;
//                 break;
//             }
//         }

//         //���ҵ��ǵ�λ�ÿ�ʼѰ��
//         if (AT_IMAGE(&img_raw, far_x1 -1 , far_y1 ) >= thres)
//             Right_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x1 -1 , far_y1 , g_far_ipts1, &g_far_ipts1_num);
//         else g_far_ipts1_num = 0;

//         // ȥ����+͸�ӱ任

//         Pespective(g_far_ipts1,g_far_ipts1_num ,  g_far_rpts1);
//         g_far_rpts1_num = g_far_ipts1_num;

//         // �����˲�

//         blur_points(g_far_rpts1, g_far_rpts1_num, g_far_rpts1b, (int) round(line_blur_kernel));
//         g_far_rpts1b_num = g_far_rpts1_num;

//         // ���ߵȾ����

//         g_far_rpts1s_num = sizeof(g_far_rpts1s) / sizeof(g_far_rpts1s[0]);
//         resample_points(g_far_rpts1b, g_far_rpts1b_num, g_far_rpts1s, &g_far_rpts1s_num, sample_dist * pixel_per_meter);

// //        // ���߾ֲ��Ƕȱ仯��
// //
// //        local_angle_points(g_far_rpts1s, g_far_rpts1s_num, g_far_rpts1a, (int) round(angle_dist / sample_dist));
// //        g_far_rpts1a_num = g_far_rpts1s_num;
// //
// //        // �Ƕȱ仯�ʷǼ�������
// //
// //        nms_angle(g_far_rpts1a, g_far_rpts1a_num, g_far_rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //        g_far_rpts1an_num = g_far_rpts1a_num;
// //
// //
// //

// }

// void garage_farline_left(void )         //ѭ��ߵ�Զ��
// {

//     //    far_x1 = cross_width, far_x2 = img_raw.width -cross_width;

//         int L_x0, L_y0 ;

//         if ( Lpt1_found  &&  N_Lpt0_found)                 //��������㶼����

//           Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &L_x0 , &L_y0 );//���ݽ�����L��,�ҵ�Զ���������
//         else {
//             L_x0 = 40 - 15 ;            //Ĭ�ϳ���Զ����ʼ�㶼��ͬ��Ϊ40 �� ��  25�У�����Ϊͼ���м��ϲ��֣�
//             L_y0 = 25 + 15;             //��Ϊ·������һ�㶼�ǳ�����

//         }

//         int begin_x0 = clip( L_x0 +15 , 2, 50 );
//         int begin_y0 = clip(  L_y0 -10 , 2, 58 );

//         int far_x0 =0, far_y0 =0 ;                  //Ѱ�ҵ��ĵ�

//         image_t img_raw =  {.data=Image_Use_Robert,          .width=UCOL, .height=UROW, .step=UCOL};

//         uint8 thres = 1;

//     // ����Ӧ�����ڽ���L�����࿪ʼ�������ǹ̶�����Ϊ������б��ʮ��

//         g_far_ipts0_num = sizeof(g_far_ipts0) / sizeof(g_far_ipts0[0]);

//         //�ȴ��������ң���Ϊ�˷�ֹԶ����ʼ���Ǻڵ�,��Ϊ�����ߵĴ�ϸ���ž������
//         for (; begin_y0 > 0; begin_y0--) {

//             if (AT_IMAGE(&img_raw, begin_x0, begin_y0) == thres) {
// //                far_x0 = begin_x0;
// //                far_y0 = begin_y0;    //��һ���Ǻڵ�
//                 break;
//             }
//         }

//         //��������һ�е��м���������
//         for (; begin_x0 > 0; begin_x0--) {

//              if (AT_IMAGE(&img_raw, begin_x0, begin_y0) < thres) {
//                  far_x0 = begin_x0;
//                  far_y0 = begin_y0;    //��һ���Ǻڵ�
//                  break;
//              }
//          }

//         //���ҵ��ǵ�λ�ÿ�ʼѰ��
//         if (AT_IMAGE(&img_raw, far_x0 + 1 , far_y0) >= thres)
//             Left_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x0 + 1, far_y0 , g_far_ipts0, &g_far_ipts0_num);
//         else g_far_ipts0_num = 0;

//         // ȥ����+͸�ӱ任

//         Pespective(g_far_ipts0,g_far_ipts0_num ,  g_far_rpts0);
//         g_far_rpts0_num = g_far_ipts0_num;

//         // �����˲�
//         blur_points(g_far_rpts0, g_far_rpts0_num, g_far_rpts0b, (int) round(line_blur_kernel));
//         g_far_rpts0b_num = g_far_rpts0_num;

//         // ���ߵȾ����
//         g_far_rpts0s_num = sizeof(g_far_rpts0s) / sizeof(g_far_rpts0s[0]);
//         resample_points(g_far_rpts0b, g_far_rpts0b_num, g_far_rpts0s, &g_far_rpts0s_num, sample_dist * pixel_per_meter);

// }
// **********************/
