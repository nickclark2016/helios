#!/bin/sh

find . -name "Makefile" -type f -delete
./dependencies/premake/premake5 gmake2