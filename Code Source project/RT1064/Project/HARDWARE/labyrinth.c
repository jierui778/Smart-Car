///*
// * Image_Control.c
// *
// *  Created on: 2022��8��27��
// *      Author:  HQC
// */
//#include "string.h"
//#include "zf_common_headfile.h"
//#include "math.h"
//#include "labyrinth.h"
//#define AT                       AT_IMAGE
//#define AT_CLIP(img, x, y)      AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1));

//uint8_t threshold;//�����ֵ

//float aim_distance;

//int drawline0[2];//������ʼ������
//int drawline1[2];//�����յ�����

//float N_drawline0[2];//������ʼ������
//float N_drawline1[2];//�����յ�����

////��͸�Ӳ�������
//float left_line[POINTS_MAX_LEN][2];
//float right_line[POINTS_MAX_LEN][2];
//int left_num, right_num;
////ƴ������
//float splicing_leftline[POINTS_MAX_LEN][2];
//float splicing_rightline[POINTS_MAX_LEN][2];
//int splicing_leftline_num,splicing_rightline_num;
////ƴ������ƽ������
//float splicing_leftline_center[POINTS_MAX_LEN][2];
//float splicing_rightline_center[POINTS_MAX_LEN][2];
//int splicing_leftline_center_num,splicing_rightline_center_num;

//uint8  Image_Use[UROW][UCOL];//�Ҷ�ͼ��5k
//uint8_t  Image_Use_Robert[UROW][UCOL];//��ֵ��ͼ��

////float pix[2];//���Բ�ֵ

//image_t img_raw = DEF_IMAGE(NULL, UCOL, UROW);

////���ұ߶���
//uint8 loseline0;
//uint8 loseline1;

//uint8 touch_boundary0;              //������ߵ�ͼ����߽�
//uint8 touch_boundary1;              //�ұ����ߵ�ͼ���ұ߽�

//uint8 touch_boundary_up0;              //������ߵ�ͼ���ϱ߽�
//uint8 touch_boundary_up1;              //�ұ����ߵ�ͼ���ϱ߽�

//int begin_x0,begin_y0;              //����ƫ�Ƶ�
//int begin_x1,begin_y1;              //����ƫ�Ƶ�

//float xielv_left_y_to_end,xielv_right_y_to_end;                 //����͸�Ӻ������ϵ����б��
////��ʼ��������
//int ipts0[POINTS_MAX_LEN][2];//��ű������ݣ���
//int ipts1[POINTS_MAX_LEN][2];//��ű������ݣ��ң�
//int ipts0_num;//��ű������ص����(��)
//int ipts1_num;//��ű������ص����(��)
//// �任�����ұ���
//float rpts0[POINTS_MAX_LEN][2];
//float rpts1[POINTS_MAX_LEN][2];
//int rpts0_num, rpts1_num;
//// �任�����ұ���+�˲�
//float rpts0b[POINTS_MAX_LEN][2];
//float rpts1b[POINTS_MAX_LEN][2];
//int rpts0b_num, rpts1b_num;
//// �任�����ұ���+�Ⱦ����
//float rpts0s[POINTS_MAX_LEN][2];
//float rpts1s[POINTS_MAX_LEN][2];
//int rpts0s_num, rpts1s_num;
//// ���ұ��߾ֲ��Ƕȱ仯��
//float rpts0a[POINTS_MAX_LEN];
//float rpts1a[POINTS_MAX_LEN];
//int rpts0a_num, rpts1a_num;
//// ���ұ��߾ֲ��Ƕȱ仯��+�Ǽ�������
//float rpts0an[POINTS_MAX_LEN];
//float rpts1an[POINTS_MAX_LEN];
//int rpts0an_num, rpts1an_num;
//// ��/������
//float rptsc0[POINTS_MAX_LEN][2];
//float rptsc1[POINTS_MAX_LEN][2];
//int rptsc0_num, rptsc1_num;
//// ���Ҹ��ٺ������
//float (*rpts)[2];
//int rpts_num;
//// ��һ����������
//float rptsn[POINTS_MAX_LEN][2];
//int rptsn_num;

//// Y�ǵ�
//int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
//bool Ypt0_found, Ypt1_found;
//// L�ǵ�
//int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
//bool Lpt0_found, Lpt1_found;
//// ��L�ǵ�
//int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
//bool N_Lpt0_found, N_Lpt1_found;
//// ��ֱ��
//bool is_straight0, is_straight1;
//// ���
//bool is_turn0, is_turn1;

//int N_Xfound_num;//����������̣�˫��L����

//// ��ǰѲ��ģʽ����������
//enum track_type_e track_type = TRACK_RIGHT;

//float error[1];
//float ave_error;//�ٶȿ����������
//// ���Ԥê��λ��
//int aim_idx[1] ;


//// ����Զê��ƫ��ֵ
//float dx[1] ;
//float dy[1] ;
//float dn[1] ;


//// �����ǽ���Զ��,�ɽ��ƹ���Stanley�㷨,����ײ·��
//// �����ê��ƫ��ֵ
//float dx_near;
//float dy_near ;
//float dn_near ;
//float pure_angle;

////image_t img_change = DEF_IMAGE(NULL, RESULT_COL, RESULT_ROW);//������͸�Ӻ�ͼ�������ʱδ��ֵ���ݣ�
////uint8_t img_thres_data[RESULT_COL][RESULT_ROW];//��ֵ��ͼ������
////image_t img_thres = DEF_IMAGE((uint8_t *) img_thres_data, RESULT_COL, RESULT_ROW);


////ͼ�����ܺ���
//void process_image(void)
//{
//    int start,end,start_last;
//    start = system_getval_us();

//    //ͼ������ѡ��
//    /****
//    //==========������ ���������˲����������������float&int
//    compressimage(); //ͼ��ѹ��,������ͼ����Image_Use,�������￪ʼ�ɼ���һ֡
//    S_Sobel( Image_Use, Image_Use_Robert ,thres);//ȫ��Sobel�ö�ֵͼ

//    img_raw.data = Image_Use_Robert;

//    Find_Borderline();//��ȡͼƬ�ı���
//    //�Ա��߷������Ĵ��任
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
//    //���������˲�
//    blur_points(rpts0,rpts0_num,rpts0b,(int) round(line_blur_kernel));
//    rpts0b_num = ipts0_num;
//    blur_points(rpts1,rpts1_num,rpts1b,(int) round(line_blur_kernel));
//    rpts1b_num = ipts1_num;

//    //==========������ ���������˲����������������float&int
//    ****/

//    /****/
//    //�Ա�����͸��
//    //==========������ ���������˲����������������float&int
////    ����TC377�õ�������canny
//      compressimage(); //ͼ��ѹ��,������ͼ����Image_Use,�������￪ʼ�ɼ���һ֡   0.6ms
////
////
////    //��ֵ��
////    if(choose_Binarization == 0) //��sobel����Ҫ����ϵͳ�����������ڿ���ʾ����
////          S_Sobel( Image_Use, Image_Use_Robert ,thres);//ȫ��Sobel�ö�ֵͼ(������) 2.8ms
//    if(choose_Binarization == 1)
//        Binarization(ostu());                        //0.74ms
////    else if(choose_Binarization == 0) //��sobel����Ҫ����ϵͳ�����������ڿ���ʾ����
////        S_Sobel( Image_Use, Image_Use_Robert ,thres);//ȫ��Sobel�ö�ֵͼ(������) 2.8ms

