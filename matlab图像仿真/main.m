%%Author��18������ͷ�� HQC
%%������������ϵ  С������ͷλ����������ԭ���Ϸ��������ýǶȣ���ͷ�����Ϸ���
%%С����ͷ����Ϊy���ᣬС������
clear all;
close all;
clc;
%%
<<<<<<< HEAD
img= imread('changzhidao.jpg');%�����bmp��β��ͼƬ�ļ���Ҫ����rgb2gray��仰��jpg��ʽ�Ͱ���ע�͵�
%img= rgb2gray(img);
img = double(img);  %�����jpg��β��ͼƬ�ļ���Ҫdouble��仰��bmp��ʽ�Ͱ���ע�͵�
[M , N] = size(img);  %ͼ����
MM = M+60;

ipts0          = zeros(MM,2);
ipts0_num      = MM;
ipts1          = zeros(MM,2);
ipts1_num      = MM;%��ű��ߵ�����
%inv_ipts0      = zeros(MM,2);
%inv_ipts0_num  = MM;
%inv_ipts1      = zeros(MM,2);
%inv_ipts1_num  = MM;

rpts0          = zeros(MM,2);
rpts0_num      = MM;
rpts1          = zeros(MM,2);
rpts1_num      = MM;%�任������ұ���


rpts0b         = zeros(MM,2);
rpts0b_num     = MM;
rpts1b         = zeros(MM,2);
rpts1b_num     = MM;%�任�����ұ���+�˲�


rpts0s         = zeros(MM,2);
rpts0s_num     = MM;
rpts1s         = zeros(MM,2);
rpts1s_num     = MM;%�任�����ұ���+�Ⱦ����

%inv_rpts0s     = zeros(MM,2);
%inv_rpts0s_num = MM;
%inv_rpts1s     = zeros(MM,2);
%inv_rpts1s_num = MM;

rptsc0         = zeros(MM,2);
rptsc0_num     = MM;
rptsc1         = zeros(MM,2);
rptsc1_num     = MM;%����ƫ������

rpts0a         = zeros(MM,1);
rpts0a_num     = MM;
rpts1a         = zeros(MM,1);
rpts1a_num     = MM;%��/������


rpts0an         = zeros(MM,1);
rpts0an_num     = MM;
rpts1an         = zeros(MM,1);
rpts1an_num     = MM;%���ұ��߾ֲ��Ƕȱ仯��+�Ǽ�������

Ypt0_found = 0;
Ypt1_found = 0;
Ypt0_rpts0s_id = 0;
Ypt1_rpts1s_id = 0;

Lpt0_found = 0;
=======
img= imread('888.bmp');%�����bmp��β��ͼƬ�ļ���Ҫ����rgb2gray��仰��jpg��ʽ�Ͱ���ע�͵�
img= rgb2gray(img);
%img = double(img);  %�����jpg��β��ͼƬ�ļ���Ҫdouble��仰��bmp��ʽ�Ͱ���ע�͵�
[M , N] = size(img);  %ͼ����
MM = 120;%���鳤��

% img1 = img;


ipts0          = zeros(MM,2);%ԭ�����
ipts0_num      = MM;
ipts1          = zeros(MM,2);%ԭ�ұ���
ipts1_num      = MM;


far_ipts0 = zeros(MM,2);%Զ�����
far_ipts0_num = MM;
far_ipts1 = zeros(MM,2);%Զ��������
far_ipts1_num = MM;
% inv_ipts0      = zeros(MM,2);
% inv_ipts0_num  = MM;
% inv_ipts1      = zeros(MM,2);
% inv_ipts1_num  = MM;






rpts0          = zeros(MM,2);%͸�Ӻ������
rpts0_num      = MM;
rpts1          = zeros(MM,2);%͸�Ӻ��ұ���
rpts1_num      = MM;

rpts0b         = zeros(MM,2);%͸��+�����˲���ı���
rpts0b_num     = MM;
rpts1b         = zeros(MM,2);
rpts1b_num     = MM;

rpts0s         = zeros(MM,2);%͸��+�����˲�+�Ⱦ������ı���
rpts0s_num     = MM;
rpts1s         = zeros(MM,2);
rpts1s_num     = MM;

% inv_rpts0s     = zeros(MM,2);
% inv_rpts0s_num = MM;
% inv_rpts1s     = zeros(MM,2);
% inv_rpts1s_num = MM;

