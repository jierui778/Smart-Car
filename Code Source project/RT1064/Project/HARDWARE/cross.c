// void check_cross(void) {


//     //双边确定十字
//     bool Xfound = Lpt0_found && Lpt1_found;
//     if (cross_type == CROSS_NONE && Xfound &&  ( -(xielv_left_y_to_end) + xielv_right_y_to_end <1 )
//             /*&& Z.all_length  > 1*ENCODER_PER_METER*/ )
//     {
//         cross_encoder = Z.all_length ;
//         cross_type = CROSS_BEGIN;
//         cross_num ++;
//     }


// }



// void run_cross(void) {
//     bool Xfound = Lpt0_found && Lpt1_found;
//     int64_t current_encoder = Z.all_length;

//     //检测到十字，先按照近线走
//     if (cross_type == CROSS_BEGIN) {
//         //对近处线截断处理
//         if (Lpt0_found) {
//             rptsc0_num = rpts0s_num = Lpt0_rpts0s_id;
//         }
//         if (Lpt1_found) {
//             rptsc1_num = rpts1s_num = Lpt1_rpts1s_id;
//         }

//         aim_distance = 0.4;//大概在rpts_id=20
//         //近角点过少，进入远线控制 Lpt0_found || Lpt1_found
//         if ((Xfound && (Lpt0_rpts0s_id < 23 || Lpt1_rpts1s_id < 23))/* || (rpts1_num <30 && rpts0_num<30)*/)
//         {
//             cross_type = CROSS_IN_DOUBLE; //双边L十字


//             cross_encoder = current_encoder;
//         }
//         else if(Lpt0_found && !Lpt1_found && Lpt0_rpts0s_id < 23 /*&& rpts0s_num > rpts1s_num/2*/)
// //        if(Lpt0_found /*&& !Lpt1_found*/ && Lpt0_rpts0s_id < 23)
//         {
//             cross_type = CROSS_INHALF_LEFT; //单边左L十字
//             cross_encoder = current_encoder;
//         }
//         else if(Lpt1_found && !Lpt0_found && Lpt1_rpts1s_id < 23)
//         {
//             cross_type = CROSS_INHALF_RIGHT; //单边右L十字
//             cross_encoder = current_encoder;
//         }
//     }
//         //远线控制进十字,begin_y渐变靠近防丢线
//     if (cross_type == CROSS_IN_DOUBLE) {
//         //寻远线,算法与近线相同
//         cross_farline();

//         if (rpts1s_num < 20 && rpts0s_num < 20) { not_have_line++; }
//         //当找到近处线，跳出十字状态
//         if (not_have_line > 2 && (rpts1s_num > 40 || rpts0s_num > 40)) {
//             cross_type = CROSS_NONE;
//             not_have_line = 0;
// //            Z.Stop = 1;
//         }

//         if(current_encoder - cross_encoder > 2.0*ENCODER_PER_METER)
//         {
//             cross_type = CROSS_NONE;//跑过1m强制退出
//         }

//         aim_distance = 0.4;

//         if (far_Lpt1_found) { track_type = TRACK_RIGHT; }
//         else if (far_Lpt0_found) { track_type = TRACK_LEFT; }
//         else if(far_rpts0s_num > far_rpts1s_num)track_type = TRACK_LEFT;
//         else if(far_rpts1s_num > far_rpts0s_num)track_type = TRACK_RIGHT;
//         else if (not_have_line > 0 && rpts1s_num < 5) { track_type = TRACK_RIGHT; }
//         else if (not_have_line > 0 && rpts0s_num < 5) { track_type = TRACK_LEFT; }

//     }
//     if(cross_type == CROSS_INHALF_LEFT)
//     {
//         //寻远线，与单边检测部分相同
//         if ( Lpt0_found   )                 //左边近的L点存在
//         {
//             Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &L_x0 , &L_y0 );

//             xielv_left = (float) ( L_x0 - x0_first ) * 1.0  / (L_y0 -  y0_first);     //直着约为-0.4，斜着约为-1


//         }
//         else
//         {
//             L_x0 = 10;
//             L_y0 = 53;//应大于60-19

//         }

