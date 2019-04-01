#!/bin/bash

make ./_Bin;

unameOut=$(uname -s)
case ${unameOut} in
    Darwin*) DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:_Drivers/darwin" _Bin/build/zenith;;
    *) LD_LIBRARY_PATH="$LD_LIBRARY_PATH:_Drivers/lib" _Bin/build/zenith;;
esac
