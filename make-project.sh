#!/bin/bash
./sciter-sdk/bin.lnx/packfolder ui resources.cpp -v "resources"

 g++ -g main.cpp helper.cpp peach-1/memory_arch.cpp  peach-1/mainMemory.cpp peach-1/pipeline.cpp ./sciter-sdk/include/sciter-gtk-main.cpp `pkg-config --cflags --libs gtk+-3.0` -I ./sciter-sdk/include/ -l sciter-gtk -L ./sciter-sdk/bin.lnx/x64/ -ldl -std=c++14
