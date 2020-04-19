#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include <fstream>
using namespace std;
class Memory {
	public:
	int ** memmywemmy;
	std::unordered_map<int, int> mem;
	// pointer from cache to main
	Memory *next_level;
	// emulating the time to look up 
	int mem_wait_time;
	int size;
	string memId;	
	int ** cacheMoney;	
	

		Memory(int** memory, Memory *main_memory, int ** nextMemoryArray, int wait_time, string id, int size); 

		// input: desired address
		// searches through the cache or main memory matrices for address
		int search(int address[], int* result);
		// if not in cache replace
		// index will be prepopulated [tag][index]
		void replace(int address[], int value, int size);

};

class Cache: public Memory {
	public:

	Cache(int** memory, Memory *main_memory, int ** nextMemoryArray, int wait_time, string id, int size)
	: Memory(memory, main_memory, nextMemoryArray, wait_time, id, size){} 
	
	int search(int address[], int* result); 

	int write(int[], int);

};

class MainMemory: public Memory {
	public:

	MainMemory(int** memory, Memory *main_memory, int ** nextMemoryArray, int wait_time, string id, int size)
	: Memory(memory, main_memory, nextMemoryArray, wait_time, id, size){} 
	
	int search(int address[], int* result); 

	int write(int[], int);
};
