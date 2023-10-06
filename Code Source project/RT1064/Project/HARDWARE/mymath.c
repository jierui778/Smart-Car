#include "mymath.h"
/**
 * @brief 快速反向平方根开方
 *
 * @param x 要开方的数
 * @return float 开方结果
 */
float Fsqrt(float x)
{
    // float res = x;
    // float eps = 1e-6; // 精度
    // while (fabs(res * res - x) > eps)
    //
    //     res = (res + x / res) / 2;
    // }
    // return res; // 牛顿迭代法：多次迭代后，结果会越来越接近真实值

    float xhalf = 0.5f * x;
    int i = *(int *)&x; // evil floating point bit level hacking
    // i = 0x5f3759df - (i >> 1); // what the fuck?
    i = 0X5F3504F3 - (i >> 1); // 精度更高
    x = *(float *)&i;
    x = x * (1.5f - (xhalf * x * x));
    return 1 / x; // 快速反向平方根，精度相对较低
}

/**
 * @brief 快速排序
 *
 * @param a 要排序的数组
 * @param low 数组起始下标
 * @param high 数组结束下标
 */
void QuickSort(int *a, int low, int high)
{
    if (low >= high)
    {
        return;
    }
    int i = low;
    int j = high;
    int key = a[i];
    while (i < j)
    {
        while (i < j && a[j] >= key)
        {
            j--;
        }
        a[i] = a[j];
        while (i < j && a[i] <= key)
        {
            i++;
        }
        a[j] = a[i];
    }
    a[i] = key;
    QuickSort(a, low, i - 1);
    QuickSort(a, i + 1, high);
}
/**
 * @brief
 *
 * @param y
 * @param x
 * @return uint8
 */
uint8 Atan2(float y, float x)
{
    float tanNum;
    uint8 alpha; // 返回角度
    tanNum = y / x;
    if (tanNum > -0.41421365 && tanNum < 0.41421356)
    {
        alpha = 0; // 水平方向
    }
    else if (tanNum >= 0.41421356 && tanNum < 2.41421356)
    {
        alpha = 1; // 右上、左下
    }
    else if (tanNum <= -0.41421356 && tanNum > -2.41421362)
    {
        alpha = 3; // 左上、右下
    }
    else
    {
        alpha = 2; // 竖直方向
    }

    return alpha; // 方向
} // end of function Atan2











