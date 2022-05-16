git submodule update --init
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Release
explorer .
pause