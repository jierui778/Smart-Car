// //#include "cross.h"


// //enum cross_type_e cross_type = CROSS_NONE;//通过枚举类型判断是不是cross

// //bool far_Lpt0_found, far_Lpt1_found;//其中 far_N_Lpt0_found 表示是否找到了反向逆透视后的左边L角点，far_N_Lpt1_found 表示是否找到了反向逆透视后的右边L角点。
// //bool far_N_Lpt0_found,far_N_Lpt1_found;//这个变量用于表示是否找到了逆逆透视后的左边L角点和右边L角点。其中，far_N_Lpt0_found表示左边L角点是否被找到，far_N_Lpt1_found表示右边L角点是否被找到。

// //int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;//far_Lpt0_rpts0s_id表示左边L角点在rpts0s数组中的索引值
// //int far_N_Lpt0_rpts0s_id,far_N_Lpt1_rpts1s_id;//这两个变量是用于记录L角点在反向逆透视后的rpts0s数组中的索引值

// //int L_x0, L_x1 , L_y0 , L_y1;                       //十字得近处两个L点的坐标

// //int low_x0,low_y0, low_x1 , low_y1;         //十字得近处两个L点的坐标得下面2个点，用来计算斜率来补线

// //int far_x0 =0, far_x1 =0 , far_y0 = 0, far_y1 = 0 ;                  //寻找到的远处起始点坐标
// //float Nfar_x0 =0, Nfar_x1 =0 , Nfar_y0 = 0, Nfar_y1 = 0 ;                  //寻找到的远处逆透视点坐标

// //float xielv_left, xielv_right;//左线斜率，右线斜率

// //// 以下定义为十字寻远线设定
// ////原始的远L角点左边线的坐标
// //int far_ipts0[L_CROSS][2];
// //int far_ipts1[L_CROSS][2];
// //int far_ipts0_num, far_ipts1_num;

// ////透视变换后的左右两边线的坐标点和坐标点的个数，用于检测单边时使用
// //float far_rpts0[L_CROSS][2];
// //float far_rpts1[L_CROSS][2];
// //int far_rpts0_num, far_rpts1_num;

// ////透视变换后的左右两边线的坐标点和坐标点的个数，用于寻找远处L角点进行补线时使用
// //float far_rpts0b[L_CROSS][2];
// //float far_rpts1b[L_CROSS][2];
// //int far_rpts0b_num, far_rpts1b_num;

// //float far_rpts0s[L_CROSS][2];
// //float far_rpts1s[L_CROSS][2];
// //int far_rpts0s_num, far_rpts1s_num;

// //float far_rpts0a[L_CROSS];
// //float far_rpts1a[L_CROSS];
// //int far_rpts0a_num, far_rpts1a_num;

// //float far_rpts0an[L_CROSS];
// //float far_rpts1an[L_CROSS];
// //int far_rpts0an_num, far_rpts1an_num;

// //int not_have_line = 0;//判断是否有线

// //extern float xielv_left_y_to_end,xielv_right_y_to_end;                 //在逆透视后得坐标系建得斜率

// //int cross_num = 0;
// // 左右边线局部角度变化率
// float rpts0a[POINTS_MAX_LEN];
// float rpts1a[POINTS_MAX_LEN];
// int rpts0a_num, rpts1a_num;
// // 左右边线局部角度变化率+非极大抑制
// float rpts0an[POINTS_MAX_LEN];
// float rpts1an[POINTS_MAX_LEN];
// int rpts0an_num, rpts1an_num;
// // 左/右中线
// float rptsc0[POINTS_MAX_LEN][2];
// float rptsc1[POINTS_MAX_LEN][2];
// int rptsc0_num, rptsc1_num;
// // 左右跟踪后的中线
// float (*rpts)[2];
// int rpts_num;
// // 归一化最终中线
// float rptsn[POINTS_MAX_LEN][2];
// int rptsn_num;

// // Y角点
// int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
// bool Ypt0_found, Ypt1_found;
// // L角点
// int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
// bool Lpt0_found, Lpt1_found;
// // 内L角点
// int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
// bool N_Lpt0_found, N_Lpt1_found;
// // 长直道
// bool is_straight0, is_straight1;
// // 弯道
// bool is_turn0, is_turn1;

