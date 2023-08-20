@echo off
echo -----------------------------------------------------
echo [TrustAgent.pro] 文件有更新时，请重新生成当前工程
echo 以下配置并不适用于所有人，如有编译报错，请自行修改。
echo 1. Qt工具集默认配置为环境变量QtPath: 当前系统QtPath:[%QtPath%] （推荐 5.9.9_msvc2015）
echo 2. 工具集默认配置为: $(DefaultPlatformToolset) （推荐 10.0.19041.0及以上）
echo 3. SDK默认配置为: $([Microsoft.Build.Utilities.ToolLocationHelper]::GetLatestSDKTargetPlatformVersion('Windows', '10.0')) （推荐 V142）
echo -----------------------------------------------------

@REM if "%QtPath%" == "" (
@REM     choice /c:yn /m "当前未配置环境变量，是否打开系统配置页面："
@REM     if %errorlevel%==1 (
@REM         start ms-settings:about
@REM     )
@REM )
