#ifndef IMGVALUE_CROSS_H_
#define IMGVALUE_CROSS_H_

#include "zf_common_headfile.h"
#define PI               3.14159265358979f

enum cross_type_e {
    CROSS_NONE = 0,     // 非十字模式
    CROSS_FOUND,        // 找到左or右两个L角点(包括单线情况)
    // CROSS_IN_DOUBLE,    // 双L角点寻远线，两个L角点很近，即进入十字内部(此时切换远线控制)
    CROSS_IN,  //
    CROSS_OUT, //
    CROSS_END,
//    CROSS_NUM,
};
extern enum cross_type_e cross_type;

void Cross_Chreck(void);
void Cross_Run(void);
//void cross_drawline();


#endif /*IMGVALUE_CROSS_H_*/
