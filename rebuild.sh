#!/bin/bash

if [ ! -d ./_Bin ]; then
	mkdir _Bin
fi

cd _Bin; rm -rf *; cmake ..; make
