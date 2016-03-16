@echo off

::cd %~dp0/android

::del /s/q obj

call "%ANDROID_NDK%/ndk-build" NDK_DEBUG=1 V=0

pause
