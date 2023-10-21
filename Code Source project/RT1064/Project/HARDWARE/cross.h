#ifndef IMGVALUE_CROSS_H_
#define IMGVALUE_CROSS_H_

#include "zf_common_headfile.h"
#define PI               3.14159265358979f

enum cross_type_e
{
    CROSS_BEGIN,//state_b
    CROSS_IN,//state_c
    CROSS_OUT,//state_d
    CROSS_NONE,
    CROSS_FOUND,

};



extern enum cross_type_e cross_type;
void Cross_Check(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r);//得考虑斜入十字的情况
void Cross_Run(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r);
int my_abs(int value);
float run_cross_begin(void);
float run_cross_in(void);
float run_cross_out(void);
//void cross_drawline();


#endif /*IMGVALUE_CROSS_H_*/
