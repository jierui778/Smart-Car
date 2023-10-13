//#include "zf_common_headfile.h"
//#ifndef __CROSS_H
//#define __CROSS_H


//#include "image.h"

//enum cross_type_e {
//    CROSS_NONE = 0,     // 非十字模式
//    CROSS_BEGIN,        // 找到左or右两个L角点(包括单线情况)
//    CROSS_IN_DOUBLE,    // 双L角点寻远线，两个L角点很近，即进入十字内部(此时切换远线控制)
//    CROSS_INHALF_LEFT,  // 单L角点寻远线(只看得到左边)
//    CROSS_INHALF_RIGHT, // 单L角点寻远线(只看得到右边)
//    CROSS_NUM,
//};
//extern enum cross_type_e cross_type;

//#define L_CROSS 40

//extern int cross_num;
//extern int L_x0, L_x1 , L_y0 , L_y1;                       //十字得近处两个L点的坐标
//extern int far_x0, far_x1 , far_y0 , far_y1 ;                  //寻找到的远处起始点
//extern float Nfar_x0, Nfar_x1  , Nfar_y0 , Nfar_y1 ;  //寻找到的远处逆透视点
//extern float xielv_left, xielv_right;
//extern int not_have_line ;

////以下为十字控制寻远线操作,算法与常规寻线相同

//extern bool far_Lpt0_found, far_Lpt1_found;
//extern bool far_N_Lpt0_found,far_N_Lpt1_found;
//extern int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;
//extern int far_N_Lpt0_rpts0s_id,far_N_Lpt1_rpts1s_id;
////原图远线左右边线
//extern int far_ipts0[L_CROSS][2];
//extern int far_ipts1[L_CROSS][2];
//extern int far_ipts0_num, far_ipts1_num;
////变换后左右远边线
//extern float far_rpts0[L_CROSS][2];
//extern float far_rpts1[L_CROSS][2];
//extern int far_rpts0_num, far_rpts1_num;
////变换后左右远边线+滤波
//extern float far_rpts0b[L_CROSS][2];
//extern float far_rpts1b[L_CROSS][2];
//extern int far_rpts0b_num, far_rpts1b_num;
////变换后左右远边线+等距采样
//extern float far_rpts0s[L_CROSS][2];
//extern float far_rpts1s[L_CROSS][2];
//extern int far_rpts0s_num, far_rpts1s_num;
//// 左右远边线局部角度变化率
//extern float far_rpts0a[L_CROSS];
//extern float far_rpts1a[L_CROSS];
//extern int far_rpts0a_num, far_rpts1a_num;
//// 左右远边线局部角度变化率+非极大抑制
//extern float far_rpts0an[L_CROSS];
//extern float far_rpts1an[L_CROSS];
//extern int far_rpts0an_num, far_rpts1an_num;

//extern float xielv_left, xielv_right;




//#endif






//void Find_Borderline(void)
//{
//    // 迷宫巡线是否走到左右边界
//    touch_boundary0 = 0; // 清零
//    touch_boundary1 = 0;

//    // 迷宫巡线是否走到上边界
//    touch_boundary_up0 = 0; // 清零
//    touch_boundary_up1 = 0;

//    // 底边扫线防止丢线 注意：由于sobel边缘检测特殊性（黑框），此丢线标志仅适用Ostu方案
//    loseline0 = 0;
//    loseline1 = 0;
//	/*添加*/
////	begin_y=Image_Get_LeftPoint(117);
//    uint8 uthres = 1;
//    //    uint8 uthres = ostu();
//    // 寻左边线
//    x1 = img_raw.width / 2 - begin_x, y1 = begin_y;
//    int TH;
//    TH = OSTU_GetThreshold(Image_Use[0], IMAGE_WIDTH, IMAGE_HEIGHT);
//    Image_Binarization(TH, Image_Use);
//    Image_Sobel(Image_Use, Image_Use_Robert, TH); // 全局Sobel得二值图(方案二) 2.8ms
//    img_raw.data = *Image_Use;

//    // 标记种子起始点(后续元素处理要用到)
//    x0_first = x1;
//    y0_first = y1;

