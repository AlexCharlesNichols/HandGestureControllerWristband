
close all
clear
clc
blelist("Name","BatteryMonitor")
%b = ble("BatteryMonitor")
b = ble("343CC92C-A550-4B33-B2F3-49E000A57C99")

%blelist()
b.Characteristics

c = characteristic(b,"Battery Service","Battery Level")
% or use c = characteristic(b,"180F","2A19")
volume = 1;
[x,fs1] = audioread('6secbeep.wav');
%sound(x,fs1)
a=1;
while 1
    close all
data = read(c)
%or 
% [data,timestamp] = read(c)
if (data == 3)
    fprintf(' Emergency Call (Tapping) \n ')
    imshow('emergency.png')
    [y,fs]=audioread('6secbeep.wav');
    soundsc(y,fs)
   
elseif(data == 2)
    
    fprintf('Volume up (Rotation_CW) \n')
    
    
    if (data == 2 && a==1)
    %volume = volume+1000
    %z=volume*x;
    [x,fs1] = audioread('6sec_tune.wav');
    %sound(z,fs1)
    sound(x,fs1)
    imshow('volume_1.jpg')
    a=0;
    elseif (data ==2 && a==0)
        volume = volume+10
    z=volume*x;
    sound(z,fs1)
    imshow('volume.jpeg')
    a=1;
    volume=1;
    
    end 
    
elseif(data == 1)
    fprintf('Light On(Arm Raise) \n')
    imshow('Light_On.png')
    title('light on')
else
    fprintf('No gesture detected \n')
    
end 
pause(5)
 end        

% bt = bluetooth("Edin",1);
% write(bt,[2,0,1,155])
% bt.NumBytesWritten
% bt.NumBytesAvailable
% name = read(bt,35);
% char(name(7:10))
% clear bt