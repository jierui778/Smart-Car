#include "zf_common_headfile.h"
#ifndef _CROSS_H_
#define _CROSS_H_
#define CROSS_ENCODER_THRES 1000 // 十字编码器积分阈值



enum cross_type_e
{
    CROSS_NONE,
    CROSS_BEGIN,
    CROSS_IN,
    CROSS_INHALF_LEFT,
    CROSS_INHALF_RIGHT,
    CROSS_OUT,
};

extern enum cross_type_e cross_type;


void Cross_FarLine(void);//十字巡远线
void Cross_NearLine(void);//十字巡近线
void Cross_DrawLine(void);//十字补线方案




#endif