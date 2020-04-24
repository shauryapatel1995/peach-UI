#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <thread>
#include <fstream>
#include "mainMemory.h"
using namespace std;

extern void run_pipeline(Cache* cache, int sizeCache);

int readMemory (int **bigDaddy, int size) {
	std::cout << "Reading file!";
	string x;
	ifstream myFile;
	myFile.open("./peach-1/sort-final.txt");
	int iter = 0;
	while (myFile >> x)
	{
		
		string tagStr = x.substr(0,6);
		string indexStr = x.substr(6,10);
		string valueStr = x.substr(16,32);
		int tag = stoi(tagStr, nullptr, 2);
		int index = stoi(indexStr, nullptr, 2);
		long value = stol(valueStr, nullptr, 2);
		//std::cout << "\n this is my tag: " << tag << ", this is my index: " << index << ", this is my value: " << value << "\n";
		if (iter < size) {
			bigDaddy[iter][0] = tag; 
			bigDaddy[iter][1] = index;
			bigDaddy[iter][2] = value;
			iter++;

		}
	}
	std::cout << "Well loop doesn't work! \n";
	return 50; 
}

/*int main() {
	// 2-D array for cache 
	int **bigDaddy = NULL;
	// similar to malloc - allocating 100 spots 
	int sizeCache = 1024;
	int sizeMain = 65536;
	int result = 0;
	// declaring the address we will be searching for in memory 
	int desiredAddress[3]{0};
	bigDaddy = new int* [sizeMain]; 
	for(int i = 0; i < sizeMain; i++){
		bigDaddy[i] = new int[3]{0};
	}
	
	// initializing main memory with the text file 
	readMemory(bigDaddy, sizeMain);
	// initializing cache	
	int **lilDaddy = NULL;
	lilDaddy = new int *[sizeCache];
	for (int j = 0; j < sizeCache; j++){
		lilDaddy[j] = new int[3];
		for(int k = 0; k < 3; k++)
		{
			if (k == 1){
				// converts decimal to a binary string
				lilDaddy[j][k] = j;
			}
			else {	
				lilDaddy[j][k] = -1;
			}	
		}
	}

	// FOR DEUBGGGGING
	// assigning values in main memory to play with 

	
	// create memory objects 
	MainMemory * mainMem_array = new MainMemory(bigDaddy, nullptr, lilDaddy, 10, "main", sizeMain);
	Cache * cache_array = new Cache(lilDaddy,mainMem_array, bigDaddy, 1, "cache", sizeCache);

	
	// address that we are searching
	desiredAddress[0] = 8;
	desiredAddress[1] = 242;
	desiredAddress[2] = -1;
	//printing the desired address
	cout << "This is the desired address: " << desiredAddress[0] << desiredAddress[1] << desiredAddress[2] <<"\n";
	// cache array 
	//cout << "The result before searching is: " << result << "\n";
	cache_array->search(desiredAddress, &result);
	cout << "Result obtained was after searching: " << result << "\n";


	cout << "Finding the desired address again. Making sure that it is written in cache.\n";	
	cache_array->search(desiredAddress, &result);
	//cout << "Result obtained was: " << result << "\n";

	int write_address[3]{0};
	
	write_address[0] = 9;
	write_address[1] = 235;
	write_address[2] = 5;
	//cout << "Getting write memory" << cache_array->search(write_address, 100) << "\n";
	// FIXME - Right now the instruction type is 2 and opcode is 8. But if we write 2^32 4294967296 to memory it will overflow.
	// cout << "Writing to cache, time taken was: " << cache_array->write(write_address, 671103640, sizeCache) << "\n";
	// cout << "Getting memory that was written to\n" << cache_array->search(write_address, sizeCache, &result) << "\n";
	// cout << "Result obtained was: " << result << "\n";

	// LDD R0 0 (Immediate load to R0) - 8427
	/*cache_array->write(write_address, 620756992, sizeCache);
	write_address[1] = 236;

	//LDD R1 1 (Immediate load to R1) -8428
	cache_array->write(write_address, 621805569, sizeCache);
	write_address[1] = 237;

	// LDD R3 [R0] (Load from memory to R3) 
	cache_array->write(write_address, 640679936, sizeCache);

	write_address[1] = 238; 
	// LDD R4 [R1] (Load from memory to R4)
	cache_array->write(write_address, 641794048, sizeCache);

	write_address[1] = 239; 

	// ADD R3 R4 R5 (ADD R3 AND R4 store to R5)
	cache_array->write(write_address, 1714176, sizeCache);

	// B 8427 (unconditional branch)
	001000001111000010000001000000000010000011101011
	
	// cout << "THIS IS THE NEW INSTURCTION:::: " << bigDaddy[7][0] << bigDaddy[7][1] << bigDaddy[7][2] << "\n";
	for(int i = 0; i <= 10; ++i)
		cout << bigDaddy[i][0] << " " << bigDaddy[i][1] << " " << bigDaddy[i][2] << "\n";

	run_pipeline(cache_array, sizeCache);
	result = 0;
	int address[3]{0,6,0};
	cache_array->search(address, &result);
	for(int i = 0; i <= 10; ++i)
		cout << bigDaddy[i][0] << " " << bigDaddy[i][1] << " " << bigDaddy[i][2] << "\n";cout << "The value at address 6 is: " << result << "\n";
}	*/