//    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]); // 求数组的长度
//    // 扫底下五行，寻找跳变点
//    for (; y0_first > begin_y - 50; y0_first--)//从所选的行，向上扫5次，每次从中间向左线扫
//    {
//        for (; x0_first > 0; x0_first--)//在选的每行中，从中间向左线扫
//            if (AT_IMAGE(&img_raw, x0_first - 1, y0_first) < uthres)//如果扫到黑点（灰度值为0），就从该点开始扫线
//                goto out1;//开始扫左线
//        x0_first = img_raw.width / 2 - begin_x;//每次每一行扫完，都把x0_first归位
//    }
//    //如果扫不到的话，判定左边的底边丢线
//    loseline0 = 1; // 底边丢线
//	out1://从起始点开始执行扫线
//	{
//		if (AT_IMAGE(&img_raw, x0_first, y0_first) >= uthres)//如果这个点是白色（且左边是黑色的话）
//			Left_Adaptive_Threshold(&img_raw, block_size, clip_value, x0_first, y0_first, ipts0, &ipts0_num);//开始跑迷宫
//		else
//			ipts0_num = 0;//如果不是的话，就不用跑了，求得的number记为0
//	}

//    // 寻右边线
//    x2 = img_raw.width / 2 + begin_x, y2 = begin_y;

//    // 标记种子起始点(后续元素处理要用到)
//    x1_first = x2;
//    y1_first = y2;

//    ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
//    for (; y1_first > begin_y - 50; y1_first--)
//    {
//        for (; x1_first < img_raw.width - 1; x1_first++)
//            if (AT_IMAGE(&img_raw, x1_first + 1, y1_first) < uthres)
//                goto out2;
//        x1_first = img_raw.width / 2 + begin_x;
//    }
//    loseline1 = 1; // 底边丢线
//	out2:
//	{
//		if (AT_IMAGE(&img_raw, x1_first, y1_first) >= uthres)
//			Right_Adaptive_Threshold(&img_raw, block_size, clip_value, x1_first, y1_first, ipts1, &ipts1_num);
//		else
//			ipts1_num = 0;
//	}

//}

//const int dir_front[4][2] = {{0, -1},
//                             {1, 0},
//                             {0, 1},
//                             {-1, 0}};
//const int dir_frontleft[4][2] = {{-1, -1},
//                                 {1, -1},
//                                 {1, 1},
//                                 {-1, 1}};
//const int dir_frontright[4][2] = {{1, -1},
//                                  {1, 1},
//                                  {-1, 1},
//                                  {-1, -1}};

//#define AT AT_IMAGE
//#define MAX_WIDTH 88 定义图像中
//void Left_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
//{
//    zf_assert(img && img->data); // 不满足则退出执行
//    zf_assert(num && *num >= 0);
//    zf_assert(block_size > 1 && block_size % 2 == 1); // 保证block_size为奇数
//    //    int half = block_size / 2;                       //上交方案
//    int half = 0;                                                                                                      // 方案二
//    int step = 0, dir = 0, turn = 0;                                                                                   // step表示前进的步数；dir通过改变索引改变当前小人朝向的方向
//    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4) // 保证block不出界
//        /*保证step步数小于Num   保证x（列）坐标>0同时小于宽度-1   保证y（行）坐标大于0小于高度-1*/
//    {
//        int local_thres = 1;
//        //        int local_thres;
//        // 自适应二值化
//        /*
//            for (int dy = -half; dy <= half; dy++)  // for循环用来计算block区域的像素值之和（自适应阈值）
//            {
//                for (int dx = -half; dx <= half; dx++)
//                {
//                    local_thres += AT(img, x + dx, y + dy);
//                }
//            }
//            local_thres /= block_size * block_size;
//            local_thres -= clip_value;   // (x,y)点block区域内的阈值
//        */
//        int current_value = AT(img, x, y);                                                   // 当前像素点灰度值
//        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);             // 正前方像素点灰度值 （dir=0 下；dir=1 右；dir=2 上；dir=3 左）
//        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]); // 左前方像素点灰度值 （dir=0左下；dir=1 右下；dir=2 右上；dir=3 左上 ）
//        //=======添加部分=======（限制条件）
//        /*  当扫点的列坐标到左黑框边界且行坐标小于20    列坐标到右边的黑框边界  行坐标为1   行坐标为88的同时步数已经大于19*/
//        if ((x == 1 && y < img->height - 10) || x == img->width - 2 || y == 1 || (y == 100 && step > 19))
//        {
//            if (x == 1 /*|| x== img->width - 2*/)
//                touch_boundary0 = 1; // 左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
//            if (y == 1)
//                touch_boundary_up0 = 1; // 走到顶边，判断坡道or障碍

