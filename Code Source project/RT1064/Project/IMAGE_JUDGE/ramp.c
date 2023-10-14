#include "ramp.h"

/**
 * @brief 坡道检测
 *
 */
void Ramp_Check(void)
{
    if(ramp_type == RAMP_FOUND)//根据角度张开特征判断出坡道
    {
        ramp_type = RAMP_BEGIN;//进入坡道开始
        //开始距离积分，用于检测特殊丢线情况下是否驶出坡道
    }
}


/**
 * @brief 进入坡道
 *
 */
void Ramp_Run(void)
{
    if(ramp_type == RAMP_BEGIN)
    {
        //进入坡道需要加速，

    }
    if(Ramp_Distance > RAMP_ENCODER_THRES)//考虑是否需要加判断条件
    {
        ramp_type = RAMP_END;//距离积分到达阈值，认为坡道结束
    }

}

