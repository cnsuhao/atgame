@echo off

cd %~dp0/Recast

::del /s/q obj 

rd /s/q obj

call "%ANDROID_NDK%/ndk-build" NDK_DEBUG=0

pause
