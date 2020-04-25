#include <iostream>
#include <string>
#include <vector>
#include "peach-1/mainMemory.h"
#include "peach-1/pipeline-helper-new.h"

extern int readMemory(int**, int);
extern void run_pipeline(Cache*, int, int, Pipeline*);


Pipeline* pipeline = nullptr;

int **bigDaddy = nullptr;
int **lilDaddy = nullptr;
MainMemory * mainMem_array = nullptr;
Cache * cache_array;

std::string pipelineInfo = "";
std::string show_cache_values() {
    std::basic_string<char> answer = "";
    std::basic_string<char> br  = "<br/>";
        for(int i = 0; i <= 10; i++)
            answer += std::to_string(lilDaddy[i][0])  + std::to_string(lilDaddy[i][1]) + std::to_string(lilDaddy[i][2]) + br;

        std::cout << "Cache returned " << answer;
    
    return answer;
}

std::string show_register_bank_values() {
    std::string info = "";
    if(pipeline != nullptr) {
        
    for(auto reg: pipeline->register_bank) {
        info += std::to_string(reg.first);
        info += " : ";
        info += std::to_string(reg.second);
        info += "<br/>";
    }
    }
    return info;
}

std::string getProg() {
    if (pipeline != nullptr)
        return to_string(pipeline->program_counter);
    else
        return "0";
    
    
}

std::string get_total_pipeline_info() {
    std::string ans = pipelineInfo;
    pipelineInfo = "";
    return ans;
}

std::string get_pipeline_info() {
    std::string info = "";
    std::cout << "Getting pipeline info!" << std::endl;
    
    info += "<tr>";
    info += "<td>";
    info += to_string(pipeline->decode_instruction);
    info += "</td>";
    
    info += "<td>";
    int ins1 = pipeline->execute_instruction->instruction_type;
    info += to_string(ins1);
    info += "</td>";

    info += "<td>";
    int ins2 = pipeline->memory_access_instruction->instruction_type;
    info += to_string(ins2);
    info += "</td>";

    info += "<td>";
    int ins3 =  pipeline->write_back_instruction->instruction_type;
    info += to_string(ins3);
    info += "</td>";
    info += "</tr>";
    
    return info;
}

std::string runPipeline(int val) {
    // 2-D array for cache 
        std::basic_string<char> answer = "";
        if(bigDaddy == nullptr) {
            
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
        std::cout << "Initializing memory!";
        // initializing main memory with the text file 
        readMemory(bigDaddy, sizeMain);
        std::cout << (bigDaddy, sizeMain) << " Answer!";
        
        std::basic_string<char> br  = "<br/>";
        for(int i = 0; i <= 40; i++)
            answer += std::to_string(bigDaddy[i][0])  + std::to_string(bigDaddy[i][1]) + std::to_string(bigDaddy[i][2]) + br;

        std::cout << "String returned " << answer;

        
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
     mainMem_array = new MainMemory(bigDaddy, nullptr, lilDaddy, 10, "main", sizeMain);
	 cache_array = new Cache(lilDaddy,mainMem_array, bigDaddy, 1, "cache", sizeCache);

    
    desiredAddress[0] = 8;
	desiredAddress[1] = 242;
	desiredAddress[2] = -1;
	//printing the desired address
	cout << "This is the desired address: " << desiredAddress[0] << desiredAddress[1] << desiredAddress[2] <<"\n";
    
    cache_array->search(desiredAddress, &result);
	cout << "Result obtained was after searching: " << result << "\n";
        } else {
            
        std::basic_string<char> br  = "<br/>";
        for(int i = 0; i <= 40; i++)
            answer += std::to_string(bigDaddy[i][0])  + std::to_string(bigDaddy[i][1]) + std::to_string(bigDaddy[i][2]) + br;

        std::cout << "String returned " << answer;
        }
    return answer;
}

void run_pipeline_real(int cycles) {

    if(pipeline == nullptr) {
        pipeline = new Pipeline();
        pipeline->cache = cache_array;
        pipeline->continue_fetch = 1;
        pipeline->continue_decode = 1;
        pipeline->continue_execute = 1;
        pipeline->continue_memory_access = 1;
        pipeline->continue_write_back = 1;
        Instruction* noop = new Instruction();
        noop->isNoop = 1;
        pipeline->execute_instructions[0] = noop;
        pipeline->write_back_instructions[0] = noop;
        pipeline->memory_access_instructions[0] = noop;
    }

    std::cout << "Running pipeline! Cycles are: " << cycles << std::endl;
    
    int result = 0;
        // declaring the address we will be searching for in memory 
    int desiredAddress[3]{0};
    desiredAddress[0] = 8;
	desiredAddress[1] = 242;
	desiredAddress[2] = -1;
	//printing the desired address
	cout << "This is the desired address: " << desiredAddress[0] << desiredAddress[1] << desiredAddress[2] <<"\n";
    
    cache_array->search(desiredAddress, &result);
	cout << "Result obtained was after searching: " << result << "\n";
    int count = 0;
    while(count < cycles) {
        run_pipeline(cache_array, 1024, 1, pipeline);
        pipelineInfo += get_pipeline_info();
        //std::cout << pipelineInfo;
        count++;
    }
    
}
