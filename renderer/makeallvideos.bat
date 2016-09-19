@echo off

vdub /c
FOR %%A IN (cam0 cam1 cam2 cam3 cam4 cam5 cam6 cam7 cam8 cam9) DO call makevideo %%A %1 %2
