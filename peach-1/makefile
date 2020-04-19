CXX=g++
CXXFLAGS=
main: mainMemory.o memory_arch.o pipeline.o
	$(CXX) $(CXXFLAGS) -o main mainMemory.o memory_arch.o pipeline.o

mainMemory.o: mainMemory.cpp mainMemory.h
	$(CXX) $(CXXFLAGS) -c mainMemory.cpp 

memory_arch.o: memory_arch.cpp 
	$(CXX) $(CXXFLAGS) -c memory_arch.cpp

pipeline.o: pipeline.cpp mainMemory.h pipeline-helper.h
	$(CXX) $(CXXFLAGS) -c pipeline.cpp

