%%Author：18届摄像头组 HQC
%%建立世界坐标系  小车摄像头位于世界坐标原点上方（调整好角度，镜头在正上方）
%%小车车头方向为y正轴，小车车体
clear all;
close all;
clc;
%%
<<<<<<< HEAD
img= imread('changzhidao.jpg');%如果是bmp结尾的图片文件需要下面rgb2gray这句话。jpg格式就把他注释掉
%img= rgb2gray(img);
img = double(img);  %如果是jpg结尾的图片文件需要double这句话。bmp格式就把他注释掉
[M , N] = size(img);  %图像宽高
MM = M+60;

ipts0          = zeros(MM,2);
ipts0_num      = MM;
ipts1          = zeros(MM,2);
ipts1_num      = MM;%存放边线的数组
%inv_ipts0      = zeros(MM,2);
%inv_ipts0_num  = MM;
%inv_ipts1      = zeros(MM,2);
%inv_ipts1_num  = MM;

rpts0          = zeros(MM,2);
rpts0_num      = MM;
rpts1          = zeros(MM,2);
rpts1_num      = MM;%变换后的左右边线


rpts0b         = zeros(MM,2);
rpts0b_num     = MM;
rpts1b         = zeros(MM,2);
rpts1b_num     = MM;%变换后左右边线+滤波


rpts0s         = zeros(MM,2);
rpts0s_num     = MM;
rpts1s         = zeros(MM,2);
rpts1s_num     = MM;%变换后左右边线+等距采样

%inv_rpts0s     = zeros(MM,2);
%inv_rpts0s_num = MM;
%inv_rpts1s     = zeros(MM,2);
%inv_rpts1s_num = MM;

rptsc0         = zeros(MM,2);
rptsc0_num     = MM;
rptsc1         = zeros(MM,2);
rptsc1_num     = MM;%左右偏移中线

rpts0a         = zeros(MM,1);
rpts0a_num     = MM;
rpts1a         = zeros(MM,1);
rpts1a_num     = MM;%左/右中线


rpts0an         = zeros(MM,1);
rpts0an_num     = MM;
rpts1an         = zeros(MM,1);
rpts1an_num     = MM;%左右边线局部角度变化率+非极大抑制

Ypt0_found = 0;
Ypt1_found = 0;
Ypt0_rpts0s_id = 0;
Ypt1_rpts1s_id = 0;

Lpt0_found = 0;
=======
img= imread('888.bmp');%如果是bmp结尾的图片文件需要下面rgb2gray这句话。jpg格式就把他注释掉
img= rgb2gray(img);
%img = double(img);  %如果是jpg结尾的图片文件需要double这句话。bmp格式就把他注释掉
[M , N] = size(img);  %图像宽高
MM = 120;%数组长度

% img1 = img;


ipts0          = zeros(MM,2);%原左边线
ipts0_num      = MM;
ipts1          = zeros(MM,2);%原右边线
ipts1_num      = MM;


far_ipts0 = zeros(MM,2);%远左边线
far_ipts0_num = MM;
far_ipts1 = zeros(MM,2);%远右线数组
far_ipts1_num = MM;
% inv_ipts0      = zeros(MM,2);
% inv_ipts0_num  = MM;
% inv_ipts1      = zeros(MM,2);
% inv_ipts1_num  = MM;






rpts0          = zeros(MM,2);%透视后左边线
rpts0_num      = MM;
rpts1          = zeros(MM,2);%透视后右边线
rpts1_num      = MM;

rpts0b         = zeros(MM,2);%透视+三角滤波后的边线
rpts0b_num     = MM;
rpts1b         = zeros(MM,2);
rpts1b_num     = MM;

rpts0s         = zeros(MM,2);%透视+三角滤波+等距采样后的边线
rpts0s_num     = MM;
rpts1s         = zeros(MM,2);
rpts1s_num     = MM;

% inv_rpts0s     = zeros(MM,2);
% inv_rpts0s_num = MM;
% inv_rpts1s     = zeros(MM,2);
% inv_rpts1s_num = MM;

