/*************************************************************************
 *  �������ƣ�void Left_Adaptive_Threshold();
 *  ����˵�����Զ�����ֵ��ֵ��+�Թ��㷨������ߣ�
 *  ����˵������ȡ��ͼ�����飬������飨����image0����ͼ���ͼ���
 *  �������أ���
 *  �޸�ʱ�䣺2022��9��24��
 *  ��    ע��
 * ǰ�������壺
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
    zf_assert(img && img->data);     // ���������˳�ִ��
    zf_assert(num && *num >= 0);
    zf_assert(block_size > 1 && block_size % 2 == 1);//��֤block_sizeΪ����
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0; // step��ʾǰ���Ĳ�����dirͨ���ı������ı䵱ǰС�˳���ķ���
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4)//��֤block������
    {
        int local_thres = 1;
        //����Ӧ��ֵ��
        /*
        for (int dy = -half; dy <= half; dy++)  // forѭ����������block���������ֵ֮�ͣ�����Ӧ��ֵ��
        {
            for (int dx = -half; dx <= half; dx++) 
            {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;   // (x,y)��block�����ڵ���ֵ
        */
        int current_value = AT(img, x, y);//��ǰ���ص�Ҷ�ֵ
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);//��ǰ�����ص�Ҷ�ֵ ��dir=0 �£�dir=1 �ң�dir=2 �ϣ�dir=3 ��
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);//��ǰ�����ص�Ҷ�ֵ ��dir=0���£�dir=1 ���£�dir=2 ���ϣ�dir=3 ���� ��
        //=======��Ӳ���=======
        if( (x==1 && y < img->height -20) || x== img->width - 2 || y==1 )     //�������ұ߽��ͣ��
        {
            if(x==1)  touch_boundary0  = 1;                //��߽�����Ϊ������߲�ͣ�����ģ�����������ߣ������ǻ�����ʮ�ֵȣ�

            break;
        }
        //=======��Ӳ���=======
        if (front_value < local_thres)  // ǰ����������Ϊ��ɫ
        {
            dir = (dir + 1) % 4;   // ����ǰ��Ϊ��ɫ��Ҫ��תһ��
            turn++;
        } 
        else if (frontleft_value < local_thres) // ǰ������Ϊ��ɫ������ǰ������Ϊ��ɫ
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;   // ������ű���������Ϣ
            pts[step][1] = y;
            step++;
            turn = 0;
            //AT(img,x,y) = RGB565_GREEN;
        } 
        else  // ǰ��Ϊ��ɫ����ǰ��Ϊ��ɫ��ǽ�ǣ�
        {
            x += dir_frontleft[dir][0];   // ����ǽ��Ҫб����
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;   // ����ǽ��Ҫ��תһ��
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            //AT(img,x,y) = RGB565_GREEN;
        }
    }
    *num = step;

}
