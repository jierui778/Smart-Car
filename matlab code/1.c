/*************************************************************************
 *  函数名称：void Left_Adaptive_Threshold();
 *  功能说明：自定义阈值二值化+迷宫算法（左边线）
 *  参数说明：获取的图像数组，输出数组（备用image0），图像宽，图像高
 *  函数返回：无
 *  修改时间：2022年9月24日
 *  备    注：
 * 前进方向定义：
 *   0
 * 3   1
 *   2
 *************************************************************************/

const int dir_front[4][2] = {{0,  -1},
                            {1,  0},
                              {0,  1},
                              {-1, 0}};
const int dir_frontleft[4][2] = {{-1, -1},
                                  {1,  -1},
                                   {1,  1},
                                  {-1, 1}};
const int dir_frontright[4][2] = {{1,  -1},
                                   {1,  1},
                                    {-1, 1},
                                  {-1, -1}};

void Left_Adaptive_Threshold(image_t *img,int block_size,int clip_value,int x,int y,int pts[][2],int *num)
{
    zf_assert(img && img->data);     // 不满足则退出执行
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1);//保证block_size为奇数
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0; // step表示前进的步数；dir通过改变索引改变当前小人朝向的方向
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4)//保证block不出界
    {
        int local_thres = 1;
        //自适应二值化
        /*
        for (int dy = -half; dy <= half; dy++)  // for循环用来计算block区域的像素值之和（自适应阈值）
        {
            for (int dx = -half; dx <= half; dx++) 
            {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;   // (x,y)点block区域内的阈值
        */
        int current_value = AT(img, x, y);//当前像素点灰度值
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);//正前方像素点灰度值 （dir=0 下；dir=1 右；dir=2 上；dir=3 左）
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);//左前方像素点灰度值 （dir=0左下；dir=1 右下；dir=2 右上；dir=3 左上 ）
        //=======添加部分=======
        if( (x==1 && y < img->height -20) || x== img->width - 2 || y==1 )     //到上左右边界就停下
        {
            if(x==1)  touch_boundary0  = 1;                //左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，

            break;
        }
        //=======添加部分=======
        if (front_value < local_thres)  // 前进方向像素为黑色
        {
            dir = (dir + 1) % 4;   // 遇到前方为黑色需要右转一次
            turn++;
        } 
        else if (frontleft_value < local_thres) // 前方像素为白色，且左前方像素为黑色
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;   // 用来存放边线坐标信息
            pts[step][1] = y;
            step++;
            turn = 0;
            //AT(img,x,y) = RGB565_GREEN;
        } 
        else  // 前方为白色，左前方为白色（墙角）
        {
            x += dir_frontleft[dir][0];   // 遇到墙角要斜着走
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;   // 遇到墙角要左转一次
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            //AT(img,x,y) = RGB565_GREEN;
        }
    }
    *num = step;

}
