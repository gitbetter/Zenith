#!/bin/bash

make;
if [[ "$OSTYPE" == "darwin"* ]]; then
	DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:_Drivers/lib" ./game
else
	LD_LIBRARY_PATH="$LD_LIBRARY_PATH:_Drivers/lib" ./game
fi