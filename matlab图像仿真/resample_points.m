% 等距采样
function [pts_out,num2] = resample_points(pts_in,num1,pts_out,num2,dist)
remain = 0;
len = 1;
i = 1;
while i<num1&&len<num2
    x0 = pts_in(i,1);
    y0 = pts_in(i,2);
    dx = pts_in(i+1,1) - x0;
    dy = pts_in(i+1,2) - y0;
    % if dx>7||dy>7 % 判断两点之间的距离是否大于阈值
    %     i = i + 1;
    %     continue;
    % end
    dn = sqrt(dx*dx+dy*dy);
    dx = dx/dn;
    dy = dy/dn;

    while remain < dn && len < num2 + 1
        x0 = x0 + dx*remain;
        pts_out(len,1) = x0;
        y0 = y0 + dy*remain;
        pts_out(len,2) = y0;

        len = len + 1;
        dn = dn - remain;
        remain = dist;
    end
    remain = remain - dn;
    i = i+1;
end


