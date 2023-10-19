/*
 * garage.c
 *
 *  Created on: 2023年1月12日
 *      Author: HQC
 */
#include "garage.h"

// // 以下定义为车库寻远线设定
// //int g_far_ipts0[LLL][2];           //种子巡线
// //int g_far_ipts1[LLL][2];
// //int g_far_ipts0_num, g_far_ipts1_num;
// //
// //float g_far_rpts0[LLL][2];         //逆透视
// //float g_far_rpts1[LLL][2];
// //int g_far_rpts0_num, g_far_rpts1_num;
// //
// //float g_far_rpts0b[LLL][2];        //逆透视滤波
// //float g_far_rpts1b[LLL][2];
// //int g_far_rpts0b_num, g_far_rpts1b_num;
// //
// //float g_far_rpts0s[LLL][2];        //逆透视等距采样
// //float g_far_rpts1s[LLL][2];
// //int g_far_rpts0s_num, g_far_rpts1s_num;
// //
// //
// //
// //
// //float g_far_rpts0a[LLL];           //每点的角度，，没用到
// //float g_far_rpts1a[LLL];
// //int g_far_rpts0a_num, g_far_rpts1a_num;
// //
// //float g_far_rpts0an[LLL];          //角度极大值抑制，，，没用到
// //float g_far_rpts1an[LLL];
// //int g_far_rpts0an_num, g_far_rpts1an_num;

// // 车库远处L角点
// int g_far_Lpt0_rpts0s_id, g_far_Lpt1_rpts1s_id;
// bool g_far_Lpt0_found, g_far_Lpt1_found;
// // 车库远处内L角点
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

enum garage_type_e garage_type = GARAGE_NONE; // 初始化为向左出库 调试状态为NONE
int Zibra_Thres = 0;
// uint8 touch_boundary0; // 左边线走到图像左边界
// uint8 touch_boundary1; // 右边线走到图像右边界
void Garage_Check(void)//找到上角点则加入上角点一起判断
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
// // 编码器，防止重复触发等情况
// int64_t garage_encoder = -10000;

// // 出库转向方向
// bool out_dir = 0;

// // 记录第几个车库
// int16_t garage_cnt = 0;