//   else if(choose_Binarization == 0) //��sobel����Ҫ����ϵͳ�����������ڿ���ʾ����
//       S_Sobel( Image_Use, Image_Use_Robert ,thres);//ȫ��Sobel�ö�ֵͼ(������) 2.8ms


//    img_raw.data = Image_Use_Robert;//��������
////    img_raw.data = Image_Use;//�Ͻ�����

//    Find_Borderline();//��ȡͼƬ�ı��� ��������0.1ms���� ;�Ͻ�������1.2ms


//    //�Ա���͸�ӱ任
//    Pespective(ipts0,ipts0_num ,  rpts0);
//    rpts0_num = ipts0_num;
//    Pespective(ipts1,ipts1_num ,  rpts1);
//    rpts1_num = ipts1_num;

//    //���������任
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

//    //���������˲�
//    blur_points(rpts0,rpts0_num,rpts0b,(int) round(line_blur_kernel));
//    rpts0b_num = rpts0_num;
//    blur_points(rpts1,rpts1_num,rpts1b,(int) round(line_blur_kernel));
//    rpts1b_num = rpts1_num;
//    //==========������ ���������˲����������������float&int
//    /****/
//    /****
//    //==========����һ ���������˲����������������float&int

//    //compressimage(); //ͼ��ѹ��,������ͼ����Image_Use,�������￪ʼ�ɼ���һ֡
//    S_Sobel(ImageUsed, Image_Use_Robert ,thres);
//    img_raw.data = Image_Use_Robert;//����һ ��͸��ͼ�����ݸ�ֵ

//    Find_Borderline();//��ȡͼƬ�ı���
//    //���������˲�
//    blur_points(ipts0,ipts0_num,rpts0b,(int) round(line_blur_kernel));
//    rpts0b_num = ipts0_num;
//    blur_points(ipts1,ipts1_num,rpts1b,(int) round(line_blur_kernel));
//    rpts1b_num = ipts1_num;
//    //==========����һ ���������˲����������������float&int
//    ****/
//    // ���ߵȾ���� numΪ��͸�Ӻ�ʵ�ʾ���㣬��������Ϊ0.02*102=2cm
//    rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);//������ĳ��� ���Ⱦ��������ߵ����
//    resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
//    rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
//    resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);
//    // ���߾ֲ��Ƕȱ仯�� round():��������
//    local_angle_points(rpts0s, rpts0s_num, rpts0a, (int) round(angle_dist / sample_dist));
//    rpts0a_num = rpts0s_num;
//    local_angle_points(rpts1s, rpts1s_num, rpts1a, (int) round(angle_dist / sample_dist));
//    rpts1a_num = rpts1s_num;
//    // �Ƕȱ仯�ʷǼ�������
//    nms_angle(rpts0a, rpts0a_num, rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);
//    rpts0an_num = rpts0a_num;
//    nms_angle(rpts1a, rpts1a_num, rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
//    rpts1an_num = rpts1a_num;

//    // �������߸���
//    track_leftline(rpts0s, rpts0s_num, rptsc0, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//    rptsc0_num = rpts0s_num;
//    track_rightline(rpts1s, rpts1s_num, rptsc1, (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//    rptsc1_num = rpts1s_num;


//    find_corners();// �ǵ���ȡ&ɸѡ


//    //============��Ԫ��������ͼ����============

//    // Ԥ�����,��̬Ч������
//    aim_distance = aim_distance_flash;
//    //�����ٶȵ���ǰհ������   �ٶȸ���80  ����Ϊ����  ǰհ�ӳ�
//    //aim_idx_d = (nowspeed - MinSpeed) * 0.2 +0.5 ;

//    // �������٣��л�Ѳ�߷���  ������Բ
//    if (rpts0s_num < rpts1s_num / 2 && rpts0s_num < 60) {         //�������߱��ұ�����һ�룬ѭ��
//        track_type = TRACK_RIGHT;
//    } else if (rpts1s_num < rpts0s_num / 2 && rpts1s_num < 60) {  //����ұ��߱��������һ�룬ѭ��
//        track_type = TRACK_LEFT;
//    } else if (rpts0s_num < 20 && rpts1s_num > rpts0s_num) {      //������������20�����ұ���������ߣ�ѭ��
//        track_type = TRACK_RIGHT;
//    } else if (rpts1s_num < 20 && rpts0s_num > rpts1s_num) {      //����ұ�������20��������������ұߣ�ѭ��
//        track_type = TRACK_LEFT;
//    }

//    //ǰ0.6mִ�г���
//    if(garage_type != GARAGE_NONE && Z.all_length < ENCODER_PER_METER*0 && podao_type == podao_NONE && block_type == BLOCK_NONE )
//    {
//        out_garage();
//    }

//    //��Ԫ�ؼ��

//    //��������߼��(���������ȼ��ߣ����ȼ��)
//    if(/*Z.all_length > ENCODER_PER_METER &&*/ garage_type == GARAGE_NONE && break_type == BREAK_NONE && block_type == BLOCK_NONE && podao_type == podao_NONE )
//        check_garage();
//    //���߼��
//    if (garage_type == GARAGE_NONE && cross_type == CROSS_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE
//            && Lpt0_found && loseline1/*rpts0s_num/2 > rpts1s_num*/ && !Lpt1_found && break_type == BREAK_NONE )
//        check_half_left();
//    if (garage_type == GARAGE_NONE && cross_type == CROSS_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE
//            && Lpt1_found && loseline0/*rpts1s_num/2 > rpts0s_num*/ && !Lpt0_found && break_type == BREAK_NONE)
//        check_half_right();
//    //˫�߲����߼��
//    if (garage_type == GARAGE_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE && break_type == BREAK_NONE)
//        check_cross();
//    if (garage_type == GARAGE_NONE && cross_type == CROSS_NONE && podao_type == podao_NONE && block_type == BLOCK_NONE && break_type == BREAK_NONE)              //���ǳ��⡢ʮ�� , �ż��Բ��
//        check_circle();






//    //��Ԫ��ִ��
//    if (garage_type != GARAGE_NONE && (garage_type != GARAGE_OUT_LEFT || garage_type != GARAGE_OUT_RIGHT)) run_garage();
//    if (cross_type != CROSS_NONE) run_cross();
//    if (circle_type != CIRCLE_NONE) run_circle();
////    if (block_type == BLOCK_NONE)
////    {
////        //�Խ����߽ضϴ���
////        if (N_Lpt0_found) {
////            rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
////        }
////        if (N_Lpt1_found) {
////            rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
////        }
////        aim_distance = 0.4;//�����rpts_id=15
////    }


