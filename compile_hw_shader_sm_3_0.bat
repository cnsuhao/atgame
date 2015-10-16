@echo off

:: 设置编译程序为当前目录下的fxc.exe
set executive=%~dp0fxc.exe

:: 跳转到 data/shaders 目录下
cd %~dp0data/shaders

:: 将%%a变量设置为当前遍历的hlsl文件.
for /r %%a in (*.hlsl) do ( 
    if not exist %%~na.dxvs (
        :: 如果不存在同名的dxvs文件. 直接编译
        "%executive%" /E vs_main /Zpc /T vs_3_0 /Fo %%~na.dxvs %%~fa
    ) else (
        :: 如果存在同名的dxvs文件. 做最新比较
        for %%i in ("%%~na.dxvs") do (
            if %%~ti lss %%~ta (
                :: 如果dxvs文件修改时间小于hlsl文件修改时间.编译覆盖
                "%executive%" /E vs_main /Zpc /T vs_3_0 /Fo %%~na.dxvs %%~fa
            )
        )
    )
    if not exist %%~na.dxps (
        "%executive%" /E ps_main /Zpc /T ps_3_0 /Fo %%~na.dxps %%~fa
    ) else (
        for %%j in ("%%~na.dxps") do (
            if %%~tj lss %%~ta (
                "%executive%" /E ps_main /Zpc /T ps_3_0 /Fo %%~na.dxps %%~fa
            )
        )
    )
)

pause