// int N_Xfound_num;//面向赛道编程，双内L计数

// // 当前巡线模式（发车方向）
// enum track_type_e track_type = TRACK_RIGHT;

// float error[1];
// float ave_error;//速度控制输入变量
// // 多个预锚点位置
// int aim_idx[1] ;


// // 计算远锚点偏差值
// float dx[1] ;
// float dy[1] ;
// float dn[1] ;

// // 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
// // 计算近锚点偏差值
// float dx_near;
// float dy_near ;
// float dn_near ;
// float pure_angle;

// enum garage_type_e garage_type = GARAGE_NONE;//初始化为向左出库 调试状态为NONE

// /**
//  * 识别图像中的Y型和L型拐点，并进行二次检查。
//  * 
//  * @param sample_dist 采样距离
//  * @param angle_dist 角度距离
//  * @param pixel_per_meter 每米像素数
//  * @param garage_type 车库类型
//  * @param rpts0s_num 左边线点数
//  * @param rpts0s 左边线点坐标
//  * @param rpts0a 左边线点角度
//  * @param rpts0an 左边线点角度是否为峰值
//  * @param rpts1s_num 右边线点数
//  * @param rpts1s 右边线点坐标
//  * @param rpts1a 右边线点角度
//  * @param rpts1an 右边线点角度是否为峰值
//  * @param Ypt0_rpts0s_id 左边线Y型拐点点号
//  * @param Ypt1_rpts1s_id 右边线Y型拐点点号
//  * @param Lpt0_rpts0s_id 左边线L型拐点点号
//  * @param Lpt1_rpts1s_id 右边线L型拐点点号
//  * @param N_Lpt0_rpts0s_id 左边线内L型拐点点号
//  * @param N_Lpt1_rpts1s_id 右边线内L型拐点点号
//  * @param xielv_left_y_to_end 左边线Y型拐点到终点的斜率
//  * @param xielv_right_y_to_end 右边线Y型拐点到终点的斜率
//  */
// void find_corners()
// {
//     // 识别Y,L拐点
//     Ypt0_found = Ypt1_found = Lpt0_found = Lpt1_found = N_Lpt1_found = N_Lpt0_found = false;
//     is_straight0 = rpts0s_num > 1.6 / sample_dist;           //逆透视之后的边线个数大于80个就是直道,也就是边线边长大于0.8m,并不是直向前0.8m
//     is_straight1 = rpts1s_num > 1.6 / sample_dist;

//     //左边线角度
//     for (int i = 1; i < rpts0s_num; i++) {              //只要数组里的角度符合的，Y或L就为true,然后就不在继续检测Y或L
//         if (rpts0an[i] == 0) continue;                      //以下检测只检测峰值（极大值）
//         int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
//         int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
//         float conf = fabs(rpts0a[i]) - (fabs(rpts0a[im1]) + fabs(rpts0a[ip1])) / 2;

//         //Y角点阈值                 (30*  ,   50*)    并且距离在40个点之内 (注：30.转化浮点数)
//         if (Ypt0_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
//             Ypt0_rpts0s_id = i;
//             Ypt0_found = true;
//         }
//         //L角点阈值                 (55*  ,    140*)  并且距离在40个点之内 修改距离为1.2m增长判断路障距离
//         if (Lpt0_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {

//             Lpt0_rpts0s_id = i;
//             Lpt0_found = true;

//             if(rpts0a[Lpt0_rpts0s_id] < 0)              //这一个L点如果是向右拐的，就是内L点
//             {
//                 N_Lpt0_rpts0s_id = Lpt0_rpts0s_id;
//                 N_Lpt0_found = true;
// //                //对内L角的线截断
// //                rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
//                 Lpt0_found = false;
//             }

//         }
//         //长直道阈值
//         if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight0 = false;           //只要中间有大角度，就不是长直道
//         if (Ypt0_found == true && Lpt0_found == true && is_straight0 == false) break;
//     }

