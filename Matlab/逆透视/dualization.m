%读取一张图片，并显示
original_picture=imread('31.bmp');



thresh=graythresh(GrayPic);
Pic2_=im2bw(GrayPic,thresh);
figure;
subplot(1,2,1);
imshow(original_picture);
title('原始灰度图像')
subplot(1,2,2);
imshow(Pic2_);
title('灰度图像转化为二值化图像')
