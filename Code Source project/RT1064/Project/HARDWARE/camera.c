#include "camera.h"

/**
 * @brief 摄像头初始化
 *
 * @return uint8 初始化完成标志：1-完成，0-未完成
 */
uint8 Camera_Init(void)
{
    uint8 init_finishflag = 0;
    init_finishflag = mt9v03x_init();
    interrupt_global_enable(0);
    return init_finishflag;
}