//         //平移一段距离
//         begin_x0 = func_limit_ab( L_x0 - 2 , 2, 78 );
//         begin_y0 = func_limit_ab ( (int)(L_y0 - 4 + 5*xielv_left) , 2, 58 );

//         far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);

//         for (; begin_y0 > 0; begin_y0--) {
//             //先白后黑，先找黑边界

//             int x = ( begin_y0 - y0_first  )* (xielv_left/2) + x0_first ;      //按斜率搜
// //            int x = begin_x0; //直接向上搜


//             if (AT_IMAGE(&img_raw, x, begin_y0-1) < thres) {
//                 far_x0 = x;
//                 far_y0 = begin_y0;    //这一点是白点，左边为黑点
//                 break;
//             }


//         }

//         Left_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x0, far_y0, far_ipts0, &far_ipts0_num);

//         //对边线透视变换
//         Pespective(far_ipts0,far_ipts0_num ,  far_rpts0);
//         far_rpts0_num = far_ipts0_num;
//         //边线三角滤波
//         blur_points(far_rpts0,far_rpts0_num,far_rpts0b,(int) round(line_blur_kernel));
//         far_rpts0b_num = far_rpts0_num;
//         // 边线等距采样
//         far_rpts0s_num = sizeof(far_rpts0s) / sizeof(far_rpts0s[0]);
//         resample_points(far_rpts0b, far_rpts0b_num, far_rpts0s, &far_rpts0s_num, sample_dist * pixel_per_meter);



//         if ( rpts0s_num < 20) { not_have_line++; }
//         //当找到近处线，跳出十字状态
//         if (not_have_line > 2 && rpts0s_num > 40  ) {
//             cross_type = CROSS_NONE;
//             not_have_line = 0;
// //            Z.Stop = 1;
//         }
//         if(current_encoder - cross_encoder > 2.0*ENCODER_PER_METER)
//         {
//             cross_type = CROSS_NONE;//跑过2m强制退出
//         }
//         aim_distance = 0.4;

//         track_type = TRACK_LEFT;

//     }
//     if(cross_type == CROSS_INHALF_RIGHT)
//     {
//         //寻远线，与单边检测部分相同
//         if ( Lpt1_found   )                 //左边近的L点存在
//         {
//             Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &L_x1 , &L_y1 );

//             xielv_right = (float) ( L_x1 - x1_first ) * 1.0  / (L_y1 -  y1_first);     //直走约为0.4，斜入约为1


//         }
//         else
//         {
//             L_x1 = 69;
//             L_y1 = 53;//应大于60-19

//         }

//         //平移一段距离右10上5
//         begin_x1 = func_limit_ab( L_x1 + 2 , 2, 78 );
//         begin_y1 = func_limit_ab ((int)(L_y1 - 4 - 5*xielv_right) , 2, 58 );

//         for (; begin_y1 > 0; begin_y1--) {
//             //先白后黑，先找黑边界

// //            int x = ( begin_y1 - y1_first  )* (xielv_right/2) +  x1_first ; //按斜率搜
//             int x = begin_x1;//直接向上搜

//             if (AT_IMAGE(&img_raw, x, begin_y1-1) < thres) {
//                 far_x1 = x;
//                 far_y1 = begin_y1;    //这一点是白点，左边为黑点
//                 break;
//             }


//         }
//         Right_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x1, far_y1, far_ipts1, &far_ipts1_num);

//         //对边线透视变换
//         Pespective(far_ipts1,far_ipts1_num ,  far_rpts1);
//         far_rpts1_num = far_ipts1_num;
//         //边线三角滤波
//         blur_points(far_rpts1,far_rpts1_num,far_rpts1b,(int) round(line_blur_kernel));
//         far_rpts1b_num = far_rpts1_num;
//         // 边线等距采样
//         far_rpts1s_num = sizeof(far_rpts1s) / sizeof(far_rpts1s[0]);
//         resample_points(far_rpts1b, far_rpts1b_num, far_rpts1s, &far_rpts1s_num, sample_dist * pixel_per_meter);

