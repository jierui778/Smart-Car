#ifndef IMGVALUE_CROSS_H_
#define IMGVALUE_CROSS_H_

#include "zf_common_headfile.h"
// #define PI 3.14159265358979f

enum cross_type_e
{
    CROSS_HALF_LEFT_FOUND,
    CROSS_HALF_RIGHT_FOUND, // 斜入十字
    CROSS_DOUBLLE_FOUND,
    CROSS_IN_LEFT,
    CROSS_IN_RIGHT,
    CROSS_RUN,
    CROSS_IN_DOUBLE,
    CROSS_OUT,
    CROSS_NONE,
    CROSS_FOUND,

};



extern enum cross_type_e cross_type;

void Cross_Run(int in_put_l[][2], int in_put_num_l, int in_put_r[][2], int in_put_num_r);
void Cross_Check(void);//得考虑斜入十字的情况
int my_abs(int value);

void Cross_Drawline_Found_Double(void);
void Cross_Drawline_Found_Right(void);
void Cross_Drawline_Found_Left(void);




void Cross_Drawline_In_Double(void);
void Cross_Drawline_In_Left(void);
void Cross_Drawline_In_Right(void);

void Cross_Run_Out(void);
void Cross_Drawline_Double_Out(void);



//void cross_drawline();

#endif
