#include <iostream>
#include <string>
#include <vector>
#include "peach-1/mainMemory.h"
#include "peach-1/pipeline-helper-new.h"

extern int readMemory(int**, int, std::string);
extern int run_pipeline(Cache*, int, int, Pipeline*);

std::string fileToDisplay;

Pipeline* pipeline = nullptr;

int **bigDaddy = nullptr;
int **lilDaddy = nullptr;
MainMemory * mainMem_array = nullptr;
Cache * cache_array;

std::string pipelineInfo = "";

int total_cycles;


std::string previous_file_name = "";

void reset() {
    cout << "Resetting values!\n";
    pipeline = nullptr; 
    bigDaddy = nullptr; 
    total_cycles = 0;
    lilDaddy = nullptr; 
    previous_file_name = "";
}

std::string show_file(std::string fileName) {
    std::cout << "File to open is: " << fileName << "\n";
    if(previous_file_name == fileName) {
        return fileToDisplay;
    } else {
        previous_file_name = fileName;
    }
    std::cout << "Show file called!";
    fileToDisplay = "";
    fstream file;
    file.open("./peach-1/assembler/" + fileName + ".txt",ios::in);
    std::string line; 

    while(getline(file, line)){
        fileToDisplay += line + "<br/>";
    }
    if(fileToDisplay == "") 
        return "Program not implemented yet";

    return fileToDisplay;
    
}
std::string getTotalCycles() {
    return std::to_string(total_cycles);
}

std::string show_cache_values() {
    std::basic_string<char> answer = "";
    std::basic_string<char> br  = "<br/>";
        for(int i = 0; i <= 10; i++)
            answer += "<tr><td>" + std::to_string(lilDaddy[i][0]) + "</td><td>" + std::to_string(lilDaddy[i][1]) + "</td><td> " + std::to_string(lilDaddy[i][2]) + "</td></tr>";

        std::cout << "Cache returned ";
    
    return answer;
}

std::string show_register_bank_values() {
    std::string info = "";
    if(pipeline != nullptr) {
        
    for(auto reg: pipeline->register_bank) {
        info += "<tr><td>";
        info += std::to_string(reg.first);
        info += "</td><td>";
        info += std::to_string(reg.second);
        info += "</td></tr>";
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

std::string runPipeline(int val, std::string fileName) {
    // 2-D array for cache 
        std::basic_string<char> answer = "";
        if(bigDaddy == nullptr || previous_file_name != fileName) {
        total_cycles = 0;
        pipeline = nullptr;
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
        readMemory(bigDaddy, sizeMain, fileName);
        std::cout << (bigDaddy, sizeMain) << " Answer!";
        
        std::basic_string<char> br  = "<br/>";
        for(int i = 0; i <= 40; i++)
            answer += "<tr><td>" + std::to_string(bigDaddy[i][0]) + "</td><td>" + std::to_string(bigDaddy[i][1]) + "</td><td> " + std::to_string(bigDaddy[i][2]) + "</td></tr>";

        std::cout << "Main mem returned returned ";

        
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

  } else {
            
        for(int i = 0; i <= 40; i++)
            answer +=  "<tr><td>" + std::to_string(bigDaddy[i][0]) + "</td><td>" + std::to_string(bigDaddy[i][1]) + "</td><td> "+ std::to_string(bigDaddy[i][2]) + "</td></tr>";

        std::cout << "Main mem returned ";
        }
    return answer;
}

void run_pipeline_real(int cycles, std::string config) {

    if(pipeline == nullptr) {
        pipeline = new Pipeline();
        pipeline->cache = cache_array;
        pipeline->memory = mainMem_array;
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
        if(previous_file_name == "matrix") 
            pipeline->last_instruction = 8450;
        else if(previous_file_name == "sort")
            pipeline->last_instruction = 8450; 
        else if(previous_file_name == "loop")
            pipeline->last_instruction = 8438;
        pipeline->register_bank.insert(pair<int, int>(6,0));

        cout << "Config is " << config;
        if(config == "cp") {
            cout << "Both pipeline and cache working right now!";
            pipeline->single_instruction = 0;
            pipeline->enable_cache = 1;
        } else if(config == "p") {
            // Disable cache
            pipeline->enable_cache = 0;
        } else if(config == "c") {
            // Disable pipeline
            pipeline->single_instruction = 1;
            pipeline->enable_cache = 1;
        }  else {
            pipeline->single_instruction = 1;
            pipeline->enable_cache = 0;
        }
        
    }

    std::cout << "Running pipeline! Cycles are: " << cycles << std::endl;
    
    int result = 0;
   
	int count = 0;
    while(count < cycles) {
        total_cycles += run_pipeline(cache_array, 1024, 1, pipeline);
        pipelineInfo += get_pipeline_info();
        //std::cout << pipelineInfo;
        if(pipeline->write_back_stopped)
            return;
        count++;
    }
    
}
