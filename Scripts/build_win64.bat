@echo off
setlocal enabledelayedexpansion

echo ===============================================================================
echo  ModelW Precision Dynamics - Windows 64-bit Build Script
echo ===============================================================================

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..
cd /d "%PROJECT_DIR%"

:: Check if cl.exe is already available in PATH
where cl.exe >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo MSVC 64-bit compiler environment is already active.
    goto :CONFIGURE
)

:: Locate vcvars64.bat
set VCVARS_PATH=""

if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set VCVARS_PATH="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set VCVARS_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set VCVARS_PATH="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set VCVARS_PATH="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set VCVARS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

if %VCVARS_PATH%=="" (
    echo [ERROR] Could not find Visual Studio vcvars64.bat.
    echo Please run this script from a Native Tools x64 Command Prompt.
    pause
    exit /b 1
)

echo Initializing MSVC x64 tools: %VCVARS_PATH%
call %VCVARS_PATH%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to initialize MSVC x64 environment.
    pause
    exit /b 1
)

:CONFIGURE
:: Prefer Ninja if available, otherwise Visual Studio generator
set CMAKE_GENERATOR="Visual Studio 17 2022"
where ninja.exe >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set CMAKE_GENERATOR=Ninja
) else if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" (
    set PATH=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;!PATH!
    set CMAKE_GENERATOR=Ninja
)

echo.
echo Configuring ModelW with CMake (Generator: %CMAKE_GENERATOR%)...
cmake -B build_win64 -G %CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo Building ModelW (Release Win64)...
cmake --build build_win64 --config Release --parallel
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo ===============================================================================
echo  Build Succeeded!
echo  VST3 Plugin:       %PROJECT_DIR%\build_win64\ModelW_artefacts\Release\VST3\ModelW.vst3
echo  Standalone App:    %PROJECT_DIR%\build_win64\ModelW_artefacts\Release\Standalone\ModelW.exe
echo ===============================================================================

endlocal
