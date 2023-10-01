#include "filter.h"

/**
 * @brief 一阶卡尔曼滤波器
 *
 * @param Data_In 输入数据
 * @param KFP 卡尔曼滤波器参数
 * @return float
 */
float Kalman_Filter(float Data_In, KalmanParam KFP)
{
    static float x_last = 0; // 上一次的状态值
    static float p_last = 0; // 上一次的协方差值

    float kg; // 卡尔曼增益
    float p_now; // 当前协方差值
    float x_now; // 当前状态值

    // 预测当前状态值
    x_now = x_last;
    // 预测当前协方差值
    p_now = p_last + KFP.Q;

    // 计算卡尔曼增益
    kg = p_now / (p_now + KFP.R);

    // 计算当前状态值
    x_now = x_now + kg * (Data_In - x_now);

    // 更新协方差值
    p_last = (1 - kg) * p_now;

    // 更新上一次的状态值
    x_last = x_now;

    return x_now;
}