// //uint8 r_garage_check_supple()
// //{
// //    int G_x1,G_y1;
// //    Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &G_x1 , &G_y1 );
// //
// //    //从L拐点向上扫线，找跳变点
// //    for (; G_y1 > 0; G_y1--) if (AT_IMAGE(&img_raw, G_x1, G_y1-1) < thres) break;
// //    if (AT_IMAGE(&img_raw, G_x1, G_y1) >= thres)
// //      Right_Adaptive_Threshold(&img_raw, block_size, clip_value, G_x1, G_y1, g_far_ipts1, &g_far_ipts1_num);
// //    else g_far_ipts1_num = 0;
// //    //对边线透视变换
// //    Pespective(g_far_ipts1,g_far_ipts1_num ,  g_far_rpts1);
// //    g_far_rpts1_num = g_far_ipts1_num;
// //    //边线三角滤波
// //    blur_points(g_far_rpts1,g_far_rpts1_num,g_far_rpts1b,(int) round(line_blur_kernel));
// //    g_far_rpts1b_num = g_far_rpts1_num;
// //    // 边线等距采样
// //    g_far_rpts1s_num = sizeof(g_far_rpts1s) / sizeof(g_far_rpts1s[0]);
// //    resample_points(g_far_rpts1b, g_far_rpts1b_num, g_far_rpts1s, &g_far_rpts1s_num, sample_dist * pixel_per_meter);
// //    // 边线局部角度变化率
// //    local_angle_points(g_far_rpts1s, g_far_rpts1s_num, g_far_rpts1a, (int) round(angle_dist / sample_dist));
// //    g_far_rpts1a_num = g_far_rpts1s_num;
// //    // 角度变化率非极大抑制
// //    nms_angle(g_far_rpts1a, g_far_rpts1a_num, g_far_rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //    g_far_rpts1an_num = g_far_rpts1a_num;
// //
// //    //远角点识别(确认车库关键)
// //    g_far_Lpt1_found = g_far_N_Lpt1_found = false; //初始化为0，防止上次标志位误判
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
// ////            if(g_far_rpts1a[g_far_Lpt1_rpts1s_id] > 0)              //这一个L点如果是向右拐的，就是内L点（>0左拐 <0右拐）
// ////            {
// ////                g_far_N_Lpt1_rpts1s_id = g_far_Lpt1_rpts1s_id;
// ////                g_far_N_Lpt1_found = true;
// ////                g_far_Lpt1_found = false;
// ////            }
// ////            if(g_far_Lpt1_found)
// ////                break;//一旦检测到第二个L角点，立刻跳出循环
// //        }
// //    }
// ////    return g_far_Lpt1_found;
// //}
// //uint8 l_garage_check_supple()
// //{
// //    int G_x0,G_y0;
// //    Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &G_x0 , &G_y0 );
// //
// //    //从L拐点向上扫线，找跳变点
// //    for (; G_y0 > 0; G_y0--) if (AT_IMAGE(&img_raw, G_x0, G_y0-1) < thres) break;
// //    if (AT_IMAGE(&img_raw, G_x0, G_y0) >= thres)
// //      Left_Adaptive_Threshold(&img_raw, block_size, clip_value, G_x0, G_y0, g_far_ipts0, &g_far_ipts0_num);
// //    else g_far_ipts0_num = 0;
// //    //对边线透视变换
// //    Pespective(g_far_ipts0,g_far_ipts0_num ,  g_far_rpts0);
// //    g_far_rpts0_num = g_far_ipts0_num;
// //    //边线三角滤波
// //    blur_points(g_far_rpts0,g_far_rpts0_num,g_far_rpts0b,(int) round(line_blur_kernel));
// //    g_far_rpts0b_num = g_far_rpts0_num;
// //    // 边线等距采样
// //    g_far_rpts0s_num = sizeof(g_far_rpts0s) / sizeof(g_far_rpts0s[0]);
// //    resample_points(g_far_rpts0b, g_far_rpts0b_num, g_far_rpts0s, &g_far_rpts0s_num, sample_dist * pixel_per_meter);
// //    // 边线局部角度变化率
// //    local_angle_points(g_far_rpts0s, g_far_rpts0s_num, g_far_rpts0a, (int) round(angle_dist / sample_dist));
// //    g_far_rpts0a_num = g_far_rpts0s_num;
// //    // 角度变化率非极大抑制
// //    nms_angle(g_far_rpts0a, g_far_rpts0a_num, g_far_rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //    g_far_rpts0an_num = g_far_rpts0a_num;
// //
// //    //远角点识别(确认车库关键)
// //    g_far_Lpt0_found = g_far_N_Lpt0_found = false; //初始化为0，防止上次标志位误判
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
// ////            if(g_far_rpts0a[g_far_Lpt0_rpts0s_id] > 0)              //这一个L点如果是向左拐的，就是内L点
// ////            {
// ////                g_far_N_Lpt0_rpts0s_id = g_far_Lpt0_rpts0s_id;
// ////                g_far_N_Lpt0_found = true;
// ////                g_far_Lpt0_found = false;
// ////            }
// ////            if(g_far_Lpt0_found)
// ////                break;//一旦检测到第二个L角点，立刻跳出循环
// //        }
// //    }
// ////    return g_far_Lpt0_found;
// //
// //}

// void check_garage(void )
// {
//     //Sobel边缘检测判断
//     //总路程走过两米
// //    if(choose_Binarization == 0)//sobel检测车库
// //    {
//         if( Lpt0_found && N_Lpt1_found  && touch_boundary0 && ! touch_boundary1)
//         {
//             garage_type = GARAGE_FOUND_LEFT;
//             garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
//             garage_cnt ++ ;
//             aim_distance = 0.3;
//     //        Z.Stop = 1;                                 //停车
//         }

