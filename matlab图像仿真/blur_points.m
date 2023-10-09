%三角滤波
function pts_out = blur_points(pts_in,num,pts_out,kernel)
half = (kernel - 1)/2;
for i = 1:num
    pts_out(i,1) = 0;
    pts_out(i,2) = 0;
    for j = -half:half
        pts_out(i,1) = pts_out(i,1) + pts_in(clip(i+j,1,num),1)*(half + 1 - abs(j));
        pts_out(i,2) = pts_out(i,2) + pts_in(clip(i+j,1,num),2)*(half + 1 - abs(j));
    end
    pts_out(i,1) = pts_out(i,1) / ((2 * half + 2) * (half + 1) / 2);
    pts_out(i,2) = pts_out(i,2) / ((2 * half + 2) * (half + 1) / 2);
end