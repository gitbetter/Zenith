#!/bin/bash

dev=false

while test $# -gt 0; do
	case "$1" in
		-d|--development) dev=true; shift;;
		*) break;;
	esac
done

if [ ! -d _Bin/ ] || [ ! -f _Bin/Makefile ]; then
  if [ "$dev" = true ]; then
		sh rebuild.sh --development
	else 
		sh rebuild.sh
	fi
else
	cd _Bin/; make; cd ..;
fi
