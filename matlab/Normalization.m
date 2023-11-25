function [rpts,rptsn_num] = Normalization(rptsc0,rptsc0_num,rptsc1,rptsc1_num)
i=0;


begin_id = -1;

for i:rptsn_num
        float dx = rpts[i][0] - cx;
        float dy = rpts[i][1] - cy;
        %float dist = mySqrt(dx * dx + dy * dy);
        float dist = sqrt(dx * dx + dy * dy); 
        if(dist<min_dist)
            min_dist = dist;
            begin_id = i;
        end

end


    if (begin_id >= 0 && rpts_num - begin_id >= 3) {
        rpts[begin_id][0] = cx;
        rpts[begin_id][1] = cy;
        rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);
        resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);
