@echo off
title Planetary Defense Command - Build System (Raylib)
color 0A

echo ========================================
echo   PLANETARY DEFENSE BUILD SYSTEM
echo   (Raylib Graphics Edition)
echo ========================================
echo.

:: Check if Raylib is installed
if not exist "src\engine\raylib\include\raylib.h" (
    color 0E
    echo [WARNING] Raylib not found!
    echo.
    echo Raylib is missing. Run SETUP_GAME.bat first to install everything.
    echo   scripts\SETUP_GAME.bat
    echo.
    pause
    exit /b 1
)

echo [1/3] Cleaning previous build...
if exist game.exe del game.exe

echo [2/3] Compiling defense systems with Raylib...
echo.

g++ src/engine/main.cpp src/student/*.cpp -o game.exe -O2 -I src/engine -I src/engine/raylib/include -L src/engine/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm 2> build_errors.txt

if %errorlevel% neq 0 (
    color 0C
    echo ========================================
    echo   [ERROR] COMPILATION FAILED!
    echo ========================================
    echo.
    echo Defense systems have critical errors.
    echo Check the code for syntax mistakes.
    echo.
    echo --- Error Details ---
    type build_errors.txt
    echo.
    del build_errors.txt
    pause
    exit /b 1
)

del build_errors.txt
echo [OK] Compilation successful!
echo.

echo [3/3] Launching Planetary Defense Command...
echo.
echo ========================================

game.exe

echo.
echo ========================================
echo   [END] Program terminated.
echo ========================================
pause
