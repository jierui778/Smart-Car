%sobel算子
function out_IMG = Sobel(in_IMG,out_IMG,Threshold)
image=double(in_IMG);
[h,w]=size(image);

if Threshold == 0 
    for i = 2:h-2
        DN = i + 1;     
        UP = i - 1;
        for j = 2:w-2
            RR = j + 1;     
            LL = j - 1;
            Gx=(image(UP,RR)+2*image(i,RR)+image(DN,RR))-(image(UP,LL)+2*image(i,LL)+image(DN,LL));
            Gy=(image(UP,LL)+2*image(UP,j)+image(UP,RR))-(image(DN,LL)+2*image(DN,j)+image(DN,RR));
            Sobel_G = abs(Gx)+abs(Gy);
            out_IMG(i,j) = Sobel_G;
        end
    end
else
        for i = 2:h-2
        DN = i + 1;     
        UP = i - 1;
            for j = 2:w-2
                RR = j + 1;     
                LL = j - 1;
                Gx=(image(UP,RR)+2*image(i,RR)+image(DN,RR))-(image(UP,LL)+2*image(i,LL)+image(DN,LL));
                Gy=(image(UP,LL)+2*image(UP,j)+image(UP,RR))-(image(DN,LL)+2*image(DN,j)+image(DN,RR));
                Sobel_G = abs(Gx)+abs(Gy);
                if Sobel_G>= Threshold
                    out_IMG(i,j) = 0;
                else
                    out_IMG(i,j) = 255;
                end
            end
        end
end