//            break;
//        }
//        //=======添加部分=======
//        if (front_value < local_thres) // 前进方向像素为黑色
//        {
//            dir = (dir + 1) % 4; // 遇到前方为黑色需要右转一次
//            turn++;
//        }
//        else if (frontleft_value < local_thres) // 前方像素为白色，且左前方像素为黑色
//        {
//            x += dir_front[dir][0];
//            y += dir_front[dir][1];
//            pts[step][0] = x; // 用来存放边线坐标信息
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            // AT(img,x,y) = RGB565_GREEN;
//        }
//        else // 前方为白色，左前方为白色（墙角）
//        {
//            x += dir_frontleft[dir][0]; // 遇到墙角要斜着走
//            y += dir_frontleft[dir][1];
//            dir = (dir + 3) % 4; // 遇到墙角要左转一次
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            // AT(img,x,y) = RGB565_GREEN;
//        }
//    }
//    // 丢线标志，否则由于sobel特殊性会一直往上巡线
//    if (step < 20 && touch_boundary0)
//        loseline0 = 1;
//    // 记录边线数目
//    *num = step;
//    uint8 i;
//   	for(i=0;i<ipts0_num;i++)
//   	{
//   		ips200_draw_line(0,0,ipts0[i][0],ipts0[i][1],RGB565_RED);
//   	}
//    // ips200_show_int(3,140,ipts0_num,3);
//    // ips200_show_int(3,160,loseline0,3);
//    // ips200_show_int(3,180,*num,3);
//    //	tft180_draw_line(0,0,ipts0[20-1][1],ipts0[20-1][0],RGB565_RED);
//    //	tft180_show_int(3,100,ipts0[20-1][1],4);
//}
///*************************************************************************
// *  函数名称：void Right_Adaptive_Threshold();
// *  功能说明：自定义阈值二值化+迷宫算法（右边线）
// *  参数说明：获取的图像数组，输出数组（备用image0），图像宽，图像高
// *  函数返回：无
// *  修改时间：2023年5月23日
// *  备    注： 增加丢线判断
// * 前进方向定义：
// *   0
// * 3   1
// *   2
// *************************************************************************/
//void Right_Adaptive_Threshold(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num)
//{
//    uint8 i;
//       zf_assert(img && img->data);
//       zf_assert(num && *num >= 0);
//       zf_assert(block_size > 1 && block_size % 2 == 1);
////       int half = block_size / 2;        //上交方案
//    int half = 0; // 方案二
//    int step = 0, dir = 0, turn = 0;
//    while (step < *num && 0 < x && x < img->width - 1 && half < y && y < img->height - half - 1 && turn < 4)
//    {
//        int local_thres = 1;
//        //        int local_thres;
//        // 自适应二值化
//        /*
//                for (int dy = -half; dy <= half; dy++) {
//                    for (int dx = -half; dx <= half; dx++) {
//                        local_thres += AT(img, x + dx, y + dy);
//                    }
//                }
//                local_thres /= block_size * block_size;
//                local_thres -= clip_value;
//        */
//        int current_value = AT(img, x, y);
//        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
//        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
//        //=======添加部分=======
//        if ((x == img->width - 2 && y < img->height - 20) || x == 1 || y == 1 || (y == 100 && step > 19)) // 丢线标志，否则由于sobel特殊性会一直往上巡线
//        {
//            if (x == img->width - 2 /*|| x==1*/)
//                touch_boundary1 = 1; // 右边界是因为到最右边才停下来的，触碰到最右边，可能是环岛，十字等，
//            if (y == 1)
//                touch_boundary_up1 = 1; // 走到顶边，判断坡道or障碍
//            break;
//        }
//        //=======添加部分=======
//        if (front_value < local_thres)
//        {
//            dir = (dir + 3) % 4;
//            turn++;
//        }
//        else if (frontright_value < local_thres)
//        {
//            x += dir_front[dir][0];
//            y += dir_front[dir][1];
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            // AT(img,x,y) = RGB565_YELLOW;
//        }
//        else
//        {
//            x += dir_frontright[dir][0];
//            y += dir_frontright[dir][1];
//            dir = (dir + 1) % 4;
//            pts[step][0] = x;
//            pts[step][1] = y;
//            step++;
//            turn = 0;
//            // AT(img,x,y) = RGB565_YELLOW;
//        }
//    }
//    // 丢线标志，否则由于sobel特殊性会一直往上巡线
//    if (step < 20 && touch_boundary1)
//        loseline1 = 1;
//    // 记录边线数目
//    *num = step;
//     for (i = 0; i < ipts1_num; i++)
//     {
//         ips200_draw_line(160,0,ipts1[i][0] + 2, ipts1[i][1] + 2, RGB565_BLUE);
//     }
//    // ips200_show_int(43,140,ipts1_num,3);
//    // ips200_show_int(43,160,loseline1,3);
//    // ips200_show_int(43,180,*num,3);
//}
