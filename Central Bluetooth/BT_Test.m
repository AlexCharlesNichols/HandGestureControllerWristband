
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
    imshow('volume.jpeg')
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