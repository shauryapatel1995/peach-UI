#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include <fstream>
#include "mainMemory.h"

using namespace std;

Memory::Memory(int** memory, Memory *main_memory, int ** nextMemoryArray, int wait_time, string id, int size) {
	memmywemmy = memory; 
	//cout << "Print array: " << memmywemmy[0][0] << memmywemmy[0][1] << memmywemmy[0][2] << "\n";
	next_level = main_memory; 
	cacheMoney = nextMemoryArray;
	mem_wait_time = wait_time;
	memId = id;
	this->size = size; 
}

// input: desired address
// searches through the cache or main memory matrices for address
// FIXME - The size cannot work for integer for 32 bits because of signed, unsigned issue. 
int Memory::search(int address[], int* result){
	cout << "searching\n";
	// found it flag 
	int foundMemory = 0;
	// address is an 1x3 array: tag, index, value 
	int searchTag = address[0];
	int searchIndex = address[1];
	// cout << "Search tag is: " << searchTag << " search index is: " << searchIndex << "\n";
	// cout << "memory ID is: " << this->memId;
	// declaring row and col index
	int r = 0;
	// searching through all of the memory 
	for (r=0; r < this->size; r++){
		//all memory should have 3 columns: tag, index, value
		if (searchTag == memmywemmy[r][0] && searchIndex == memmywemmy[r][1]) {
			// found it flag 
			foundMemory = 1;
			// if found in main memory - add the value to cache
			int foundValue = memmywemmy[r][2];
			// cout << "we found the value: " << memmywemmy[r][2] << " in " << memId << "\n";
			//cout << "replacing the value " << foundValue << " in cache\n";
			//cout << "this is the index in memory: " << r << "\n";
			*result = foundValue;
			// return the cycles it took to run it
			return mem_wait_time;	
		} else {
			// if(this->memId == "main")
			// 	cout << "Search tag is: " << memmywemmy[r][0] << " search index is: " << memmywemmy[r][1] << "\n";
		}
	}
	//TODO: -1 can also be a valid memory number right?
	return -1;
}

// if not in cache replace
// index will be prepopulated [tag][index]
void Memory::replace(int address[], int value, int size){
	// for now will always be replacing in cache - can make size standard
	// for debugging - found flag
	int replaced = 0;
	int index = address[1];
	int rows = 0;
	for (rows=0; rows<size; rows++) {
		if (memmywemmy[rows][1] == index){
			// replacing tag 
			memmywemmy[rows][0] = address[0];
			// replacing value
			//cout << "Replacing the value in cache: " << value << "\n"; 
			//cout << "replaced addresss!!" << address[0] << address[1] << "\n";
			memmywemmy[rows][2] = value;
			replaced = 1;
			break;
		}
	}
	if (replaced == 0){
		cout<<"did not find the address to replace\n";
	}
}


int Cache::search(int address[], int* result) {
	
	// TODO: Change this. We are using this for now.
	*result = -1;
	int wait_time = Memory::search(address, result);
	cout << "Result is: " << *result << "\n";

	if(*result == -1) {
		cout << "Going to the next level!\n";
		int wait_time_next_level = next_level->search(address, result);
		int value = *result;
		Memory::replace(address, value, size);
		return wait_time + wait_time_next_level;
	} else {
		return wait_time;
	}
}

int Cache::write(int address[], int value) {
	int r = 0;
	for(r = 0; r < this->size; r++) {
		int index = memmywemmy[r][1];
		int tag = memmywemmy[r][0];
		if(address[1] == index) {
			// cout << "This is the index and value of the write to cache: " << r << ", " << value << "\n";
			memmywemmy[r][0] = address[0];
			memmywemmy[r][2] = value;
			break;
		}
	} 

	// Write back semantics.
	if(next_level != nullptr) {
		// FIXME - Hack af, I don't know how to achieve this in C++
		MainMemory* next = (MainMemory*) next_level;
		return mem_wait_time + next->write(address, value);
	}
		
	else 
		return mem_wait_time;
}

int MainMemory::write(int address[], int value){
	int r = 0;
	for(r = 0; r < this->size; r++) {
		int index = memmywemmy[r][1];
		int tag = memmywemmy[r][0];
		if(address[1] == index && address[0] == tag) {
			//cout << "Index is: " << r << "\n";		
			memmywemmy[r][2] = value;	
		}
	}
	return mem_wait_time; 
}
