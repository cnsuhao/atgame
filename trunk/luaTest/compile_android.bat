@echo off

::del /s/q obj

call "%ANDROID_NDK%/ndk-build" clean

call "%ANDROID_NDK%/ndk-build" NDK_DEBUG=0 V=1

pause
