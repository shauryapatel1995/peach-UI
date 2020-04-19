
C1- home/shaurya/cs535/sciter-sdk/bin.lnx/packfolder ui resources.cpp -v "resources"

C2 - g++ main.cpp helper.cpp peach-1/memory_arch.cpp peach-1/pipeline1.cpp peach-1/mainMemory.cpp /home/shaurya/cs535/sciter-sdk/include/sciter-gtk-main.cpp `pkg-config --cflags --libs gtk+-3.0` -I /home/shaurya/cs535/sciter-sdk/include/ -l sciter-gtk -L /home/shaurya/cs535/sciter-sdk/bin.lnx/x64/ -ldl -std=c++11

We also need to add the sciter sdk path for that sciter-sdk should be under this folder. 
