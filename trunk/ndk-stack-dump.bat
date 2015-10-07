set dir=%~dp0

"%ANDROID_NDK%/ndk-stack" -sym "%dir%/android/obj/local/armeabi" -dump "%dir%/logcat.txt"

