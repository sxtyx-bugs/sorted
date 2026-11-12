@echo off
title SORTED --- C++17 Test Suite
cd /d "%~dp0\.."
set PATH=C:\QtTools\Tools\mingw1310_64\bin;%PATH%
echo Running compiled C++17 Core Verification Suite...
if not exist build mkdir build
g++ -std=c++17 tests\cpp\test_standalone_core.cpp src\core\FileScanner.cpp src\core\FileClassifier.cpp src\core\OrganizationPlanner.cpp -Isrc -Isrc\core -Isrc\models -o build\test_core.exe
build\test_core.exe
echo.
