%角度变化率非极大抑制
function angle_out = nms_angle(angle_in,num,angle_out,kernel)

half = 10;
for i = 1:num
    angle_out(i) = angle_in(i);
    for j = -half:half
        if abs(angle_in(clip(i+j,1,num))) > abs(angle_out(i))
            angle_out(i) = 0;
            break;
        end
    end

end