rptsc0         = zeros(MM,2);%左右中线数组
rptsc0_num     = MM;
rptsc1         = zeros(MM,2);
rptsc1_num     = MM;


rpts0a         = zeros(MM,1);%左右边线局部角度变化率
rpts0a_num     = MM;
rpts1a         = zeros(MM,1);
rpts1a_num     = MM;


rpts0an         = zeros(MM,1);%左右边线局部角度变化率+非极大抑制
rpts0an_num     = MM;
rpts1an         = zeros(MM,1);
rpts1an_num     = MM;

Ypt0_found = 0;
Ypt1_found = 0;
Ypt0_rpts0s_id = 0;%y角点
Ypt1_rpts1s_id = 0;

Lpt0_found = 0;%L角点
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
Lpt1_found = 0;
Lpt0_rpts0s_id = 0;
Lpt1_rpts1s_id = 0;

<<<<<<< HEAD
N_Lpt0_found = 0;
=======
N_Lpt0_found = 0;%内L角点
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
N_Lpt1_found = 0;
N_Lpt0_rpts0s_id = 0;
N_Lpt1_rpts1s_id = 0;

<<<<<<< HEAD
is_straight0 = 0;
is_straight1 = 0;

sample_dist = 0.022;
ROAD_WIDTH = 0.38;
kernel = 7;
begin_x = 5;
begin_y = 58;
=======
is_straight0 = 0;%长直道
is_straight1 = 0;

sample_dist = 0.032;
ROAD_WIDTH = 0.40;%赛道宽度
kernel = 5;%三角滤波的卷积核
begin_x = 5;%起始点
begin_y = 98;

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
x1 = N/2 - begin_x;
x2 = N/2 + begin_x;
y1 = begin_y;
y2 = begin_y;
clip_value = 2;
<<<<<<< HEAD
block = 7;
=======
block = 9;
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
thres = 120;

out_put3 = zeros(M,N);
x_data1 = zeros(M,N);
y_data1 = zeros(M,1);

%%
%上交方案运算仿真
output_data = img;
<<<<<<< HEAD
%output_data = Sobel(img,output_data,thres)%边缘提取 得到二值图像(针对灰度图)
%寻找起始点
x0_first = x1;    y0_first = y1;%对应

=======
% output_data = Sobel(img,output_data,thres)%边缘提取 得到二值图像(针对灰度图)
%寻找起始点
x0_first = x1;    y0_first = y1;
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
for x0_first = N/2 - begin_x:-1:2
    if output_data(y0_first,x0_first-1)==0

        break
    end
end
<<<<<<< HEAD


=======
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
x1_first = x2;    y1_first = y2;
for x1_first = N/2 + begin_x:N-1
    if output_data(y1_first,x1_first+1)==0

        break
    end
<<<<<<< HEAD
end%判断左右线起始点


=======
end
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
if output_data(y1,x1) >= 1
    [ipts0,ipts0_num] = Left_Adaptive_Threshold(output_data,x0_first,y0_first,ipts0,ipts0_num);
else
    ipts0_num = 0;
end


if output_data(y1,x1) >= 1
    [ipts1,ipts1_num] = Right_Adaptive_Threshold(output_data,x1_first,y1_first,ipts1,ipts1_num);
else
    ipts1_num = 0;
end
<<<<<<< HEAD
=======

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
%%
% 左侧为x轴负向，右侧为正向建立世界坐标系，在坐标系一个矩形，定点放在整数点位置
%%我这里世界坐标取点（-15,20），（-15，40），（15,20），（15,40）
%%然后用上位机拍照截图，在matlab中读图数据游标标读处矩形在图像中的坐标u，v
%%将这四个一一对应的点带入下方u，v,x,y(注意数据位置的一一对应)

<<<<<<< HEAD
u=[21,63,95,133]%带入拍照的矩形坐标u横向(x)
v=[77,29,29,76]%带入拍照的矩形坐标v纵向（y）
x=[-30,-30,30,30]%带入矩形世界坐标x
y=[30,140,140,30]%带入矩形世界坐标y
=======
u=[31,52,125,143]%带入拍照的矩形坐标u横向(x)
v=[62,39,42,66]%带入拍照的矩形坐标v纵向（y）