//         if ( rpts1s_num < 20) { not_have_line++; }
//         //当找到近处线，跳出十字状态
//         if (not_have_line > 2 && rpts1s_num > 40  ) {
//             cross_type = CROSS_NONE;
//             not_have_line = 0;
// //            Z.Stop = 1;
//         }
//         if(current_encoder - cross_encoder > 2.0*ENCODER_PER_METER)
//         {
//             cross_type = CROSS_NONE;//跑过1m强制退出
//         }
//         aim_distance = 0.4;

//         track_type = TRACK_RIGHT;

//     }
// }

// //双边寻远线
// void cross_farline()
// {
//     if ( Lpt0_found   )                 //左边近的L点存在
//     {
//         Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &L_x0 , &L_y0 );
//         Pespective_anti(rpts0s[Lpt0_rpts0s_id / 2][0]  ,  rpts0s[Lpt0_rpts0s_id / 2][1] , &low_x0 , &low_y0 );

//         xielv_left = (float) ( L_x0 - x0_first ) * 1.0  / (L_y0 -  y0_first);     //一般情况符号为    +  / -   最后是负


//     }
//     else
//     {
//         L_x0 = 5 ;
//         L_y0 = 50;

//     }


//     if ( Lpt1_found   )
//     {
//         Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &L_x1 , &L_y1 );
//         Pespective_anti(rpts1s[Lpt1_rpts1s_id / 2][0]  ,  rpts1s[Lpt1_rpts1s_id / 2][1] , &low_x1 , &low_y1 );
//         xielv_right = (float)  ( L_x1 - x1_first ) * 1.0  / (L_y1 -  y1_first);     //一般情况符号为    -  / -   最后是正
//     }
//     else
//     {
//         L_x1 = 75;
//         L_y1 = 50;

//     }



//     begin_x0 = func_limit_ab( L_x0 - 2 , 2, 78 );
//     begin_y0 = func_limit_ab ( L_y0 - 2 , 2, 58 );

//     begin_x1 = func_limit_ab ( L_x1 + 2, 2, 78 );
//     begin_y1 = func_limit_ab (L_y1 - 2 ,2, 58 );     //开始点

//     // 这里应该是在近的L点两侧开始，而不是固定，因为极大是斜入十字

//         if ( Lpt0_found   )                 //左边近的L点存在，先找左边，这两个的目的是先搜能找到近L点的边，
//         {
//             far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);

//             //全白 从 far_x1 , begin_y,从边界找
//             for (; begin_y0 > 0; begin_y0--) {
//                 //先白后黑，先找黑边界

//                 int x = ( begin_y0 - y0_first  )* xielv_left +  x0_first ;           // x =  delta_y * xielv  +  x_begin

//                 int temp = x + 3;
//                 for ( ; temp  > x  - 5; temp--)
//                 {
//                     if (AT_IMAGE(&img_raw, temp, begin_y0-1) < thres) {
//                         far_x0 = temp;
//                         far_y0 = begin_y0;    //这一点是白点，上边为黑点
//                         goto out0;
//                     }
//                 }

//             }
//         out0:
//             //从找到角点位置开始寻找
// //                if (AT_IMAGE(&img_raw, far_x0, far_y0 + 1) >= thres)
//         Left_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x0, far_y0 , far_ipts0, &far_ipts0_num);
// //                else far_ipts0_num = 0;

//         }

//         if ( Lpt1_found   )                 //右边近的L点存在，这两个的目的是先搜能找到近L点的边，
//         {
//             far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);


//             for (; begin_y1 > 0; begin_y1--) {

//                 int x = ( begin_y1 - y1_first  )* xielv_right +  x1_first ;           // x =  delta_y * xielv  +  x_begin

//                 int temp = x - 3;

//                 for (; temp  < x + 3 ; temp++){
//                     //先黑后白，先找white
//                     if (AT_IMAGE(&img_raw, temp, begin_y1-1) <thres) {
//                         far_x1 = temp;
//                         far_y1 = begin_y1; //这一点是白点，上边为黑点
//                         goto out1;
//                     }

//                 }

//             }
//             out1:
//                     //从找到角点位置开始寻找
// //                        if (AT_IMAGE(&img_raw, far_x1, far_y1 + 1) == thres)
//             Right_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x1, far_y1 , far_ipts1, &far_ipts1_num);
// //                        else far_ipts1_num = 0;
//         }
//         if( !Lpt0_found && !Lpt1_found)                 //如果两个近L点都没找到，那么就从起始点向上搜
//         {
//             far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);