rptsc0         = zeros(MM,2);%������������
rptsc0_num     = MM;
rptsc1         = zeros(MM,2);
rptsc1_num     = MM;


rpts0a         = zeros(MM,1);%���ұ��߾ֲ��Ƕȱ仯��
rpts0a_num     = MM;
rpts1a         = zeros(MM,1);
rpts1a_num     = MM;


rpts0an         = zeros(MM,1);%���ұ��߾ֲ��Ƕȱ仯��+�Ǽ�������
rpts0an_num     = MM;
rpts1an         = zeros(MM,1);
rpts1an_num     = MM;

Ypt0_found = 0;
Ypt1_found = 0;
Ypt0_rpts0s_id = 0;%y�ǵ�
Ypt1_rpts1s_id = 0;

Lpt0_found = 0;%L�ǵ�
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
Lpt1_found = 0;
Lpt0_rpts0s_id = 0;
Lpt1_rpts1s_id = 0;

<<<<<<< HEAD
N_Lpt0_found = 0;
=======
N_Lpt0_found = 0;%��L�ǵ�
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
is_straight0 = 0;%��ֱ��
is_straight1 = 0;

sample_dist = 0.032;
ROAD_WIDTH = 0.40;%�������
kernel = 5;%�����˲��ľ����
begin_x = 5;%��ʼ��
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
%�Ͻ������������
output_data = img;
<<<<<<< HEAD
%output_data = Sobel(img,output_data,thres)%��Ե��ȡ �õ���ֵͼ��(��ԻҶ�ͼ)
%Ѱ����ʼ��
x0_first = x1;    y0_first = y1;%��Ӧ

=======
% output_data = Sobel(img,output_data,thres)%��Ե��ȡ �õ���ֵͼ��(��ԻҶ�ͼ)
%Ѱ����ʼ��
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
end%�ж���������ʼ��


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
% ���Ϊx�Ḻ���Ҳ�Ϊ��������������ϵ��������ϵһ�����Σ��������������λ��
%%��������������ȡ�㣨-15,20������-15��40������15,20������15,40��
%%Ȼ������λ�����ս�ͼ����matlab�ж�ͼ�����α�����������ͼ���е�����u��v
%%�����ĸ�һһ��Ӧ�ĵ�����·�u��v,x,y(ע������λ�õ�һһ��Ӧ)

<<<<<<< HEAD
u=[21,63,95,133]%�������յľ�������u����(x)
v=[77,29,29,76]%�������յľ�������v����y��
x=[-30,-30,30,30]%���������������x
y=[30,140,140,30]%���������������y
=======
u=[31,52,125,143]%�������յľ�������u����(x)
v=[62,39,42,66]%�������յľ�������v����y��

%pb 20 30
x=[60,60,100,100]%���������������x
y=[30,50,50,30]%���������������y


%u=[23,50,113,131]%�������յľ�������u����(x)
%v=[48,80,80,70]%�������յľ�������v����y��
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
a=-u.*x%%���￴����  �������ļ������ı��ĵ�  ������Ӧ4������
b=-v.*x
c=-u.*y
d=-v.*y
A=[u(1),v(1),1,0,0,0,a(1),b(1);0,0,0,u(1),v(1),1,c(1),d(1);
  
u(2),v(2),1,0,0,0,a(2),b(2);0,0,0,u(2),v(2),1,c(2),d(2);
  
u(3),v(3),1,0,0,0,a(3),b(3);0,0,0,u(3),v(3),1,c(3),d(3);
  
u(4),v(4),1,0,0,0,a(4),b(4);0,0,0,u(4),v(4),1,c(4),d(4)
 ]
%�˴���ΪB=[x1,y1,x2,y2,x3,y3,x4,y4]
<<<<<<< HEAD
 B=[-20,30,-20,70,20,70,20,30]'  %����Ϊ�������������x��y            
W=A\B%w��Ϊ������͸�Ӿ���Ĳ���m1  m2  m3  m4  m5  m6  m7 m8 �������е�����д1
%����C���ҵ�ʱ���W������д�ɵľ�����ʽC=[m1  m2  m3 ; m4  m5  m6 ; m7 m8 1]
%C��Ϊͼ�����������͸�Ӿ���
C=[W(1) W(2) W(3);W(4) W(5) W(6); W(7) W(8) 1]
=======
 B=[60,30,60,50,100,50,100,30]'  %����Ϊ�������������x��y           
