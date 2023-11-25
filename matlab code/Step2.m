
%% 镜头失真校正--逆变换   2019.12.6   
clear,clc
img_origin1 = imread('fir.BMP'); %读取图像文件
%img_origin2 = imread('D:\rt1061\图像透视处理\4.BMP'); %读取图像文件
%img_origin2 = rgb2gray(img_origin2); %将图像转换为灰度图像二维矩阵
img_origin = rgb2gray(img_origin1); %将图像转换为灰度图像二维矩阵
img_size = size( img_origin1 ); % 返回矩阵的行数或列数 其中r=size(A,1)该语句返回矩阵A的行数row为垂直方向

m1= 5.2245; % -0.4266   逆透视矩阵参数  根据现实世界和图像坐标对应得到 测四组数据，世界坐标最好是矩形

m2=-0.0000;%0.0245   将step1中的所求的W带入即C矩阵

m3=-211.5918;%36.3723

m4= -0.0000;%0.0129

m5= -2.8469;%0.1698

 m6= 257.1122;%-52.4729

m7=0.0000;%-0.0034

m8=  0.1020;% 0.9007


u=80; % 水平方向column124   算出透视后图像大小
v=10; % 行row为垂直方向43   抛弃图像最上端13行不要
x=(m1*u+m2*v+m3)/(m7*u+m8*v+1);% 列水平方向
y=(m4*u+m5*v+m6)/(m7*u+m8*v+1); % 行垂直方向
x=uint16(x);
y=uint16(y); %本段为生成新矩阵容纳校正后的图像
img_undist = zeros(y ,x ); % zeros(m,n)产生m×n的全0矩阵 所有元素初始化为0
new_size = size(img_undist); % 返回m×n全0矩阵的行数或列数
img_undist = uint8( img_undist );
img_undist=im2bw(img_undist);   % 二值化图像
%~~~~~~~~~~~~~~~~%
%  t=new_size(1);     %此处逆透视  无差值处理
% for i =(img_size(1)):-1: 10 % 行垂直方向
% 
% for j = 1:(img_size(2)) % 水平方向j
% 
% u=j; % 水平方向column
% 
% v=i; % 垂直方向
% 
% x=(m1*u+m2*v+m3)/(m7*u+m8*v+1); % 水平方向column
% 
%  y=(m4*u+m5*v+m6)/(m7*u+m8*v+1);% 行row为垂直方向
% 
% x=int16(x)+new_size(2)/2; %物理坐标用负,现修正显示
% 
%  y=int16(y);
% 
%   if(t-y>0&& x>0 )
% 
%   img_undist(t-y,x)= img_origin(i,j); %对应坐标
% % img_undist(i,x)= img_origin(i,j); %对应坐标
% 
%   end
% 
% end
% 
% end

% img_undist=flipud(img_undist); %矩阵  以水平轴翻转
%~~~~~~~~~~~~%

% C=[0.1687 0.1173  1.4787;%图像到世界的转化矩阵
%          0.0022 -0.0026 3.0900;
%          0.0000 0.0013  0.0156;];        
% D=[6.0861   -0.2428 -530.5047; %世界到图像的转化矩阵
%     -0.1742   -3.6928  750.3649;
%     -0.0044    0.3207    1.0000];
 img_origin=im2bw(img_origin);   % 二值化图像
H1= 0.1914  ;H2=0.1421;H3= 3.9646;%世界坐标到图像坐标的转化矩阵参数 即step1的D矩阵
H4= -0.0000;H5=-0.0344;H6=8.8407;
H7=0.0000;H8=0.0035;H9=0.0979;
%插值处理 
for i = 1:new_size(1)                      
    for j = 1:new_size(2)
        s=H7*(j-new_size(2)/2)+H8 *i+H9; %从变换图像中反向寻找原图像的点，以免出现空洞，和旋转放大原理一样
        s=1/s;   %这里看不懂看所带的文档  
        X=(H1*(j-new_size(2)/2)*s+H2*i*s+H3*s);
        Y=(H4*(j-new_size(2)/2)*s+H5*i*s+H6*s);
        t=new_size(1)-i;% 倒转图像   137为矫正后图像纵向宽度最大
         if   Y>=0.5 && X>=0.5 && Y<=60 && X<=80  %防止越界
                 if(t>0)
                     
                  img_undist(t,j)=img_origin(round(Y),round(X));     %最邻近插值,也可以用双线性或双立方插值
                end 
         else                  
                  img_undist(t,j)=255;          
        end
    end
end  
subplot(2,2,1);imshow(img_origin);title('原图，120*188');
imwrite(img_undist,'result.bmp'); 
 %img_origin=im2bw(img_origin);   % 二值化图像
  %img_undist=im2bw(img_undist);   % 二值化图像
  
  %img_origin2 = edge(img_origin2,'sobel',0.025);
  
subplot(2,2,2);imshow(img_undist);title('校正图');	

subplot(2,2,3);imshow(img_origin);title('校正图');