//     //右边线角度
//     for (int i = 1; i < rpts1s_num; i++) {
//         if (rpts1an[i] == 0) continue;
//         int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);
//         int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);
//         float conf = fabs(rpts1a[i]) - (fabs(rpts1a[im1]) + fabs(rpts1a[ip1])) / 2;
//         if (Ypt1_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
//             Ypt1_rpts1s_id = i;
//             Ypt1_found = true;
//         }
//         if (Lpt1_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {
//             Lpt1_rpts1s_id = i;
//             Lpt1_found = true;

//             if(rpts1a[Lpt1_rpts1s_id] > 0)              //这一个L点如果是向左拐的，就是内L点
//             {
//                 N_Lpt1_rpts1s_id = Lpt1_rpts1s_id;
//                 N_Lpt1_found = true;
// //                //对内L角的线截断
// //                rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
//                 Lpt1_found = false;
//             }
//         }

//         if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight1 = false;
//         if (Ypt1_found == true && Lpt1_found == true && is_straight1 == false) break;
//     }

//     // Y点二次检查,依据两角点距离及角点后张开特性
//     if (Ypt0_found && Ypt1_found) {
//         //==================================================================原检查
//         float dx = rpts0s[Ypt0_rpts0s_id][0] - rpts1s[Ypt1_rpts1s_id][0];
//         float dy = rpts0s[Ypt0_rpts0s_id][1] - rpts1s[Ypt1_rpts1s_id][1];
//         float dn = sqrtf(dx * dx + dy * dy);
//         if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)             //两点距离不能大于60cm  (赛道40cm)
//         {
//             float dwx = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                         rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//             float dwy = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                         rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//             float dwn = sqrtf(dwx * dwx + dwy * dwy);
//             if (!(dwn > 0.7 * pixel_per_meter &&                            //如两个Y点之后第50个 点之间的距离小于0.7m
//                   rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Ypt0_rpts0s_id][0] &&
//                   rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Ypt1_rpts1s_id][0]))
//             {
//                 Ypt0_found = Ypt1_found = false;
//             }
//         } else
//         {
//             Ypt0_found = Ypt1_found = false;
//         }
//         //zqkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk的检查

//         float dx_temp ,dy_temp ;



//         dx_temp = rpts0s[Ypt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
//         dy_temp = rpts0s[Ypt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
//         xielv_left_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


//         dx_temp = rpts1s[Ypt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
//         dy_temp = rpts1s[Ypt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
//         xielv_right_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


//     }
//     // L点二次检查，车库模式不检查, 依据L角点距离及角点后张开特性
//     if (garage_type == GARAGE_NONE) {
//         if (Lpt0_found && Lpt1_found) {
//             float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//             float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//             float dn = sqrtf(dx * dx + dy * dy);
//             if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)            //两点距离不能大于60cm  (赛道40cm)
//             {
//                 float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                             rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//                 float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                             rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//                 float dwn = sqrtf(dwx * dwx + dwy * dwy);
//                 if (!(dwn > 0.5 * pixel_per_meter &&                                //符合直角的特征必须50个点后距离大于0.7m，
//                       rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
//                       rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
//                 {
//                     Lpt0_found = Lpt1_found = false;
//                 }
//             } else
//             {
//                 Lpt0_found = Lpt1_found = false;
//             }

//             float dx_temp ,dy_temp ;



//             dx_temp = rpts0s[Lpt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
//             dy_temp = rpts0s[Lpt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
//             xielv_left_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子  一般是负


//             dx_temp = rpts1s[Lpt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
//             dy_temp = rpts1s[Lpt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
//             xielv_right_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


//         }
//     }

//     //左车库第二次检查
//     if (Lpt0_found && N_Lpt1_found) {                   //理论上是左车库
//         float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//         float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//         float dn = sqrtf(dx * dx + dy * dy);
//         if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //两点距离不能大于60cm  (赛道45cm)
//         {
//             float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                         rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//             float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                         rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//             float dwn = sqrtf(dwx * dwx + dwy * dwy);
//             if (!(dwn < 0.5 * pixel_per_meter &&                                //符合左车库特征必须50个点后距离小于0.5m，
//                   rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
//                   rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] < rpts1s[Lpt1_rpts1s_id][0]))
//             {
//                 Lpt0_found = N_Lpt1_found = false;
//             }
//         } else
//         {
//             Lpt0_found = N_Lpt1_found = false;
//         }
//     }