//         if(Lpt1_found && N_Lpt0_found  && touch_boundary1 && ! touch_boundary0)
//         {
//             garage_type = GARAGE_FOUND_RIGHT;
//             garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
//             garage_cnt ++;
//             aim_distance = 0.3;
//     //        Z.Stop = 1;                                 //停车
//         }
// //    }
// //    else if(choose_Binarization == 1)
// //    {
// //        // 根据当前的L角点情况决定，取左(右)中线，进行斑马线的寻找
// //        if (Lpt0_found && !Lpt1_found) {
// //            // 仅左角点，则在右中线上找斑马线
// //            garage_rpts = rptsc1;
// //            garage_rpts_num = rptsc1_num;
// //        } else if (Lpt1_found && !Lpt0_found) {
// //            // 仅右角点，则在左中线上找斑马线
// //            garage_rpts = rptsc0;
// //            garage_rpts_num = rptsc0_num;
// //        } else {
// //            // 其余情况不找
// //            garage_rpts_num = 0;
// //        }
// //        int pt[2];
// //        // 没有能找的中线就退出
// //        if (garage_rpts_num == 0) return;
// //        // 在中线上一定范围内找斑马线
// //        for (int i = 10; i < MIN(80, garage_rpts_num); i++) {
// //            Pespective_anti(garage_rpts[i][0],garage_rpts[i][1],pt[0],pt[1]);//反变换求原图线段
// //            if(pt[0] < 1 || pt[0] > 78 || pt[1] < 1 || pt[1] > 58) return;//超出图像范围则跳过
// //
// //            zebra_cross_flag_begin = AT_IMAGE(&img_raw, pt[0], pt[1]) > thres;
// //
// //            memset(zebra_cross_flag0, 0, sizeof(zebra_cross_flag0));//动态分配空间
// //            //数斑马线黑白跳变
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
// //            //数斑马线黑白跳变
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
// //            // 判断连续跳变的阈值条件以识别斑马线
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

// //    //Ostu车库检测,根据18届华北区赛赛道编程，判断一条直线另一条含有近L点和远L点则判断为车库
// //    if(Lpt1_found && Z.all_length - garage_encoder > ENCODER_PER_METER * 1 && ! is_straight1 && Lpt1_rpts1s_id < 45 && touch_boundary1 && ! touch_boundary0)
// //    {
// //        //此处可以加一个补搜远线双L角点判断
// //        int temp1 = r_garage_check_supple();
// //        if(temp1)
// //        {
// //            garage_type = GARAGE_FOUND_RIGHT;
// //            garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
// //            garage_cnt ++ ;
// //        }
// //    }
// //
// //    if(Lpt0_found && Z.all_length - garage_encoder > ENCODER_PER_METER * 1 && ! is_straight0 && Lpt0_rpts0s_id < 45 && touch_boundary0 && ! touch_boundary1)
// //    {
// //        //此处可以加一个补搜远线双L角点判断
// //        int temp2 = l_garage_check_supple();
// //        if(temp2)
// //        {
// //            garage_type = GARAGE_FOUND_LEFT;
// //            garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
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
//             if(Image_Use_Robert[j][i] == 0)                //底部两行黑点数            一般正常最多不超过100，斑马线大概300，阈值garage_thres可以设置120(Sobel) 440(Ostu)
//             {
//                 bottom_black_point_num ++;
//             }

//         }
//     }

//     //根据不同预处理选择斑马线阈值
//     uint16 garage_thres;
//     if(choose_Binarization == 0) //Sobel
//         garage_thres = 120;
//     else if(choose_Binarization == 1)
//         garage_thres = 440;

//     //路过左车库
//      if(garage_type == GARAGE_FOUND_LEFT && garage_cnt ==1 && bottom_black_point_num > garage_thres)
//      {
//          garage_type = GARAGE_GO_LEFT;

//      }

