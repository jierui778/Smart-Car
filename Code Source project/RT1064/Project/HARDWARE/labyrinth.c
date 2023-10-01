///*
// * Image_Control.c
// *
// *  Created on: 2022年8月27日
// *      Author:  HQC
// */
//#include "string.h"
//#include "zf_common_headfile.h"
//#include "math.h"
//#include "labyrinth.h"
//#define AT                       AT_IMAGE
//#define AT_CLIP(img, x, y)      AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1));

//uint8_t threshold;//大津法阈值

//float aim_distance;

//int drawline0[2];//补线起始点坐标
//int drawline1[2];//补线终点坐标

//float N_drawline0[2];//补线起始点坐标
//float N_drawline1[2];//补线终点坐标

////逆透视补线数组
//float left_line[POINTS_MAX_LEN][2];
//float right_line[POINTS_MAX_LEN][2];
//int left_num, right_num;
////拼接数组
//float splicing_leftline[POINTS_MAX_LEN][2];
//float splicing_rightline[POINTS_MAX_LEN][2];
//int splicing_leftline_num,splicing_rightline_num;
////拼接数组平移中线
//float splicing_leftline_center[POINTS_MAX_LEN][2];
//float splicing_rightline_center[POINTS_MAX_LEN][2];
//int splicing_leftline_center_num,splicing_rightline_center_num;

//uint8  Image_Use[UROW][UCOL];//灰度图像5k
//uint8_t  Image_Use_Robert[UROW][UCOL];//二值化图像

////float pix[2];//线性插值

//image_t img_raw = DEF_IMAGE(NULL, UCOL, UROW);

////左右边丢线
//uint8 loseline0;
//uint8 loseline1;

//uint8 touch_boundary0;              //左边线走到图像左边界
//uint8 touch_boundary1;              //右边线走到图像右边界

//uint8 touch_boundary_up0;              //左边线走到图像上边界
//uint8 touch_boundary_up1;              //右边线走到图像上边界

//int begin_x0,begin_y0;              //找线偏移点
//int begin_x1,begin_y1;              //找线偏移点

//float xielv_left_y_to_end,xielv_right_y_to_end;                 //在逆透视后得坐标系建得斜率
////初始边线数据
//int ipts0[POINTS_MAX_LEN][2];//存放边线数据（左）
//int ipts1[POINTS_MAX_LEN][2];//存放边线数据（右）
//int ipts0_num;//存放边线像素点个数(左)
//int ipts1_num;//存放边线像素点个数(右)
//// 变换后左右边线
//float rpts0[POINTS_MAX_LEN][2];
//float rpts1[POINTS_MAX_LEN][2];
//int rpts0_num, rpts1_num;
//// 变换后左右边线+滤波
//float rpts0b[POINTS_MAX_LEN][2];
//float rpts1b[POINTS_MAX_LEN][2];
//int rpts0b_num, rpts1b_num;
//// 变换后左右边线+等距采样
//float rpts0s[POINTS_MAX_LEN][2];
//float rpts1s[POINTS_MAX_LEN][2];
//int rpts0s_num, rpts1s_num;
//// 左右边线局部角度变化率
//float rpts0a[POINTS_MAX_LEN];
//float rpts1a[POINTS_MAX_LEN];
//int rpts0a_num, rpts1a_num;
//// 左右边线局部角度变化率+非极大抑制
//float rpts0an[POINTS_MAX_LEN];
//float rpts1an[POINTS_MAX_LEN];
//int rpts0an_num, rpts1an_num;
//// 左/右中线
//float rptsc0[POINTS_MAX_LEN][2];
//float rptsc1[POINTS_MAX_LEN][2];
//int rptsc0_num, rptsc1_num;
//// 左右跟踪后的中线
//float (*rpts)[2];
//int rpts_num;
//// 归一化最终中线
//float rptsn[POINTS_MAX_LEN][2];
//int rptsn_num;

//// Y角点
//int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
//bool Ypt0_found, Ypt1_found;
//// L角点
//int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
//bool Lpt0_found, Lpt1_found;
//// 内L角点
//int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
//bool N_Lpt0_found, N_Lpt1_found;
//// 长直道
//bool is_straight0, is_straight1;
//// 弯道
//bool is_turn0, is_turn1;

//int N_Xfound_num;//面向赛道编程，双内L计数

//// 当前巡线模式（发车方向）
//enum track_type_e track_type = TRACK_RIGHT;

//float error[1];
//float ave_error;//速度控制输入变量
//// 多个预锚点位置
//int aim_idx[1] ;


//// 计算远锚点偏差值
//float dx[1] ;
//float dy[1] ;
//float dn[1] ;


//// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
//// 计算近锚点偏差值
//float dx_near;
//float dy_near ;
//float dn_near ;
//float pure_angle;

////image_t img_change = DEF_IMAGE(NULL, RESULT_COL, RESULT_ROW);//定义逆透视后图像对象（暂时未赋值数据）
////uint8_t img_thres_data[RESULT_COL][RESULT_ROW];//二值化图像数据
////image_t img_thres = DEF_IMAGE((uint8_t *) img_thres_data, RESULT_COL, RESULT_ROW);


////图像处理总函数
//void process_image(void)
//{
//    int start,end,start_last;
//    start = system_getval_us();

//    //图像处理方案选择
//    /****
//    //==========方案三 更改三角滤波输入数组参数类型float&int
//    compressimage(); //图像压缩,并将生图赋给Image_Use,并从这里开始采集下一帧
//    S_Sobel( Image_Use, Image_Use_Robert ,thres);//全局Sobel得二值图

//    img_raw.data = Image_Use_Robert;

//    Find_Borderline();//获取图片的边线
//    //对边线方案三的打表变换
//    for (int i = 0; i < ipts0_num; i++) {
//        rpts0[i][0] = x_data[ipts0[i][0]+UCOL*ipts0[i][1]];
//        rpts0[i][1] = y_data[ipts0[i][1]];
//    }
//    rpts0_num = ipts0_num;
//    for (int i = 0; i < ipts1_num; i++) {
//        rpts1[i][0] = x_data[ipts1[i][0]+UCOL*ipts1[i][1]];
//        rpts1[i][1] = y_data[ipts1[i][1]];
//    }
//    rpts1_num = ipts1_num;
//    //边线三角滤波
//    blur_points(rpts0,rpts0_num,rpts0b,(int) round(line_blur_kernel));
//    rpts0b_num = ipts0_num;
//    blur_points(rpts1,rpts1_num,rpts1b,(int) round(line_blur_kernel));
//    rpts1b_num = ipts1_num;

//    //==========方案三 更改三角滤波输入数组参数类型float&int
//    ****/

//    /****/
//    //对边线逆透视
//    //==========方案二 更改三角滤波输入数组参数类型float&int
////    更换TC377用第三核上canny
//      compressimage(); //图像压缩,并将生图赋给Image_Use,并从这里开始采集下一帧   0.6ms
////
////
////    //二值化
////    if(choose_Binarization == 0) //换sobel后需要重启系统！否则最外层黑框显示不了
////          S_Sobel( Image_Use, Image_Use_Robert ,thres);//全局Sobel得二值图(方案二) 2.8ms
//    if(choose_Binarization == 1)
//        Binarization(ostu());                        //0.74ms
////    else if(choose_Binarization == 0) //换sobel后需要重启系统！否则最外层黑框显示不了
////        S_Sobel( Image_Use, Image_Use_Robert ,thres);//全局Sobel得二值图(方案二) 2.8ms

//   else if(choose_Binarization == 0) //换sobel后需要重启系统！否则最外层黑框显示不了
//       S_Sobel( Image_Use, Image_Use_Robert ,thres);//全局Sobel得二值图(方案二) 2.8ms


//    img_raw.data = Image_Use_Robert;//方案二√
////    img_raw.data = Image_Use;//上交方案

