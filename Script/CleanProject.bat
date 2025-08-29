@echo off
pushd ..\

echo Cleaning Binaries and Intermediates

:: Delete all `bin` folders except `Vendor\premake\bin`
for /d /r . %%d in (bin) do (
    if not "%%d"=="%CD%\Vendor\premake\bin" (
        if exist "%%d" (
            echo Deleting: %%d
            rd /s /q "%%d"
        )
    )
)

:: Delete all `bin-int` folders
for /d /r . %%d in (bin-int) do (
    if exist "%%d" (
        echo Deleting: %%d
        rd /s /q "%%d"
    )
)

:: Remove all .dll, .exe, .pdb files from RootDir/LostSheepCore
set "targetDir=%CD%\LostSheepCore"
if exist "%targetDir%" (
    echo Deleting .dll, .exe, and .pdb files from %targetDir%
    del /s /q "%targetDir%\*.dll"
    del /s /q "%targetDir%\*.exe"
    del /s /q "%targetDir%\*.pdb"
)

echo Cleaning Makefiles / Visual Studio files...

:: Visual Studio files
if exist *.sln (
    echo Deleting: Solution files (*.sln)
    del /s /q *.sln
)
if exist *.vcxproj (
    echo Deleting: Project files (*.vcxproj)
    del /s /q *.vcxproj
)
if exist *.vcxproj.filters (
    echo Deleting: Filter files (*.vcxproj.filters)
    del /s /q *.vcxproj.filters
)
if exist *.vcxproj.user (
    echo Deleting: User settings (*.vcxproj.user)
    del /s /q *.vcxproj.user
)
if exist *.suo (
    echo Deleting: Solution user options (*.suo)
    del /s /q *.suo
)
if exist *.sdf (
    echo Deleting: Solution database (*.sdf)
    del /s /q *.sdf
)

:: Makefiles
if exist Makefile (
    echo Deleting: Makefile
    del /s /q Makefile
)

echo Done!

popd
pause