%pb 20 30
x=[60,60,100,100]%带入矩形世界坐标x
y=[30,50,50,30]%带入矩形世界坐标y


%u=[23,50,113,131]%带入拍照的矩形坐标u横向(x)
%v=[48,80,80,70]%带入拍照的矩形坐标v纵向（y）
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
a=-u.*x%%这里看不懂  看看我文件带的文本文档  四组点对应4个方程
b=-v.*x
c=-u.*y
d=-v.*y
A=[u(1),v(1),1,0,0,0,a(1),b(1);0,0,0,u(1),v(1),1,c(1),d(1);
  
u(2),v(2),1,0,0,0,a(2),b(2);0,0,0,u(2),v(2),1,c(2),d(2);
  
u(3),v(3),1,0,0,0,a(3),b(3);0,0,0,u(3),v(3),1,c(3),d(3);
  
u(4),v(4),1,0,0,0,a(4),b(4);0,0,0,u(4),v(4),1,c(4),d(4)
 ]
%此处中为B=[x1,y1,x2,y2,x3,y3,x4,y4]
<<<<<<< HEAD
 B=[-20,30,-20,70,20,70,20,30]'  %更改为矩形世界坐标的x，y            
W=A\B%w即为所求逆透视矩阵的参数m1  m2  m3  m4  m5  m6  m7 m8 最后第三行第三列写1
%下面C是我当时算的W的数据写成的矩阵形式C=[m1  m2  m3 ; m4  m5  m6 ; m7 m8 1]
%C即为图像到世界坐标的透视矩阵
C=[W(1) W(2) W(3);W(4) W(5) W(6); W(7) W(8) 1]
=======
 B=[60,30,60,50,100,50,100,30]'  %更改为矩形世界坐标的x，y           
W=A\B%w即为所求逆透视矩阵的参数m1  m2  m3  m4  m5  m6  m7 m8 最后第三行第三列写1
%下面C是我当时算的W的数据写成的矩阵形式C=[m1  m2  m3 ; m4  m5  m6 ; m7 m8 1]
%C即为图像到世界坐标的透视矩阵
% C=[
%   -1.9446   -0.0768  152.9928;
%   -0.0901    0.4470 -190.8998;
%   -0.0030   -0.0846    1.0000;]%test
C=[W(1) W(2) W(3);W(4) W(5) W(6); W(7) W(8) 1]
% C=[-2.2450 -8.5371 252.9928;
%    -0.0901 0.4470 -190.8998;
%    -0.0030 -0.0846 1;]
% C = [2.4483 ]    2.4483
%     9.9016
%  -187.3382
%    -0.0240
%    -0.9154
%   308.7282
%    -0.0062
%     0.1221

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5

D=inv(C)%求透视矩阵   即c取逆  D为世界坐标到图像坐标的变换矩阵


<<<<<<< HEAD
=======

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
%%
%打表变换
%文件写入两个数据，行列要分开写入
fid=fopen('x_data1.txt','w');
fid2=fopen('y_data1.txt','w');
height = M;
width = N;
%M,N为高，宽
for i=1:height
    for j=1:width 
        X = round((j * C(1,1) + i * C(1,2) + C(1,3)) / (j * C(3,1) + i * C(3,2) + C(3,3)));
        Y = round((j * C(2,1) + i * C(2,2) + C(2,3)) / (j * C(3,1) + i * C(3,2) + C(3,3)));        
        fprintf(fid,'%d,',X);  
    end 
    fprintf(fid2,'%d,',Y);  
    fprintf(fid,'%d\n','');
end
%%
<<<<<<< HEAD
%%对边线透视变换
=======
%%对边线逆透视变换
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
for i=1:ipts0_num
        rpts0(i,1) = round((ipts0(i,1) * C(1,1) + ipts0(i,2) * C(1,2) + C(1,3)) / (ipts0(i,1) * C(3,1) + ipts0(i,2) * C(3,2) + C(3,3)));
        rpts0(i,2) = round((ipts0(i,1) * C(2,1) + ipts0(i,2) * C(2,2) + C(2,3)) / (ipts0(i,1) * C(3,1) + ipts0(i,2) * C(3,2) + C(3,3)));        