//    Find_Borderline();//获取图片的边线 方案二：0.1ms以内 ;上交方案：1.2ms


//    //对边线透视变换
//    Pespective(ipts0,ipts0_num ,  rpts0);
//    rpts0_num = ipts0_num;
//    Pespective(ipts1,ipts1_num ,  rpts1);
//    rpts1_num = ipts1_num;

//    //方案二打表变换
//    //    for (int i = 0; i < ipts0_num; i++) {
//    //        rpts0[i][0] = x_data1[ipts0[i][0]+UCOL*ipts0[i][1]];
//    //        rpts0[i][1] = y_data1[ipts0[i][1]];
//    //    }
//    //    rpts0_num = ipts0_num;
//    //    for (int i = 0; i < ipts1_num; i++) {
//    //        rpts1[i][0] = x_data1[ipts1[i][0]+UCOL*ipts1[i][1]];
//    //        rpts1[i][1] = y_data1[ipts1[i][1]];
//    //    }
//    //    rpts1_num = ipts1_num;

//    //边线三角滤波
//    blur_points(rpts0,rpts0_num,rpts0b,(int) round(line_blur_kernel));
//    rpts0b_num = rpts0_num;
//    blur_points(rpts1,rpts1_num,rpts1b,(int) round(line_blur_kernel));
//    rpts1b_num = rpts1_num;
//    //==========方案二 更改三角滤波输入数组参数类型float&int
//    /****/
//    /****
//    //==========方案一 更改三角滤波输入数组参数类型float&int

//    //compressimage(); //图像压缩,并将生图赋给Image_Use,并从这里开始采集下一帧
//    S_Sobel(ImageUsed, Image_Use_Robert ,thres);
//    img_raw.data = Image_Use_Robert;//方案一 对透视图像数据赋值

//    Find_Borderline();//获取图片的边线
//    //边线三角滤波
//    blur_points(ipts0,ipts0_num,rpts0b,(int) round(line_blur_kernel));
//    rpts0b_num = ipts0_num;
//    blur_points(ipts1,ipts1_num,rpts1b,(int) round(line_blur_kernel));
//    rpts1b_num = ipts1_num;
//    //==========方案一 更改三角滤波输入数组参数类型float&int
//    ****/
//    // 边线等距采样 num为逆透视后实际距离点，两点间距离为0.02*102=2cm
//    rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);//求数组的长度 即等距采样后边线点个数
//    resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
//    rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
//    resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);
//    // 边线局部角度变化率 round():四舍五入
//    local_angle_points(rpts0s, rpts0s_num, rpts0a, (int) round(angle_dist / sample_dist));
//    rpts0a_num = rpts0s_num;
//    local_angle_points(rpts1s, rpts1s_num, rpts1a, (int) round(angle_dist / sample_dist));
//    rpts1a_num = rpts1s_num;
//    // 角度变化率非极大抑制
//    nms_angle(rpts0a, rpts0a_num, rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);
//    rpts0an_num = rpts0a_num;
//    nms_angle(rpts1a, rpts1a_num, rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
//    rpts1an_num = rpts1a_num;

//    // 左右中线跟踪
//    track_leftline(rpts0s, rpts0s_num, rptsc0, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//    rptsc0_num = rpts0s_num;
//    track_rightline(rpts1s, rpts1s_num, rptsc1, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//    rptsc1_num = rpts1s_num;


//    find_corners();// 角点提取&筛选


//    //============各元素特征的图像处理============

//    // 预瞄距离,动态效果更佳
//    aim_distance = aim_distance_flash;
//    //根据速度调整前瞻的因子   速度高于80  增益为负数  前瞻加长
//    //aim_idx_d = (nowspeed - MinSpeed) * 0.2 +0.5 ;

//    // 单侧线少，切换巡线方向  切外向圆
//    if (rpts0s_num < rpts1s_num / 2 && rpts0s_num < 60) {         //如果左边线比右边线少一半，循右
//        track_type = TRACK_RIGHT;
//    } else if (rpts1s_num < rpts0s_num / 2 && rpts1s_num < 60) {  //如果右边线比左边线少一半，循左
//        track_type = TRACK_LEFT;
//    } else if (rpts0s_num < 20 && rpts1s_num > rpts0s_num) {      //如果左边线少于20，且右边数大于左边，循右
//        track_type = TRACK_RIGHT;
//    } else if (rpts1s_num < 20 && rpts0s_num > rpts1s_num) {      //如果右边线少于20，且左边数大于右边，循左
//        track_type = TRACK_LEFT;
//    }

//    //前0.6m执行出库
//    if(garage_type != GARAGE_NONE && Z.all_length < ENCODER_PER_METER*0 && podao_type == podao_NONE && block_type == BLOCK_NONE )
//    {
//        out_garage();
//    }

//    //各元素检测

//    //车库斑马线检查(斑马线优先级高，最先检查)
//    if(/*Z.all_length > ENCODER_PER_METER &&*/ garage_type == GARAGE_NONE && break_type == BREAK_NONE && block_type == BLOCK_NONE && podao_type == podao_NONE )
//        check_garage();
//    //单边检测
//    if (garage_type == GARAGE_NONE && cross_type == CROSS_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE
//            && Lpt0_found && loseline1/*rpts0s_num/2 > rpts1s_num*/ && !Lpt1_found && break_type == BREAK_NONE )
//        check_half_left();
//    if (garage_type == GARAGE_NONE && cross_type == CROSS_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE
//            && Lpt1_found && loseline0/*rpts1s_num/2 > rpts0s_num*/ && !Lpt0_found && break_type == BREAK_NONE)
//        check_half_right();
//    //双边不丢线检测
//    if (garage_type == GARAGE_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE && break_type == BREAK_NONE)
//        check_cross();
//    if (garage_type == GARAGE_NONE && cross_type == CROSS_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE && break_type == BREAK_NONE)              //不是车库、十字 , 才检测圆环
//        check_circle();






//    //各元素执行
//    if (garage_type != GARAGE_NONE && (garage_type != GARAGE_OUT_LEFT || garage_type != GARAGE_OUT_RIGHT)) run_garage();
//    if (cross_type != CROSS_NONE) run_cross();
//    if (circle_type != CIRCLE_NONE) run_circle();
////    if (block_type == BLOCK_NONE)
////    {
////        //对近处线截断处理
////        if (N_Lpt0_found) {
////            rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
////        }
////        if (N_Lpt1_found) {
////            rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
////        }
////        aim_distance = 0.4;//大概在rpts_id=15
////    }


//    //============各元素特征的图像处理============


//    // 锁定中线跟踪
//    if (cross_type != CROSS_IN_DOUBLE && cross_type != CROSS_INHALF_LEFT && cross_type != CROSS_INHALF_RIGHT) {             //不是十字
////        if(circle_type == CIRCLE_LEFT_OUT || circle_type == CIRCLE_LEFT_IN)
////        {
////            rpts = splicing_rightline_center;
////            rpts_num = splicing_rightline_center_num;
////        }
////        else if(circle_type == CIRCLE_RIGHT_OUT || circle_type == CIRCLE_RIGHT_IN)
////        {
////            rpts = splicing_leftline_center;
////            rpts_num = splicing_leftline_center_num;
////        }
////        else{
//            if (track_type == TRACK_LEFT) {
//                rpts = rptsc0;
//                rpts_num = rptsc0_num;
//            } else {
//                rpts  = rptsc1;
//                rpts_num = rptsc1_num;
//            }
////        }
//    } else {
//        //十字根据远线控制 地址平移+数组删减
//        if (track_type == TRACK_LEFT) {
//            track_leftline(far_rpts0s + far_Lpt0_rpts0s_id, far_rpts0s_num - far_Lpt0_rpts0s_id, rpts,
//                           (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//            rpts_num = far_rpts0s_num - far_Lpt0_rpts0s_id;
//        } else {
//            track_rightline(far_rpts1s + far_Lpt1_rpts1s_id, far_rpts1s_num - far_Lpt1_rpts1s_id, rpts,
//                            (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//            rpts_num = far_rpts1s_num - far_Lpt1_rpts1s_id;
//        }
////        //十字补线方案
////        if (track_type == TRACK_LEFT) {
////            rpts = rptsc0;
////            rpts_num = rptsc0_num;
////        } else {
////            rpts  = rptsc1;
////            rpts_num = rptsc1_num;
////        }
//    }

