%左边线跟踪中线
function pts_out = track_leftline(pts_in,num,pts_out,approx_num,dist)
for i = 1:num
    dx = pts_in(clip(i + approx_num,1,num),1) - pts_in(clip(i - approx_num,1,num),1);
    dy = pts_in(clip(i + approx_num,1,num),2) - pts_in(clip(i - approx_num,1,num),2);
    dn = sqrt(dx*dx+dy*dy);
    dx = dx / dn;
    dy = dy / dn;
    pts_out(i,1) = pts_in(i,1) + dy*dist;
    pts_out(i,2) = pts_in(i,2) - dx*dist;
end