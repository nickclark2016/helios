#!/bin/sh

find . -name "*.vcxproj*" -type f -delete
find . -name "*.sln*" -type f -delete
./dependencies/premake/premake5.exe vs2019