//    //纯跟踪起始点 根据方案不同调整！
//    float cx, cy; //思考：由于逆透视出来的是现实坐标系，考虑把图像最低行中间起始点改为小车重心点会不会好一些；若是重心，则cx，cy为摄像头坐标距重心坐标距离
////    cx = UCOL/2+0.5;//方案一
////    cy = UROW;
//    Pespective_xy( UCOL/2+0.5 ,begin_y ,  &cx, &cy);//方案二
////    rot_xy( UCOL/2+0.5 ,begin_y ,  &cx, &cy);//方案三



//    //找最近点(起始点中线归一化)
//    float min_dist = 1e10;
//    int   begin_id = -1;
//    for (int i = 0; i < rpts_num; i++) {
//        float dx = rpts[i][0] - cx;
//        float dy = rpts[i][1] - cy;
//        //float dist = mySqrt(dx * dx + dy * dy);
//        float dist = sqrt(dx * dx + dy * dy);
//        if (dist < min_dist) {
//            min_dist = dist;
//            begin_id = i;
//        }
//    }
//    // 中线有点，同时最近点不是最后几个点
//    if (begin_id >= 0 && rpts_num - begin_id >= 3) {
//        // 归一化中线
//        rpts[begin_id][0] = cx;
//        rpts[begin_id][1] = cy;
//        rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);
//        resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);



//        /***************计算远处偏差均值*************/
//        // 预锚点位置 第34点作为控制偏差 注意：最低行距离车头约10cm距离，距离摄像头25cm距离 故预瞄点现实中为60cm
////        int i;
////        for(i = 0;i<5;i++){
////            aim_idx[i] = clip(round(aim_dist[i] / sample_dist), 0, rptsn_num - 1);
////
////
////            // 计算远锚点偏差值      cx，cy为摄像头坐标距重心坐标距离 0,-7
////            dx[i] = rptsn[aim_idx[i]][0] - cx;
////            dy[i] = cy - rptsn[aim_idx[i]][1] ;
////            dn[i] = sqrt(dx[i] * dx[i] + dy[i] * dy[i]);
////            //dn = mySqrt(dx * dx + dy * dy);
////            //error>0 左 error<0 右 以下为角度制换算为弧度制
////            error[i] = atan2f(dx[i], -dy[i]) * 180 / PI;//1弧度=180/π度 1度=π/180弧度
////            //assert(!isnan(error));
////
////
////        }
////        ave_error = error[0]*0.3 + error[1]*0.27 + error[2]*0.21 + error[3]*0.14 + error[4]*0.08;//正态分布
//        /***************计算远处偏差均值*************/

////        aim_idx[0] = clip(round(aim_dist[0] / sample_dist), 0, rptsn_num - 1);

//        aim_idx[0] = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);


//        // 计算远锚点偏差值      cx，cy为摄像头坐标距重心坐标距离 0,-7
//        dx[0] = rptsn[aim_idx[0]][0] - cx;
//        dy[0] = rptsn[aim_idx[0]][1] - cy;
//        dn[0] = sqrt(dx[0] * dx[0] + dy[0] * dy[0]);
//        //dn = mySqrt(dx * dx + dy * dy);
//        //error<0(输出为正) 左 error>0(输出为负) 右 以下为角度制换算为弧度制
//        error[0] = atan2f(dx[0], dy[0]) * 180 / PI;//1弧度=180/π度 1度=π/180弧度
//        //assert(!isnan(error));



//        ave_error = 0;
//        // 远近锚点综合考虑
//        //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
//        // 根据偏差进行PD计算
//        //float angle = pid_solve(&servo_pid, error);

//        // 纯跟踪算法(只考虑远点)三轮控制用不到
////        pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / 1;
//      } else {
//          // 中线点过少(出现问题)，则不控制舵机
//          rptsn_num = 0;
//      }

//     end = system_getval_us();
//     //timer_cost = end - start;                              //处理每一帧的时间

//}
//// 角点提取&筛选 Y角点用于检测坡道，L角点用于检测圆环、十字、路障、断路
//void find_corners(){
//    // 识别Y,L拐点
//    Ypt0_found = Ypt1_found = Lpt0_found = Lpt1_found = N_Lpt1_found = N_Lpt0_found = false;
//    is_straight0 = rpts0s_num > 1.6 / sample_dist;           //逆透视之后的边线个数大于80个就是直道,也就是边线边长大于0.8m,并不是直向前0.8m
//    is_straight1 = rpts1s_num > 1.6 / sample_dist;

//    //左边线角度
//    for (int i = 1; i < rpts0s_num; i++) {              //只要数组里的角度符合的，Y或L就为true,然后就不在继续检测Y或L
//        if (rpts0an[i] == 0) continue;                      //以下检测只检测峰值（极大值）
//        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
//        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
//        float conf = fabs(rpts0a[i]) - (fabs(rpts0a[im1]) + fabs(rpts0a[ip1])) / 2;

//        //Y角点阈值                 (30*  ,   50*)    并且距离在40个点之内 (注：30.转化浮点数)
//        if (Ypt0_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
//            Ypt0_rpts0s_id = i;
//            Ypt0_found = true;
//        }
//        //L角点阈值                 (55*  ,    140*)  并且距离在40个点之内 修改距离为1.2m增长判断路障距离
//        if (Lpt0_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {

//            Lpt0_rpts0s_id = i;
//            Lpt0_found = true;

//            if(rpts0a[Lpt0_rpts0s_id] < 0)              //这一个L点如果是向右拐的，就是内L点
//            {
//                N_Lpt0_rpts0s_id = Lpt0_rpts0s_id;
//                N_Lpt0_found = true;
////                //对内L角的线截断
////                rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
//                Lpt0_found = false;
//            }

//        }
//        //长直道阈值
//        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight0 = false;           //只要中间有大角度，就不是长直道
//        if (Ypt0_found == true && Lpt0_found == true && is_straight0 == false) break;
//    }

//    //右边线角度
//    for (int i = 1; i < rpts1s_num; i++) {
//        if (rpts1an[i] == 0) continue;
//        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);
//        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);
//        float conf = fabs(rpts1a[i]) - (fabs(rpts1a[im1]) + fabs(rpts1a[ip1])) / 2;
//        if (Ypt1_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
//            Ypt1_rpts1s_id = i;
//            Ypt1_found = true;
//        }
//        if (Lpt1_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {
//            Lpt1_rpts1s_id = i;
//            Lpt1_found = true;

//            if(rpts1a[Lpt1_rpts1s_id] > 0)              //这一个L点如果是向左拐的，就是内L点
//            {
//                N_Lpt1_rpts1s_id = Lpt1_rpts1s_id;
//                N_Lpt1_found = true;
////                //对内L角的线截断
////                rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
//                Lpt1_found = false;
//            }
//        }

//        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight1 = false;
//        if (Ypt1_found == true && Lpt1_found == true && is_straight1 == false) break;
//    }

