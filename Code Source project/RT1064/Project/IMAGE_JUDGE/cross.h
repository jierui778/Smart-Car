#include "zf_common_headfile.h"
#ifndef _CROSS_H_
#define _CROSS_H_

enum cross_type_e
{
    CROSS_NONE,
    CROSS_FOUND,
    CROSS_IN,
    CROSS_OUT,
};

extern enum cross_type_e cross_type;

extern bool far_Lpt0_found, far_Lpt1_found;
extern bool far_N_Lpt0_found, far_N_Lpt1_found;
extern int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;
extern int far_N_Lpt0_rpts0s_id, far_N_Lpt1_rpts1s_id;

// 原图远线左右边线
extern int far_ipts0[L_CROSS][2];
extern int far_ipts1[L_CROSS][2];
extern int far_ipts0_num, far_ipts1_num;
// 变换后左右远边线
extern float far_rpts0[L_CROSS][2];
extern float far_rpts1[L_CROSS][2];
extern int far_rpts0_num, far_rpts1_num;
// 变换后左右远边线+滤波
extern float far_rpts0b[L_CROSS][2];
extern float far_rpts1b[L_CROSS][2];
extern int far_rpts0b_num, far_rpts1b_num;
// 变换后左右远边线+等距采样
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

// void Cross_FarLine(void);//十字巡远线
// void Cross_NearLine(void);//十字巡近线
// void Cross_DrawLine(void);//十字补线方案
void Cross_Check(void);
void Cross_Run(void);

#endif