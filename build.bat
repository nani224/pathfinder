@echo off
echo ===================================================
echo        Building PathFinder Graph Engine            
echo ===================================================

if not exist "bin" mkdir bin

gcc -O2 -Wall src\main.c src\pathfinder.c -o bin\pathfinder.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] PathFinder compilation failed!
    exit /b 1
)

echo [SUCCESS] bin\pathfinder.exe created successfully.
echo.
echo You can run PathFinder via run.bat or bin\pathfinder.exe data\graph.txt
