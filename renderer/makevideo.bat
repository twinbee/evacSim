@echo off

REM %1 cam#
REM %2 name
REM %3 date

cd %1

if NOT EXIST 00000.JPG GOTO error1

REM echo Creating Video Batch Job...
..\vdub.exe /cmd VirtualDub.video.SetCompression(0x64697678,0,10000,0) /p 00000.jpg %1_%2_%3.avi

REM echo Running Video Batch Job...
..\vdub.exe /r /x

move %1_%2_%3.avi ../../videos/

goto end
:error1
echo Input files not found for %1
:end

REM exit cam directory
cd ..
