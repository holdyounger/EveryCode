@echo off
echo -----------------------------------------------------
echo [TrustAgent.pro] �ļ��и���ʱ�����������ɵ�ǰ����
echo �������ò��������������ˣ����б��뱨���������޸ġ�
echo 1. Qt���߼�Ĭ������Ϊ��������QtPath: ��ǰϵͳQtPath:[%QtPath%] ���Ƽ� 5.9.9_msvc2015��
echo 2. ���߼�Ĭ������Ϊ: $(DefaultPlatformToolset) ���Ƽ� 10.0.19041.0�����ϣ�
echo 3. SDKĬ������Ϊ: $([Microsoft.Build.Utilities.ToolLocationHelper]::GetLatestSDKTargetPlatformVersion('Windows', '10.0')) ���Ƽ� V142��
echo -----------------------------------------------------

@REM if "%QtPath%" == "" (
@REM     choice /c:yn /m "��ǰδ���û����������Ƿ��ϵͳ����ҳ�棺"
@REM     if %errorlevel%==1 (
@REM         start ms-settings:about
@REM     )
@REM )
