
%% ��ͷʧ��У��--��任   2019.12.6   
clear,clc
img_origin1 = imread('fir.BMP'); %��ȡͼ���ļ�
%img_origin2 = imread('D:\rt1061\ͼ��͸�Ӵ���\4.BMP'); %��ȡͼ���ļ�
%img_origin2 = rgb2gray(img_origin2); %��ͼ��ת��Ϊ�Ҷ�ͼ���ά����
img_origin = rgb2gray(img_origin1); %��ͼ��ת��Ϊ�Ҷ�ͼ���ά����
img_size = size( img_origin1 ); % ���ؾ�������������� ����r=size(A,1)����䷵�ؾ���A������rowΪ��ֱ����

m1= 5.2245; % -0.4266   ��͸�Ӿ������  ������ʵ�����ͼ�������Ӧ�õ� ���������ݣ�������������Ǿ���

m2=-0.0000;%0.0245   ��step1�е������W���뼴C����

m3=-211.5918;%36.3723

m4= -0.0000;%0.0129

m5= -2.8469;%0.1698

 m6= 257.1122;%-52.4729

m7=0.0000;%-0.0034

m8=  0.1020;% 0.9007


u=80; % ˮƽ����column124   ���͸�Ӻ�ͼ���С
v=10; % ��rowΪ��ֱ����43   ����ͼ�����϶�13�в�Ҫ
x=(m1*u+m2*v+m3)/(m7*u+m8*v+1);% ��ˮƽ����
y=(m4*u+m5*v+m6)/(m7*u+m8*v+1); % �д�ֱ����
x=uint16(x);
y=uint16(y); %����Ϊ�����¾�������У�����ͼ��
img_undist = zeros(y ,x ); % zeros(m,n)����m��n��ȫ0���� ����Ԫ�س�ʼ��Ϊ0
new_size = size(img_undist); % ����m��nȫ0���������������
img_undist = uint8( img_undist );
img_undist=im2bw(img_undist);   % ��ֵ��ͼ��
%~~~~~~~~~~~~~~~~%
%  t=new_size(1);     %�˴���͸��  �޲�ֵ����
% for i =(img_size(1)):-1: 10 % �д�ֱ����
% 
% for j = 1:(img_size(2)) % ˮƽ����j
% 
% u=j; % ˮƽ����column
% 
% v=i; % ��ֱ����
% 
% x=(m1*u+m2*v+m3)/(m7*u+m8*v+1); % ˮƽ����column
% 
%  y=(m4*u+m5*v+m6)/(m7*u+m8*v+1);% ��rowΪ��ֱ����
% 
% x=int16(x)+new_size(2)/2; %���������ø�,��������ʾ
% 
%  y=int16(y);
% 
%   if(t-y>0&& x>0 )
% 
%   img_undist(t-y,x)= img_origin(i,j); %��Ӧ����
% % img_undist(i,x)= img_origin(i,j); %��Ӧ����
% 
%   end
% 
% end
% 
% end

% img_undist=flipud(img_undist); %����  ��ˮƽ�ᷭת
%~~~~~~~~~~~~%

% C=[0.1687 0.1173  1.4787;%ͼ�������ת������
%          0.0022 -0.0026 3.0900;
%          0.0000 0.0013  0.0156;];        
% D=[6.0861   -0.2428 -530.5047; %���絽ͼ���ת������
%     -0.1742   -3.6928  750.3649;
%     -0.0044    0.3207    1.0000];
 img_origin=im2bw(img_origin);   % ��ֵ��ͼ��
H1= 0.1914  ;H2=0.1421;H3= 3.9646;%�������굽ͼ�������ת��������� ��step1��D����
H4= -0.0000;H5=-0.0344;H6=8.8407;
H7=0.0000;H8=0.0035;H9=0.0979;
%��ֵ���� 
for i = 1:new_size(1)                      
    for j = 1:new_size(2)
        s=H7*(j-new_size(2)/2)+H8 *i+H9; %�ӱ任ͼ���з���Ѱ��ԭͼ��ĵ㣬������ֿն�������ת�Ŵ�ԭ��һ��
        s=1/s;   %���￴�������������ĵ�  
        X=(H1*(j-new_size(2)/2)*s+H2*i*s+H3*s);
        Y=(H4*(j-new_size(2)/2)*s+H5*i*s+H6*s);
        t=new_size(1)-i;% ��תͼ��   137Ϊ������ͼ�����������
         if   Y>=0.5 && X>=0.5 && Y<=60 && X<=80  %��ֹԽ��
                 if(t>0)
                     
                  img_undist(t,j)=img_origin(round(Y),round(X));     %���ڽ���ֵ,Ҳ������˫���Ի�˫������ֵ
                end 
         else                  
                  img_undist(t,j)=255;          
        end
    end
end  
subplot(2,2,1);imshow(img_origin);title('ԭͼ��120*188');
imwrite(img_undist,'result.bmp'); 
 %img_origin=im2bw(img_origin);   % ��ֵ��ͼ��
  %img_undist=im2bw(img_undist);   % ��ֵ��ͼ��
  
  %img_origin2 = edge(img_origin2,'sobel',0.025);
  
subplot(2,2,2);imshow(img_undist);title('У��ͼ');	

subplot(2,2,3);imshow(img_origin);title('У��ͼ');
