git submodule update --init
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
set /P c=Open build folder[Y/N]?
if /I "%c%" EQU "N" goto :end
explorer .
:end
pause