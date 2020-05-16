#!/bin/sh

find . -name "Makefile" -type f -delete
sh ./premake_common.sh
./dependencies/premake/premake5 gmake2