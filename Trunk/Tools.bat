@echo off
REM Run Tools from Project Home Directory
REM with Project name as parameter
REM Allows to apply generic tools to the project
REM If no generic tools exists, do nothing
if exist %TOOLS_DIR%\Bin\Tools.bat call %TOOLS_DIR%\Bin\Tools.bat WxPic
pause