//             //全白 从 far_x1 , begin_y,从边界找
//             for (; begin_y0 > 0; begin_y0--) {
//                 //先白后黑，先找黑边界
//                 if (AT_IMAGE(&img_raw, begin_x0, begin_y0-1) < thres) {
//                     far_x0 = begin_x0;
//                     far_y0 = begin_y0;    //这一点是白点，上面是黑点
//                     break;
//                 }
//             }

//                         //从找到角点位置开始寻找
// //                        if (AT_IMAGE(&img_raw, far_x0, far_y0 + 1) >= thres)
//             Left_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x0, far_y0, far_ipts0, &far_ipts0_num);
// //                        else far_ipts0_num = 0;


//             far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);


//                for (; begin_y1 > 0; begin_y1--) {
//                    //先黑后白，先找white
//                    if (AT_IMAGE(&img_raw, begin_x1, begin_y1-1) <thres) {
//                        far_x1 = begin_x1;
//                        far_y1 = begin_y1;  //这一点是白点，上面是黑点
//                        break;
//                    }
//                }

//                            //从找到角点位置开始寻找
// //                           if (AT_IMAGE(&img_raw, far_x1, far_y1 + 1) == thres)
//              Right_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x1, far_y1, far_ipts1, &far_ipts1_num);
// //                           else far_ipts1_num = 0;

//         }


//         far_Lpt0_rpts0s_id = 5 , far_Lpt1_rpts1s_id = 5 ;             //每次如果没找到远方的L点，默认是第5个点


//         // 去畸变+透视变换

//         Pespective(far_ipts0,far_ipts0_num ,  far_rpts0);
//         far_rpts0_num = far_ipts0_num;

//         Pespective(far_ipts1,far_ipts1_num ,  far_rpts1);
//         far_rpts1_num = far_ipts1_num;


//         // 边线滤波
//         blur_points(far_rpts0, far_rpts0_num, far_rpts0b, (int) round(line_blur_kernel));
//         far_rpts0b_num = far_rpts0_num;
//         blur_points(far_rpts1, far_rpts1_num, far_rpts1b, (int) round(line_blur_kernel));
//         far_rpts1b_num = far_rpts1_num;

//         // 边线等距采样
//         far_rpts0s_num = sizeof(far_rpts0s) / sizeof(far_rpts0s[0]);
//         resample_points(far_rpts0b, far_rpts0b_num, far_rpts0s, &far_rpts0s_num, sample_dist * pixel_per_meter);
//         far_rpts1s_num = sizeof(far_rpts1s) / sizeof(far_rpts1s[0]);
//         resample_points(far_rpts1b, far_rpts1b_num, far_rpts1s, &far_rpts1s_num, sample_dist * pixel_per_meter);

// //        // 边线局部角度变化率
// //        local_angle_points(far_rpts0s, far_rpts0s_num, far_rpts0a, (int) round(angle_dist / sample_dist));
// //        far_rpts0a_num = far_rpts0s_num;
// //        local_angle_points(far_rpts1s, far_rpts1s_num, far_rpts1a, (int) round(angle_dist / sample_dist));
// //        far_rpts1a_num = far_rpts1s_num;
// //
// //        // 角度变化率非极大抑制
// //        nms_angle(far_rpts0a, far_rpts0a_num, far_rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //        far_rpts0an_num = far_rpts0a_num;
// //        nms_angle(far_rpts1a, far_rpts1a_num, far_rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
// //        far_rpts1an_num = far_rpts1a_num;


// }

// //十字补线方案 舍弃
// //思路：找到L角点沿斜率向上搜线，找到第二个近似L角点（非角点计算得到的），补线，然后用正常扫线的数组再执行一边迷宫+逆透视...
// void cross_drawline()
// {
//     if ( Lpt0_found   )                 //左边近的L点存在
//     {
//         Pespective_anti(rpts0s[Lpt0_rpts0s_id][0]  ,  rpts0s[Lpt0_rpts0s_id][1] , &L_x0 , &L_y0 );
//         Pespective_anti(rpts0s[Lpt0_rpts0s_id / 2][0]  ,  rpts0s[Lpt0_rpts0s_id / 2][1] , &low_x0 , &low_y0 );

