:: called from visual studio
@ECHO OFF
py Engine\Scripts\create_module.py %1
call GenerateProjectFiles.bat
PAUSE