#!/bin/bash

dev=false
xcode=false
demo=false

while test $# -gt 0; do
	case "$1" in
		-d|--development) dev=true; shift;;
		-x|--xcode) xcode=true; shift;;
		-D|--demo) demo=true; shift;;
		*) break;;
	esac
done

if [ $"dev" = true ]; then
	flags="--development"
fi

if [ $"xcode" = true]; then
	flags=" $flags --xcode"
fi

if [ $"demo" = true]; then
	flags=" $flags --demo"
fi

if [ ! -d _Bin/ ] || [ ! -f _Bin/Makefile ]; then
	sh rebuild.sh $flags
elif [ "$xcode" = false ]; then
	cd _Bin/; make; cd ..;
fi

if [ "$xcode" = true ]; then
	open _Bin/
fi