//    // Y点二次检查,依据两角点距离及角点后张开特性
//    if (Ypt0_found && Ypt1_found) {
//        //==================================================================原检查
//        float dx = rpts0s[Ypt0_rpts0s_id][0] - rpts1s[Ypt1_rpts1s_id][0];
//        float dy = rpts0s[Ypt0_rpts0s_id][1] - rpts1s[Ypt1_rpts1s_id][1];
//        float dn = sqrtf(dx * dx + dy * dy);
//        if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)             //两点距离不能大于60cm  (赛道40cm)
//        {
//            float dwx = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                        rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//            float dwy = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                        rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//            float dwn = sqrtf(dwx * dwx + dwy * dwy);
//            if (!(dwn > 0.7 * pixel_per_meter &&                            //如两个Y点之后第50个 点之间的距离小于0.7m
//                  rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Ypt0_rpts0s_id][0] &&
//                  rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Ypt1_rpts1s_id][0]))
//            {
//                Ypt0_found = Ypt1_found = false;
//            }
//        } else
//        {
//            Ypt0_found = Ypt1_found = false;
//        }
//        //zqkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk的检查

//        float dx_temp ,dy_temp ;



//        dx_temp = rpts0s[Ypt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
//        dy_temp = rpts0s[Ypt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
//        xielv_left_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


//        dx_temp = rpts1s[Ypt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
//        dy_temp = rpts1s[Ypt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
//        xielv_right_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


//    }
//    // L点二次检查，车库模式不检查, 依据L角点距离及角点后张开特性
//    if (garage_type == GARAGE_NONE) {
//        if (Lpt0_found && Lpt1_found) {
//            float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//            float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//            float dn = sqrtf(dx * dx + dy * dy);
//            if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)            //两点距离不能大于60cm  (赛道40cm)
//            {
//                float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//                float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//                float dwn = sqrtf(dwx * dwx + dwy * dwy);
//                if (!(dwn > 0.5 * pixel_per_meter &&                                //符合直角的特征必须50个点后距离大于0.7m，
//                      rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
//                      rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
//                {
//                    Lpt0_found = Lpt1_found = false;
//                }
//            } else
//            {
//                Lpt0_found = Lpt1_found = false;
//            }

//            float dx_temp ,dy_temp ;



//            dx_temp = rpts0s[Lpt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
//            dy_temp = rpts0s[Lpt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
//            xielv_left_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子  一般是负


//            dx_temp = rpts1s[Lpt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
//            dy_temp = rpts1s[Lpt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
//            xielv_right_y_to_end =  dy_temp / dx_temp;                                   //这里得算法包含十字得情况，所以dy 有可能为0，所以dy做分子


//        }
//    }

//    //左车库第二次检查
//    if (Lpt0_found && N_Lpt1_found) {                   //理论上是左车库
//        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//        float dn = sqrtf(dx * dx + dy * dy);
//        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //两点距离不能大于60cm  (赛道45cm)
//        {
//            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//            float dwn = sqrtf(dwx * dwx + dwy * dwy);
//            if (!(dwn < 0.5 * pixel_per_meter &&                                //符合左车库特征必须50个点后距离小于0.5m，
//                  rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
//                  rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] < rpts1s[Lpt1_rpts1s_id][0]))
//            {
//                Lpt0_found = N_Lpt1_found = false;
//            }
//        } else
//        {
//            Lpt0_found = N_Lpt1_found = false;
//        }
//    }


//    //右车库第二次检查
//    if (Lpt1_found && N_Lpt0_found) {                   //理论上是右车库
//        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//        float dn = sqrtf(dx * dx + dy * dy);
//        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //两点距离不能大于60cm  (赛道45cm)
//        {
//            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//            float dwn = sqrtf(dwx * dwx + dwy * dwy);
//            if (!(dwn < 0.5 * pixel_per_meter &&                                //符合左车库特征必须50个点后距离小于0.5m，
//                  rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] > rpts0s[Lpt0_rpts0s_id][0] &&
//                  rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
//            {
//                Lpt1_found = N_Lpt0_found = false;
//            }
//        } else
//        {
//            Lpt1_found = N_Lpt0_found = false;
//        }
//    }
//}

////void clone_image(image_t *img0, image_t *img1)
////{
////    assert(img0 && img0->data);
////    assert(img1 && img1->data);
////    assert(img0->width == img1->width && img0->height == img1->height);
////    assert(img0 != img1 && img0->data != img1->data);
////
////    if (img0->width == img0->step && img1->width == img1->step) {
////        memcpy(img1->data, img0->data, img0->width * img0->height);
////    } else {
////        for (int y = 0; y < img0->height; y++) {
////            memcpy(&AT(img1, 0, y), &AT(img0, 0, y), img0->width);
////        }
////    }
////}


///*************************************************************************
// *  函数名称：void compressimage (void);
// *  功能说明：压缩图像
// *  参数说明：无
// *  函数返回：无
// *  修改时间：2022年12月17日
// *  备    注：  IMAGEW为原始图像的宽度
// *       IMAGEH为原始图像的高度
// *        mt9v03x_image[MT9V03X_H][MT9V03X_W]为获取的图像，将其压缩到Image_Use[UROW][UCOL]
// *************************************************************************/
//void compressimage(void)
//{
//    int i, j, row, line;
//      const float div_h = MT9V03X_H / UROW, div_w = MT9V03X_W / UCOL;
//      for (i = 0; i < UROW; i++) {
//        row = i * div_h + 0.5;
//        for (j = 0; j < UCOL; j++) {
//          line = j * div_w + 0.5;
//          Image_Use[i][j] = mt9v03x_image[row][line];
//        }
//      }
//      mt9v03x_finish_flag = 0;  //使用完一帧DMA传输的图像图像  可以开始传输下一帧

//}
///*************************************************************************
// *  函数名称：void uncompressimage (void);
// *  功能说明：解压图像
// *  参数说明：无
// *  函数返回：无
// *  修改时间：2022年11月30 日
// *  备    注：主要应用在图像显示上，display.c/keyboard.c应用
// *************************************************************************/
//void uncompressimage(void) {
//  int i = 0, j = 0, row = 0, line = 0;
//  int div_h = uLCDH / UROW;
//  int div_w = uLCDW / UCOL;
//  for (i = 0; i < uLCDH; i++) {
//    for (j = 0; j < uLCDW; j++) {
//      row = (int)(i / div_h);
//      line = (int)(j / div_w);
//      uPixle[i][j] = Image_Use_Robert[row][line];
////      uPixle[i][j] = Pixle[row][line];

//    }
//  }
//}
///*************************************************************************
// *  函数名称：uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)；
// *  功能说明：获取图像阈值
// *  参数说明：图像地址
// *  函数返回：阈值
// *  修改时间：2022年9月30日
// *  备    注：
// *************************************************************************/
//uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)
//{
//    #define GrayScale 256
//    int pixelCount[GrayScale] = {0};//每个灰度值所占像素个数
//    float pixelPro[GrayScale] = {0};//每个灰度值所占总像素比例
//    int i,j;
//    int Sumpix = width * height;   //总像素点
//    uint8 threshold = 0;
//    uint8* data = image;  //指向像素数据的指针

//    //统计灰度级中每个像素在整幅图像中的个数
//    for (i = 0; i < height; i++)
//    {
//        for (j = 0; j < width; j++)
//        {
//            pixelCount[(int)data[i * width + j]]++;  //将像素值作为计数数组的下标
//          //   pixelCount[(int)image[i][j]]++;    若不用指针用这个
//        }
//    }
//    float u = 0;
//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //计算每个像素在整幅图像中的比例
//        u += i * pixelPro[i];  //总平均灰度
//    }


//    float maxVariance=0.0;  //最大类间方差
//    float w0 = 0, avgValue  = 0;  //w0 前景比例 ，avgValue 前景平均灰度
//    for(int i = 0; i < 256; i++)     //每一次循环都是一次完整类间方差计算 (两个for叠加为1个)
//    {
//        w0 += pixelPro[i];  //假设当前灰度i为阈值, 0~i 灰度像素所占整幅图像的比例即前景比例
//        avgValue  += i * pixelPro[i];

