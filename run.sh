#!/bin/bash

cd _Bin; make;

unameOut=$(uname -s)
case ${unameOut} in
    Darwin*) DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:../_Drivers/darwin" build/zenith;;
    *) LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../_Drivers/lib" build/zenith;;
esac
