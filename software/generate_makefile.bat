rem windows command shell batch script - generate_makefile.cmd
cmake -E chdir build/ cmake -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=cs-make -DCMAKE_TOOLCHAIN_FILE=toolchains/arm-none-eabi.cmake ../
cmake --build build/ --target clean
cmake --build build/ --use-stderr
pause
