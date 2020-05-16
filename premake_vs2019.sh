#!/bin/sh

find . -name "*.vcxproj*" -type f -delete
find . -name "*.sln*" -type f -delete
sh ./premake_common.sh
./dependencies/premake/premake5.exe vs2019