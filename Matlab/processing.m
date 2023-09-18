%�Ҷ�ת�����������񻯡�ֱ��ͼ���⻯������Ե���
clear;
clc;
%��ȡͼ��
[filename,filepath]=uigetfile({'*.*','*.jpg'},'���ļ�');%gui�д��ļ�
filep=strcat(filepath,filename);
I=imread(filep);
figure
subplot(2,2,1); 
imshow(I)
title('ԭͼ');

%ת���Ҷ�ͼ
try
    I=rgb2gray(I);    %�����RGBͼ��ת�ɻҶ�ͼ
end
subplot(2,2,2); 
imshow(I);
title('�Ҷ�ͼ��');

%�������
I=imnoise(I,'salt & pepper',0.02); % �����ܶ�Ϊ0.02�Ľ�������
%��һ��������ֵİ׵���ߺڵ㣬���������������к�ɫ���ػ����ڰ��������а�ɫ���أ��������߽��У�
subplot(2,2,3);
imshow(I);
title('���������ͼ��');

%ͼ��ȥ��
I=medfilt2(I,[2 4]);   %���ö�ά��ֵ�˲�����medfilt2���ܽ����������ŵ�ͼ���˲�
subplot(2,2,4); 
imshow(I);
title('ȥ����ͼ��');

%ͼ����
[M,N]=size(I);   % ����ͼ��ĳߴ�
f=double(I);     % ��������ת����MATLAB��֧��ͼ����޷������͵ļ���
g=fft2(f);       % ��ά����Ҷ�任���õ�Ƶ����Ϣ
g=fftshift(g);   % 0Ƶ���Ƶ�(M/2,N/2)��

n=2;            % ���װ�����˹(Butterworth)�˲���
D0=3;           % ������˹�˲�����D0��D0Խ�󣬱����ĸ�Ƶ�źž�Խ�٣����ڲ�ͬ��ͼƬ���������е��ڣ��Ա�õ��õ�Ч����
for i=1:M
    for j=1:N 
        D=sqrt((i-M/2)^2+(j-N/2)^2);   %������0Ƶ��(M/2,N/2)�ľ���
        h=1/(1+(D0/D)^(2*n));          %���㴫�ݺ�����ֵ
        output(i,j)=h*g(i,j);          %��ͨ�˲�֮��(i,j)����ֵ
    end
end
result=ifftshift(output);   %��g=fftshift(g)��Ӧ����ԭ��ȥ
I=ifft2(result);   %����Ҷ���任
I=uint8(real(I));  %�õ��˲�֮���ͼ����֮���ͼ��
figure
subplot(2,1,1); 
imshow(I);          %�˲���ͼ����ʾ
title('��֮���ͼ��')

%ֱ��ͼ���⻯����
I=histeq(I);    %�õ�ֱ��ͼ���⻯֮���ͼ��
subplot(2,1,2); 
imshow(I);
title('ֱ��ͼ����֮���ͼ��')

%��Ե���
I1=edge(I,'prewitt');
I2=edge(I,'roberts');
I3=edge(I,'sobel');
I4=edge(I,'log'); 
I5=edge(I,'canny'); 
h=fspecial('gaussian',5);%?��˹�˲�
I6=edge(I,'canny');

figure
subplot(2,3,1); imshow(I1);title('prewitt��Ե���');
subplot(2,3,2); imshow(I2);title('roberts��Ե���');
subplot(2,3,3); imshow(I3);title('sobel��Ե���');
subplot(2,3,4); imshow(I4);title('Log��Ե���');
subplot(2,3,5); imshow(I5);title('canny��Ե���');
subplot(2,3,6); imshow(I6);title('canny&��˹�˲���Ե���');

