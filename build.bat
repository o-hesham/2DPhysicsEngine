@echo off
REM 2D Physics Engine - Windows Build Script
REM Requires MinGW-w64 and SDL2 development libraries

echo ========================================
echo 2D Physics Engine - Windows Build
echo ========================================

REM Check if make is available
where /q make
if errorlevel 1 (
    echo Error: 'make' not found in PATH
    echo Please install MinGW-w64 or ensure it's in your PATH
    pause
    exit /b 1
)

REM Check if g++ is available
where /q g++
if errorlevel 1 (
    echo Error: 'g++' not found in PATH
    echo Please install MinGW-w64 or ensure it's in your PATH
    pause
    exit /b 1
)

REM Parse command line arguments
set BUILD_TARGET=all
set BUILD_MODE=release

:parse_args
if "%1"=="debug" (
    set BUILD_MODE=debug
    shift
    goto parse_args
)
if "%1"=="release" (
    set BUILD_MODE=release
    shift
    goto parse_args
)
if "%1"=="clean" (
    set BUILD_TARGET=clean
    shift
    goto parse_args
)
if "%1"=="run" (
    set BUILD_TARGET=run
    shift
    goto parse_args
)
if "%1"=="help" (
    set BUILD_TARGET=help
    shift
    goto parse_args
)
if not "%1"=="" (
    set BUILD_TARGET=%1
    shift
    goto parse_args
)

echo Building target: %BUILD_TARGET%
echo Build mode: %BUILD_MODE%
echo.

REM Run make with specified parameters
if "%BUILD_TARGET%"=="all" (
    make BUILD_MODE=%BUILD_MODE%
) else (
    make %BUILD_TARGET% BUILD_MODE=%BUILD_MODE%
)

if errorlevel 1 (
    echo.
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!

REM If running, don't pause
if not "%BUILD_TARGET%"=="run" (
    pause
)
