%补线 逆透视视角
function [pts_out,num] = draw_line2(X0,Y0, X1,Y1,pts_out,num,dist)
remain = 0;
len = 1;


x0 = X0;
y0 = Y0;
dx = X1 - x0;
dy = Y1 - y0;
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

num = len;