// //     else if(garage_type == GARAGE_PASS_LEFT)
// //     {
// //         garage_farline_right();             //循右边的远线
// //
// //
// //         if ( rpts0s_num < 16) { g_not_have_line++; }               //左边的线先消失
// //         if (g_not_have_line > 2 && rpts1s_num > 20 && rpts0s_num > 20
// //                 && bottom_black_point_num <garage_thres) {           //然后左右两边都出现，且底部没有黑线，车库就过去了
// //             garage_type = GARAGE_NONE;
// //             g_not_have_line = 0;
// //         }
// //
// //     }

//      //  进左车库
// //      else if(garage_type == GARAGE_FOUND_LEFT && garage_cnt ==2)
//       if((/*garage_type == GARAGE_FOUND_LEFT ||*/ garage_type == GARAGE_GO_LEFT) && garage_cnt ==1)
//      {
//           Z.integral_angle_start_flag = 1;
// //         aim_distance = 0.2;

//          if( ( N_Lpt1_found && N_Lpt0_found )  || (Z.all_length - garage_encoder > ENCODER_PER_METER * 0.6 && CarAngle.Turn_Angle < -2.5))
// //         if( ( N_Lpt1_found || N_Lpt0_found )  || Z.all_length - garage_encoder > ENCODER_PER_METER * 1.2)
//                             //看到了车库里,或者看到车库后0.5M
//          {
//              garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
//              garage_type = GARAGE_IN_LEFT;
//              Z.integral_angle_start_flag = 0;
//          }

//      }

//      else if(garage_type == GARAGE_IN_LEFT  && Z.all_length - garage_encoder > ENCODER_PER_METER * 0.2 )
//      {
//          Z.Stop = 1;                                 //停车
//          garage_type = GARAGE_STOP;

//          tft180_clear();

//      }

// //    //  进左车库
// //     else if(garage_type == GARAGE_FOUND_LEFT && garage_cnt ==2)
// //    {
// //
// //        if( ( N_Lpt1_found && N_Lpt0_found  ) || Z.all_length - garage_encoder > ENCODER_PER_METER * 0.6
// //                || (Z.all_length - garage_encoder > ENCODER_PER_METER * 0.4 && ( N_Lpt1_found || N_Lpt0_found ))
// //
// //        )                    //看到了车库里,或者看到车库后0.5M
// //        {
// //            //garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
// //            garage_type = GARAGE_IN_LEFT;
// //
// //            aim_distance = 0.3;
// //        }
// //
// //    }
// //
// //    else if(garage_type == GARAGE_IN_LEFT /* && Z.all_length - garage_encoder > ENCODER_PER_METER * Z.stop_all_dis*/ )
// //    {
// //        Z.Stop = 1;                                 //停车
// //        garage_type = GARAGE_STOP;
// //
// //        ips200_clear(0x07E0);
// //
// //    }

//      //路过右车库 不用
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
// //        garage_farline_left();             //循左边的远线
// //
// //
// //        if ( rpts1s_num < 16) { g_not_have_line++; }               //右边的线先消失
// //        if (g_not_have_line > 2 && rpts1s_num > 30 && rpts0s_num > 30
// //                && bottom_black_point_num <garage_thres)
// //        {           //然后左右两边都出现，且底部没有黑线，车库就过去了
// //            garage_type = GARAGE_NONE;
// //            g_not_have_line = 0;
// //
// //        }
// //
// //    }

//     //进右车库
// //    else if(garage_type == GARAGE_FOUND_RIGHT  && garage_cnt ==2 )
//     if((/*garage_type == GARAGE_FOUND_RIGHT ||*/ garage_type == GARAGE_GO_RIGHT) && garage_cnt == 1 )
//     {
// //        if ( rpts1s_num < 16) { g_not_have_line++; }               //右边的线先消失
// //        if (g_not_have_line > 2 && rpts1s_num > 30 && rpts0s_num > 30
// //                && bottom_black_point_num <garage_thres)
// //        {           //然后左右两边都出现，且底部没有黑线，车库就过去了
// //            garage_type = GARAGE_GO_RIGHT;
// //            g_not_have_line = 0;
// //
// //        }
//         Z.integral_angle_start_flag = 1;
// //        aim_distance = 0.2;

