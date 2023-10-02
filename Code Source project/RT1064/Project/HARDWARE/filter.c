#include "filter.h"

KalmanParam test = {
    1,   // lastp协方差
    0,   // nowp协方差
    0,   // out
    1,   // 增益
    100, // Q过程噪声
    4000 // 观测噪声R};
};

/**
 * @brief 一阶卡尔曼滤波器
 *
 * @param Data_In 输入数据
 * @param KFP 卡尔曼滤波器参数
 * @return float
 */
float KalmanFilter(float input, KalmanParam *kfp)
{
    // 预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
    kfp->Now_P = kfp->LastP + kfp->Q;
    // 卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
    // 更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
    kfp->out = kfp->out + kfp->Kg * (input - kfp->out); // 因为这一次的预测值就是上一次的输出值
    // 更新协方差方程: 本次的系统协方差赋给 kfp->LastP 为下一次运算准备。
    kfp->LastP = (1 - kfp->Kg) * kfp->Now_P;
    return kfp->out;
}