//    //============��Ԫ��������ͼ����============


//    // �������߸���
//    if (cross_type != CROSS_IN_DOUBLE && cross_type != CROSS_INHALF_LEFT && cross_type != CROSS_INHALF_RIGHT) {             //����ʮ��
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
//        //ʮ�ָ���Զ�߿��� ��ַƽ��+����ɾ��
//        if (track_type == TRACK_LEFT) {
//            track_leftline(far_rpts0s + far_Lpt0_rpts0s_id, far_rpts0s_num - far_Lpt0_rpts0s_id, rpts,
//                           (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//            rpts_num = far_rpts0s_num - far_Lpt0_rpts0s_id;
//        } else {
//            track_rightline(far_rpts1s + far_Lpt1_rpts1s_id, far_rpts1s_num - far_Lpt1_rpts1s_id, rpts,
//                            (int) round(angle_dist / sample_dist), pixel_per_meter * ROAD_WIDTH / 2);
//            rpts_num = far_rpts1s_num - far_Lpt1_rpts1s_id;
//        }
////        //ʮ�ֲ��߷���
////        if (track_type == TRACK_LEFT) {
////            rpts = rptsc0;
////            rpts_num = rptsc0_num;
////        } else {
////            rpts  = rptsc1;
////            rpts_num = rptsc1_num;
////        }
//    }

//    //��������ʼ�� ���ݷ�����ͬ������
//    float cx, cy; //˼����������͸�ӳ���������ʵ����ϵ�����ǰ�ͼ��������м���ʼ���ΪС�����ĵ�᲻���һЩ���������ģ���cx��cyΪ����ͷ����������������
////    cx = UCOL/2+0.5;//����һ
////    cy = UROW;
//    Pespective_xy( UCOL/2+0.5 ,begin_y ,  &cx, &cy);//������
////    rot_xy( UCOL/2+0.5 ,begin_y ,  &cx, &cy);//������



//    //�������(��ʼ�����߹�һ��)
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
//    // �����е㣬ͬʱ����㲻����󼸸���
//    if (begin_id >= 0 && rpts_num - begin_id >= 3) {
//        // ��һ������
//        rpts[begin_id][0] = cx;
//        rpts[begin_id][1] = cy;
//        rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);
//        resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);



//        /***************����Զ��ƫ���ֵ*************/
//        // Ԥê��λ�� ��34����Ϊ����ƫ�� ע�⣺����о��복ͷԼ10cm���룬��������ͷ25cm���� ��Ԥ�����ʵ��Ϊ60cm
////        int i;
////        for(i = 0;i<5;i++){
////            aim_idx[i] = clip(round(aim_dist[i] / sample_dist), 0, rptsn_num - 1);
////
////
////            // ����Զê��ƫ��ֵ      cx��cyΪ����ͷ���������������� 0,-7
////            dx[i] = rptsn[aim_idx[i]][0] - cx;
////            dy[i] = cy - rptsn[aim_idx[i]][1] ;
////            dn[i] = sqrt(dx[i] * dx[i] + dy[i] * dy[i]);
////            //dn = mySqrt(dx * dx + dy * dy);
////            //error>0 �� error<0 �� ����Ϊ�Ƕ��ƻ���Ϊ������
////            error[i] = atan2f(dx[i], -dy[i]) * 180 / PI;//1����=180/�ж� 1��=��/180����
////            //assert(!isnan(error));
////
////
////        }
////        ave_error = error[0]*0.3 + error[1]*0.27 + error[2]*0.21 + error[3]*0.14 + error[4]*0.08;//��̬�ֲ�
//        /***************����Զ��ƫ���ֵ*************/

////        aim_idx[0] = clip(round(aim_dist[0] / sample_dist), 0, rptsn_num - 1);

//        aim_idx[0] = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);


//        // ����Զê��ƫ��ֵ      cx��cyΪ����ͷ���������������� 0,-7
//        dx[0] = rptsn[aim_idx[0]][0] - cx;
//        dy[0] = rptsn[aim_idx[0]][1] - cy;
//        dn[0] = sqrt(dx[0] * dx[0] + dy[0] * dy[0]);
//        //dn = mySqrt(dx * dx + dy * dy);
//        //error<0(���Ϊ��) �� error>0(���Ϊ��) �� ����Ϊ�Ƕ��ƻ���Ϊ������
//        error[0] = atan2f(dx[0], dy[0]) * 180 / PI;//1����=180/�ж� 1��=��/180����
//        //assert(!isnan(error));



//        ave_error = 0;
//        // Զ��ê���ۺϿ���
//        //angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
//        // ����ƫ�����PD����
//        //float angle = pid_solve(&servo_pid, error);

//        // �������㷨(ֻ����Զ��)���ֿ����ò���
////        pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / 1;
//      } else {
//          // ���ߵ����(��������)���򲻿��ƶ��
//          rptsn_num = 0;
//      }

//     end = system_getval_us();
//     //timer_cost = end - start;                              //����ÿһ֡��ʱ��

//}
//// �ǵ���ȡ&ɸѡ Y�ǵ����ڼ���µ���L�ǵ����ڼ��Բ����ʮ�֡�·�ϡ���·
//void find_corners(){
//    // ʶ��Y,L�յ�
//    Ypt0_found = Ypt1_found = Lpt0_found = Lpt1_found = N_Lpt1_found = N_Lpt0_found = false;
//    is_straight0 = rpts0s_num > 1.6 / sample_dist;           //��͸��֮��ı��߸�������80������ֱ��,Ҳ���Ǳ��߱߳�����0.8m,������ֱ��ǰ0.8m
//    is_straight1 = rpts1s_num > 1.6 / sample_dist;

//    //����߽Ƕ�
//    for (int i = 1; i < rpts0s_num; i++) {              //ֻҪ������ĽǶȷ��ϵģ�Y��L��Ϊtrue,Ȼ��Ͳ��ڼ������Y��L
//        if (rpts0an[i] == 0) continue;                      //���¼��ֻ����ֵ������ֵ��
//        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
//        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);
//        float conf = fabs(rpts0a[i]) - (fabs(rpts0a[im1]) + fabs(rpts0a[ip1])) / 2;

//        //Y�ǵ���ֵ                 (30*  ,   50*)    ���Ҿ�����40����֮�� (ע��30.ת��������)
//        if (Ypt0_found == false && 6. / 180. * PI < conf && conf < 15. / 180. * PI && i < 0.8 / sample_dist) {
//            Ypt0_rpts0s_id = i;
//            Ypt0_found = true;
//        }
//        //L�ǵ���ֵ                 (55*  ,    140*)  ���Ҿ�����40����֮�� �޸ľ���Ϊ1.2m�����ж�·�Ͼ���
//        if (Lpt0_found == false && 45. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.8 / sample_dist) {

//            Lpt0_rpts0s_id = i;
//            Lpt0_found = true;