end
for i=1:ipts1_num

        rpts1(i,1) = round((ipts1(i,1) * C(1,1) + ipts1(i,2) * C(1,2) + C(1,3)) / (ipts1(i,1) * C(3,1) + ipts1(i,2) * C(3,2) + C(3,3)));
        rpts1(i,2) = round((ipts1(i,1) * C(2,1) + ipts1(i,2) * C(2,2) + C(2,3)) / (ipts1(i,1) * C(3,1) + ipts1(i,2) * C(3,2) + C(3,3)));        

end
<<<<<<< HEAD
=======
% for x=1:N
%     for y=1:M
%     x1=round(( img(y,x) * C(1,1) +  img(y,x) * C(1,2) + C(1,3)) / ( img(y,x) * C(3,1) +  img(y,x) * C(3,2) + C(3,3)));
%     y1=round(( img(y,x) * C(2,1) +  img(y,x) * C(2,2) + C(2,3)) / ( img(y,x) * C(3,1) +  img(y,x) * C(3,2) + C(3,3)));
%     img(x,y) =img1(x1,y1)
%     end
% end



>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
%透视变换计算过程，遍历坐标点，矩阵计算，写入新的矩阵
% for i=1:M
%     for j=1:N
%         X = round((j * C(1,1) + i * C(1,2) + C(1,3)) / (j * C(3,1) + i * C(3,2) + C(3,3)));
%         Y = round((j * C(2,1) + i * C(2,2) + C(2,3)) / (j * C(3,1) + i * C(3,2) + C(3,3)));     
%         out_put3(Y,X) = img(i,j);   
%         x_data1(j,i) = X;
%         y_data1(j) = Y;
%     end
%     
% end
%打表变换的思路就是找数组的索引，这个主要是要在单片机上完成的
% for i=1:M
%     for j=1:N 
%         out_put3(y_data1(i),x_data1(i,j)) = img(i,j); 
%     end
% end
%%
%后续计算

<<<<<<< HEAD
[ rpts0b ] = blur_points(rpts0,rpts0_num,rpts0b,kernel);
rpts0b_num = rpts0_num; 
[ rpts1b ] = blur_points(rpts1,rpts1_num,rpts1b,kernel);
rpts1b_num = rpts1_num;

dist = sample_dist*45;
[rpts0s,rpts0s_num] = resample_points(rpts0b,rpts0b_num,rpts0s,rpts0s_num,dist);
[rpts1s,rpts1s_num] = resample_points(rpts1b,rpts1b_num,rpts1s,rpts1s_num,dist);

for i = 1: rpts0s_num
    rpts0s(i,2) = rpts0s(i,2);
    rpts0s(i,1) = rpts0s(i,1);
end
for i = 1: rpts1s_num
    rpts1s(i,2) = rpts1s(i,2);
    rpts1s(i,1) = rpts1s(i,1);
end

[ rpts0a ] = local_angle_points(rpts0s,rpts0s_num,10);%第三个参数要在函数内修改
rpts0a_num = rpts0s_num;
[ rpts1a ] = local_angle_points(rpts1s,rpts1s_num,10);
rpts1a_num = rpts1s_num;

[rpts0an] = nms_angle(rpts0a,rpts0a_num,21);
=======
[ rpts0b ] = blur_points(rpts0,ipts0_num,rpts0b,kernel);%三角滤波
rpts0b_num = ipts0_num; 
[ rpts1b ] = blur_points(rpts1,ipts1_num,rpts1b,kernel);
rpts1b_num = ipts1_num;


dist = sample_dist*45;
[rpts0s,rpts0s_num] = resample_points(rpts0b,rpts0b_num,rpts0s,rpts0s_num,dist);%等距采样
[rpts1s,rpts1s_num] = resample_points(rpts1b,rpts1b_num,rpts1s,rpts1s_num,dist);

%testfor i = 1: rpts0s_num
%    rpts0s(i,2) = rpts0s(i,2);
%    rpts0s(i,1) = rpts0s(i,1);
%end
%for i = 1: rpts1s_num
%    rpts1s(i,2) = rpts1s(i,2);
%    rpts1s(i,1) = rpts1s(i,1);
%end





