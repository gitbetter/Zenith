#!/bin/bash

make;

unameOut=$(uname -s)
case ${unameOut} in
    Darwin*) DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:_Drivers/lib" ./game;;
    CYGWIN*) export PATH=_Drivers/lib/:$PATH; ./game;; 
    *) LD_LIBRARY_PATH="$LD_LIBRARY_PATH:_Drivers/lib" ./game;;
esac
