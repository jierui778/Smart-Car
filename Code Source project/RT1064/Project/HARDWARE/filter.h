#include "zf_common_headfile.h"
#ifndef _FILTER_H
#define _FILTER_H

// 卡尔曼滤波器参数结构体
typedef struct
{
    float LastP; // 上次估算协方差 初始化值为0.02
    float Now_P; // 当前估算协方差 初始化值为0
    float out;   // 卡尔曼滤波器输出 初始化值为0
    float Kg;    // 卡尔曼增益 初始化值为0
    float Q;     // 过程噪声协方差 初始化值为0.001
    float R;     // 观测噪声协方差 初始化值为0.543
} KalmanParam;

extern KalmanParam EncoderParam; // 全局声明编码器卡尔曼滤波参数

/**
 * @brief 一维卡尔曼滤波
 * *      备注：一维卡尔曼滤波（单系统变量实际值估计）
 *          调参根据经验值对KFP.Q&KFP.R(过程噪声和观测噪声协方差)调整，调整到两个值差不多且延迟不高
 *          来源：CSDN卡尔曼滤波C语言实现
 */
// 背景知识/深入了解
/* 协方差：>0 两者正相关 0 两者独立 <0两者负相关 故协方差是用来描述多个变量的相关性 在这里用来衡量数据中噪声成分的多少
 * out = k*测量值+（1-k）*估计值 = 估计值+k*（测量值-估计值） k为卡尔曼增益
 * 故卡尔曼本质即为数值不断迭代调节k，获取不确定性最小数值
 * >>如何获得估计值？
 * 估计值Xt = AXt-1+BUt-1+W，噪声满足正态分布W~(0,Q)，A、B为数学公司计算矩阵，X为状态量，U为控制量
 * 测量值Zt = HXt+V，H一般为单位矩阵 噪声满足正态分布V~N(0,R)
 * >>如何测数据中噪声成分
 * 协方差矩阵：（Pt）' = APt-1A^t+Q,（Pt）'为先验数据，APt-1A^t来自上一次估计（一维时值为1），Q为这一次的噪声
 * 由于上一次数据就有噪声，基于上一次估计必然带有上一次噪声，而估计本身就引入新噪声，我们认为这是固定的，故得到估计值的不确定性
 * 而测量值噪声R不需要计算，因为厂家会把传感器测量误差写好，这是固定的数据
 * >>获取卡尔曼增益ｋ
 * （公式上网查）
 * >>如何完成迭代，即更新输出的不确定性
 * Pt　= (I-KtH)(Pt)',(Pt)'为先验（一维时H、I为1）
 * 总结：五个变量（协方差，上一次协方差，过程噪声，观测噪声，增益），五个公式
 */
float KalmanFilter(float DataIn, KalmanParam *kfp); // 卡尔曼滤波器

float Iir_Filter(float DataIn); // IIR滤波器

#endif
