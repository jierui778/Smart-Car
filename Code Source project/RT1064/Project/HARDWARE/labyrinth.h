//#ifndef __LABYRINTH_H
//#define __LABYRINTH_H


//#include "zf_common_headfile.h"

////ͼ�������Ϣ
//typedef struct img{
//    uint8_t *data;
//    uint32_t width;
//    uint32_t height;
//    uint32_t step;//�߹�·���ĳ���
//} image_t;
//#define AT_IMAGE(img, x, y)          ((img)->data[(y)*(img)->step+(x)]) //����ͼ������ ��ά����ת��һά����
//#define AT_IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))//��ֹ���ʳ����߽�

//#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}//����ͼ��ṹ�������Ϣ
//#define ROI_IMAGE(img, x1, y1, w, h) {.data=&AT_IMAGE(img, x1, y1), .width=w, .height=h, .step=img.width}



////�����Ĺ���ͼ��Ķ���
//extern uint8_t threshold;
//extern image_t img_raw;
//extern image_t img_line;
///*extern image_t img_thres;
// */

//#define ROAD_WIDTH      (0.39)        //�������45cm ��ʱ���� ע�⣺Ӧ�÷�����ʱ�������Ϊ����-0.40,����0.43
//#define POINTS_MAX_LEN  (140)//���ߵ������������>num


//#define UROW 60
//#define UCOL 80
////��ͼ
//extern image_t img_raw;
////�任ͼ��
//extern image_t img_change;
///*//��ֵ��ͼ������
// *extern uint8_t img_thres_data[RESULT_COL][RESULT_ROW];
// */
//extern float aim_distance;
//extern int drawline0[2];//������ʼ������
//extern int drawline1[2];//�����յ�����

//extern float N_drawline0[2];//������ʼ������
//extern float N_drawline1[2];//�����յ�����


////��͸�Ӳ�������
//extern float left_line[POINTS_MAX_LEN][2];
//extern float right_line[POINTS_MAX_LEN][2];
//extern int left_num, right_num;
////ƴ������
//extern float splicing_leftline[POINTS_MAX_LEN][2];
//extern float splicing_rightline[POINTS_MAX_LEN][2];
//extern int splicing_leftline_num,splicing_rightline_num;
////ƴ������ƽ������
//extern float splicing_leftline_center[POINTS_MAX_LEN][2];
//extern float splicing_rightline_center[POINTS_MAX_LEN][2];
//extern int splicing_leftline_center_num,splicing_rightline_center_num;

////���ұ߶���
//extern uint8 loseline0;
//extern uint8 loseline1;

//extern int x0_first , y0_first , x1_first ,y1_first;
//extern int x1 , y1 ;
//extern int x2 , y2 ;

//extern int begin_x0,begin_y0;              //����ƫ�Ƶ�
//extern int begin_x1,begin_y1;              //����ƫ�Ƶ�

//extern uint8  Image_Use[UROW][UCOL];
//extern uint8 Image_Use_Robert[UROW][UCOL];
//extern uint8 touch_boundary0;              //������ߵ�ͼ��߽�
//extern uint8 touch_boundary1;              //�ұ����ߵ�ͼ��߽�
//extern uint8 touch_boundary_up0;              //������ߵ�ͼ����߽�
//extern uint8 touch_boundary_up1;              //�ұ����ߵ�ͼ���ұ߽�

//extern float xielv_left_y_to_end,xielv_right_y_to_end;                 //����͸�Ӻ������ϵ����б��

//// ԭͼ���ұ���
//extern int ipts0[POINTS_MAX_LEN][2];
//extern int ipts1[POINTS_MAX_LEN][2];
//extern int ipts0_num, ipts1_num;
//// �任�����ұ���
//extern float rpts0[POINTS_MAX_LEN][2];
//extern float rpts1[POINTS_MAX_LEN][2];
//extern int rpts0_num, rpts1_num;
//// �任�����ұ���+�˲�
//extern float rpts0b[POINTS_MAX_LEN][2];
//extern float rpts1b[POINTS_MAX_LEN][2];
//extern int rpts0b_num, rpts1b_num;
//// �任�����ұ���+�Ⱦ����
//extern float rpts0s[POINTS_MAX_LEN][2];
//extern float rpts1s[POINTS_MAX_LEN][2];
//extern int rpts0s_num, rpts1s_num;
//// ���ұ��߾ֲ��Ƕȱ仯��
//extern float rpts0a[POINTS_MAX_LEN];
//extern float rpts1a[POINTS_MAX_LEN];
//extern int rpts0a_num, rpts1a_num;
//// ���ұ��߾ֲ��Ƕȱ仯��+�Ǽ�������
//extern float rpts0an[POINTS_MAX_LEN];
//extern float rpts1an[POINTS_MAX_LEN];
//extern int rpts0an_num, rpts1an_num;
//// ��/������
//extern float rptsc0[POINTS_MAX_LEN][2];
//extern float rptsc1[POINTS_MAX_LEN][2];
//extern int rptsc0_num, rptsc1_num;
//// ��һ������
//extern float rptsn[POINTS_MAX_LEN][2];
//extern int rptsn_num;

//// Y�ǵ�
//extern int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
//extern bool Ypt0_found, Ypt1_found;
//// L�ǵ�
//extern int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
//extern bool Lpt0_found, Lpt1_found;
//// ��L�ǵ�
//extern int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
//extern bool N_Lpt0_found, N_Lpt1_found;

//extern int N_Xfound_num;//����������̣�˫��L����
//// ��ֱ��
//extern bool is_straight0, is_straight1;
//// ���
//extern bool is_turn0, is_turn1;

//enum track_type_e {
//    TRACK_LEFT,
//    TRACK_RIGHT,
//};
//extern enum track_type_e track_type;       //��ǰѲ��ģʽ

//extern float error[1] ;
//extern float ave_error;//ave_error��Ϊƽ��ƫ�ʵ�ʰ���̬�ֲ���
//// ԶԤê��λ��
//extern int aim_idx[1] ;

//// ����Զê��ƫ��ֵ
//extern float dx[1] ;
//extern float dy[1] ;
//extern float dn[1] ;
//// �����ǽ���Զ��,�ɽ��ƹ���Stanley�㷨,����ײ·��

//extern float pure_angle;

////extern float line_blur_kernel;
////extern float sample_dist;
////extern float pixel_per_meter;


//void process_image(void);
//void find_corners();
//void compressimage(void);
//void uncompressimage(void);
//void Binarization(uint8 thro);
//uint8 ostu(void);
//void S_Sobel(uint8 in_IMG[UROW][UCOL], uint8_t out_IMG[UROW][UCOL], uint16_t Threshold);
//void Find_Borderline(void);
//void Left_Adaptive_Threshold(image_t *img,int block_size, int clip_value, int x, int y, int pts[][2], int *num);
//void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
//uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height);
//void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value);
//void draw_line2(float pt0[2], float pt1[2], float pts_out[][2], int *num, float dist);
//void SplicingArray(float pt0[][2], int num1, float pt1[][2], int num2, float pt_out[][2], int *num, uint8 x);
//void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel);
//void erode3(image_t *img0, image_t *img1);
//void dilate3(image_t *img0, image_t *img1); 
//void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist);
//void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
//void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
//void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);
//void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);
//int regression(int pts_in[][2]);

//#endif/*Image_Control_H_*/



//#endif