W=A\B%w��Ϊ������͸�Ӿ���Ĳ���m1  m2  m3  m4  m5  m6  m7 m8 �������е�����д1
%����C���ҵ�ʱ���W������д�ɵľ�����ʽC=[m1  m2  m3 ; m4  m5  m6 ; m7 m8 1]
%C��Ϊͼ�����������͸�Ӿ���
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

D=inv(C)%��͸�Ӿ���   ��cȡ��  DΪ�������굽ͼ������ı任����


<<<<<<< HEAD
=======

>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
%%
%���任
%�ļ�д���������ݣ�����Ҫ�ֿ�д��
fid=fopen('x_data1.txt','w');
fid2=fopen('y_data1.txt','w');
height = M;
width = N;
%M,NΪ�ߣ���
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
%%�Ա���͸�ӱ任
=======
%%�Ա�����͸�ӱ任
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
%͸�ӱ任������̣���������㣬������㣬д���µľ���
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
%���任��˼·����������������������Ҫ��Ҫ�ڵ�Ƭ������ɵ�
% for i=1:M
%     for j=1:N 
%         out_put3(y_data1(i),x_data1(i,j)) = img(i,j); 
%     end
% end
%%
%��������

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

[ rpts0a ] = local_angle_points(rpts0s,rpts0s_num,10);%����������Ҫ�ں������޸�
rpts0a_num = rpts0s_num;
[ rpts1a ] = local_angle_points(rpts1s,rpts1s_num,10);
rpts1a_num = rpts1s_num;

[rpts0an] = nms_angle(rpts0a,rpts0a_num,21);
=======
[ rpts0b ] = blur_points(rpts0,ipts0_num,rpts0b,kernel);%�����˲�
rpts0b_num = ipts0_num; 
[ rpts1b ] = blur_points(rpts1,ipts1_num,rpts1b,kernel);
rpts1b_num = ipts1_num;


dist = sample_dist*45;
[rpts0s,rpts0s_num] = resample_points(rpts0b,rpts0b_num,rpts0s,rpts0s_num,dist);%�Ⱦ����
[rpts1s,rpts1s_num] = resample_points(rpts1b,rpts1b_num,rpts1s,rpts1s_num,dist);

%testfor i = 1: rpts0s_num
%    rpts0s(i,2) = rpts0s(i,2);
%    rpts0s(i,1) = rpts0s(i,1);
%end
%for i = 1: rpts1s_num
%    rpts1s(i,2) = rpts1s(i,2);
%    rpts1s(i,1) = rpts1s(i,1);
%end





[ rpts0a ] = local_angle_points(rpts0s,rpts0s_num,10);%����������Ҫ�ں������޸�
rpts0a_num = rpts0s_num;
[ rpts1a ] = local_angle_points(rpts1s,rpts1s_num,10);%�ֲ��Ǳ仯��
rpts1a_num = rpts1s_num;

[rpts0an] = nms_angle(rpts0a,rpts0a_num,21);%�Ǳ仯�ʷǼ���ֵ����
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


