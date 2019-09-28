#!/bin/bash

if [ -r VMMDataBase ]; then
/bin/rm -rf VMMDataBase
fi

if [ -r $1 ]; then
echo $1 "will compile soon"
g++ $1 -O2 `root-config --cflags` `root-config --libs --glibs` -o VMMDataBase
##g++ -g -std=c++0x -I /usr/include/root $1 `root-config --libs --glibs` -o VMMStatus
ls -ltF VMMDataBase
else
echo "any file for compilation is missing"
fi

if [ -r VMMDataBase ]; then
./VMMDataBase
else
echo "executable is missing"
fi
