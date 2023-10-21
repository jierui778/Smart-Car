#ifndef IMGVALUE_CROSS_H_
#define IMGVALUE_CROSS_H_

#include "zf_common_headfile.h"
#define PI 3.14159265358979f

enum cross_type_e
{
    CROSS_BEGIN,
    CROSS_IN,
    CROSS_OUT,
    CROSS_NONE,
    CROSS_FOUND,
};

enum turn_type
{
    TURN_LEFT,
    TURN_LEFT_MID,
    TURN_RIGHT,
    TURN_RIGHT_MID,
    STRAIGHT,
};

extern enum cross_type_e cross_type;

void Cross_Chreck(void);
void Cross_Run(void);
// void cross_drawline();

#endif /*IMGVALUE_CROSS_H_*/
