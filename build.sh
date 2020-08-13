#!/bin/bash

dev=false
xcode=false
headless=false
clean=false
project=""

while test $# -gt 0; do
	case "$1" in
		-d|--development) dev=true; shift;;
		-x|--xcode) xcode=true; shift;;
		-h|--headless) headless=true; shift;;
		-c|--clean) clean=true; shift;;
		-p|--project-name)
			if [ -n "$2" ]; then
				project=$2
				shift 2
			fi
			;;
		*) break;;
	esac
done

if [ ! -d ./Engine/_Bin ]; then
	mkdir Engine/_Bin
fi

cd Engine/_Bin

if [ "$clean" = true ]; then
	rm -rf ./*
fi

if [ "$xcode" = true ]; then
	flags="-G Xcode"
fi

if [ "$dev" = true ]; then
	flags=" $flags -DDEVELOPMENT=ON"
fi

if [ $"headless" = true ]; then
	flags=" $flags -DHEADLESS=ON"
fi

if [ -n "$project" ]; then
	flags=" $flags -DUSER_PROJECT_NAME=$project"
fi

cmake $flags ../..;

if [ -n "$project" ]; then
	cd ../../$project;
	if [ ! -d ./Build ]; then
		mkdir Build
	fi
	cd Build
	if [ "$clean" = true ]; then
		rm -rf ./*
	fi
    cmake $flags ..;
fi


if [ "$xcode" = false ]; then
	make
else
	open .
fi