//         if( (N_Lpt1_found && N_Lpt0_found  ) || ( Z.all_length - garage_encoder > ENCODER_PER_METER * 0.6 && CarAngle.Turn_Angle > 2.5))         //看到了车库里
//     //        if(   (N_Lpt1_found || N_Lpt0_found  )  || Z.all_length - garage_encoder > ENCODER_PER_METER * 1.2  )                    //看到了车库里
//         {
//             Z.integral_angle_start_flag = 0;
//             garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
//             garage_type = GARAGE_IN_RIGHT;
//         }
// //        if((N_Lpt1_found && N_Lpt0_found  )|| Z.all_length - garage_encoder > ENCODER_PER_METER * 0.2)
// //        {
// //            garage_encoder = Z.all_length;                   //此刻的车库路程标记下来了
// //            garage_type = GARAGE_IN_RIGHT;
// //        }
//     }

//     else if(garage_type == GARAGE_IN_RIGHT  && Z.all_length - garage_encoder > ENCODER_PER_METER * 0.2 )
//     {
//         Z.Stop = 1;                                 //停车
//         garage_type = GARAGE_STOP;

//     }

//     if (garage_type == GARAGE_FOUND_LEFT || garage_type == GARAGE_IN_LEFT || garage_type == GARAGE_PASS_RIGHT ) {
//         track_type = TRACK_LEFT;

//     } else if (garage_type == GARAGE_FOUND_RIGHT || garage_type == GARAGE_IN_RIGHT || garage_type == GARAGE_PASS_LEFT) {

//         track_type = TRACK_RIGHT;
//     }

// }

// //发车时放车库中心，距离出赛道一个手掌距离即可
// void out_garage(void )
// {
//     Z.integral_angle_start_flag = 1;
//     //偏航角角度辅助判断出库
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

//     //规定出发方向
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
//           Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &L_x1 , &L_y1 );  //根据近的右L点,找到远处的右边线
//         //本来逆透视的L点在退回到60*80的图像坐标
//         else
//         {
//              L_x1 = 40 + 18;
//              L_y1 = 25 + 15;

//          }

//         int     begin_x1 = clip(  L_x1 -18, 30, 78 );      //车库的图像一般不会偏，所以限定远处开始的点在右半部分
//         int     begin_y1 = clip(L_y1 -10,2, 58 );     //开始点

//         int  far_x1 =0 , far_y1 = 0;                  //寻找到的点

//         image_t img_raw =  {.data=Image_Use_Robert,          .width=UCOL, .height=UROW, .step=UCOL};

//         uint8 thres = 1;

//         //右边

//         g_far_ipts1_num = sizeof(g_far_ipts1) / sizeof(g_far_ipts1[0]);

//                     //先从下往上找，是为了防止远处初始点是黑点
//         for (; begin_y1 >0; begin_y1--) {
//             //找白变黑
//             if (AT_IMAGE(&img_raw, begin_x1, begin_y1) >=thres) {

//                 break;
//             }
//         }

//         //在从左往右找

//         for (; begin_x1 <UCOL -1; begin_x1++) {
//             //找白变黑
//             if (AT_IMAGE(&img_raw, begin_x1, begin_y1) <thres) {
//                 far_x1 = begin_x1;
//                 far_y1 = begin_y1;
//                 break;
//             }
//         }

//         //从找到角点位置开始寻找
//         if (AT_IMAGE(&img_raw, far_x1 -1 , far_y1 ) >= thres)
//             Right_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x1 -1 , far_y1 , g_far_ipts1, &g_far_ipts1_num);
//         else g_far_ipts1_num = 0;

//         // 去畸变+透视变换

//         Pespective(g_far_ipts1,g_far_ipts1_num ,  g_far_rpts1);
//         g_far_rpts1_num = g_far_ipts1_num;

//         // 边线滤波