[ rpts0a ] = local_angle_points(rpts0s,rpts0s_num,10);%第三个参数要在函数内修改
rpts0a_num = rpts0s_num;
[ rpts1a ] = local_angle_points(rpts1s,rpts1s_num,10);%局部角变化率
rpts1a_num = rpts1s_num;

[rpts0an] = nms_angle(rpts0a,rpts0a_num,21);%角变化率非极大值抑制
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
rpts0an_num = rpts0a_num;
[rpts1an] = nms_angle(rpts1a,rpts1a_num,21);
rpts1an_num = rpts1a_num;

<<<<<<< HEAD
pixel_per_meter = 90;
[rptsc0] = track_leftline(rpts0s, rpts0s_num, rptsc0, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc0_num = rpts0s_num;
[rptsc1] = track_rightline(rpts1s, rpts1s_num, rptsc1, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc1_num = rpts1s_num;
=======


pixel_per_meter = 95;%中线偏移量
[rptsc0] = track_leftline(rpts0s, rpts0s_num, rptsc0, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc0_num = rpts0s_num;
[rptsc1] = track_rightline(rpts1s, rpts1s_num, rptsc1, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc1_num = rpts1s_num;%%跟踪左右线
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5

%角点提取&筛选
%     Ypt0_found = 0;
%     Ypt1_found = 0;
%     Lpt0_found = 0;
%     Lpt1_found = 0;
%     N_Lpt1_found = 0;
%     N_Lpt0_found = 0;
% if rpts0s_num > (1.6 / sample_dist)
%     is_straight0 = 1;
% else
%     is_straight0 = 0;
% end
% if rpts1s_num > (1.6 / sample_dist)
%     is_straight1 = 1;
% else
%     is_straight1 = 0;
% end
is_straight0 = rpts0s_num > (1.6 / sample_dist);
is_straight1 = rpts1s_num > (1.6 / sample_dist);
for i = 1 : rpts0s_num
    if(rpts0an(i) == 0)
        continue;
    end
    im1 = clip(i - 10,1,rpts0s_num);
    ip1 = clip(i + 10,1,rpts0s_num);
    conf = abs(rpts0a(i)) - (abs(rpts0a(im1)) + abs(rpts0a(ip1))) / 2;
    if(Ypt0_found == false && 30. / 180. * pi < conf && conf < 50. / 180. * pi && i < 0.8 / sample_dist)
            Ypt0_rpts0s_id = i;
            Ypt0_found = 1;
    end
    if (Lpt0_found == false && 45. / 180. * pi < conf && conf < 140. / 180. * pi && i < 0.8 / sample_dist) 


    Lpt0_rpts0s_id = i;
    Lpt0_found = 1;

        if(rpts0a(Lpt0_rpts0s_id) < 0)              %这一个L点如果是向右拐的，就是内L点
        
            N_Lpt0_rpts0s_id = Lpt0_rpts0s_id;
            N_Lpt0_found = 1;
    
            Lpt0_found = 0;
        end

    end
    if (conf > 5. / 180. * pi && i < 1.4 / sample_dist) 
        is_straight0 = 0;
    end
    if (Ypt0_found == 1 && Lpt0_found == 1 && is_straight0 == 0) 
        break;
    end
end


for i = 1 : rpts1s_num
    if(rpts1an(i) == 0)
        continue;
    end
    im1 = clip(i - 10,1,rpts1s_num);
    ip1 = clip(i + 10,1,rpts1s_num);
    conf = abs(rpts1a(i)) - (abs(rpts1a(im1)) + abs(rpts1a(ip1))) / 2;
    if(Ypt1_found == false && 30. / 180. * pi < conf && conf < 50. / 180. * pi && i < 0.8 / sample_dist)
            Ypt1_rpts0s_id = i;
            Ypt1_found = 1;
    end
    if (Lpt1_found == false && 45. / 180. * pi < conf && conf < 140. / 180. * pi && i < 0.8 / sample_dist) 

    
        Lpt1_rpts1s_id = i;
        Lpt1_found = 1;

        if(rpts1a(Lpt1_rpts1s_id) > 0)              %这一个L点如果是向右拐的，就是内L点
        
            N_Lpt1_rpts1s_id = Lpt1_rpts1s_id;
            N_Lpt1_found = 1;
    
            Lpt1_found = 0;
        end

    end
    if (conf > 5. / 180. * pi && i < 1.4 / sample_dist) 
        is_straight1 = 0;
    end
    if (Ypt1_found == 1 && Lpt1_found == 1 && is_straight1 == 0) 
        break;
    end
end
%%
%远线循迹
LLL = 100;

%补线
left_line          = zeros(LLL,2);
left_num           = LLL;
right_line         = zeros(LLL,2);
right_num           = LLL;

far_ipts0          = zeros(LLL,2);
far_ipts0_num      = LLL;
far_ipts1          = zeros(LLL,2);
far_ipts1_num      = LLL;

far_rpts0          = zeros(LLL,2);
far_rpts0_num      = LLL;
far_rpts1          = zeros(LLL,2);
far_rpts1_num      = LLL;
far_rpts0b         = zeros(LLL,2);
far_rpts0b_num     = LLL;
far_rpts1b         = zeros(LLL,2);
far_rpts1b_num     = LLL;
far_rpts0s         = zeros(LLL,2);
far_rpts0s_num     = LLL;
far_rpts1s         = zeros(LLL,2);
far_rpts1s_num     = LLL;

far_Lpt0_found = 0;
far_Lpt1_found = 0;
far_Lpt0_rpts0s_id = 0;
far_Lpt1_rpts1s_id = 0;

far_N_Lpt0_found = 0;
far_N_Lpt1_found = 0;
far_N_Lpt0_rpts0s_id = 0;
far_N_Lpt1_rpts1s_id = 0;

%右边线远线控制
if Lpt1_found 
<<<<<<< HEAD
        %G_x1,G_y1是反向逆透视的L拐点
    %     G_x1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(1,1) + rpts1s(Lpt1_rpts1s_id,2) * D(1,2) + D(1,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    %     G_y1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(2,1) + rpts1s(Lpt1_rpts1s_id,2) * D(2,2) + D(2,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    
    %     xielv_right = (G_x1 - x1_first) / (G_y1 - y1_first);
=======
    %G_x1,G_y1是反向逆透视的L拐点    
    %G_x1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(1,1) + rpts1s(Lpt1_rpts1s_id,2) * D(1,2) + D(1,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    %G_y1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(2,1) + rpts1s(Lpt1_rpts1s_id,2) * D(2,2) + D(2,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    
    %xielv_right = (G_x1 - x1_first) / (G_y1 - y1_first);
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
    
    
    
    
    
<<<<<<< HEAD
    for i = G_y1:-1:1
        if output_data(i-1,G_x1) == 0
            G_y1 = i;
            %最终得出G_x1,G_y1是远线找点的起始点
            break
        end 
    end
    
    [far_ipts1,g_far_ipts1_num] = Right_Adaptive_Threshold(output_data,G_x1,G_y1,far_ipts1,far_ipts1_num);
=======
    % for i = G_y1:-1:1
    %     if output_data(i-1,G_x1) == 0
    %         G_y1 = i;
    %         %最终得出G_x1,G_y1是远线找点的起始点
    %         break
    %     end 
    % end
    
    % [far_ipts1,g_far_ipts1_num] = Right_Adaptive_Threshold(output_data,G_x1,G_y1,far_ipts1,far_ipts1_num);
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
    
    for i=1:far_ipts1_num
        far_rpts1(i,1) = round((far_ipts1(i,1) * C(1,1) + far_ipts1(i,2) * C(1,2) + C(1,3)) / (far_ipts1(i,1) * C(3,1) + far_ipts1(i,2) * C(3,2) + C(3,3)));
        far_rpts1(i,2) = round((far_ipts1(i,1) * C(2,1) + far_ipts1(i,2) * C(2,2) + C(2,3)) / (far_ipts1(i,1) * C(3,1) + far_ipts1(i,2) * C(3,2) + C(3,3)));        
    end
<<<<<<< HEAD
    
=======

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
    [ far_rpts1b ] = blur_points(far_rpts1,far_rpts1_num,far_rpts1b,kernel);
    far_rpts1b_num = far_rpts1_num;
    
    [far_rpts1s,far_rpts1s_num] = resample_points(far_rpts1b,far_rpts1b_num,far_rpts1s,far_rpts1s_num,dist);
    
    [ far_rpts1a ] = local_angle_points(far_rpts1s,far_rpts1s_num,10);
    far_rpts1a_num = far_rpts1s_num;
    
   
    [far_rpts1an] = nms_angle(far_rpts1a,far_rpts1a_num,21);
    far_rpts1an_num = far_rpts1a_num;
    
    for i = 1 : far_rpts1s_num
        if(far_rpts1an(i) == 0)
            continue;
        end
        im1 = clip(i - 10,1,far_rpts1s_num);
        ip1 = clip(i + 10,1,far_rpts1s_num);
        conf = abs(far_rpts1a(i)) - (abs(far_rpts1a(im1)) + abs(far_rpts1a(ip1))) / 2;
    
        if (far_Lpt1_found == false && 45. / 180. * pi < conf && conf < 140. / 180. * pi) %&& i < 0.8 / sample_dist) 这里注意，远线L角点不受距离限制
            far_Lpt1_rpts1s_id = i;
            far_Lpt1_found = 1;
    
            if(far_rpts1a(far_Lpt1_rpts1s_id) > 0)              %这一个L点如果是向右拐的，就是内L点
            
                far_N_Lpt1_rpts1s_id = far_Lpt1_rpts1s_id;
                far_N_Lpt1_found = 1;
        
                far_Lpt1_found = 0;
            end
    
        end
    end
end

%左边线远线控制
if Lpt0_found == 0 
        %G_x0,G_y0是反向逆透视的L拐点
    %     G_x0 = round((rpts0s(Lpt0_rpts0s_id,1) * D(1,1) + rpts0s(Lpt0_rpts0s_id,2) * D(1,2) + D(1,3)) / (rpts1s(Lpt0_rpts0s_id,1) * D(3,1) + rpts0s(Lpt0_rpts0s_id,2) * D(3,2) + D(3,3)));
    %     G_y0 = round((rpts0s(Lpt0_rpts0s_id,1) * D(2,1) + rpts0s(Lpt0_rpts0s_id,2) * D(2,2) + D(2,3)) / (rpts1s(Lpt0_rpts0s_id,1) * D(3,1) + rpts0s(Lpt0_rpts0s_id,2) * D(3,2) + D(3,3)));
    
    %     xielv_right = (G_x0 - x0_first) / (G_y0 - y0_first);
    
    G_x0 = ipts0(10,1);
    G_y0 = ipts0(10,2);
    
    
    for i = G_y0:-1:1
        if output_data(i-1,G_x0) == 0
            G_y0 = i;
            %最终得出G_x0,G_y0是远线找点的起始点
            break
        end 
    end
    
    [far_ipts0,far_ipts0_num] = Left_Adaptive_Threshold(output_data,G_x0,G_y0,far_ipts0,far_ipts0_num);
    
    for i=1:far_ipts0_num
        far_rpts0(i,1) = round((far_ipts0(i,1) * C(1,1) + far_ipts0(i,2) * C(1,2) + C(1,3)) / (far_ipts0(i,1) * C(3,1) + far_ipts0(i,2) * C(3,2) + C(3,3)));
        far_rpts0(i,2) = round((far_ipts0(i,1) * C(2,1) + far_ipts0(i,2) * C(2,2) + C(2,3)) / (far_ipts0(i,1) * C(3,1) + far_ipts0(i,2) * C(3,2) + C(3,3)));        
    end
    
    [ far_rpts0b ] = blur_points(far_rpts0,far_rpts0_num,far_rpts0b,kernel);
    far_rpts0b_num = far_rpts0_num;
    
    [far_rpts0s,far_rpts0s_num] = resample_points(far_rpts0b,far_rpts0b_num,far_rpts0s,far_rpts0s_num,dist);
    
    [ far_rpts0a ] = local_angle_points(far_rpts0s,far_rpts0s_num,10);
    far_rpts0a_num = far_rpts0s_num;
    
    
    [far_rpts0an] = nms_angle(far_rpts0a,far_rpts0a_num,21);
    far_rpts0an_num = far_rpts0a_num;
    
    for i = 1 : far_rpts0s_num
        if(far_rpts0an(i) == 0)
            continue;
        end
        im1 = clip(i - 10,1,far_rpts0s_num);
        ip1 = clip(i + 10,1,far_rpts0s_num);
        conf = abs(far_rpts0a(i)) - (abs(far_rpts0a(im1)) + abs(far_rpts0a(ip1))) / 2;
   
        if (far_Lpt0_found == false && 45. / 180. * pi < conf && conf < 140. / 180. * pi )%&& i < 0.8 / sample_dist) 这里注意，远线L角点不受距离限制
            far_Lpt0_rpts0s_id = i;
            far_Lpt0_found = 1;
    
            if(far_rpts0a(far_Lpt0_rpts0s_id) < 0)              %这一个L点如果是向右拐的，就是内L点
            
                far_N_Lpt0_rpts0s_id = far_Lpt0_rpts0s_id;
                far_N_Lpt0_found = 1;
        
                far_Lpt0_found = 0;
            end
    
        end
    end
end

N_x1_first = round((x1_first * C(1,1) + x1_first * C(1,2) + C(1,3)) / (x1_first * C(3,1) + x1_first * C(3,2) + C(3,3)));
N_y1_first = round((y1_first * C(2,1) + y1_first * C(2,2) + C(2,3)) / (y1_first * C(3,1) + y1_first * C(3,2) + C(3,3)));    
% [right_line,right_num] = draw_line2(N_x1_first,N_y1_first,far_rpts0s(far_Lpt0_rpts0s_id,1),far_rpts0s(far_Lpt0_rpts0s_id,2),right_line,right_num);


%%
%数据显示

subplot(1,3,1)
hold on
title('原   图')
imshow(uint8(img));
for i = 1:ipts0_num
    plot(ipts0(i,1),ipts0(i,2),'ro'); 
end
for i = 1:ipts1_num
    plot(ipts1(i,1),ipts1(i,2),'bo');
end
%车库远线
% for i = 1:g_far_ipts1_num
%     plot(g_far_ipts1(i,1),g_far_ipts1(i,2),'bo');
% end

<<<<<<< HEAD
=======
% subplot(1,3,3)
% imshow(img1);
% hold on


>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
subplot(1,3,2)
hold on
title('打   表')
% imshow(uint8(out_put3));
for i = 1:rpts0s_num
    plot(rpts0s(i,1),rpts0s(i,2),'ro'); 
end
for i = 1:rpts1s_num
    plot(rpts1s(i,1),rpts1s(i,2),'bo');
end
<<<<<<< HEAD
=======


%subplot(1,3,3)
%hold on
%title('test')
% imshow(uint8(out_put3));
% for i = 1:rpts0s_num
%    plot(rpts0(i,1),rpts0(i,2),'ro'); 
% end
% for i = 1:rpts1s_num
%    plot(rpts1(i,1),rpts1(i,2),'bo');
% end

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
% %远线（逆透视）
% for i = 1:far_rpts0s_num
%     plot(far_rpts0s(i,1),far_rpts0s(i,2),'bo');
% end
<<<<<<< HEAD
%for i = 1:right_num
 %   plot(right_line(i,1),right_line(i,2),'bo');
%end
for i = 1:rptsc0_num
    plot(rptsc0(i,1),rptsc0(i,2),'ro'); 
end
%for i = 1:rptsc1_num
%    plot(rptsc1(i,1),rptsc1(i,2),'bo');
%end
=======


for i = 1:right_num
    plot(right_line(i,1),right_line(i,2),'bo');
end
for i = 1:rptsc0_num
    plot(rptsc0(i,1),rptsc0(i,2),'ro'); 
end
for i = 1:rptsc1_num
    plot(rptsc1(i,1),rptsc1(i,2),'bo');
end
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5


