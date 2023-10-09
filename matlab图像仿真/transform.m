clear all;
close all;
clc;
%%
%数据预处理 （18届摄像头组 黄庆成修改）此脚本应传入拍摄的灰度图像
%要将你的摄像头拍的图片放到同文件夹下
img= imread('1.5.jpg');%如果是bmp结尾的图片文件需要下面rgb2gray这句话。jpg格式就把他注释掉
%img= rgb2gray(img);
img = double(img);  %如果是jpg结尾的图片文件需要double这句话。bmp格式就把他注释掉
[M , N] = size(img);  %图像宽高
MM = M+40;
data = zeros(100,20);

touch_boundary0 = 0;
touch_boundary1 = 0;

ipts0          = zeros(MM,2);
ipts0_num      = MM;
ipts1          = zeros(MM,2);
ipts1_num      = MM;
inv_ipts0      = zeros(MM,2);
inv_ipts0_num  = MM;
inv_ipts1      = zeros(MM,2);
inv_ipts1_num  = MM;


rpts0          = zeros(MM,2);
rpts0_num      = MM;
rpts1          = zeros(MM,2);
rpts1_num      = MM;
rpts0b         = zeros(MM,2);
rpts0b_num     = MM;
rpts1b         = zeros(MM,2);
rpts1b_num     = MM;
rpts0s         = zeros(MM,2);
rpts0s_num     = MM;
rpts1s         = zeros(MM,2);
rpts1s_num     = MM;
inv_rpts0s     = zeros(MM,2);
inv_rpts0s_num = MM;
inv_rpts1s     = zeros(MM,2);
inv_rpts1s_num = MM;

rptsc0         = zeros(MM,2);
rptsc0_num     = MM;
rptsc1         = zeros(MM,2);
rptsc1_num     = MM;

rpts0a         = zeros(MM,1);
rpts0a_num     = MM;
rpts1a         = zeros(MM,1);
rpts1a_num     = MM;
rpts0an         = zeros(MM,1);
rpts0an_num     = MM;
rpts1an         = zeros(MM,1);
rpts1an_num     = MM;

Ypt0_found = 0;
Ypt1_found = 0;
Ypt0_rpts0s_id = 0;
Ypt1_rpts1s_id = 0;

Lpt0_found = 0;
Lpt1_found = 0;
Lpt0_rpts0s_id = 0;
Lpt1_rpts1s_id = 0;

N_Lpt0_found = 0;
N_Lpt1_found = 0;
N_Lpt0_rpts0s_id = 0;
N_Lpt1_rpts1s_id = 0;

is_straight0 = 0;
is_straight1 = 0;

sample_dist = 0.02;
ROAD_WIDTH = 0.45;
kernel = 7;
begin_x = 5;
begin_y = 58;
x1 = N/2 - begin_x;
x2 = N/2 + begin_x;
y1 = begin_y;
y2 = begin_y;
clip_value = 2;
block = 7;
thres = 120;

output_data = zeros([M,N]);
output_data1 = zeros([M,N]);

%可以指定透视变换后图像out_put尺寸大小，两个变量是height，width大小确定后就不用再修改了，基本上和原图尺寸差不多大小就行
%其次原图像我需要指定四个点，这四个点可以是四个顶点，比如我选取的是（1，1），（1，img_width），（img_hegith，1），（img_hegith，img_width）这四个点
%刚刚四个点在out_put图像中的坐标是（1，1），（1，width），（height，N/2-18），（height，N/2+18）
%其中N/2-18，N/2+18是img图像中的原左下右下顶点横坐标经过矩阵运算得到的新的坐标，这是新图像的最下行有效值，18是偏移量，这个数值需要手动调整
height = 200;
width = 400;%变换后整幅图像宽度
location_L = width/2-21;%变换后左下顶点
location_R = width/2+21;%换变后右下顶点
img_height = M;
img_width = N;

x=[1, 1,img_height,img_height];  % 依次为A、B、D、C在摄像头获取的照片中的的纵坐标
y=[1, img_width,1,img_width]; % 依次为A、B、D、C在摄像头获取的照片中的的横坐标
X=[1,1,height,height];  % 依次为A、B、D、C在逆透视变换后的照片中的的纵坐标
Y=[1,width,location_L,location_R]; % 依次为A、B、D、C在逆透视变换后的照片中的的纵坐标
%x代表height，y代表width
% x0_L = 23;
% x0_R = 23;
% x1_L = 59;
% x1_R = 59;
% y0_L = 30;
% y0_R = 50;
% y1_L = 9;
% y1_R = 71;
% x=[x0_L, x0_R,x1_R,x1_L];  % 依次为A、B、D、C在摄像头获取的照片中的的纵坐标
% y=[y0_L, y0_R,y1_R,y1_L]; % 依次为A、B、D、C在摄像头获取的照片中的的横坐标
out_put = zeros(height,width);
out_put1 = zeros(height,width);
out_put2 = zeros(height,width);
out_put3 = zeros(height,width);
[rot,inv_rot]=rot_cal(x,y,X,Y);

x_data = zeros(M,N);%数组打表
y_data = zeros(M,1);
%%
%上交方案运算仿真
output_data = img;
%output_data = Sobel(img,output_data,thres)%边缘提取 得到二值图像(针对灰度图)
%寻找起始点
for x1 = N/2 - begin_x:-1:2
    if output_data(y1,x1-1)==0
        break
    end
end
for x2 = N/2 + begin_x:N-1
    if output_data(y2,x2+1)==0
        break
    end
end
if output_data(y1,x1) >= 1
    [ipts0,ipts0_num] = Left_Adaptive_Threshold(output_data,x1,y1,ipts0,ipts0_num);
else
    ipts0_num = 0;
