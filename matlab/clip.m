%限幅函数
function y = clip(x,low,up)

if x > up
    y = up;
elseif x < low
    y = low;
else
    y = x;
end
