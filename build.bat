@echo off
echo "Updating submodules... "
git submodule update --init -- recursive
echo [32mDone![0m
if not exist build mkdir build
cd build
cmake ..
:choice
set /P c=Run unit tests[Y/N]?
if /I "%c%" EQU "Y" goto :tests
if /I "%c%" EQU "N" goto :open_folder
goto :open_folder
:tests
cmake --build . --config Release
.\Release\runUnitTests.exe
:open_folder
set /P c=Open Solution[Y/N]?
if /I "%c%" EQU "N" goto :end
dle_internship.sln
:end
explorer .
pause