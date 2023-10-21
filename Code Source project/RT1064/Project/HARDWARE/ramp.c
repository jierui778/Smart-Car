#include "ramp.h"
#include "image.h"
#include "encoder.h"
int Ramp_num = 0;

enum ramp_type_e ramp_type = RAMP_NONE;
void Ramp_Check(void) // 根据边沿外张的特性
{
    if (ramp_type != RAMP_NONE && !touch_boundary0 && !touch_boundary1 && touch_boundary_up0 && touch_boundary_up1) // 非坡道模式
    {
        for (int i = 0; i < ipts0_num; i++)
        {
            if (ipts1[i][0] - ipts0[i][0] > ipts1[i + 3][0] - ipts0[i + 3][0] && ipts1[i][0] - ipts0[i][0] > ipts1[i - 3][0] - ipts0[i - 3][0] && ipts1[i][0] - ipts0[i + 6][0] > 0 && ipts1[i][0] - ipts0[i - 6][0] > 0 && ipts1[i][1] - ipts0[i + 9][1] > 0 && ipts1[i][1] - ipts0[i - 9][1] > 0 && ipts1[i][0] - ipts0[i][0] > 30)
            {
                Ramp_num++;
                ramp_type = RAMP_FOUND; // 枚举状态为找到坡道
                break;
            }
            else
            {
                ramp_type = RAMP_NONE;
            }
        }
    }
}

void Ramp_Run(void)
{
    if (ramp_type == RAMP_FOUND)
    {
        DisCnt_Flag = 1; // 开启编码器积分
    }



    if ((Encoder_L_Dis + Encoder_R_Dis)/2>1000)//编码器积分到达阈值,退出坡道模式
    {
        ramp_type = RAMP_END;
    }
}