//         xielv_left = (float) ( L_x0 - x0_first ) * 1.0  / (L_y0 -  y0_first);     //一般情况符号为    +  / -   最后是负


//     }
//     else
//     {
//         L_x0 = 24 ;
//         L_y0 = 29;

//     }


//     if ( Lpt1_found   )
//     {
//         Pespective_anti(rpts1s[Lpt1_rpts1s_id][0]  ,  rpts1s[Lpt1_rpts1s_id][1] , &L_x1 , &L_y1 );
//         Pespective_anti(rpts1s[Lpt1_rpts1s_id / 2][0]  ,  rpts1s[Lpt1_rpts1s_id / 2][1] , &low_x1 , &low_y1 );
//         xielv_right = (float)  ( L_x1 - x1_first ) * 1.0  / (L_y1 -  y1_first);     //一般情况符号为    -  / -   最后是正
//     }
//     else
//     {
//          L_x1 = 56;
//          L_y1 = 29;

//     }



//     begin_x0 = func_limit_ab( L_x0 - 2 , 2, 78 );
//     begin_y0 = func_limit_ab ( L_y0 - 5 , 2, 58 );

//     begin_x1 = func_limit_ab ( L_x1 + 2, 2, 78 );
//     begin_y1 = func_limit_ab (L_y1 - 5 ,2, 58 );     //开始点


//     if ( Lpt0_found   )                 //左边近的L点存在，先找左边，这两个的目的是先搜能找到近L点的边，
//     {
// //                far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);

//         //全白 从 far_x1 , begin_y,从边界找
//         for (; begin_y0 > 0; begin_y0--) {
//             //先白后黑，先找黑边界

//             int x = ( begin_y0 - y0_first  )* xielv_left +  x0_first ;           // x =  delta_y * xielv  +  x_begin

//             int temp = x + 3;
//             for ( ; temp  > x  - 5; temp--)
//             {
//                 if (AT_IMAGE(&img_raw, temp, begin_y0-1) < thres) {
//                     far_x0 = temp;
//                     far_y0 = begin_y0;    //这一点是白点，上边为黑点
//                     goto out0;
//                 }
//             }

//         }
//         out0:
//         //从找到角点位置开始寻找
// //                if (AT_IMAGE(&img_raw, far_x0, far_y0 + 1) >= thres)
//         Left_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x0, far_y0 , far_ipts0, &far_ipts0_num);
// //                else far_ipts0_num = 0;
//         //补线
//         drawline0[0] = begin_x0;        drawline0[1] = begin_y0;
//         drawline1[0] = far_x0;        drawline1[1] = far_y0;
//         if(ipts0_num < 25)//待测
//         {
//             drawline0[0] = x0_first;        drawline0[1] = y0_first;
//         }
//         draw_line(&img_raw, drawline0, drawline1 ,0);

//     }

//     if ( Lpt1_found   )                 //右边近的L点存在，这两个的目的是先搜能找到近L点的边，
//     {
// //                far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);


//         for (; begin_y1 > 0; begin_y1--) {

//             int x = ( begin_y1 - y1_first  )* xielv_right +  x1_first ;           // x =  delta_y * xielv  +  x_begin

//             int temp = x - 3;

//             for (; temp  < x + 3 ; temp++){
//                 //先黑后白，先找white
//                 if (AT_IMAGE(&img_raw, temp, begin_y1-1) <thres) {
//                     far_x1 = temp;
//                     far_y1 = begin_y1; //这一点是白点，上边为黑点
//                     goto out1;
//                 }

//             }