//        float variance = pow((avgValue/w0 - u), 2) * w0 /(1 - w0);    //类间方差
//        if(variance > maxVariance)
//        {
//            maxVariance = variance;
//            threshold = i;
//        }
//    }


//    return threshold;

//}
//uint8 OSTU(void)
//    {
//    uint8 HistGram[256] = {0,};
//    uint16 x,y;
//    int16 Y;
//    uint32 Amount = 0;
//    uint32 PixelBack = 0;
//    uint32 PixelIntegralBack = 0;
//    uint32 PixelIntegral = 0;
//    int32 PixelIntegralFore = 0;
//    int32 PixelFore = 0;
//    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
//    int16 MinValue, MaxValue;
//    uint8 Threshold = 0;

//        for (y = 0; y < 256; y++)
//        {
//            HistGram[y] = 0; //初始化灰度直方图
//        }
//        for (y = 0; y < MT9V03X_H; y++)
//        {
//            for (x = 0; x < MT9V03X_W; x++)
//            {
//                HistGram[mt9v03x_image[y][x]]++; //统计每个灰度值的个数信息
//            }
//        }


//    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
//    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值

//    if (MaxValue == MinValue)
//    {
//        return MaxValue;          // 图像中只有一个颜色
//    }
//    if (MinValue + 1 == MaxValue)
//    {
//        return MinValue;      // 图像中只有二个颜色
//    }

//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        Amount += HistGram[Y];        //  像素总数
//    }

//    PixelIntegral = 0;
//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        PixelIntegral += HistGram[Y] * Y;//灰度值总数
//    }
//    SigmaB = -1;
//    for (Y = MinValue; Y < MaxValue; Y++)
//    {
//        PixelBack = PixelBack + HistGram[Y];    //前景像素点数
//        PixelFore = Amount - PixelBack;         //背景像素点数
//        OmegaBack = (double)PixelBack / Amount;//前景像素百分比
//        OmegaFore = (double)PixelFore / Amount;//背景像素百分比
//        PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
//        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
//        MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
//        MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
//        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
//        if (Sigma > SigmaB)//遍历最大的类间方差g
//        {
//            SigmaB = Sigma;
//            Threshold = Y;
//        }
//        if (Sigma < SigmaB)//遍历最大的类间方差g
//        {
//            break;
//        }
//    }
//    return Threshold;
//}
//uint8 ostu(void)   //注意计算阈值的一定要是原图像
//{
//    #define GrayScale 256
//    static uint8 lasthreshold=0;
//    uint16 width = UCOL; //自己图像的宽度
//    uint16 height = UROW; //自己图像的高度
//    //uint16 width = MT9V03X_W;
//    //uint16 height = MT9V03X_H;
//    int pixelCount[GrayScale];
//    float pixelPro[GrayScale];
//    int i, j, pixelSum = width * height/4;
//    uint8 threshold = 0;
//    uint8* data = Image_Use[0];//uint8* data = mt9v03x_image[0];  //指向像素数据的指针
//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelCount[i] = 0;
//        pixelPro[i] = 0;
//    }

//    uint32 gray_sum=0;
//    //统计灰度级中每个像素在整幅图像中的个数
//    for (i = 0; i < height; i+=2)
//    {
//        for (j = 0; j < width; j+=2)
//        {
//            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
//            gray_sum+=(int)data[i * width + j];       //灰度值总和
//        }
//    }

//    //计算每个像素值的点在整幅图像中的比例

//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / pixelSum;

//    }

//    //遍历灰度级[0,255]
//    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


//        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//        if( lasthreshold-50>0)
//    j= lasthreshold-50;
//        else
//    j=0;
//        for (; j < GrayScale; j++)
//        {

//                w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
//                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

//               w1=1-w0;
//               u1tmp=gray_sum/pixelSum-u0tmp;

//                u0 = u0tmp / w0;              //背景平均灰度
//                u1 = u1tmp / w1;              //前景平均灰度
//                u = u0tmp + u1tmp;            //全局平均灰度
//                deltaTmp = w0 * (u0 - u) * (u0 - u) + w1 * (u1 - u) * (u1 - u);
//                if (deltaTmp > deltaMax)
//                {
//                    deltaMax = deltaTmp;
//                    threshold = j;
//                }
//                if (deltaTmp < deltaMax)
//                {
//                break;
//                }

//         }

//    return threshold;
//}
////-------------------------------------------------------------------------------------------------------------------
//// @brief       二值化
//// @param       *picture_buffer     被转换的数组地址
//// @param       value               转换阈值
//// @return      void
//// Sample usage:                Binarization(Img[0], 128)
////-------------------------------------------------------------------------------------------------------------------
//void Binarization(uint8 thro)
//{
//    uint32 i,j;

//    uint16 temp = 0;

//    for(j=0;j<UROW;j++)
//    {
//        for(i=0;i<UCOL;i++)
//        {
//            temp = *(Image_Use[0]+j*UCOL+i);//读取像素点
//            if(j == 0 || j == UROW-1 || i == 0 || i == UCOL-1)//大津法加一个黑框
//            {
//                *(Image_Use_Robert[0]+j*UCOL+i)=0;
//            }
//            else
//            {
//                if(temp>=thro) *(Image_Use_Robert[0]+j*UCOL+i)=255;
//                else *(Image_Use_Robert[0]+j*UCOL+i)=0;

//            }
//        }
//    }
//}
///************************************Sobel算子************************************
//**          Gx={    {-1,  0,  1},                   Gy={    {  1,  2,  1},
//**                  {-2,  0,  2},                           {  0,  0,  0},
//**                  {-1,  0,  1}}                           { -1, -2, -1}}
//**      最上下左右边缘舍去
//**
//**      Gx = (-1)*f(x-1, y-1) + 0*f(x,y-1) + 1*f(x+1,y-1)
//**            +(-2)*f(x-1,y) + 0*f(x,y)+2*f(x+1,y)
//**            +(-1)*f(x-1,y+1) + 0*f(x,y+1) + 1*f(x+1,y+1)
//**      = [f(x+1,y-1)+2*f(x+1,y)+f(x+1,y+1)]-[f(x-1,y-1)+2*f(x-1,y)+f(x-1,y+1)]
//**
//**      Gy =1* f(x-1, y-1) + 2*f(x,y-1)+ 1*f(x+1,y-1)
//**            +0*f(x-1,y) 0*f(x,y) + 0*f(x+1,y)
//**            +(-1)*f(x-1,y+1) + (-2)*f(x,y+1) + (-1)*f(x+1, y+1)
//**      = [f(x-1,y-1) + 2f(x,y-1) + f(x+1,y-1)]-[f(x-1, y+1) + 2*f(x,y+1)+f(x+1,y+1)]
//**      若Threshold=0则输出灰度图      其他则二值化
//***********************************************************************************/
//#define Sobel_Gx(addr, y, x)    (addr[UP][RR]+2*addr[y][RR]+addr[DN][RR]-(addr[UP][LL]+2*addr[y][LL]+addr[DN][LL]))
//#define Sobel_Gy(addr, y, x)    (addr[UP][LL]+2*addr[UP][x]+addr[UP][RR]-(addr[DN][LL]+2*addr[DN][x]+addr[DN][RR]))
//#define Sobel_G(addr, y, x)     (abs(Sobel_Gx(addr, y, x)) + abs(Sobel_Gy(addr, y, x)))
//void S_Sobel(uint8 in_IMG[UROW][UCOL], uint8_t out_IMG[UROW][UCOL], uint16 Threshold)
//{
//    uint8_t i, j;
//    uint8_t UP, DN, LL, RR;
//    if (Threshold == 0)//观察每点梯度值
//    {
//        for (i = 1; i < UROW-1; i++)
//        {
//            DN = i + 1;     UP = i - 1;
//            for (j = 1; j < UCOL-1; j++)
//            {
//                RR = j + 1;     LL = j - 1;
//                out_IMG[i][j] = Sobel_G(in_IMG, i, j);
//            }
//        }
//    }
//    else//根据梯度值二值化
//    {
//        for (i = 1; i <  UROW-1; i++)
//        {
//            DN = i + 1;     UP = i - 1;
//            for (j = 1; j < UCOL-1; j++)
//            {
//                RR = j + 1;     LL = j - 1;
//                out_IMG[i][j] = (Sobel_G(in_IMG, i, j) >= Threshold ? 0 : 255);                    //修改的tft二值化图库函数：黑0；白1
//            }
//        }
//    }
//}

