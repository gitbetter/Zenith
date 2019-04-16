#!/bin/bash

dev=false
xcode=false

while test $# -gt 0; do
	case "$1" in
		-d|--development) dev=true; shift;;
		-x|--xcode) xcode=true; shift;;
		*) break;;
	esac
done

if [ ! -d ./_Bin ]; then
	mkdir _Bin
fi

cd _Bin; rm -rf *; 

if [ "$dev" = true ]; then 
	if [ "$xcode" = true ]; then
		cmake -G Xcode -DDEVELOPMENT=ON ..; 
	else
  		cmake -DDEVELOPMENT=ON ..; 
	fi	
else
	if [ "$xcode" = true ]; then
		cmake -G Xcode ..; 
	else
  		cmake ..; 
	fi	
fi

if [ "$xcode" = false ]; then
	make
else
	open .
fi