//         blur_points(g_far_rpts1, g_far_rpts1_num, g_far_rpts1b, (int) round(line_blur_kernel));
//         g_far_rpts1b_num = g_far_rpts1_num;

//         // 边线等距采样

//         g_far_rpts1s_num = sizeof(g_far_rpts1s) / sizeof(g_far_rpts1s[0]);
//         resample_points(g_far_rpts1b, g_far_rpts1b_num, g_far_rpts1s, &g_far_rpts1s_num, sample_dist * pixel_per_meter);

// //        // 边线局部角度变化率
// //
// //        local_angle_points(g_far_rpts1s, g_far_rpts1s_num, g_far_rpts1a, (int) round(angle_dist / sample_dist));
// //        g_far_rpts1a_num = g_far_rpts1s_num;
// //
// //        // 角度变化率非极大抑制
// //
// //        nms_angle(g_far_rpts1a, g_far_rpts1a_num, g_far_rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //        g_far_rpts1an_num = g_far_rpts1a_num;
// //
// //
// //

// }

// void garage_farline_left(void )         //循左边的远线
// {

//     //    far_x1 = cross_width, far_x2 = img_raw.width -cross_width;

//         int L_x0, L_y0 ;

//         if ( Lpt1_found  &&  N_Lpt0_found)                 //两个车库点都存在

//           Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &L_x0 , &L_y0 );//根据近的左L点,找到远处的左边线
//         else {
//             L_x0 = 40 - 15 ;            //默认车库远点起始点都相同，为40 列 ，  25行，大致为图像中间上部分，
//             L_y0 = 25 + 15;             //因为路过车库一般都是车身正

//         }

//         int begin_x0 = clip( L_x0 +15 , 2, 50 );
//         int begin_y0 = clip(  L_y0 -10 , 2, 58 );

//         int far_x0 =0, far_y0 =0 ;                  //寻找到的点

//         image_t img_raw =  {.data=Image_Use_Robert,          .width=UCOL, .height=UROW, .step=UCOL};

//         uint8 thres = 1;

//     // 这里应该是在近的L点两侧开始，而不是固定，因为极大是斜入十字

//         g_far_ipts0_num = sizeof(g_far_ipts0) / sizeof(g_far_ipts0[0]);

//         //先从下往上找，是为了防止远处初始点是黑点,因为斑马线的粗细随着距离会变大
//         for (; begin_y0 > 0; begin_y0--) {

//             if (AT_IMAGE(&img_raw, begin_x0, begin_y0) == thres) {
// //                far_x0 = begin_x0;
// //                far_y0 = begin_y0;    //这一点是黑点
//                 break;
//             }
//         }

//         //在正常从一行的中间向左搜索
//         for (; begin_x0 > 0; begin_x0--) {

//              if (AT_IMAGE(&img_raw, begin_x0, begin_y0) < thres) {
//                  far_x0 = begin_x0;
//                  far_y0 = begin_y0;    //这一点是黑点
//                  break;
//              }
//          }

//         //从找到角点位置开始寻找
//         if (AT_IMAGE(&img_raw, far_x0 + 1 , far_y0) >= thres)
//             Left_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x0 + 1, far_y0 , g_far_ipts0, &g_far_ipts0_num);
//         else g_far_ipts0_num = 0;

//         // 去畸变+透视变换

//         Pespective(g_far_ipts0,g_far_ipts0_num ,  g_far_rpts0);
//         g_far_rpts0_num = g_far_ipts0_num;

//         // 边线滤波
//         blur_points(g_far_rpts0, g_far_rpts0_num, g_far_rpts0b, (int) round(line_blur_kernel));
//         g_far_rpts0b_num = g_far_rpts0_num;

//         // 边线等距采样
//         g_far_rpts0s_num = sizeof(g_far_rpts0s) / sizeof(g_far_rpts0s[0]);
//         resample_points(g_far_rpts0b, g_far_rpts0b_num, g_far_rpts0s, &g_far_rpts0s_num, sample_dist * pixel_per_meter);

// }
// **********************/
