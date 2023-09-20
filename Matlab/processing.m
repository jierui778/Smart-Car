%灰度转换、噪声、锐化、直方图均衡化处理、边缘检测
clear;
clc;
%读取图像
[filename,filepath]=uigetfile({'*.*','*.jpg'},'打开文件');%gui中打开文件
filep=strcat(filepath,filename);
I=imread(filep);
figure
subplot(2,2,1); 
imshow(I)
title('原图');

%转换灰度图
try
    I=rgb2gray(I);    %如果是RGB图像，转成灰度图
end
subplot(2,2,2); 
imshow(I);
title('灰度图像');

%添加噪声
I=imnoise(I,'salt & pepper',0.02); % 叠加密度为0.02的椒盐噪声
%是一种随机出现的白点或者黑点，可能是亮的区域有黑色像素或是在暗的区域有白色像素（或是两者皆有）
subplot(2,2,3);
imshow(I);
title('添加噪声后图像');

%图像去噪
I=medfilt2(I,[2 4]);   %采用二维中值滤波函数medfilt2对受椒盐噪声干扰的图像滤波
subplot(2,2,4); 
imshow(I);
title('去噪后的图像');

%图像锐化
[M,N]=size(I);   % 计算图像的尺寸
f=double(I);     % 数据类型转换，MATLAB不支持图像的无符号整型的计算
g=fft2(f);       % 二维傅立叶变换，得到频域信息
g=fftshift(g);   % 0频率移到(M/2,N/2)处

n=2;            % 二阶巴特沃斯(Butterworth)滤波器
D0=3;           % 巴特沃斯滤波器的D0，D0越大，保留的高频信号就越少（对于不同的图片，可以自行调节，以便得到好的效果）
for i=1:M
    for j=1:N 
        D=sqrt((i-M/2)^2+(j-N/2)^2);   %计算离0频率(M/2,N/2)的距离
        h=1/(1+(D0/D)^(2*n));          %计算传递函数的值
        output(i,j)=h*g(i,j);          %高通滤波之后(i,j)处的值
    end
end
result=ifftshift(output);   %与g=fftshift(g)对应，还原回去
I=ifft2(result);   %傅里叶反变换
I=uint8(real(I));  %得到滤波之后的图像（锐化之后的图像）
figure
subplot(2,1,1); 
imshow(I);          %滤波后图像显示
title('锐化之后的图像')

%直方图均衡化处理
I=histeq(I);    %得到直方图均衡化之后的图像
subplot(2,1,2); 
imshow(I);
title('直方图均衡之后的图像')

%边缘检测
I1=edge(I,'prewitt');
I2=edge(I,'roberts');
I3=edge(I,'sobel');
I4=edge(I,'log'); 
I5=edge(I,'canny'); 
h=fspecial('gaussian',5);%?高斯滤波
I6=edge(I,'canny');

figure
subplot(2,3,1); imshow(I1);title('prewitt边缘检测');
subplot(2,3,2); imshow(I2);title('roberts边缘检测');
subplot(2,3,3); imshow(I3);title('sobel边缘检测');
subplot(2,3,4); imshow(I4);title('Log边缘检测');
subplot(2,3,5); imshow(I5);title('canny边缘检测');
subplot(2,3,6); imshow(I6);title('canny&高斯滤波边缘检测');

