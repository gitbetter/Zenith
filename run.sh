#!/bin/bash

make ./_Bin;

unameOut=$(uname -s)
case ${unameOut} in
    Darwin*) DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:_Drivers/lib" ./_Bin/zenith;;
    CYGWIN*) export PATH=_Drivers/lib/:$PATH; ./_Bin/zenith;; 
    *) LD_LIBRARY_PATH="$LD_LIBRARY_PATH:_Drivers/lib" ./_Bin/zenith;;
esac
