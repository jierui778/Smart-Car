%右手迷宫巡线
%左手迷宫巡线
function [ipts1,ipts1_num] = Right_Adaptive_Threshold(input_data,x2,y2,pts,ipts1_num)

%image=input_data;
[h,w]=size(input_data);
ipts1 = zeros(size(pts));
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
        && 0 < x2 ...
        && x2 < w - 1 ...
        && 0 < y2 ...
        && y2 < h - 1 ...
        && turn < 4)
    local_thres = 1;
    current_value = input_data(y2,x2);
    Dir = dir + 1;
    front_value = input_data(y2 + dir_front(Dir,2),x2 + dir_front(Dir,1));
    frontright_value = input_data(y2 + dir_frontright(Dir,2),x2 + dir_frontright(Dir,1));
    if ((x2==w-2 && y2 < h -20) || x2==1 || y2==1)
        if x2==1
            touch_boundary1 =1;
            break;
        end
    end
    if front_value < local_thres
        dir = mod(dir + 3,4);
        turn = turn + 1;
    elseif frontright_value < local_thres
        x2 = x2 + dir_front(Dir,1);
        y2 = y2 + dir_front(Dir,2);
        pts(step,1) = x2;
        pts(step,2) = y2;
        step = step +1;
        turn = 0;
    else
        x2 = x2 + dir_frontright(Dir,1);
        y2 = y2 + dir_frontright(Dir,2);
        dir = mod(dir + 1,4);
        pts(step,1) = x2;
        pts(step,2) = y2;
        step = step +1;
        turn = 0;
    end
    ipts1_num = step-1;
    ipts1 = pts;
end