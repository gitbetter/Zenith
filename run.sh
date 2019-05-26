#!/bin/bash

cd _Bin; make;

unameOut=$(uname -s)
case ${unameOut} in
    Darwin*) DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:../Engine/_Drivers/darwin" build/Zenith;;
    *) LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../Engine/_Drivers/lib" build/Zenith;;
esac