//            if(rpts0a[Lpt0_rpts0s_id] < 0)              //��һ��L����������ҹյģ�������L��
//            {
//                N_Lpt0_rpts0s_id = Lpt0_rpts0s_id;
//                N_Lpt0_found = true;
////                //����L�ǵ��߽ض�
////                rptsc0_num = rpts0s_num = N_Lpt0_rpts0s_id;
//                Lpt0_found = false;
//            }

//        }
//        //��ֱ����ֵ
//        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight0 = false;           //ֻҪ�м��д�Ƕȣ��Ͳ��ǳ�ֱ��
//        if (Ypt0_found == true && Lpt0_found == true && is_straight0 == false) break;
//    }

//    //�ұ��߽Ƕ�
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

//            if(rpts1a[Lpt1_rpts1s_id] > 0)              //��һ��L�����������յģ�������L��
//            {
//                N_Lpt1_rpts1s_id = Lpt1_rpts1s_id;
//                N_Lpt1_found = true;
////                //����L�ǵ��߽ض�
////                rptsc1_num = rpts1s_num = N_Lpt1_rpts1s_id;
//                Lpt1_found = false;
//            }
//        }

//        if (conf > 5. / 180. * PI && i < 1.4 / sample_dist) is_straight1 = false;
//        if (Ypt1_found == true && Lpt1_found == true && is_straight1 == false) break;
//    }

//    // Y����μ��,�������ǵ���뼰�ǵ���ſ�����
//    if (Ypt0_found && Ypt1_found) {
//        //==================================================================ԭ���
//        float dx = rpts0s[Ypt0_rpts0s_id][0] - rpts1s[Ypt1_rpts1s_id][0];
//        float dy = rpts0s[Ypt0_rpts0s_id][1] - rpts1s[Ypt1_rpts1s_id][1];
//        float dn = sqrtf(dx * dx + dy * dy);
//        if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)             //������벻�ܴ���60cm  (����40cm)
//        {
//            float dwx = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                        rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//            float dwy = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                        rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//            float dwn = sqrtf(dwx * dwx + dwy * dwy);
//            if (!(dwn > 0.7 * pixel_per_meter &&                            //������Y��֮���50�� ��֮��ľ���С��0.7m
//                  rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Ypt0_rpts0s_id][0] &&
//                  rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Ypt1_rpts1s_id][0]))
//            {
//                Ypt0_found = Ypt1_found = false;
//            }
//        } else
//        {
//            Ypt0_found = Ypt1_found = false;
//        }
//        //zqkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk�ļ��

//        float dx_temp ,dy_temp ;



//        dx_temp = rpts0s[Ypt0_rpts0s_id][0] - rpts0s[rpts0s_num-5][0];
//        dy_temp = rpts0s[Ypt0_rpts0s_id][1] - rpts0s[rpts0s_num-5][1];
//        xielv_left_y_to_end =  dy_temp / dx_temp;                                   //������㷨����ʮ�ֵ����������dy �п���Ϊ0������dy������


//        dx_temp = rpts1s[Ypt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
//        dy_temp = rpts1s[Ypt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
//        xielv_right_y_to_end =  dy_temp / dx_temp;                                   //������㷨����ʮ�ֵ����������dy �п���Ϊ0������dy������


//    }
//    // L����μ�飬����ģʽ�����, ����L�ǵ���뼰�ǵ���ſ�����
//    if (garage_type == GARAGE_NONE) {
//        if (Lpt0_found && Lpt1_found) {
//            float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//            float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//            float dn = sqrtf(dx * dx + dy * dy);
//            if (fabs(dn - 0.40 * pixel_per_meter) < 0.20 * pixel_per_meter)            //������벻�ܴ���60cm  (����40cm)
//            {
//                float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//                float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                            rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//                float dwn = sqrtf(dwx * dwx + dwy * dwy);
//                if (!(dwn > 0.5 * pixel_per_meter &&                                //����ֱ�ǵ���������50�����������0.7m��
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
//            xielv_left_y_to_end =  dy_temp / dx_temp;                                   //������㷨����ʮ�ֵ����������dy �п���Ϊ0������dy������  һ���Ǹ�


//            dx_temp = rpts1s[Lpt1_rpts1s_id][0] - rpts1s[rpts1s_num-5][0];
//            dy_temp = rpts1s[Lpt1_rpts1s_id][1] - rpts1s[rpts1s_num-5][1];
//            xielv_right_y_to_end =  dy_temp / dx_temp;                                   //������㷨����ʮ�ֵ����������dy �п���Ϊ0������dy������


//        }
//    }

//    //�󳵿�ڶ��μ��
//    if (Lpt0_found && N_Lpt1_found) {                   //���������󳵿�
//        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//        float dn = sqrtf(dx * dx + dy * dy);
//        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //������벻�ܴ���60cm  (����45cm)
//        {
//            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//            float dwn = sqrtf(dwx * dwx + dwy * dwy);
//            if (!(dwn < 0.5 * pixel_per_meter &&                                //�����󳵿���������50��������С��0.5m��
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


//    //�ҳ���ڶ��μ��
//    if (Lpt1_found && N_Lpt0_found) {                   //���������ҳ���
//        float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
//        float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
//        float dn = sqrtf(dx * dx + dy * dy);
//        if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)            //������벻�ܴ���60cm  (����45cm)
//        {
//            float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
//            float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
//                        rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
//            float dwn = sqrtf(dwx * dwx + dwy * dwy);
//            if (!(dwn < 0.5 * pixel_per_meter &&                                //�����󳵿���������50��������С��0.5m��
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
// *  �������ƣ�void compressimage (void);
// *  ����˵����ѹ��ͼ��
// *  ����˵������
// *  �������أ���
// *  �޸�ʱ�䣺2022��12��17��
// *  ��    ע��  IMAGEWΪԭʼͼ��Ŀ��
// *       IMAGEHΪԭʼͼ��ĸ߶�
// *        mt9v03x_image[MT9V03X_H][MT9V03X_W]Ϊ��ȡ��ͼ�񣬽���ѹ����Image_Use[UROW][UCOL]
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
//      mt9v03x_finish_flag = 0;  //ʹ����һ֡DMA�����ͼ��ͼ��  ���Կ�ʼ������һ֡

//}
///*************************************************************************
// *  �������ƣ�void uncompressimage (void);
// *  ����˵������ѹͼ��
// *  ����˵������
// *  �������أ���
// *  �޸�ʱ�䣺2022��11��30 ��
// *  ��    ע����ҪӦ����ͼ����ʾ�ϣ�display.c/keyboard.cӦ��
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
// *  �������ƣ�uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)��
// *  ����˵������ȡͼ����ֵ
// *  ����˵����ͼ���ַ
// *  �������أ���ֵ
// *  �޸�ʱ�䣺2022��9��30��
// *  ��    ע��
// *************************************************************************/
//uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)
//{
//    #define GrayScale 256
//    int pixelCount[GrayScale] = {0};//ÿ���Ҷ�ֵ��ռ���ظ���
//    float pixelPro[GrayScale] = {0};//ÿ���Ҷ�ֵ��ռ�����ر���
//    int i,j;
//    int Sumpix = width * height;   //�����ص�
//    uint8 threshold = 0;
//    uint8* data = image;  //ָ���������ݵ�ָ��

//    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
//    for (i = 0; i < height; i++)
//    {
//        for (j = 0; j < width; j++)
//        {
//            pixelCount[(int)data[i * width + j]]++;  //������ֵ��Ϊ����������±�
//          //   pixelCount[(int)image[i][j]]++;    ������ָ�������
//        }
//    }
//    float u = 0;
//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //����ÿ������������ͼ���еı���
//        u += i * pixelPro[i];  //��ƽ���Ҷ�
//    }


//    float maxVariance=0.0;  //�����䷽��
//    float w0 = 0, avgValue  = 0;  //w0 ǰ������ ��avgValue ǰ��ƽ���Ҷ�
//    for(int i = 0; i < 256; i++)     //ÿһ��ѭ������һ��������䷽����� (����for����Ϊ1��)
//    {
//        w0 += pixelPro[i];  //���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷ�������ռ����ͼ��ı�����ǰ������
//        avgValue  += i * pixelPro[i];

//        float variance = pow((avgValue/w0 - u), 2) * w0 /(1 - w0);    //��䷽��
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
//    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
//    int16 MinValue, MaxValue;
//    uint8 Threshold = 0;

//        for (y = 0; y < 256; y++)
//        {
//            HistGram[y] = 0; //��ʼ���Ҷ�ֱ��ͼ
//        }
//        for (y = 0; y < MT9V03X_H; y++)
//        {
//            for (x = 0; x < MT9V03X_W; x++)
//            {
//                HistGram[mt9v03x_image[y][x]]++; //ͳ��ÿ���Ҷ�ֵ�ĸ�����Ϣ
//            }
//        }


//    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //��ȡ��С�Ҷȵ�ֵ
//    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //��ȡ���Ҷȵ�ֵ

//    if (MaxValue == MinValue)
//    {
//        return MaxValue;          // ͼ����ֻ��һ����ɫ
//    }
//    if (MinValue + 1 == MaxValue)
//    {
//        return MinValue;      // ͼ����ֻ�ж�����ɫ
//    }

//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        Amount += HistGram[Y];        //  ��������
//    }

//    PixelIntegral = 0;
//    for (Y = MinValue; Y <= MaxValue; Y++)
//    {
//        PixelIntegral += HistGram[Y] * Y;//�Ҷ�ֵ����
//    }
//    SigmaB = -1;
//    for (Y = MinValue; Y < MaxValue; Y++)
//    {
//        PixelBack = PixelBack + HistGram[Y];    //ǰ�����ص���
//        PixelFore = Amount - PixelBack;         //�������ص���
//        OmegaBack = (double)PixelBack / Amount;//ǰ�����ذٷֱ�
//        OmegaFore = (double)PixelFore / Amount;//�������ذٷֱ�
//        PixelIntegralBack += HistGram[Y] * Y;  //ǰ���Ҷ�ֵ
//        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//�����Ҷ�ֵ
//        MicroBack = (double)PixelIntegralBack / PixelBack;//ǰ���ҶȰٷֱ�
//        MicroFore = (double)PixelIntegralFore / PixelFore;//�����ҶȰٷֱ�
//        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
//        if (Sigma > SigmaB)//����������䷽��g
//        {
//            SigmaB = Sigma;
//            Threshold = Y;
//        }
//        if (Sigma < SigmaB)//����������䷽��g
//        {
//            break;
//        }
//    }
//    return Threshold;
//}
//uint8 ostu(void)   //ע�������ֵ��һ��Ҫ��ԭͼ��
//{
//    #define GrayScale 256
//    static uint8 lasthreshold=0;
//    uint16 width = UCOL; //�Լ�ͼ��Ŀ��
//    uint16 height = UROW; //�Լ�ͼ��ĸ߶�
//    //uint16 width = MT9V03X_W;
//    //uint16 height = MT9V03X_H;
//    int pixelCount[GrayScale];
//    float pixelPro[GrayScale];
//    int i, j, pixelSum = width * height/4;
//    uint8 threshold = 0;
//    uint8* data = Image_Use[0];//uint8* data = mt9v03x_image[0];  //ָ���������ݵ�ָ��
//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelCount[i] = 0;
//        pixelPro[i] = 0;
//    }

//    uint32 gray_sum=0;
//    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
//    for (i = 0; i < height; i+=2)
//    {
//        for (j = 0; j < width; j+=2)
//        {
//            pixelCount[(int)data[i * width + j]]++;  //����ǰ�ĵ������ֵ��Ϊ����������±�
//            gray_sum+=(int)data[i * width + j];       //�Ҷ�ֵ�ܺ�
//        }
//    }

//    //����ÿ������ֵ�ĵ�������ͼ���еı���

//    for (i = 0; i < GrayScale; i++)
//    {
//        pixelPro[i] = (float)pixelCount[i] / pixelSum;

//    }

//    //�����Ҷȼ�[0,255]
//    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


//        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//        if( lasthreshold-50>0)
//    j= lasthreshold-50;
//        else
//    j=0;
//        for (; j < GrayScale; j++)
//        {

//                w0 += pixelPro[j];  //��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
//                u0tmp += j * pixelPro[j];  //�������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ

//               w1=1-w0;
//               u1tmp=gray_sum/pixelSum-u0tmp;

//                u0 = u0tmp / w0;              //����ƽ���Ҷ�
//                u1 = u1tmp / w1;              //ǰ��ƽ���Ҷ�
//                u = u0tmp + u1tmp;            //ȫ��ƽ���Ҷ�
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
//// @brief       ��ֵ��
//// @param       *picture_buffer     ��ת���������ַ
//// @param       value               ת����ֵ
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
//            temp = *(Image_Use[0]+j*UCOL+i);//��ȡ���ص�
//            if(j == 0 || j == UROW-1 || i == 0 || i == UCOL-1)//��򷨼�һ���ڿ�
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
///************************************Sobel����************************************
//**          Gx={    {-1,  0,  1},                   Gy={    {  1,  2,  1},
//**                  {-2,  0,  2},                           {  0,  0,  0},
//**                  {-1,  0,  1}}                           { -1, -2, -1}}
//**      ���������ұ�Ե��ȥ
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
//**      ��Threshold=0������Ҷ�ͼ      �������ֵ��
//***********************************************************************************/
//#define Sobel_Gx(addr, y, x)    (addr[UP][RR]+2*addr[y][RR]+addr[DN][RR]-(addr[UP][LL]+2*addr[y][LL]+addr[DN][LL]))
//#define Sobel_Gy(addr, y, x)    (addr[UP][LL]+2*addr[UP][x]+addr[UP][RR]-(addr[DN][LL]+2*addr[DN][x]+addr[DN][RR]))
//#define Sobel_G(addr, y, x)     (abs(Sobel_Gx(addr, y, x)) + abs(Sobel_Gy(addr, y, x)))
//void S_Sobel(uint8 in_IMG[UROW][UCOL], uint8_t out_IMG[UROW][UCOL], uint16 Threshold)
//{
//    uint8_t i, j;
//    uint8_t UP, DN, LL, RR;
//    if (Threshold == 0)//�۲�ÿ���ݶ�ֵ
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
//    else//�����ݶ�ֵ��ֵ��
//    {
//        for (i = 1; i <  UROW-1; i++)
//        {
//            DN = i + 1;     UP = i - 1;
//            for (j = 1; j < UCOL-1; j++)
//            {
//                RR = j + 1;     LL = j - 1;
//                out_IMG[i][j] = (Sobel_G(in_IMG, i, j) >= Threshold ? 0 : 255);                    //�޸ĵ�tft��ֵ��ͼ�⺯������0����1
//            }
//        }
//    }
//}

