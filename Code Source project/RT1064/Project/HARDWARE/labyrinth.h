//#ifndef __LABYRINTH_H
//#define __LABYRINTH_H


//#include "zf_common_headfile.h"

////图像基本信息
//typedef struct img{
//    uint8_t *data;
//    uint32_t width;
//    uint32_t height;
//    uint32_t step;//走过路径的长度
//} image_t;
//#define AT_IMAGE(img, x, y)          ((img)->data[(y)*(img)->step+(x)]) //访问图像像素 二维数组转换一维数组
//#define AT_IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))//防止访问超出边界

//#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}//定义图像结构体基本信息
//#define ROI_IMAGE(img, x1, y1, w, h) {.data=&AT_IMAGE(img, x1, y1), .width=w, .height=h, .step=img.width}



////创建的关于图像的对象
//extern uint8_t threshold;
//extern image_t img_raw;
//extern image_t img_line;
///*extern image_t img_thres;
// */

//#define ROAD_WIDTH      (0.39)        //赛道宽度45cm 适时调整 注意：应用方案三时情况特殊为负数-0.40,正常0.43
//#define POINTS_MAX_LEN  (140)//边线点最多的情况——>num


//#define UROW 60
//#define UCOL 80
////生图
//extern image_t img_raw;
////变换图像
//extern image_t img_change;
///*//二值化图像数据
// *extern uint8_t img_thres_data[RESULT_COL][RESULT_ROW];
// */
//extern float aim_distance;
//extern int drawline0[2];//补线起始点坐标
//extern int drawline1[2];//补线终点坐标

//extern float N_drawline0[2];//补线起始点坐标
//extern float N_drawline1[2];//补线终点坐标


////逆透视补线数组
//extern float left_line[POINTS_MAX_LEN][2];
//extern float right_line[POINTS_MAX_LEN][2];
//extern int left_num, right_num;
////拼接数组
//extern float splicing_leftline[POINTS_MAX_LEN][2];
//extern float splicing_rightline[POINTS_MAX_LEN][2];
//extern int splicing_leftline_num,splicing_rightline_num;
////拼接数组平移中线
//extern float splicing_leftline_center[POINTS_MAX_LEN][2];
//extern float splicing_rightline_center[POINTS_MAX_LEN][2];
//extern int splicing_leftline_center_num,splicing_rightline_center_num;

////左右边丢线
//extern uint8 loseline0;
//extern uint8 loseline1;

//extern int x0_first , y0_first , x1_first ,y1_first;
//extern int x1 , y1 ;
//extern int x2 , y2 ;

//extern int begin_x0,begin_y0;              //找线偏移点
//extern int begin_x1,begin_y1;              //找线偏移点

//extern uint8  Image_Use[UROW][UCOL];
//extern uint8 Image_Use_Robert[UROW][UCOL];
//extern uint8 touch_boundary0;              //左边线走到图像边界
//extern uint8 touch_boundary1;              //右边线走到图像边界
//extern uint8 touch_boundary_up0;              //左边线走到图像左边界
//extern uint8 touch_boundary_up1;              //右边线走到图像右边界

//extern float xielv_left_y_to_end,xielv_right_y_to_end;                 //在逆透视后得坐标系建得斜率

//// 原图左右边线
//extern int ipts0[POINTS_MAX_LEN][2];
//extern int ipts1[POINTS_MAX_LEN][2];
//extern int ipts0_num, ipts1_num;
//// 变换后左右边线
//extern float rpts0[POINTS_MAX_LEN][2];
//extern float rpts1[POINTS_MAX_LEN][2];
//extern int rpts0_num, rpts1_num;
//// 变换后左右边线+滤波
//extern float rpts0b[POINTS_MAX_LEN][2];
//extern float rpts1b[POINTS_MAX_LEN][2];
//extern int rpts0b_num, rpts1b_num;
//// 变换后左右边线+等距采样
//extern float rpts0s[POINTS_MAX_LEN][2];
//extern float rpts1s[POINTS_MAX_LEN][2];
//extern int rpts0s_num, rpts1s_num;
//// 左右边线局部角度变化率
//extern float rpts0a[POINTS_MAX_LEN];
//extern float rpts1a[POINTS_MAX_LEN];
//extern int rpts0a_num, rpts1a_num;
//// 左右边线局部角度变化率+非极大抑制
//extern float rpts0an[POINTS_MAX_LEN];
//extern float rpts1an[POINTS_MAX_LEN];
//extern int rpts0an_num, rpts1an_num;
//// 左/右中线
//extern float rptsc0[POINTS_MAX_LEN][2];
//extern float rptsc1[POINTS_MAX_LEN][2];
//extern int rptsc0_num, rptsc1_num;
//// 归一化中线
//extern float rptsn[POINTS_MAX_LEN][2];
//extern int rptsn_num;

//// Y角点
//extern int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
//extern bool Ypt0_found, Ypt1_found;
//// L角点
//extern int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
//extern bool Lpt0_found, Lpt1_found;
//// 内L角点
//extern int N_Lpt0_rpts0s_id, N_Lpt1_rpts1s_id;
//extern bool N_Lpt0_found, N_Lpt1_found;

//extern int N_Xfound_num;//面向赛道编程，双内L计数
//// 长直道
//extern bool is_straight0, is_straight1;
//// 弯道
//extern bool is_turn0, is_turn1;

//enum track_type_e {
//    TRACK_LEFT,
//    TRACK_RIGHT,
//};
//extern enum track_type_e track_type;       //当前巡线模式

//extern float error[1] ;
//extern float ave_error;//ave_error名为平均偏差，实际按正态分布算
//// 远预锚点位置
//extern int aim_idx[1] ;

//// 计算远锚点偏差值
//extern float dx[1] ;
//extern float dy[1] ;
//extern float dn[1] ;
//// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩

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