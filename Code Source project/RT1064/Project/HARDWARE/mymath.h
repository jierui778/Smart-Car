#include "zf_common_headfile.h"
#ifndef _MYMATH_H
#define _MYMATH_H

//一些用到的数学函数

void QuickSort(int *arr, int low, int high);//快速排序
float Fsqrt(float x);//浮点数开方
uint8 Atan2(float y, float x);
int my_abs(int value);
int16 limit_a_b(int16 x, int a, int b);
float slope_to_degree(float slope) ;

int clip(int x, int low, int up);
#endif