//     //右车库第二次检查
//     if (Lpt1_found && N_Lpt0_found) {                   //理论上是右车库
//         float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//         float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//         float dn = sqrtf(dx * dx + dy * dy);
//         if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //两点距离不能大于60cm  (赛道45cm)
//         {
//             float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                         rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//             float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                         rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//             float dwn = sqrtf(dwx * dwx + dwy * dwy);
//             if (!(dwn < 0.5 * pixel_per_meter &&                                //符合左车库特征必须50个点后距离小于0.5m，
//                   rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] > rpts0s[Lpt0_rpts0s_id][0] &&
//                   rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
//             {
//                 Lpt1_found = N_Lpt0_found = false;
//             }
//         } else
//         {
//             Lpt1_found = N_Lpt0_found = false;
//         }
//     }
// }


// /**
//  * @brief 透视变换函数，将提取出来的两条边线进行透视变换
//  * 
//  * @param pts_in 二维数组，表示输入的点集，每个点有两个坐标值
//  * @param int_num 整型，表示输入的点集中点的数量
//  * @param pts_out 二维数组，表示输出的点集，每个点有两个坐标值
//  */
// void Pespective(int pts_in[][2],int int_num ,  float pts_out[][2])
// //带入提取出来的两条边线，得到透视变换后的两条边线
// {
//     float x, y, w;
//     for (int16_t i = 0; i < int_num; i++)
//     {
//         x = getx((pts_in[i][0]), (pts_in[i][1]));
//         y = gety((pts_in[i][0]), (pts_in[i][1]));
//         w = getw((pts_in[i][0]), (pts_in[i][1]));
//         pts_out[i][0] = x / w;
//         pts_out[i][1] = y / w;
//     }
// 	int i;
// 	for(i=0;i<int_num;i++)
// 	{
// 	ips200_draw_point(pts_out[i][0]+20,pts_out[i][1]+20,RGB565_RED);
// 	}
// //	for(i=0;i<int_num;i++)
// //	{
// //	ips200_draw_line(0,0,pts_in[i][0]+5,pts_in[i][1],RGB565_RED);
// //	}
// 	ips200_show_uint(43,200,int_num,4);
// //	ips200_show_uint(43,160,pts_out[int_num-2][1],4);
// //	ips200_show_uint(43,180,pts_out[int_num-1][1],4);
// //	ips200_show_uint(43,140,pts_out[int_num-2][0],4);
// //	ips200_show_uint(3,180,pts_out[int_num-3][1],4);
// //	ips200_show_uint(3,200,pts_out[int_num-3][0],4);
// }

// /*

// */
// //void Normalization(float in[][2],int16 num,float out[][2])
// //{
// //    for (int i = 0; i < rpts_num; i++) 
// //    {
// //        float dx = rpts[i][0] - cx;
// //        float dy = rpts[i][1] - cy;//求出每个点到x,y变化值
// //        //float dist = mySqrt(dx * dx + dy * dy);
// //        float dist = sqrt(dx * dx + dy * dy);
// //        //计算rpts上每一点到起始点cx,cy的距离
// //        if (dist < min_dist)//如果当前点到起始点的距离小于已知的最小距离，则更新最小距离和最近点的下标索引
// //        {
// //            min_dist = dist;
// //            begin_id = i;
// //        }
// //    }
// //    // 中线有点，同时最近点不是最后几个点
// //    if (begin_id >= 0 && rpts_num - begin_id >= 3)//这个if条件是用来判断中线是否有足够的点数，同时最近点不是最后几个点
// //    {
// //        // 归一化中线
// //        rpts[begin_id][0] = cx;
// //        rpts[begin_id][1] = cy;//选定起始点
// //        rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);//sizeof(rptsn) 表示 rptsn 数组的总字节数，sizeof(rptsn[0]) 表示 rptsn 数组中每个元素的字节数
// //        resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);
// //         aim_idx[0] = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);