pixel_per_meter = 95;%����ƫ����
[rptsc0] = track_leftline(rpts0s, rpts0s_num, rptsc0, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc0_num = rpts0s_num;
[rptsc1] = track_rightline(rpts1s, rpts1s_num, rptsc1, 10.0, pixel_per_meter * ROAD_WIDTH / 2);
rptsc1_num = rpts1s_num;%%����������
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5

%�ǵ���ȡ&ɸѡ
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

        if(rpts0a(Lpt0_rpts0s_id) < 0)              %��һ��L����������ҹյģ�������L��
        
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

        if(rpts1a(Lpt1_rpts1s_id) > 0)              %��һ��L����������ҹյģ�������L��
        
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
%Զ��ѭ��
LLL = 100;

%����
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

%�ұ���Զ�߿���
if Lpt1_found 
<<<<<<< HEAD
        %G_x1,G_y1�Ƿ�����͸�ӵ�L�յ�
    %     G_x1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(1,1) + rpts1s(Lpt1_rpts1s_id,2) * D(1,2) + D(1,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    %     G_y1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(2,1) + rpts1s(Lpt1_rpts1s_id,2) * D(2,2) + D(2,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    
    %     xielv_right = (G_x1 - x1_first) / (G_y1 - y1_first);
=======
    %G_x1,G_y1�Ƿ�����͸�ӵ�L�յ�    
    %G_x1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(1,1) + rpts1s(Lpt1_rpts1s_id,2) * D(1,2) + D(1,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    %G_y1 = round((rpts1s(Lpt1_rpts1s_id,1) * D(2,1) + rpts1s(Lpt1_rpts1s_id,2) * D(2,2) + D(2,3)) / (rpts1s(Lpt1_rpts1s_id,1) * D(3,1) + rpts1s(Lpt1_rpts1s_id,2) * D(3,2) + D(3,3)));
    
    %xielv_right = (G_x1 - x1_first) / (G_y1 - y1_first);
>>>>>>> 02e42dc0749423afe2fd0acff66ab4075d3fb4c5
    
    
    
    
    
<<<<<<< HEAD
    for i = G_y1:-1:1
        if output_data(i-1,G_x1) == 0
            G_y1 = i;
            %���յó�G_x1,G_y1��Զ���ҵ����ʼ��
            break
        end 
    end
    
    [far_ipts1,g_far_ipts1_num] = Right_Adaptive_Threshold(output_data,G_x1,G_y1,far_ipts1,far_ipts1_num);
=======
    % for i = G_y1:-1:1
    %     if output_data(i-1,G_x1) == 0
    %         G_y1 = i;
    %         %���յó�G_x1,G_y1��Զ���ҵ����ʼ��
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
    
        if (far_Lpt1_found == false && 45. / 180. * pi < conf && conf < 140. / 180. * pi) %&& i < 0.8 / sample_dist) ����ע�⣬Զ��L�ǵ㲻�ܾ�������
            far_Lpt1_rpts1s_id = i;
            far_Lpt1_found = 1;
    
            if(far_rpts1a(far_Lpt1_rpts1s_id) > 0)              %��һ��L����������ҹյģ�������L��
            
                far_N_Lpt1_rpts1s_id = far_Lpt1_rpts1s_id;
                far_N_Lpt1_found = 1;
        
                far_Lpt1_found = 0;
            end
    
        end
    end
end

%�����Զ�߿���
if Lpt0_found == 0 
        %G_x0,G_y0�Ƿ�����͸�ӵ�L�յ�
    %     G_x0 = round((rpts0s(Lpt0_rpts0s_id,1) * D(1,1) + rpts0s(Lpt0_rpts0s_id,2) * D(1,2) + D(1,3)) / (rpts1s(Lpt0_rpts0s_id,1) * D(3,1) + rpts0s(Lpt0_rpts0s_id,2) * D(3,2) + D(3,3)));
    %     G_y0 = round((rpts0s(Lpt0_rpts0s_id,1) * D(2,1) + rpts0s(Lpt0_rpts0s_id,2) * D(2,2) + D(2,3)) / (rpts1s(Lpt0_rpts0s_id,1) * D(3,1) + rpts0s(Lpt0_rpts0s_id,2) * D(3,2) + D(3,3)));
    
    %     xielv_right = (G_x0 - x0_first) / (G_y0 - y0_first);
    
    G_x0 = ipts0(10,1);
    G_y0 = ipts0(10,2);
    
    
    for i = G_y0:-1:1
        if output_data(i-1,G_x0) == 0
            G_y0 = i;
            %���յó�G_x0,G_y0��Զ���ҵ����ʼ��
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
   
        if (far_Lpt0_found == false && 45. / 180. * pi < conf && conf < 140. / 180. * pi )%&& i < 0.8 / sample_dist) ����ע�⣬Զ��L�ǵ㲻�ܾ�������
            far_Lpt0_rpts0s_id = i;
            far_Lpt0_found = 1;
    
            if(far_rpts0a(far_Lpt0_rpts0s_id) < 0)              %��һ��L����������ҹյģ�������L��
            
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
%������ʾ

subplot(1,3,1)
hold on
title('ԭ   ͼ')
imshow(uint8(img));
for i = 1:ipts0_num
    plot(ipts0(i,1),ipts0(i,2),'ro'); 
end
for i = 1:ipts1_num
    plot(ipts1(i,1),ipts1(i,2),'bo');
end
%����Զ��
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
title('��   ��')
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
% %Զ�ߣ���͸�ӣ�
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


