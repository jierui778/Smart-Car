%PID Controller with changing integration rate
clear all;
close all;
%Big time delay Plant
ts=20;
sys=tf([1],[60,1],'inputdelay',80);
dsys=c2d(sys,ts,'zoh');
[num,den]=tfdata(dsys,'v');

u_1=0;u_2=0;u_3=0;u_4=0;u_5=0;
y_1=0;y_2=0;y_3=0;
error_1=0;error_2=0;
ei=0;

for k=1:1:200
time(k)=k*ts;

yd(k)=1.0;  %Step Signal

%Linear model
y(k)=-den(2)*y_1+num(2)*u_5;
error(k)=yd(k)-y(k);

kp=0.45;kd=12;ki=0.0048;
A=0.4;B=0.6;

%T type integration
ei=ei+(error(k)+error_1)/2*ts;    %%积分过程

M=2;
if M==1     %Changing integration rate
if abs(error(k))<=B  %%偏差很小
   f(k)=1;
elseif abs(error(k))>B&abs(error(k))<=A+B  
   f(k)=(A-abs(error(k))+B)/A;
else
   f(k)=0;  %%偏差非常大
end
elseif M==2  %Not changing integration rate
	f(k)=1;
end

u(k)=kp*error(k)+kd*(error(k)-error_1)/ts+ki*f(k)*ei;

if u(k)>=10     %%限制幅度
   u(k)=10;
end
if u(k)<=-10    %%限制幅度
   u(k)=-10;
end
%Return of PID parameters
u_5=u_4;u_4=u_3;u_3=u_2;u_2=u_1;u_1=u(k);   
y_3=y_2;y_2=y_1;y_1=y(k);   
error_2=error_1;  %%积分的回代
error_1=error(k);
end
figure(1);
plot(time,yd,'r',time,y,'k:','linewidth',2);
xlabel('time(s)');ylabel('yd,y');
legend('Ideal position signal','Position tracking');
figure(2);
plot(time,f,'r','linewidth',2);
xlabel('time(s)');ylabel('Integration rate f');