///*************************************************************************
// *  �������ƣ�Find_Borderline();
// *  ����˵����Ѱ�ұ���
// *  ����˵������
// *  �������أ���
// *  �޸�ʱ�䣺2023��5��10��
// *  ��    ע��https://zhuanlan.zhihu.com/p/391392970 CSDN�����Ͻ��Թ��� �����15cm�޸��Ż���
// *
// *************************************************************************/
//int x0_first , y0_first , x1_first ,y1_first;
//int x1 , y1 ;
//int x2 , y2 ;
//void Find_Borderline(void)
//{
//    //�Թ�Ѳ���Ƿ��ߵ����ұ߽�
//    touch_boundary0 = 0;            //����
//    touch_boundary1 = 0;

//    //�Թ�Ѳ���Ƿ��ߵ��ϱ߽�
//    touch_boundary_up0 = 0;            //����
//    touch_boundary_up1 = 0;

//    //�ױ�ɨ�߷�ֹ���� ע�⣺����sobel��Ե��������ԣ��ڿ򣩣��˶��߱�־������Ostu����
//    loseline0 = 0;
//    loseline1 = 0;

//    uint8 uthres = 1;
////    uint8 uthres = ostu();
//  //Ѱ�����
//  x1 = img_raw.width / 2 - begin_x, y1 = begin_y;

//  //���������ʼ��(����Ԫ�ش���Ҫ�õ�)
//  x0_first = x1;    y0_first = y1;

//  ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);//������ĳ���
//  //ɨ�������У�Ѱ�������
//  for (; y0_first > begin_y-5; y0_first--)
//  {
//      for (; x0_first > 0; x0_first--)
//          if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)
//              goto out1;
//      x0_first = img_raw.width / 2 - begin_x;
//  }
//  loseline0 = 1;  //�ױ߶���
//  out1:
//  {
//      if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres)
//        Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);
//      else ipts0_num = 0;
//  }



//  //Ѱ�ұ���
//  x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

//  //���������ʼ��(����Ԫ�ش���Ҫ�õ�)
//  x1_first = x2;    y1_first = y2;

//  ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
//  for (; y1_first > begin_y-5; y1_first--)
//  {
//      for (; x1_first < img_raw.width - 1; x1_first++)
//          if (AT_IMAGE(&img_raw, x1_first + 1, y1_first) < uthres)
//              goto out2;
//      x1_first = img_raw.width / 2 + begin_x;
//  }
//  loseline1 = 1;  //�ױ߶���
//  out2:
//  {
//      if (AT_IMAGE(&img_raw, x1_first, y1_first) >= uthres)
//        Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts1, &ipts1_num);
//      else ipts1_num = 0;
//  }
//}
///*************************************************************************
// *  �������ƣ�void Left_Adaptive_Threshold();
// *  ����˵�����Զ�����ֵ��ֵ��+�Թ��㷨������ߣ�
// *  ����˵������ȡ��ͼ�����飬������飨����image0����ͼ���ͼ���
// *  �������أ���
// *  �޸�ʱ�䣺2023��5��23��
// *  ��    ע�� ���Ӷ����ж�
// * ǰ�������壺
// *   0
// * 3   1
// *   2
// *************************************************************************/

//const int dir_front[4][2] = { {0, -1},//����
//                              {1,  0},//����
//                              {0,  1},//����
//                              {-1, 0}};//����
//const int dir_frontleft[4][2] = {{-1, -1},//����
//                                  {1,  -1},//����
//                                   {1,  1},//����
//                                  {-1, 1}};//���ϣ���ʱ��
//const int dir_frontright[4][2] = {{1,  -1},//����
//                                   {1,  1},//����
//                                   {-1, 1},//����
//                                  {-1, -1}};//���ϣ���ʱ��

//void Left_Adaptive_Threshold(image_t *img,int block_size,int clip_value,int x,int y,int pts[][2],int *num)
//{
//    zf_assert(img && img->data);     // ���������˳�ִ��
//    zf_assert(num && *num >= 0);
//    zf_assert(block_size > 1 && block_size % 2 == 1);//��֤block_sizeΪ����
////    int half = block_size / 2;                       //�Ͻ�����
//    int half = 0;                                  //������
//    int step = 0, dir = 0, turn = 0; // step��ʾǰ���Ĳ�����dirͨ���ı������ı䵱ǰС�˳���ķ���
//    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4)//��֤block������
//    {
//        int local_thres = 1;
////        int local_thres;
//        //����Ӧ��ֵ��
//    /*
//        for (int dy = -half; dy <= half; dy++)  // forѭ����������block���������ֵ֮�ͣ�����Ӧ��ֵ��
//        {
//            for (int dx = -half; dx <= half; dx++)
//            {
//                local_thres += AT(img, x + dx, y + dy);
//            }
//        }
//        local_thres /= block_size * block_size;
//        local_thres -= clip_value;   // (x,y)��block�����ڵ���ֵ
//    */
//        int current_value = AT(img, x, y);//��ǰ���ص�Ҷ�ֵ
//        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);//��ǰ�����ص�Ҷ�ֵ ��dir=0 �£�dir=1 �ң�dir=2 �ϣ�dir=3 ��
//        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);//��ǰ�����ص�Ҷ�ֵ ��dir=0���£�dir=1 ���£�dir=2 ���ϣ�dir=3 ���� ��
//        //=======��Ӳ���=======
//        if( (x==1 && y < img->height -20) || x== img->width - 2 || y==1 || (y==58 && step > 19))
//        {
//            if(x==1 /*|| x== img->width - 2*/)  touch_boundary0  = 1;                //��߽�����Ϊ������߲�ͣ�����ģ�����������ߣ������ǻ�����ʮ�ֵȣ�
//            if(y==1) touch_boundary_up0 = 1;               //�ߵ����ߣ��ж��µ�or�ϰ�

//            break;
//        }
//        //=======��Ӳ���=======
//        if (front_value < local_thres)  // ǰ����������Ϊ��ɫ
//        {
//            dir = (dir + 1) % 4;   // ����ǰ��Ϊ��ɫ��Ҫ��תһ��
//            turn++;
//        }
//        else if (frontleft_value < local_thres) // ǰ������Ϊ��ɫ������ǰ������Ϊ��ɫ
//        {
//            x += dir_front[dir][0];
//            y += dir_front[dir][1];
//            pts[step][0] = x;   // ������ű���������Ϣ
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            //AT(img,x,y) = RGB565_GREEN;
//        }
//        else  // ǰ��Ϊ��ɫ����ǰ��Ϊ��ɫ��ǽ�ǣ�
//        {
//            x += dir_frontleft[dir][0];   // ����ǽ��Ҫб����
//            y += dir_frontleft[dir][1];
//            dir = (dir + 3) % 4;   // ����ǽ��Ҫ��תһ��
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            //AT(img,x,y) = RGB565_GREEN;
//        }
//    }
//    //���߱�־����������sobel�����Ի�һֱ����Ѳ��
//    if(step < 20 && touch_boundary0)
//        loseline0 = 1;
//    //��¼������Ŀ
//    *num = step;
//}
///*************************************************************************
// *  �������ƣ�void Right_Adaptive_Threshold();
// *  ����˵�����Զ�����ֵ��ֵ��+�Թ��㷨���ұ��ߣ�
// *  ����˵������ȡ��ͼ�����飬������飨����image0����ͼ���ͼ���
// *  �������أ���
// *  �޸�ʱ�䣺2023��5��23��
// *  ��    ע�� ���Ӷ����ж�
// * ǰ�������壺
// *   0
// * 3   1
// *   2
// *************************************************************************/
//void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {
//    zf_assert(img && img->data);
//    zf_assert(num && *num >= 0);
//    zf_assert(block_size > 1 && block_size % 2 == 1);
////    int half = block_size / 2;        //�Ͻ�����
//    int half = 0;                     //������
//    int step = 0, dir = 0, turn = 0;
//    while (step < *num && 0 < x && x < img->width - 1 && half < y && y < img->height - half - 1 && turn < 4) {
//        int local_thres = 1;
////        int local_thres;
//        //����Ӧ��ֵ��
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
//        //=======��Ӳ���=======
//        if( (x==img->width - 2  && y < img->height -20)   || x==1 || y==1 || (y==58 && step > 19)) //���߱�־����������sobel�����Ի�һֱ����Ѳ��
//        {

//            if(x==img->width - 2 /*|| x==1*/)  touch_boundary1  = 1;                //�ұ߽�����Ϊ�����ұ߲�ͣ�����ģ����������ұߣ������ǻ�����ʮ�ֵȣ�
//            if(y==1) touch_boundary_up1 = 1;               //�ߵ����ߣ��ж��µ�or�ϰ�

//            break;
//        }
//        //=======��Ӳ���=======
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
//    //���߱�־����������sobel�����Ի�һֱ����Ѳ��
//    if(step < 20 && touch_boundary1)
//        loseline1 = 1;
//    //��¼������Ŀ
//    *num = step;
//}
////���� ԭͼ
//void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value)
//{
//    int dx = pt1[0] - pt0[0];
//    int dy = pt1[1] - pt0[1];
//    if (abs(dx) > abs(dy)) {
//        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1)) {
//            int y = pt0[1] + (x - pt0[0]) * dy / dx;// y = ���ߺ����� + x������ֵռ�ܲ�ֵ���� * y�����ֵ
//                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value; //��x��y���������أ��������߽磩��ֵ
//        }
//    } else {
//        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1)) {
//            int x = pt0[0] + (y - pt0[1]) * dx / dy;
//                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
//        }
//    }
//}
///*************************************************************************
// *  �������ƣ�void draw_line2();
// *  ����˵�������� ��͸��
// *  ����˵����
// *  �������أ���
// *  �޸�ʱ�䣺2023��6��1��
// *  ��    ע�� �����ǵȾ����
// *************************************************************************/
//void draw_line2(float pt0[2], float pt1[2], float pts_out[][2], int *num, float dist){
//    int remain = 0, len = 0;//��һ�β������߶ξ���
//    float x0 = pt0[0];
//    float y0 = pt0[1];
//    float dx = pt1[0] - x0;
//    float dy = pt1[1] - y0;
//    float dn = mySqrt(dx*dx+dy*dy);//��ƽ���� �󻡳����֣��������߶�ǰ���������
//    //float dn = sqrt(dx*dx+dy*dy);//��ƽ���� �󻡳����֣��������߶�ǰ���������
//    dx /= dn;//�˵㴦��cos��
//    dy /= dn;//�˵㴦��sin��

//    //ÿ�εȾ��������
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
// *  �������ƣ�void SplicingArray();
// *  ����˵��������ƴ��
// *  ����˵��������������������������������Լ�������x=0����ƴ�ӣ�x=1����ƴ��
// *  �������أ���
// *  �޸�ʱ�䣺2023��6��2��
// *  ��    ע�� �������Ӳ��ߵ�����
// *************************************************************************/
//void SplicingArray(float pt0[][2], int num1, float pt1[][2], int num2, float pt_out[][2], int *num, uint8 x)
//{
//    int i ,count;//��������
//    for(i = 0;i<num1;i++)//��������1
//    {
//        pt_out[i][0] = pt0[i][0];
//        pt_out[i][1] = pt0[i][1];
//        count++;
//    }

//    if(x)//����ƴ��
//    {
//        for(i = 0;i<num2;i++)
//        {
//            pt_out[num1+i][0] = pt1[num2-i-1][0];
//            pt_out[num1+i][1] = pt1[num2-i-1][1];
//            count++;
//        }

//    }else//����ƴ��
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
// *  �������ƣ�void blur_points();
// *  ����˵�����㼯�����˲�
// *  ����˵��������������飬�����ܸ���������������飬�㼯��Χ
// *  �������أ���
// *  �޸�ʱ�䣺2022��10��4��
// *  ��    ע��
// * ���磺kernel = 5
// *  xi = (0*xi-3 + 1*xi-2 + 2*xi-1 + 3*xi + 2*xi+1 + 1*xi+2 + 0*xi+3)/9
// *  yi ͬ��
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
// *  �������ƣ�void resample_points����;
// *  ����˵�����㼯�Ⱦ����
// *  ����˵��������������飬ԭ�����ܸ���������������飬��ȡ������������������������
// *  �������أ���
// *  �޸�ʱ�䣺2022��10��27��
// *  ��    ע��ʹ�߹��Ĳ���ǰ���߶εľ���Ϊ`dist`
// *************************************************************************/
//void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist){
//    int remain = 0, len = 0;//��һ�β������߶ξ���
//    for(int i=0; i<num1-1 && len < *num2; i++){
//        float x0 = pts_in[i][0];
//        float y0 = pts_in[i][1];
//        float dx = pts_in[i+1][0] - x0;
//        float dy = pts_in[i+1][1] - y0;
//        float dn = mySqrt(dx*dx+dy*dy);//��ƽ���� �󻡳����֣��������߶�ǰ���������
//        //float dn = sqrt(dx*dx+dy*dy);//��ƽ���� �󻡳����֣��������߶�ǰ���������
//        dx /= dn;//�˵㴦��cos��
//        dy /= dn;//�˵㴦��sin��

