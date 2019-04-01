#!/bin/bash

if [ ! -d _Bin/ ] || [ ! -f _Bin/Makefile ]; then
	sh rebuild.sh
else
	make ./_Bin
fi
