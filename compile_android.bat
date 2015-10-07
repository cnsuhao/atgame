@echo off

cd %~dp0/android

::del /s/q obj

call "%ANDROID_NDK%/ndk-build" clean

call "%ANDROID_NDK%/ndk-build" NDK_DEBUG=0 V=0

pause
