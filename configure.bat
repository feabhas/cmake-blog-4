set CMAKE="C:\Program Files\CMake\bin\cmake.exe"
set MAKE="C:\msys64\usr\bin\make.exe"
set ARMTOOLS=C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2020-q4-major\bin
set PATH=%PATH%;%ARMTOOLS%
%CMAKE% -S . -B build/debug ^
        -G "Unix Makefiles" ^
        -DCMAKE_MAKE_PROGRAM=%MAKE% ^
        -DCMAKE_TOOLCHAIN_FILE=toolchain-STM32F407.cmake
%MAKE% -C build/debug VERBOSE=1
