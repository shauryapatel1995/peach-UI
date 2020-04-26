/* File foo.  */
#ifndef FILE_PIPELINEHELPER_SEEN
#define FILE_PIPELINEHELPER_SEEN

#include <set>

struct Instruction
{
    int instruction_type;
    int opcode;
    int condition_code;
    int addressing_mode;
    int immediate;
    vector<int> operands;
    int result;
    int memory_access_address;
    int write_back_register;
    int write_back_memory;
    int isNoop;
    int condition_bits;
};

typedef Instruction Instruction;

struct Pipeline
{
    Cache* cache;
    Memory* memory;
    int enable_cache; 
    int cache_size;
    int pre_decode_instruction;
    int decode_instruction;
    int isDecodeNoop;
    int single_instruction;
    Instruction *execute_instruction;
    Instruction *memory_access_instruction;
    Instruction *write_back_instruction;

    // Booleans to control the stages
    int continue_fetch;
    int continue_decode;
    int continue_execute;
    int continue_memory_access;
    int continue_write_back;

    // Booleans to control the stages

    // Cycle wait times for each stage
    int fetch_wait_time;
    int decode_wait_time;
    int execute_wait_time;
    int memory_access_wait_time;
    int write_back_wait_time;

    int program_counter = 8427;
    int last_instruction;
    // Set condition register statically for now
    int condition_register = 0;

    int condition_being_written = 0;

    // Variable to store the total cycles of a program
    int total_cycles = 0;
    
    int stored_fetch_result = -1;

    Instruction *stored_memory_instruction;

    int decode_instructions[1] = {-1};

    Instruction *noop = new Instruction();
    
    Instruction *execute_instructions[1] = {noop};
    Instruction *memory_access_instructions[1] = {noop};
    Instruction *write_back_instructions[1] = {noop};

    bool write_back_completed = false;
    int squash_instructions = 0;

    // Set to store the registers which will be written to in the future
    set<int> register_in_flight;

    // Map of register bank, initialize to zeros
    unordered_map<int, int> register_bank;

    // Flag to indicate data hazard.
    int data_hazard = 0;
};

typedef Pipeline Pipeline; 

#endif