///*************************************************************************
// *  函数名称：Find_Borderline();
// *  功能说明：寻找边线
// *  参数说明：无
// *  函数返回：无
// *  修改时间：2023年5月10日
// *  备    注：https://zhuanlan.zhihu.com/p/391392970 CSDN搜索上交迷宫法 （针对15cm限高优化）
// *
// *************************************************************************/
//int x0_first , y0_first , x1_first ,y1_first;
//int x1 , y1 ;
//int x2 , y2 ;
//void Find_Borderline(void)
//{
//    //迷宫巡线是否走到左右边界
//    touch_boundary0 = 0;            //清零
//    touch_boundary1 = 0;

//    //迷宫巡线是否走到上边界
//    touch_boundary_up0 = 0;            //清零
//    touch_boundary_up1 = 0;

//    //底边扫线防止丢线 注意：由于sobel边缘检测特殊性（黑框），此丢线标志仅适用Ostu方案
//    loseline0 = 0;
//    loseline1 = 0;

//    uint8 uthres = 1;
////    uint8 uthres = ostu();
//  //寻左边线
//  x1 = img_raw.width / 2 - begin_x, y1 = begin_y;

//  //标记种子起始点(后续元素处理要用到)
//  x0_first = x1;    y0_first = y1;

//  ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);//求数组的长度
//  //扫底下五行，寻找跳变点
//  for (; y0_first > begin_y-5; y0_first--)
//  {
//      for (; x0_first > 0; x0_first--)
//          if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)
//              goto out1;
//      x0_first = img_raw.width / 2 - begin_x;
//  }
//  loseline0 = 1;  //底边丢线
//  out1:
//  {
//      if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres)
//        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);
//      else ipts0_num = 0;
//  }



//  //寻右边线
//  x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

//  //标记种子起始点(后续元素处理要用到)
//  x1_first = x2;    y1_first = y2;

//  ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
//  for (; y1_first > begin_y-5; y1_first--)
//  {
//      for (; x1_first < img_raw.width - 1; x1_first++)
//          if (AT_IMAGE(&img_raw, x1_first + 1, y1_first) < uthres)
//              goto out2;
//      x1_first = img_raw.width / 2 + begin_x;
//  }
//  loseline1 = 1;  //底边丢线
//  out2:
//  {
//      if (AT_IMAGE(&img_raw, x1_first, y1_first) >= uthres)
//        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts1, &ipts1_num);
//      else ipts1_num = 0;
//  }
//}
///*************************************************************************
// *  函数名称：void Left_Adaptive_Threshold();
// *  功能说明：自定义阈值二值化+迷宫算法（左边线）
// *  参数说明：获取的图像数组，输出数组（备用image0），图像宽，图像高
// *  函数返回：无
// *  修改时间：2023年5月23日
// *  备    注： 增加丢线判断
// * 前进方向定义：
// *   0
// * 3   1
// *   2
// *************************************************************************/

//const int dir_front[4][2] = { {0, -1},//正左
//                              {1,  0},//正下
//                              {0,  1},//正右
//                              {-1, 0}};//正上
//const int dir_frontleft[4][2] = {{-1, -1},//左上
//                                  {1,  -1},//左下
//                                   {1,  1},//右下
//                                  {-1, 1}};//右上，逆时针
//const int dir_frontright[4][2] = {{1,  -1},//左下
//                                   {1,  1},//右下
//                                   {-1, 1},//右上
//                                  {-1, -1}};//左上，逆时针

//void Left_Adaptive_Threshold(image_t *img,int block_size,int clip_value,int x,int y,int pts[][2],int *num)
//{
//    zf_assert(img && img->data);     // 不满足则退出执行
//    zf_assert(num && *num >= 0);
//    zf_assert(block_size > 1 && block_size % 2 == 1);//保证block_size为奇数
////    int half = block_size / 2;                       //上交方案
//    int half = 0;                                  //方案二
//    int step = 0, dir = 0, turn = 0; // step表示前进的步数；dir通过改变索引改变当前小人朝向的方向
//    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4)//保证block不出界
//    {
//        int local_thres = 1;
////        int local_thres;
//        //自适应二值化
//    /*
//        for (int dy = -half; dy <= half; dy++)  // for循环用来计算block区域的像素值之和（自适应阈值）
//        {
//            for (int dx = -half; dx <= half; dx++)
//            {
//                local_thres += AT(img, x + dx, y + dy);
//            }
//        }
//        local_thres /= block_size * block_size;
//        local_thres -= clip_value;   // (x,y)点block区域内的阈值
//    */
//        int current_value = AT(img, x, y);//当前像素点灰度值
//        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);//正前方像素点灰度值 （dir=0 下；dir=1 右；dir=2 上；dir=3 左）
//        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);//左前方像素点灰度值 （dir=0左下；dir=1 右下；dir=2 右上；dir=3 左上 ）
//        //=======添加部分=======
//        if( (x==1 && y < img->height -20) || x== img->width - 2 || y==1 || (y==58 && step > 19))
//        {
//            if(x==1 /*|| x== img->width - 2*/)  touch_boundary0  = 1;                //左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
//            if(y==1) touch_boundary_up0 = 1;               //走到顶边，判断坡道or障碍

//            break;
//        }
//        //=======添加部分=======
//        if (front_value < local_thres)  // 前进方向像素为黑色
//        {
//            dir = (dir + 1) % 4;   // 遇到前方为黑色需要右转一次
//            turn++;
//        }
//        else if (frontleft_value < local_thres) // 前方像素为白色，且左前方像素为黑色
//        {
//            x += dir_front[dir][0];
//            y += dir_front[dir][1];
//            pts[step][0] = x;   // 用来存放边线坐标信息
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            //AT(img,x,y) = RGB565_GREEN;
//        }
//        else  // 前方为白色，左前方为白色（墙角）
//        {
//            x += dir_frontleft[dir][0];   // 遇到墙角要斜着走
//            y += dir_frontleft[dir][1];
//            dir = (dir + 3) % 4;   // 遇到墙角要左转一次
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            //AT(img,x,y) = RGB565_GREEN;
//        }
//    }
//    //丢线标志，否则由于sobel特殊性会一直往上巡线
//    if(step < 20 && touch_boundary0)
//        loseline0 = 1;
//    //记录边线数目
//    *num = step;
//}
///*************************************************************************
// *  函数名称：void Right_Adaptive_Threshold();
// *  功能说明：自定义阈值二值化+迷宫算法（右边线）
// *  参数说明：获取的图像数组，输出数组（备用image0），图像宽，图像高
// *  函数返回：无
// *  修改时间：2023年5月23日
// *  备    注： 增加丢线判断
// * 前进方向定义：
// *   0
// * 3   1
// *   2
// *************************************************************************/
//void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {
//    zf_assert(img && img->data);
//    zf_assert(num && *num >= 0);
//    zf_assert(block_size > 1 && block_size % 2 == 1);
////    int half = block_size / 2;        //上交方案
//    int half = 0;                     //方案二
//    int step = 0, dir = 0, turn = 0;
//    while (step < *num && 0 < x && x < img->width - 1 && half < y && y < img->height - half - 1 && turn < 4) {
//        int local_thres = 1;
////        int local_thres;
//        //自适应二值化
///*
//        for (int dy = -half; dy <= half; dy++) {
//            for (int dx = -half; dx <= half; dx++) {
//                local_thres += AT(img, x + dx, y + dy);
//            }
//        }
//        local_thres /= block_size * block_size;
//        local_thres -= clip_value;
//*/
//        int current_value = AT(img, x, y);
//        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
//        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
//        //=======添加部分=======
//        if( (x==img->width - 2  && y < img->height -20)   || x==1 || y==1 || (y==58 && step > 19)) //丢线标志，否则由于sobel特殊性会一直往上巡线
//        {

