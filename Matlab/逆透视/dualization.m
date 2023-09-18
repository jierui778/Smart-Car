A=imread('1.png');   %读取到一张图片   
thresh = graythresh(A);     %自动确定二值化阈值
I2 = im2bw(A,120/255); %对图像二值化
figure();
subplot(1,2,1);  
imshow(A);    %显示二值化之前的图片
title('原图'); 
subplot(1,2,2);
imshow(I2);    %显示二值化之后的图片
title('二值化'); 