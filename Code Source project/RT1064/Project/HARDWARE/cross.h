#ifndef IMGVALUE_CROSS_H_
#define IMGVALUE_CROSS_H_

#include "zf_common_headfile.h"
#define PI               3.14159265358979f

enum cross_type_e {
    CROSS_NONE = 0,     // 非十字模式
    CROSS_BEGIN,        // 找到左or右两个L角点(包括单线情况)
    CROSS_IN_DOUBLE,    // 双L角点寻远线，两个L角点很近，即进入十字内部(此时切换远线控制)
    CROSS_INHALF_LEFT,  // 单L角点寻远线(只看得到左边)
    CROSS_INHALF_RIGHT, // 单L角点寻远线(只看得到右边)
    CROSS_NUM,
};
extern enum cross_type_e cross_type;

#define L_CROSS 40

extern int cross_num;
extern int L_x0, L_x1 , L_y0 , L_y1;                       //十字得近处两个L点的坐标
extern int far_x0, far_x1 , far_y0 , far_y1 ;                  //寻找到的远处起始点
extern float Nfar_x0, Nfar_x1  , Nfar_y0 , Nfar_y1 ;  //寻找到的远处逆透视点
extern float xielv_left, xielv_right;
extern int not_have_line ;

//以下为十字控制寻远线操作,算法与常规寻线相同

extern bool far_Lpt0_found, far_Lpt1_found;
extern bool far_N_Lpt0_found,far_N_Lpt1_found;
extern int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;
extern int far_N_Lpt0_rpts0s_id,far_N_Lpt1_rpts1s_id;
//原图远线左右边线
extern int far_ipts0[L_CROSS][2];
extern int far_ipts1[L_CROSS][2];
extern int far_ipts0_num, far_ipts1_num;
//变换后左右远边线
extern float far_rpts0[L_CROSS][2];
extern float far_rpts1[L_CROSS][2];
extern int far_rpts0_num, far_rpts1_num;
//变换后左右远边线+滤波
extern float far_rpts0b[L_CROSS][2];
extern float far_rpts1b[L_CROSS][2];
extern int far_rpts0b_num, far_rpts1b_num;
//变换后左右远边线+等距采样
extern float far_rpts0s[L_CROSS][2];
extern float far_rpts1s[L_CROSS][2];
extern int far_rpts0s_num, far_rpts1s_num;
// 左右远边线局部角度变化率
extern float far_rpts0a[L_CROSS];
extern float far_rpts1a[L_CROSS];
extern int far_rpts0a_num, far_rpts1a_num;
// 左右远边线局部角度变化率+非极大抑制
extern float far_rpts0an[L_CROSS];
extern float far_rpts1an[L_CROSS];
extern int far_rpts0an_num, far_rpts1an_num;

extern float xielv_left, xielv_right;

void check_half_left();
void check_half_right();
void run_cross(void);
void cross_farline();
void cross_drawline();


#endif /*IMGVALUE_CROSS_H_*/
