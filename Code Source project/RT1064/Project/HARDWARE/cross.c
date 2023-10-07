//#include "cross.h"


//enum cross_type_e cross_type = CROSS_NONE;//通过枚举类型判断是不是cross

//bool far_Lpt0_found, far_Lpt1_found;//其中 far_N_Lpt0_found 表示是否找到了反向逆透视后的左边L角点，far_N_Lpt1_found 表示是否找到了反向逆透视后的右边L角点。
//bool far_N_Lpt0_found,far_N_Lpt1_found;//这个变量用于表示是否找到了逆逆透视后的左边L角点和右边L角点。其中，far_N_Lpt0_found表示左边L角点是否被找到，far_N_Lpt1_found表示右边L角点是否被找到。

//int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;//far_Lpt0_rpts0s_id表示左边L角点在rpts0s数组中的索引值
//int far_N_Lpt0_rpts0s_id,far_N_Lpt1_rpts1s_id;//这两个变量是用于记录L角点在反向逆透视后的rpts0s数组中的索引值

//int L_x0, L_x1 , L_y0 , L_y1;                       //十字得近处两个L点的坐标

//int low_x0,low_y0, low_x1 , low_y1;         //十字得近处两个L点的坐标得下面2个点，用来计算斜率来补线

//int far_x0 =0, far_x1 =0 , far_y0 = 0, far_y1 = 0 ;                  //寻找到的远处起始点坐标
//float Nfar_x0 =0, Nfar_x1 =0 , Nfar_y0 = 0, Nfar_y1 = 0 ;                  //寻找到的远处逆透视点坐标

//float xielv_left, xielv_right;//左线斜率，右线斜率

//// 以下定义为十字寻远线设定
////原始的远L角点左边线的坐标
//int far_ipts0[L_CROSS][2];
//int far_ipts1[L_CROSS][2];
//int far_ipts0_num, far_ipts1_num;

////透视变换后的左右两边线的坐标点和坐标点的个数，用于检测单边时使用
//float far_rpts0[L_CROSS][2];
//float far_rpts1[L_CROSS][2];
//int far_rpts0_num, far_rpts1_num;

////透视变换后的左右两边线的坐标点和坐标点的个数，用于寻找远处L角点进行补线时使用
//float far_rpts0b[L_CROSS][2];
//float far_rpts1b[L_CROSS][2];
//int far_rpts0b_num, far_rpts1b_num;

//float far_rpts0s[L_CROSS][2];
//float far_rpts1s[L_CROSS][2];
//int far_rpts0s_num, far_rpts1s_num;

//float far_rpts0a[L_CROSS];
//float far_rpts1a[L_CROSS];
//int far_rpts0a_num, far_rpts1a_num;

//float far_rpts0an[L_CROSS];
//float far_rpts1an[L_CROSS];
//int far_rpts0an_num, far_rpts1an_num;

//int not_have_line = 0;//判断是否有线

//extern float xielv_left_y_to_end,xielv_right_y_to_end;                 //在逆透视后得坐标系建得斜率

//int cross_num = 0;