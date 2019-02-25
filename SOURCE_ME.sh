#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
	export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:_Drivers/lib"
else
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:_Drivers/lib"
fi
