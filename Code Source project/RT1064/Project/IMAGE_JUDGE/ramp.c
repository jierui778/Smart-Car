#include "ramp.h"

int ramp_num = 0;

void Ramp_Check(void)
{
    if ((cross_type == CROSS_NONE&&ramp_type == RAMP_NONE&&circle_type == CIRCLE_NONE))
    {
        ramp_type = RAMP_FOUND;//枚举状态为找到坡道
        ramp_num++;//记录坡道数量
    }
}


void Ramp_Run(void)
{
    if(1)//边线触碰上边界，进入坡道
    {
        ramp_type = RAMP_IN;//枚举状态为进入坡道
    }
    if(1)//
    {
        ramp_type = RAMP_OUT;//枚举状态为准备出坡道
    }
    {

    }
}