// //        // 计算远锚点偏差值      cx，cy为摄像头坐标距重心坐标距离 0,-7
// //        dx[0] = rptsn[aim_idx[0]][0] - cx;
// //        dy[0] = rptsn[aim_idx[0]][1] - cy;
// //        dn[0] = sqrt(dx[0] * dx[0] + dy[0] * dy[0]);
// //        //dn = mySqrt(dx * dx + dy * dy);
// //        //error<0(输出为正) 左 error>0(输出为负) 右 以下为角度制换算为弧度制
// //        error[0] = atan2f(dx[0], dy[0]) * 180 / PI;//1弧度=180/π度 1度=π/180弧度
// //        //assert(!isnan(error));



// //        ave_error = 0;
// //        // 远近锚点综合考虑
// //        //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
// //        // 根据偏差进行PD计算
// //        //float angle = pid_solve(&servo_pid, error);

// //        // 纯跟踪算法(只考虑远点)三轮控制用不到
// ////        pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / 1;
// //      } else {
// //          // 中线点过少(出现问题)，则不控制舵机
// //          rptsn_num = 0;
// //      }

// //     end = system_getval_us();/*************************************************************************
//  *  函数名称：void blur_points();
//  *  功能说明：点集三角滤波
//  *  参数说明：输入边线数组pits_in[][2]，边线总个数num，输出边线数组pts_out，点集范围kernel
//  *  函数返回：无
//  *  修改时间：2022年10月4日
//  *  备    注：这个函数中的kernel参数是用来指定点集三角滤波的范围的。具体来说，它是一个奇数，表示以每个点为中心的滤波器的大小。
//  *  如果kernel为5，则每个点周围的5个点将被用于计算平均值，以平滑该点。kernel越大，平滑效果越明显，但也会导致失去一些细节
//  *  例如：kernel = 5
//  *  xi = (0*xi-3 + 1*xi-2 + 2*xi-1 + 3*xi + 2*xi+1 + 1*xi+2 + 0*xi+3)/9
//  *  yi 同理
//  *************************************************************************/
// void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel)
// {
//     zf_assert(kernel % 2 == 1);//检查变量 kernel 是否为奇数
//     int half = kernel / 2;//用于计算滤波器的大小
//     for (int i = 0; i < num; i++) //对于每个点，都会计算它周围的点的加权平均值，并将结果存储在输出点集数组中
//     {
//         pts_out[i][0] = pts_out[i][1] = 0;
//         for (int j = -half; j <= half; j++) {
//             pts_out[i][0] += (float)pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
//             pts_out[i][1] += (float)pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
//         }
//         pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
//         pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
//     }
// }
// /*************************************************************************
//  *  函数名称：void resample_points（）;
//  *  功能说明：点集等距采样
//  *  参数说明：输入边线数组pts_in[][2]，输入原边线总个数num1，输出边线数组pts_out[][2]，获取输出边线数组个数num2，采样距离dist
//  *  函数返回：无
//  *  修改时间：2022年10月27日
//  *  备    注：使走过的采样前折线段的距离为`dist`
//  *            这个函数的作用是对输入的点集进行等距采样处理，输出采样后的点集。
//  *            函数的实现过程是对输入点集中的每个线段进行等距采样，采样后的点集存储在输出点集中。
//  *************************************************************************/
// void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist)
// {
//     int remain = 0, len = 0;//下一次采样折线段距离
//     for(int i=0; i<num1-1 && len < *num2; i++){
//         float x0 = pts_in[i][0];
//         float y0 = pts_in[i][1];
//         float dx = pts_in[i+1][0] - x0;
//         float dy = pts_in[i+1][1] - y0;
//         float dn = mySqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
//         //float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
//         dx /= dn;//此点处的cosθ
//         dy /= dn;//此点处的sinθ

//         //每次等距采样处理
//         while(remain < dn && len < *num2){
//             x0 += dx * remain;
//             pts_out[len][0] = x0;
//             y0 += dy * remain;
//             pts_out[len][1] = y0;

//             len++;
//             dn -= remain;
//             remain = dist;
//         }
//         remain -= dn;//当跨越一点采样折线距离近似直线
//     }
//     *num2 = len;
// }

// 角点提取&筛选 Y角点用于检测坡道，L角点用于检测圆环、十字、路障、断路
// //     //timer_cost = end - start;                              //处理每一帧的时间

// //}