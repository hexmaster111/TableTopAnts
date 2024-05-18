#!/bin/bash
set -e
if ! test -d bin; then
    mkdir bin
fi

cc -I. -ggdb             \
    src/main.cpp         \
    src/ants.cpp         \
    src/timer.cpp        \
     -o bin/main -lraylib -lm -std=c++11
./bin/main
