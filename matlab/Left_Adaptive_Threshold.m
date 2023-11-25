%左手迷宫巡线
function [ipts0,ipts0_num] = Left_Adaptive_Threshold(input_data,x1,y1,pts,ipts0_num)
%inputdata为输入图像，x1和y1为寻找黑点的
%image=input_data;
[h,w]=size(input_data);%读取输入图像的高和宽
ipts0 = zeros(size(pts));%边线数组
step = 1;%迷宫步数
dir = 0;%方向
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



while (step < 121 ...%限制边线数组长度
        && half < x1 ...
        && x1 < w - half ...
        && half < y1 ...
        && y1 < h - half - 1 ...
        && turn < 4)
    local_thres = 1;

    current_value = input_data(y1,x1);%读取当前像素点的灰度值

    Dir = dir + 1;
    front_value = input_data(y1 + dir_front(Dir,2),x1 + dir_front(Dir,1));
    frontleft_value = input_data(y1 + dir_frontleft(Dir,2),x1 + dir_frontleft(Dir,1));
    frontright_value = input_data(y1 + dir_frontright(Dir,2),x1 + dir_frontright(Dir,1));
    if (((x1==3 && y1 < h -40) || x1 == w - 1 || y1==2)&&step>30)%限制条件，后期看情况修改
        if x1==3
            touch_boundary0 =1;%触碰到最左边，可能是环岛，十字
            break;
        end
        % if y1==1
        %     touch_boundary_up0==1;%触碰到上边界，可能是坡道
        % end
    end

    

    if front_value < local_thres%前像素为黑
        dir = mod(dir + 1,4);%右转一次
        turn = turn + 1;%转向
    elseif frontleft_value < local_thres%前方像素为白色且左前方像素为黑色
        x1 = x1 + dir_front(Dir,1);
        y1 = y1 + dir_front(Dir,2);


        if(x1> 3)%防止迷宫沿边框爬动，造成干扰
        pts(step,1) = x1;
        pts(step,2) = y1;
        step = step +1;
        turn = 0;
        end
    else
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





%tips考虑到后面三角滤波和等距采样会对有十字空白跳跃的数组产生干扰，我们选择迷宫触碰到边界则停止巡线，给予标志位对远线进行迷宫