//         }
//         out1:
//                 //从找到角点位置开始寻找
// //                        if (AT_IMAGE(&img_raw, far_x1, far_y1 + 1) == thres)
//         Right_Adaptive_Threshold(&img_raw, block_size, clip_value, far_x1, far_y1 , far_ipts1, &far_ipts1_num);
// //                        else far_ipts1_num = 0;
//         //补线
//         drawline0[0] = begin_x1;        drawline0[1] = begin_y1;
//         drawline1[0] = far_x1;        drawline1[1] = far_y1;
//         if(ipts1_num < 25)//待测
//         {
//             drawline0[0] = x1_first;        drawline0[1] = y1_first;
//         }
//         draw_line(&img_raw, drawline0, drawline1 ,0);
//     }
//     if( !Lpt0_found && !Lpt1_found)                 //如果两个近L点都没找到，那么就从起始点向上搜
//     {
// //                far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);


//         //全白 从 far_x1 , begin_y,从边界找
//         for (; begin_y0 > 0; begin_y0--) {
//             //先白后黑，先找黑边界
//             if (AT_IMAGE(&img_raw, begin_x0, begin_y0-1) < thres) {
//                 far_x0 = begin_x0;
//                 far_y0 = begin_y0;    //这一点是白点，上面是黑点
//                 break;
//             }
//         }
//         //补线
//         drawline0[0] = begin_x0;        drawline0[1] = begin_y0;
//         drawline1[0] = far_x0;        drawline1[1] = far_y0;
//         if(ipts0_num < 25)
//         {
//             drawline0[0] = x0_first;        drawline0[1] = y0_first;
//         }
//         draw_line(&img_raw, drawline0, drawline1 ,0);

//                     //从找到角点位置开始寻找
// //                        if (AT_IMAGE(&img_raw, far_x0, far_y0 + 1) >= thres)
//         Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);
// //                        else far_ipts0_num = 0;


//         far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);


//         for (; begin_y1 > 0; begin_y1--) {
//             //先黑后白，先找white
//             if (AT_IMAGE(&img_raw, begin_x1, begin_y1-1) <thres) {
//                 far_x1 = begin_x1;
//                 far_y1 = begin_y1;  //这一点是白点，上面是黑点
//                 break;
//             }
//         }
//         //补线
//         drawline0[0] = begin_x1;        drawline0[1] = begin_y1;
//         drawline1[0] = far_x1;        drawline1[1] = far_y1;
//         if(ipts1_num < 25)
//         {
//             drawline0[0] = x1_first;        drawline0[1] = y1_first;
//         }
//         draw_line(&img_raw, drawline0, drawline1 ,0);
//                        //从找到角点位置开始寻找
// //                           if (AT_IMAGE(&img_raw, far_x1, far_y1 + 1) == thres)
//         Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts1, &ipts1_num);
// //                           else far_ipts1_num = 0;

//     }

//         //对边线透视变换
//         Pespective(ipts0,ipts0_num ,  rpts0);
//         rpts0_num = ipts0_num;
//         Pespective(ipts1,ipts1_num ,  rpts1);
//         rpts1_num = ipts1_num;

//         //边线三角滤波
//         blur_points(rpts0,rpts0_num,rpts0b,(int) round(line_blur_kernel));
//         rpts0b_num = rpts0_num;
//         blur_points(rpts1,rpts1_num,rpts1b,(int) round(line_blur_kernel));
//         rpts1b_num = rpts1_num;

//         // 边线等距采样 num为逆透视后实际距离点，两点间距离为0.02*102=2cm
//         rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);//求数组的长度 即等距采样后边线点个数
//         resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
//         rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
//         resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);

//         // 边线局部角度变化率
//         local_angle_points(rpts0s, rpts0s_num, rpts0a, (int) round(angle_dist / sample_dist));
//         rpts0a_num = rpts0s_num;
//         local_angle_points(rpts1s, rpts1s_num, rpts1a, (int) round(angle_dist / sample_dist));
//         rpts1a_num = rpts1s_num;

//         // 角度变化率非极大抑制
//         nms_angle(rpts0a, rpts0a_num, rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);
//         rpts0an_num = rpts0a_num;
//         nms_angle(rpts1a, rpts1a_num, rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
//         rpts1an_num = rpts1a_num;

//         // 左右中线跟踪
//         track_leftline(rpts0s, rpts0s_num, rptsc0, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//         rptsc0_num = rpts0s_num;
//         track_rightline(rpts1s, rpts1s_num, rptsc1, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//         rptsc1_num = rpts1s_num;


// }