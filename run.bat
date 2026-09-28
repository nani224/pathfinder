@echo off
title PathFinder - Graph Shortest Path
echo ===================================================
echo               Starting PathFinder                  
echo ===================================================

if not exist "bin\pathfinder.exe" (
    echo Binary not found. Compiling first...
    call build.bat
)

bin\pathfinder.exe data\graph.txt
pause
