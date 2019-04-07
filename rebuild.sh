#!/bin/bash

dev=false

while test $# -gt 0; do
	case "$1" in
		-d|--development) dev=true; shift;;
		*) break;;
	esac
done

if [ ! -d ./_Bin ]; then
	mkdir _Bin
fi

cd _Bin; rm -rf *; 

if [ "$dev" = true ]; then 
  cmake -DDEVELOPMENT=ON ..; 
else
  cmake ..;
fi


make
