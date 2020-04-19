/* File foo.  */
#ifndef FILE_PIPELINEHELPER_SEEN
#define FILE_PIPELINEHELPER_SEEN
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

};

typedef Pipeline Pipeline; 

#endif