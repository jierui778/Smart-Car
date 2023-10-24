#ifndef IMGVALUE_CROSS_H_
#define IMGVALUE_CROSS_H_

#include "zf_common_headfile.h"
#define PI 3.14159265358979f

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
};

extern enum cross_type_e cross_type;

void Cross_Check(void);
void Cross_Run(void);

#endif
