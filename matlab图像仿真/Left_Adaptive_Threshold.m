%左手迷宫巡线
function [ipts0,ipts0_num] = Left_Adaptive_Threshold(input_data,x1,y1,pts,ipts0_num)

%image=input_data;
[h,w]=size(input_data);
ipts0 = zeros(size(pts));
%上交方案
step = 1;
dir = 0;
turn = 0;
half = 0;



%定义转向索引矩阵
dir_front = [0,-1;
             1, 0;
             0, 1;
            -1, 0];
dir_frontleft = [-1,-1;
                  1,-1;
                  1, 1;
                 -1, 1];
dir_frontright = [ 1,-1;
                   1, 1;
                  -1, 1;
                  -1,-1];

while (step < 121 ...
        && half < x1 ...
        && x1 < w - half - 1 ...
        && half < y1 ...
        && y1 < h - half - 1 ...
        && turn < 4)
    local_thres = 1;
    current_value = input_data(y1,x1);%读取当前像素点灰度值

    Dir = dir + 1;

    front_value = input_data(y1 + dir_front(Dir,2),x1 + dir_front(Dir,1));%读取正前像素点像素值
    frontleft_value = input_data(y1 + dir_frontleft(Dir,2),x1 + dir_frontleft(Dir,1));%读取左前方像素点像素值

    if ((x1==1 && y1 < h -10) || x1 == w - 2 || y1==1||(y1==100&&step>19))
        if x1==1
            touch_boundary0 =1;%左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，
            break;

        end

    end
    if front_value < local_thres%前进方向像素为黑色
        dir = mod(dir + 1,4);%遇到前方为黑色需要右转一次
        turn = turn + 1;

    elseif frontleft_value < local_thres%前方像素为白色，且左前方像素为黑色
        x1 = x1 + dir_front(Dir,1);
        y1 = y1 + dir_front(Dir,2);
        pts(step,1) = x1;%用来存放边线坐标信息
        pts(step,2) = y1;
        step = step +1;
        turn = 0;
    else%/ 前方为白色，左前方为白色（墙角）
        x1 = x1 + dir_frontleft(Dir,1);
        y1 = y1 + dir_frontleft(Dir,2);
        dir = mod(dir + 3,4);
        pts(step,1) = x1;
        pts(step,2) = y1;
        step = step +1;
        turn = 0;
    end
    ipts0_num = step-1;
    ipts0 = pts;
end

