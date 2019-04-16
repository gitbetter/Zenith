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

if [ ! -d _Bin/ ] || [ ! -f _Bin/Makefile ]; then
  if [ "$dev" = true ]; then
	if [ "$xcode" = true ]; then
		sh rebuild.sh --development --xcode
	else
		sh rebuild.sh --development
	fi
  else 
	if [ "$xcode" = true ]; then
		sh rebuild.sh --xcode
	else
		sh rebuild.sh
	fi
  fi
elif [ "$xcode" = false]; then
	cd _Bin/; make; cd ..;
fi

if [ "$xcode" = true ]; then
	open _Bin/
fi
