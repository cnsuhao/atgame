::@echo off
::查看targets可用版本
::call "%ANDROID_SDK%\tools\android.bat" list targets 

call "%ANDROID_SDK%\tools\android.bat" update project -t "android-10" -p . -s 

call ant uninstall

call ant debug install

"%ANDROID_SDK%/platform-tools/adb" shell am start com.atgame.luaTest/android.app.NativeActivity

pause

