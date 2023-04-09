function Serial()   %创建函数
delete(instrfindall);   %先关闭串口，否则可能导致出错

global x            %全局变量，供串口中断函数使用

global t;			%全局变量，这里根据需要绘制图形的个人需要而设
global m;
global i;

t = [0];      %时间轴
m = [0];      %数据轴
i = 0;        %用于计数

p = plot(t,m);
axis([i-50 i+50 27.5 29]);
grid on;

%%----------------------------配置部分----------------------------

%创建串口对象，直接在命令框输入serial('com1')可查看串口参数
x = serial('com5');
%设置或修改串口通信参数
set(x,'BaudRate',38400);

%设置中断触发方式

set(x,'BytesAvailableFcnMode','Terminator') %ASCII触发，字符触发
set(x,'Terminator','CR/LF')         %接收到\r\n后触发中断

% set(x,'BytesAvailableFcnMode','byte')     %数据触发，根据发送来的数据长度来触发中断
% set(x,'BytesAvailableFcnCount',1)


x.BytesAvailableFcn = @Callback  %定义中断响应函数对象

fopen(x);                %打开串口，类似于开关

fwrite(x,255); 	  %发送握手信号0xff，接收到后开始回传数据
pause           	  %需要暂停，来接收回传数据
     
fclose(x);

end

function Callback(obj,event)   %创建中断服务函数，绘制图像

global t; %时间
global m; %纵坐标
global i; %时间变化值


    disp('I receive!')
    out = fscanf(obj);
    data = str2num(out)	%将接收到的字符转换为数值 %lf\r\n
     
     %%----------------------以下根据需要自行编写-------------------------------
    t = [t i];							
    m = [m data];
    plot(t,m)
    xlabel('t');
    ylabel('data');
    axis([i-50 i+50 27.5 29]);
    grid on;
    
    i=i+1;

end