//            if(x==img->width - 2 /*|| x==1*/)  touch_boundary1  = 1;                //右边界是因为到最右边才停下来的，触碰到最右边，可能是环岛，十字等，
//            if(y==1) touch_boundary_up1 = 1;               //走到顶边，判断坡道or障碍

//            break;
//        }
//        //=======添加部分=======
//        if (front_value < local_thres) {
//            dir = (dir + 3) % 4;
//            turn++;
//        } else if (frontright_value < local_thres) {
//            x += dir_front[dir][0];
//            y += dir_front[dir][1];
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            //AT(img,x,y) = RGB565_YELLOW;
//        } else {
//            x += dir_frontright[dir][0];
//            y += dir_frontright[dir][1];
//            dir = (dir + 1) % 4;
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            //AT(img,x,y) = RGB565_YELLOW;
//        }
//    }
//    //丢线标志，否则由于sobel特殊性会一直往上巡线
//    if(step < 20 && touch_boundary1)
//        loseline1 = 1;
//    //记录边线数目
//    *num = step;
//}
////补线 原图
//void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value)
//{
//    int dx = pt1[0] - pt0[0];
//    int dy = pt1[1] - pt0[1];
//    if (abs(dx) > abs(dy)) {
//        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1)) {
//            int y = pt0[1] + (x - pt0[0]) * dy / dx;// y = 左线横坐标 + x遍历差值占总差值比例 * y方向差值
//                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; //（x，y）坐标像素（不超出边界）赋值
//        }
//    } else {
//        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1)) {
//            int x = pt0[0] + (y - pt0[1]) * dx / dy;
//                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
//        }
//    }
//}
///*************************************************************************
// *  函数名称：void draw_line2();
// *  功能说明：补线 逆透视
// *  参数说明：
// *  函数返回：无
// *  修改时间：2023年6月1日
// *  备    注： 本质是等距采样
// *************************************************************************/
//void draw_line2(float pt0[2], float pt1[2], float pts_out[][2], int *num, float dist){
//    int remain = 0, len = 0;//下一次采样折线段距离
//    float x0 = pt0[0];
//    float y0 = pt0[1];
//    float dx = pt1[0] - x0;
//    float dy = pt1[1] - y0;
//    float dn = mySqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
//    //float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
//    dx /= dn;//此点处的cosθ
//    dy /= dn;//此点处的sinθ

//    //每次等距采样处理
//    while(remain < dn){
//        x0 += dx * remain;
//        pts_out[len][0] = x0;
//        y0 += dy * remain;
//        pts_out[len][1] = y0;

//        len++;
//        dn -= remain;
//        remain = dist;
//    }


//    *num = len;
//}
///*************************************************************************
// *  函数名称：void SplicingArray();
// *  功能说明：数组拼接
// *  参数说明：输入的两个数组和容量，输出数组以及容量，x=0正向拼接，x=1反向拼接
// *  函数返回：无
// *  修改时间：2023年6月2日
// *  备    注： 用于连接补线的数组
// *************************************************************************/
//void SplicingArray(float pt0[][2], int num1, float pt1[][2], int num2, float pt_out[][2], int *num, uint8 x)
//{
//    int i ,count;//用来计数
//    for(i = 0;i<num1;i++)//复制数组1
//    {
//        pt_out[i][0] = pt0[i][0];
//        pt_out[i][1] = pt0[i][1];
//        count++;
//    }

//    if(x)//反向拼接
//    {
//        for(i = 0;i<num2;i++)
//        {
//            pt_out[num1+i][0] = pt1[num2-i-1][0];
//            pt_out[num1+i][1] = pt1[num2-i-1][1];
//            count++;
//        }

//    }else//正向拼接
//    {
//        for(i = 0;i<num2;i++)
//        {
//            pt_out[num1+i][0] = pt1[i][0];
//            pt_out[num1+i][1] = pt1[i][1];
//            count++;
//        }
//    }
//    *num = count;
//}


///*************************************************************************
// *  函数名称：void blur_points();
// *  功能说明：点集三角滤波
// *  参数说明：输入边线数组，边线总个数，输出边线数组，点集范围
// *  函数返回：无
// *  修改时间：2022年10月4日
// *  备    注：
// * 例如：kernel = 5
// *  xi = (0*xi-3 + 1*xi-2 + 2*xi-1 + 3*xi + 2*xi+1 + 1*xi+2 + 0*xi+3)/9
// *  yi 同理
// *************************************************************************/
//void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel)
//{
//    zf_assert(kernel % 2 == 1);
//    int half = kernel / 2;
//    for (int i = 0; i < num; i++) {
//        pts_out[i][0] = pts_out[i][1] = 0;
//        for (int j = -half; j <= half; j++) {
//            pts_out[i][0] += (float)pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
//            pts_out[i][1] += (float)pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
//        }
//        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
//        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
//    }
//}
///*************************************************************************
// *  函数名称：void resample_points（）;
// *  功能说明：点集等距采样
// *  参数说明：输入边线数组，原边线总个数，输出边线数组，获取输出边线数组个数，采样距离
// *  函数返回：无
// *  修改时间：2022年10月27日
// *  备    注：使走过的采样前折线段的距离为`dist`
// *************************************************************************/
//void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist){
//    int remain = 0, len = 0;//下一次采样折线段距离
//    for(int i=0; i<num1-1 && len < *num2; i++){
//        float x0 = pts_in[i][0];
//        float y0 = pts_in[i][1];
//        float dx = pts_in[i+1][0] - x0;
//        float dy = pts_in[i+1][1] - y0;
//        float dn = mySqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
//        //float dn = sqrt(dx*dx+dy*dy);//求平方根 求弧长积分，即输入线段前后两点距离
//        dx /= dn;//此点处的cosθ
//        dy /= dn;//此点处的sinθ

//        //每次等距采样处理
//        while(remain < dn && len < *num2){
//            x0 += dx * remain;
//            pts_out[len][0] = x0;
//            y0 += dy * remain;
//            pts_out[len][1] = y0;

