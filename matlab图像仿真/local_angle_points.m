%局部角度变化率
function angle_out = local_angle_points(pts_in,num,angle_out,dist)
dist = 10;
for i = 1:num
    if i <= 1 || i >= num
        angle_out(i) = 0;
        continue;
    end
    dx1 = pts_in(i,1) - pts_in(clip(i-dist,1,num),1);
    dy1 = pts_in(i,2) - pts_in(clip(i-dist,1,num),2);
    dn1 = sqrt(dx1*dx1 + dy1*dy1);

    dx2 = pts_in(clip(i+dist,1,num),1) - pts_in(i,1);
    dy2 = pts_in(clip(i+dist,1,num),2) - pts_in(i,2);
    dn2 = sqrt(dx2*dx2 + dy2*dy2);

    c1 = dx1 / dn1;
    s1 = dy1 / dn1;
    c2 = dx2 / dn2;
    s2 = dy2 / dn2;

    angle_out(i) = atan2(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
end