//        //ÿ�εȾ��������
//        while(remain < dn && len < *num2){
//            x0 += dx * remain;
//            pts_out[len][0] = x0;
//            y0 += dy * remain;
//            pts_out[len][1] = y0;

//            len++;
//            dn -= remain;
//            remain = dist;
//        }
//        remain -= dn;//����Խһ��������߾������ֱ��
//    }
//    *num2 = len;
//}
///*************************************************************************
// *  �������ƣ�void local_angle_points();
// *  ����˵�����㼯�ֲ��Ƕȱ仯��
// *  ����˵��������������飬���ߵ����������Ƕȱ任�����飬�����̶�����
// *  �������أ���
// *  �޸�ʱ�䣺2022��11��23��
// *  ��    ע������õ�ǰ���������ߵ�б����Ϊ�õ�����б�ʵĽ���
// *************************************************************************/
//void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist) {
//    for (int i = 0; i < num; i++) {
//        if (i <= 0 || i >= num - 1) {
//            angle_out[i] = 0;
//            continue;
//        }
//        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
//        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
//        float dn1 = mySqrt(dx1 * dx1 + dy1 * dy1);//�˵���ǰһ������
//        //float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);//�˵���ǰһ������
//        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
//        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
//        float dn2 = mySqrt(dx2 * dx2 + dy2 * dy2);//�˵����һ������
//        //float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);//�˵����һ������
//        float c1 = dx1 / dn1;//cos��1
//        float s1 = dy1 / dn1;//sin��1
//        float c2 = dx2 / dn2;//cos��2
//        float s2 = dy2 / dn2;//sin��2
//        //����չ���õ��ǹ�ʽ(k1-k2)/(1+k1k2),��tan�Ĳ�ǹ�ʽ==>�ó������Ƕȱ仯==>����б�ʱ仯
//        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);//atan2f(y,x):�����Ի��ȱ�ʾ��y/x������
//    }
//}
///*************************************************************************
// *  �������ƣ�void nms_angle();
// *  ����˵�����Ƕȱ仯�ʷǼ�������
// *  ����˵��������Ƕȱ仯�����飬�Ƕȱ任�����ܸ��������ƺ�ĽǶȱ仯���飬�㼯������
// *  �������أ���
// *  �޸�ʱ�䣺2022��11��25��
// *  ��    ע���Ͻ�������kernel = 21�����漫��ֵ�����飬��һ�η�Χ��б�ʱ任����ֵ���Ǽ���ֵ����Ϊ��
// *************************************************************************/
//void nms_angle(float angle_in[], int num, float angle_out[], int kernel){
//    zf_assert(kernel % 2 == 1);
//    int half = kernel / 2;
//    for (int i = 0; i < num; i++) {
//        angle_out[i] = angle_in[i];
//        for (int j = -half; j <= half; j++) {
//            //fabs():�����ֵ
//            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
//                angle_out[i] = 0;
//                break;
//            }
//        }
//    }
//}
///*************************************************************************
// *  �������ƣ�void track_leftline();void track_rightline();
// *  ����˵�������ұ��߸�������
// *  ����˵��������Ⱦ�����������飬�����ܸ������������飬�����̶����룬һ���������Ӧ�����ظ���
// *  �������أ���
// *  �޸�ʱ�䣺2022��11��25��
// *  ��    ע��
// *************************************************************************/
//void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist){
//    for (int i = 0; i < num; i++) {
//            float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
//            float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
//            float dn = mySqrt(dx * dx + dy * dy);
//            //float dn = sqrt(dx * dx + dy * dy);
//            dx /= dn;//cos��
//            dy /= dn;//sin��
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
// *  �������ƣ�void sobel3();
// *  ����˵����3*3 sobel�㷨��ȡ
// *  ����˵�������ͺˣ�ͼ������
// *  �������أ���
// *  �޸�ʱ�䣺2022��10��21��
// *  ��    ע��
// *************************************************************************/
//void sobel3(image_t *img0, image_t *img1) {
//    zf_assert(img0 && img0->data);
//    zf_assert(img1 && img1->data);
//    zf_assert(img0->width == img1->width && img0->height == img1->height);
//    zf_assert(img0 != img1 && img0->data != img1->data);

//  int gx, gy;
//  // �ȱ���y�����x�Ƚ�cache-friendly
//  for (int y = 1; y < img0->height - 1; y++) {
//    for (int x = 1; x < img0->width - 1; x++) {
//      //�����ݶ�
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
// *  �������ƣ�void erode3();
// *  ����˵����3*3��ʴ �ڵ㸯ʴ����
// *  ����˵�������ͺˣ�ͼ������
// *  �������أ���
// *  �޸�ʱ�䣺2022��10��8��
// *  ��    ע��
// *************************************************************************/
//void erode3(image_t *img0, image_t *img1){
//    zf_assert(img0 && img0->data);
//    zf_assert(img1 && img1->data);
//    zf_assert(img0->width == img1->width && img0->height == img1->height);
//    zf_assert(img0 != img1 && img0->data != img1->data);

//    int min_value;
//    // �ȱ���y�����x�Ƚ�cache-friendly
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
// *  �������ƣ�void dilate3();
// *  ����˵����3*3���� ��������ȥ���ڵ�
// *  ����˵�������ͺˣ�ͼ������
// *  �������أ���
// *  �޸�ʱ�䣺2022��10��8��
// *  ��    ע��
// *************************************************************************/
//void dilate3(image_t *img0, image_t *img1) {
//    zf_assert(img0 && img0->data);
//    zf_assert(img1 && img1->data);
//    zf_assert(img0->width == img1->width && img0->height == img1->height);
//    zf_assert(img0 != img1 && img0->data != img1->data);

//    int max_value;
//    // �ȱ���y�����x�Ƚ�cache-friendly
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
// *  �������ƣ�int regression();
// *  ����˵�������Իع飨��С���˷������б��
// *  ����˵���������������
// *  �������أ�б��
// *  �޸�ʱ�䣺2023��6��1��
// *  ��    ע�� ���Իع飨��С���˷������б�ʣ�������͸��ǰ��ͼ��
// *************************************************************************/
//int regression(int pts_in[][2])
//{

//    int num = sizeof(pts_in) / sizeof(pts_in[0]);//������ĳ���
//      int i=0,SumX=0,SumY=0,SumLines = 0;
//      float SumUp=0,SumDown=0,avrX=0,avrY=0,B,A;
//      SumLines=pts_in[0][1]-pts_in[num][1];   // pts_in[0][1] Ϊ��ʼ�У� //pts_in[num][1] ������ //SumLines

//      for(i=0;i<=num;i++)
//      {
//        SumY+=pts_in[i][1];
//        SumX+=pts_in[i][0];    //�Ա߽�X,Y���
//      }
//      avrY=SumY/SumLines;     //Y��ƽ��ֵ
//      avrX=SumX/SumLines;     //X��ƽ��ֵ
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
//        A=(SumX-B*SumY)/SumLines;  //�ؾ�
//        return B;  //����б��
//}