//            len++;
//            dn -= remain;
//            remain = dist;
//        }
//        remain -= dn;//当跨越一点采样折线距离近似直线
//    }
//    *num2 = len;
//}
///*************************************************************************
// *  函数名称：void local_angle_points();
// *  功能说明：点集局部角度变化率
// *  参数说明：输入边线数组，边线点个数，输出角度变换率数组，两点间固定距离
// *  函数返回：无
// *  修改时间：2022年11月23日
// *  备    注：计算该点前后两点连线的斜率作为该点切线斜率的近似
// *************************************************************************/
//void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist) {
//    for (int i = 0; i < num; i++) {
//        if (i <= 0 || i >= num - 1) {
//            angle_out[i] = 0;
//            continue;
//        }
//        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
//        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
//        float dn1 = mySqrt(dx1 * dx1 + dy1 * dy1);//此点与前一点间距离
//        //float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);//此点与前一点间距离
//        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
//        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
//        float dn2 = mySqrt(dx2 * dx2 + dy2 * dy2);//此点与后一点间距离
//        //float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);//此点与后一点间距离
//        float c1 = dx1 / dn1;//cosθ1
//        float s1 = dy1 / dn1;//sinθ1
//        float c2 = dx2 / dn2;//cosθ2
//        float s2 = dy2 / dn2;//sinθ2
//        //化简展开得到角公式(k1-k2)/(1+k1k2),即tan的差角公式==>得出两点间角度变化==>曲线斜率变化
//        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);//atan2f(y,x):返回以弧度表示的y/x反正切
//    }
//}
///*************************************************************************
// *  函数名称：void nms_angle();
// *  功能说明：角度变化率非极大抑制
// *  参数说明：输入角度变化率数组，角度变换数组总个数，抑制后的角度变化数组，点集奇数核
// *  函数返回：无
// *  修改时间：2022年11月25日
// *  备    注：上交方案：kernel = 21，保存极大值的数组，即一段范围内斜率变换最大的值，非极大值均设为零
// *************************************************************************/
//void nms_angle(float angle_in[], int num, float angle_out[], int kernel){
//    zf_assert(kernel % 2 == 1);
//    int half = kernel / 2;
//    for (int i = 0; i < num; i++) {
//        angle_out[i] = angle_in[i];
//        for (int j = -half; j <= half; j++) {
//            //fabs():求绝对值
//            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
//                angle_out[i] = 0;
//                break;
//            }
//        }
//    }
//}
///*************************************************************************
// *  函数名称：void track_leftline();void track_rightline();
// *  功能说明：左右边线跟踪中线
// *  参数说明：输入等距采样边线数组，边线总个数，中线数组，两点间固定距离，一半赛道宽对应的像素个数
// *  函数返回：无
// *  修改时间：2022年11月25日
// *  备    注：
// *************************************************************************/
//void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist){
//    for (int i = 0; i < num; i++) {
//            float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
//            float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
//            float dn = mySqrt(dx * dx + dy * dy);
//            //float dn = sqrt(dx * dx + dy * dy);
//            dx /= dn;//cosθ
//            dy /= dn;//sinθ
//            pts_out[i][0] = pts_in[i][0] + dy * dist;
//            pts_out[i][1] = pts_in[i][1] - dx * dist;
//        }
//}
//void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist){
//    for (int i = 0; i < num; i++) {
//           float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
//           float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
//           float dn = mySqrt(dx * dx + dy * dy);
//           //float dn = sqrt(dx * dx + dy * dy);
//           dx /= dn;
//           dy /= dn;
//           pts_out[i][0] = pts_in[i][0] - dy * dist;
//           pts_out[i][1] = pts_in[i][1] + dx * dist;
//       }
//}
///*************************************************************************
// *  函数名称：void sobel3();
// *  功能说明：3*3 sobel算法提取
// *  参数说明：框型核，图像数组
// *  函数返回：无
// *  修改时间：2022年10月21日
// *  备    注：
// *************************************************************************/
//void sobel3(image_t *img0, image_t *img1) {
//    zf_assert(img0 && img0->data);
//    zf_assert(img1 && img1->data);
//    zf_assert(img0->width == img1->width && img0->height == img1->height);
//    zf_assert(img0 != img1 && img0->data != img1->data);

//  int gx, gy;
//  // 先遍历y后遍历x比较cache-friendly
//  for (int y = 1; y < img0->height - 1; y++) {
//    for (int x = 1; x < img0->width - 1; x++) {
//      //计算梯度
//      gx = (-1 * AT(img0, x - 1, y - 1) + 1 * AT(img0, x + 1, y - 1) +
//            -2 * AT(img0, x - 1, y) + 2 * AT(img0, x + 1, y) +
//              -1 * AT(img0, x - 1, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 4;
//      gy = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
//            -1 * AT(img0, x - 1, y + 1) - 2 * AT(img0, x, y + 1) - 1 * AT(img0, x + 1, y + 1)) / 4;
//      AT(img1, x, y) = (abs(gx) + abs(gy)) / 2;
//    }
//  }
//}
///*************************************************************************
// *  函数名称：void erode3();
// *  功能说明：3*3腐蚀 黑点腐蚀扩张
// *  参数说明：框型核，图像数组
// *  函数返回：无
// *  修改时间：2022年10月8日
// *  备    注：
// *************************************************************************/
//void erode3(image_t *img0, image_t *img1){
//    zf_assert(img0 && img0->data);
//    zf_assert(img1 && img1->data);
//    zf_assert(img0->width == img1->width && img0->height == img1->height);
//    zf_assert(img0 != img1 && img0->data != img1->data);

//    int min_value;
//    // 先遍历y后遍历x比较cache-friendly
//    for (int y = 1; y < img0->height - 1; y++) {
//        for (int x = 1; x < img0->width - 1; x++) {
//            min_value = 255;
//            for (int dy = -1; dy <= 1; dy++) {
//                for (int dx = -1; dx <= 1; dx++) {
//                    if (AT(img0, x + dx, y + dy) < min_value)
//                      min_value = AT(img0, x + dx, y + dy);
//                }
//            }
//                    AT(img1, x, y) = min_value;
//        }
//    }
//}
///*************************************************************************
// *  函数名称：void dilate3();
// *  功能说明：3*3膨胀 亮点膨胀去除黑点
// *  参数说明：框型核，图像数组
// *  函数返回：无
// *  修改时间：2022年10月8日
// *  备    注：
// *************************************************************************/
//void dilate3(image_t *img0, image_t *img1) {
//    zf_assert(img0 && img0->data);
//    zf_assert(img1 && img1->data);
//    zf_assert(img0->width == img1->width && img0->height == img1->height);
//    zf_assert(img0 != img1 && img0->data != img1->data);

//    int max_value;
//    // 先遍历y后遍历x比较cache-friendly
//    for (int y = 1; y < img0->height - 1; y++) {
//        for (int x = 1; x < img0->width - 1; x++) {
//            max_value = 0;
//            for (int dy = -1; dy <= 1; dy++) {
//                for (int dx = -1; dx <= 1; dx++) {
//                    if (AT(img0, x + dx, y + dy) > max_value) max_value = AT(img0, x + dx, y + dy);
//                }
//            }
//                    AT(img1, x, y) = max_value;
//        }
//    }
//}
///*************************************************************************
// *  函数名称：int regression();
// *  功能说明：线性回归（最小二乘法）拟合斜率
// *  参数说明：输入边线数组
// *  函数返回：斜率
// *  修改时间：2023年6月1日
// *  备    注： 线性回归（最小二乘法）拟合斜率，用于逆透视前的图像
// *************************************************************************/
//int regression(int pts_in[][2])
//{

//    int num = sizeof(pts_in) / sizeof(pts_in[0]);//求数组的长度
//      int i=0,SumX=0,SumY=0,SumLines = 0;
//      float SumUp=0,SumDown=0,avrX=0,avrY=0,B,A;
//      SumLines=pts_in[0][1]-pts_in[num][1];   // pts_in[0][1] 为开始行， //pts_in[num][1] 结束行 //SumLines

//      for(i=0;i<=num;i++)
//      {
//        SumY+=pts_in[i][1];
//        SumX+=pts_in[i][0];    //对边界X,Y求和
//      }
//      avrY=SumY/SumLines;     //Y的平均值
//      avrX=SumX/SumLines;     //X的平均值
//      SumUp=0;
//      SumDown=0;
//      for(i=0;i<num;i++)
//      {
//        SumUp+=(pts_in[i][0]-avrX)*(pts_in[i][1]-avrY);
//        SumDown+=(pts_in[i][1]-avrY)*(pts_in[i][1]-avrY);
//      }
//      if(SumDown==0)
//        B=0;
//      else
//        B=(int)(SumUp/SumDown);
//        A=(SumX-B*SumY)/SumLines;  //截距
//        return B;  //返回斜率
//}


