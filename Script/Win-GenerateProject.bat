@echo off
python Setup.py
if errorlevel 1 (
    echo Failed to run Setup.py
    pause
    exit /b 1
)

pushd ..\
if not exist "Vendor\premake\bin\premake5.exe" (
    echo Error: Couldn't find premake5.exe!
    popd
    pause
    exit /b 1
)

echo Generating project files...
call Vendor\premake\bin\premake5.exe vs2022
if errorlevel 1 (
    echo Failed to generate project files!
    popd
    pause
    exit /b 1
)

popd
echo Project files generated successfully!
pause