end
if output_data(y1,x1) >= 1
    [ipts1,ipts1_num] = Right_Adaptive_Threshold(output_data,x2,y2,ipts1,ipts1_num);
else
    ipts1_num = 0;
end
%%
%透视变换计算过程，遍历坐标点，矩阵计算，写入新的矩阵
for i=1:N
    for j=1:M
        X = round((j * rot(1,1) + i * rot(1,2) + rot(1,3)) / (j * rot(3,1) + i * rot(3,2) + rot(3,3)));
        Y = round((j * rot(2,1) + i * rot(2,2) + rot(2,3)) / (j * rot(3,1) + i * rot(3,2) + rot(3,3)));     
        out_put(X,Y) = output_data(j,i);   
        x_data(j,i) = Y;
        y_data(j) = X;
    end
    
end

%裁剪透视后图像

% for i = height - 80:height %高
%     for j = width/2 - 40:width/2 + 40 %宽
%         out_put1(i,j) = out_put(i,j);
%     end
% end



%%
%文件写入两个数据，行列要分开写入
fid=fopen('x_data.txt','w');
fid2=fopen('y_data.txt','w');
for i=1:N
    for j=1:M 
        X = round((j * rot(1,1) + i * rot(1,2) + rot(1,3)) / (j * rot(3,1) + i * rot(3,2) + rot(3,3)));
        Y = round((j * rot(2,1) + i * rot(2,2) + rot(2,3)) / (j * rot(3,1) + i * rot(3,2) + rot(3,3)));        
        fprintf(fid,'%d,',Y);  
    end 
    fprintf(fid2,'%d,',X);  
    fprintf(fid,'%d\n','');
end



%%
%打表变换的思路就是找数组的索引，这个主要是要在单片机上完成的
for i=1:N
    for j=1:M
        if y_data(j)>height-80&&y_data(j)<height&&x_data(j,i)>width/2-40&&x_data(j,i)<width/2+40
            out_put3(y_data(j),x_data(j,i)) = output_data(j,i); 
        end
    end
end


%%
%线性插值把没有映射到的点补上
for i=1:height
    for j=1:width
        X = round((i * inv_rot(1,1) + j * inv_rot(1,2) + inv_rot(1,3)) / (i * inv_rot(3,1) + j * inv_rot(3,2) + inv_rot(3,3)));
        Y = round((i * inv_rot(2,1) + j * inv_rot(2,2) + inv_rot(2,3)) / (i * inv_rot(3,1) + j * inv_rot(3,2) + inv_rot(3,3)));  
        if(1<=X&&X<=M&&1<=Y&&Y<=N)
            out_put2(i,j)=output_data(X,Y);      
        end
    end
end
%%
%修改部分
rpts0_num = ipts0_num;
for i = 1: ipts0_num
    rpts0(i,2) = round((rot(1,1)*ipts0(i,2) + rot(1,2)*ipts0(i,1) + rot(1,3))/(rot(3,1)*ipts0(i,2)+rot(3,2)*ipts0(i,1)+rot(3,3)));
    rpts0(i,1) = round((rot(2,1)*ipts0(i,2) + rot(2,2)*ipts0(i,1) + rot(2,3))/(rot(3,1)*ipts0(i,2)+rot(3,2)*ipts0(i,1)+rot(3,3)));
end

rpts1_num = ipts1_num;
for i = 1: ipts1_num
    rpts1(i,2) = round((rot(1,1)*ipts1(i,2) + rot(1,2)*ipts1(i,1) + rot(1,3))/(rot(3,1)*ipts1(i,2)+rot(3,2)*ipts1(i,1)+rot(3,3)));
    rpts1(i,1) = round((rot(2,1)*ipts1(i,2) + rot(2,2)*ipts1(i,1) + rot(2,3))/(rot(3,1)*ipts1(i,2)+rot(3,2)*ipts1(i,1)+rot(3,3)));
end

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
rpts0an_num = rpts0a_num;
[rpts1an] = nms_angle(rpts1a,rpts1a_num,21);
rpts1an_num = rpts1a_num;

pixel_per_meter = 45;
[rptsc0] = track_leftline(rpts0s, rpts0s_num, rptsc0, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc0_num = rpts0s_num;
[rptsc1] = track_rightline(rpts1s, rpts1s_num, rptsc1, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc1_num = rpts1s_num;

%角点提取&筛选
is_straight0 = rpts0s_num > (1.5 / sample_dist);
is_straight1 = rpts1s_num > (1.5 / sample_dist);
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

        if(rpts1a(Lpt1_rpts1s_id) < 0)              %这一个L点如果是向右拐的，就是内L点
        
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
%数据显示



subplot(1,3,1)
hold on
title('原   图')
imshow(uint8(img));
% for i = 1:ipts0_num
%     plot(ipts0(i,1),ipts0(i,2),'ro'); 
% end
% for i = 1:rpts1_num
%     plot(ipts1(i,1),ipts1(i,2),'bo');
% end

subplot(1,3,2)
hold on
title('打   表')
imshow(uint8(out_put));
for i = 1:rpts0_num
    plot(rpts0s(i,1),rpts0s(i,2),'ro'); 
end
for i = 1:rpts1_num
    plot(rpts1s(i,1),rpts1s(i,2),'bo');
end


subplot(1,3,3)
hold on
title('插   值')
imshow(uint8(out_put2));
for i = 1:rpts0_num
    plot(rpts0s(i,1),rpts0s(i,2),'ro'); 
end
for i = 1:rpts1_num
    plot(rpts1s(i,1),rpts1s(i,2),'bo');
end


rot=roundn(rot,-4);%四舍五入
inv_rot=roundn(inv_rot,-4);%四舍五入
fclose(fid);
fclose(fid2);
