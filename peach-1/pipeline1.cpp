#include <iostream>
#include <bitset>
#include "mainMemory.h"
#include <unordered_map>
#include <vector>
#include "pipeline-helper.h"
#include <deque>
#include <set>

using namespace std;


// Implement the sort
// TODO: Create a Pipeline object that contains the stall and the instruction object for all stages.
// TODO: Handle a data hazard. 
// TODO: If a conditional branch is taken, mark instructions behind it as squashed (Conditional hazard)
// TODO: Making conditional and other registers a part of general register groups make it easier to check for data hazards. 

// Let's make this start at 0
// This currently stands for tag 8 and index 235.
int program_counter = 8427;

// Set condition register statically for now
int condition_register = 0;

int condition_being_written = 0;

// Variable to store the total cycles of a program
int total_cycles = 0;
Pipeline* pipeline = new Pipeline();
int stored_fetch_result = -1;

Instruction* stored_memory_instruction;

int decode_instructions[1] = {-1};

Instruction* noop = new Instruction();

Instruction * execute_instructions[1] = {noop};
Instruction * memory_access_instructions[1] = {noop};
Instruction * write_back_instructions[1] = {noop};

bool write_back_completed = false;
int squash_instructions = 0;

// Set to store the registers which will be written to in the future
set<int> register_in_flight;

// Map of register bank, initialize to zeros
unordered_map<int, int> register_bank;

// Flag to indicate data hazard. 
int data_hazard = 0; 



void restart_fetch(Pipeline* pipeline) {
    pipeline->continue_fetch = 1;
}

void restart_decode(Pipeline* pipeline) {
    pipeline->continue_decode = 1;
    if(pipeline->decode_wait_time == 0 && !pipeline->single_instruction)
        restart_fetch(pipeline);
}

void restart_execute(Pipeline* pipeline) {
    pipeline->continue_execute = 1;
    if(pipeline->execute_wait_time == 0) 
        restart_decode(pipeline);
}

void restart_memory_access(Pipeline* pipeline) {
    pipeline->continue_memory_access = 1; 
    if(pipeline->memory_access_wait_time == 0)
        restart_execute(pipeline);
}

int get_program_counter(){
    return program_counter;
}

Pipeline* get_pipeline() {
    std::cout << "Returning pipeline object!" << std::endl;
    return pipeline;
}

bool data_hazard_check(int reg) {
    if(register_in_flight.count(reg) > 0) {
        // Stop decode and fetch

        cout << "Stopping fetch and decode for reg: " << reg << " ";
        pipeline->continue_decode = 0;
        pipeline->continue_fetch = 0;
        data_hazard = 1;
        cout << "Data hazard set to " << data_hazard;
        return true;
    } else {
        return false;
    }
}

/*
    Branch instruction decode function. 
    This must flush the pipeline
*/
void branch_decode(Instruction* instruction, string binary_instruction) {
    if(condition_being_written) {
        cout << "Conditional stop! ";
        data_hazard = 1;
        pipeline->continue_decode = 0; 
        pipeline->continue_fetch = 0;
        return;
    }
    instruction->opcode = stoi(binary_instruction.substr(3, 3), nullptr, 2);
    instruction->addressing_mode = stoi(binary_instruction.substr(6, 1), nullptr, 2);
    instruction->immediate = stoi(binary_instruction.substr(7, 1), nullptr, 2);
    int newPC =0;
    if (instruction->opcode == 0)
    {
        cout << "Found unconditional branch";
        if(instruction->immediate == 1)
        {
            if (binary_instruction.substr(16, 16).at(0) == '1')
            {
                // negative number
                int result = stoi(binary_instruction.substr(16, 16), nullptr, 2);
                string result_str = bitset<16>(result).flip().to_string();
                newPC = -(stoi(result_str, nullptr, 2) + 1);
                cout << newPC << "New pc is ";
            }
            else
            {
                newPC = stoi(binary_instruction.substr(8, 24), nullptr, 2);
            }
            program_counter += newPC;
            cout << "Program counter is: " << program_counter;
        } 

    } else {
        instruction->addressing_mode = stoi(binary_instruction.substr(10, 1), nullptr, 2);
        instruction->immediate = stoi(binary_instruction.substr(11, 1), nullptr, 2);
        // BL
        // This instruction will be register direct. Which means PC + reg
        if(instruction->immediate == 0) {

            int reg = stoi(binary_instruction.substr(12, 4), nullptr, 2);

            // Reg that contains where to go
            instruction->operands.push_back(register_bank.at(reg));
            instruction->condition_bits = stoi(binary_instruction.substr(6, 4), nullptr, 2);
        } else {
            cout << "Immediate jump value is: " << stoi(binary_instruction.substr(16, 16), nullptr, 2);
            if (binary_instruction.substr(16, 16).at(0) == '1') {
                // negative number
                int result = stoi(binary_instruction.substr(16, 16), nullptr, 2);
                string result_str = bitset<16>(result).flip().to_string();
                newPC = -(stoi(result_str, nullptr, 2) + 1);
                instruction->operands.push_back(newPC);
                instruction->condition_bits = stoi(binary_instruction.substr(6, 4), nullptr, 2);
                
            }
            else
            {
                newPC = stoi(binary_instruction.substr(16, 16), nullptr, 2);
                instruction->operands.push_back(newPC);
                instruction->condition_bits = stoi(binary_instruction.substr(6, 4), nullptr, 2);
            }
        }
    
    }
    
    return;
}

/*
    ALU decode, can lead to data hazards. 
*/
void alu_decode(Instruction *instruction, string binary_instruction) {
    
    instruction->opcode = stoi(binary_instruction.substr(3, 4), nullptr, 2);
    instruction->addressing_mode = stoi(binary_instruction.substr(7, 1), nullptr, 2);
    instruction->immediate = stoi(binary_instruction.substr(8, 1), nullptr, 2);

    if(instruction->opcode == 13)
        condition_being_written = 1;

       if (instruction->immediate != 1)
        {
            int reg1 = stoi(binary_instruction.substr(9, 4), nullptr, 2);
            int reg2 = stoi(binary_instruction.substr(13, 4), nullptr, 2);

            // Check for data hazards
            if (data_hazard_check(reg1) || data_hazard_check(reg2))
            {
                return;
            }
            int operand1 = register_bank.at(reg1);
            instruction->operands.push_back(operand1);

            int operand2 = register_bank.at(reg2);
            instruction->operands.push_back(operand2);

            int reg3 = stoi(binary_instruction.substr(17, 4), nullptr, 2);

            // Let's keep the write back semantics like this for now
            // This will push the register to infligth values
            instruction->write_back_register = reg3;
            cout << "Entering to inflight: " << instruction->write_back_register << " ";
            register_in_flight.insert(instruction->write_back_register);
        }
    else
    {
        // Immediate add
        int operand1 = stoi(binary_instruction.substr(16,16), nullptr, 2);
        instruction->operands.push_back(operand1);
        int operand2 = register_bank.at(stoi(binary_instruction.substr(9, 4), nullptr, 2));
        cout << "Operand in immediate is: " << operand1 << " ";
        instruction->operands.push_back(operand2);
        instruction->write_back_register = stoi(binary_instruction.substr(9, 4), nullptr, 2);
        cout << "Entering to inflight: " << instruction->write_back_register << " ";
        register_in_flight.insert(instruction->write_back_register);
        cout << "Found the write back register!" << instruction->write_back_register << " ";
    }
}

/*
    Decode for Load and store instructions
    Can also lead to data hazards
*/
void load_store_decode(Instruction *instruction, string binary_instruction)
{
    cout << "Encountered load or store\n";
    instruction->opcode = stoi(binary_instruction.substr(3, 3), nullptr, 2);
    instruction->addressing_mode = stoi(binary_instruction.substr(6, 1), nullptr, 2);
    instruction->immediate = stoi(binary_instruction.substr(7, 1), nullptr, 2);
    
    if (instruction->opcode == 1) {
        // Load instruction with immediate
        if (instruction->immediate == 1) {
            
            instruction->write_back_register = stoi(binary_instruction.substr(8, 4), nullptr, 2);
            cout << "Entering to inflight: " << instruction->write_back_register << " ";
            register_in_flight.insert(instruction->write_back_register);
            
            // Immediate value to be stored to register
            if (binary_instruction.substr(16, 16).at(0) == '1') {
                // negative number
                int result = stoi(binary_instruction.substr(16, 16), nullptr, 2);
                string result_str = bitset<16>(result).flip().to_string();
                instruction->result = -(stoi(result_str, nullptr, 2) + 1);
            } else {
                instruction->result = stoi(binary_instruction.substr(16, 16), nullptr, 2);
            }
            
            
            cout << "Result at load store decode is: " << instruction->result << "\n";
        } else {
            
            instruction->write_back_register = stoi(binary_instruction.substr(8, 4), nullptr, 2);
            cout << "Entering to inflight: " << instruction->write_back_register << " ";
            register_in_flight.insert(instruction->write_back_register);
            
            // Location to access memory bank
            int memory_access_address = stoi(binary_instruction.substr(12, 4), nullptr, 2);
            if(data_hazard_check(memory_access_address))
                return; 

            instruction->memory_access_address = register_bank.at(memory_access_address);
        }
    } else if(instruction->opcode == 2) {
        cout << "This is a STR instruction " << std::endl;
        int write_back_reg = stoi(binary_instruction.substr(8, 4), nullptr, 2);
        
        cout << "Reg 1 is : " << write_back_reg << std::endl;
        // Location to access memory bank
        int memory_access_address = stoi(binary_instruction.substr(12, 4), nullptr, 2);
        
        if (data_hazard_check(write_back_reg) || data_hazard_check(memory_access_address))
             return;

        instruction->result = register_bank.at(write_back_reg);
        cout << "Result is: " << instruction->result << std::endl;
        instruction->write_back_memory = register_bank.at(memory_access_address);
        cout << "Write back memory is: " << instruction->write_back_memory << std::endl;
    }
}

/*  
    Reads the instruction from main memory based on the PC. 
*/
int fetch(Cache* cache, int cache_size) {
    cout << "Fetch: ";
    write_back_completed = false;
    if(pipeline->fetch_wait_time != 0) {
        cout << "Executing instruction " << pipeline->fetch_wait_time;
        pipeline->fetch_wait_time--; 

        // Means NOOP
        if(pipeline->fetch_wait_time == 0) {
            // Push the stored value in this
            decode_instructions[0] = stored_fetch_result;
            cout << "Putting to decode! " << stored_fetch_result << " ";
            program_counter++;
        } else {
            if (pipeline->continue_decode) {
                decode_instructions[0] = -1;
                cout << "Putting NOOP! HAHA ";
            }
                
        }
        
    } else if(pipeline->continue_fetch) {
        cout << "PROGRAM COUNTER IS: " << program_counter << " ";
        // Convert program counter to address array
        string address = bitset<16>(program_counter).to_string();
        // cout << "Address is: " << address << "\n";

        // Convert it into tag and index field
        string tagStr = address.substr(0, 6);
        string indexStr = address.substr(6, 10);


        int tag = stoi(tagStr, nullptr, 2);
        int index = stoi(indexStr, nullptr, 2);


        // cout << "Tag in fetch is: " << tag << " Index is: " << index << "\n";
        int next_instruction[3]{tag, index, 0};
        int result = 0;
        pipeline->fetch_wait_time = cache->search(next_instruction, &result);
        
        cout << "Result of fetch stage is: " << result << " ";
        
        pipeline->fetch_wait_time--;

        if(pipeline->fetch_wait_time != 0) {
            // Store the result for later use somehow. 
            stored_fetch_result = result;

            // TODO: Do I need to check this?
            if(pipeline->continue_decode)
                decode_instructions[0] = -1;
        } else {
            decode_instructions[0] = result;
            program_counter++;
        }
        if(pipeline->single_instruction)
            pipeline->continue_fetch = 0;
    } else {
        cout << "Stalled!! ";
    }
    
}

/*
    Decode the current instruction string and convert it to instruction object. 
*/
int decode() {
    
    // TODO - For now we will just decode the instructions and get the register values in operands. 
    // TODO - We will only use the 3 register arch for now for ALU. 
    cout << "Decode: " << std::endl;

    

    if(pipeline->decode_wait_time != 0) {
        cout << "Executing "; 
        
        pipeline->decode_wait_time--; 
        
        // Pass noop to the next one. 
        
        if(pipeline->decode_wait_time == 0 && pipeline->continue_decode == 1)
            restart_fetch(pipeline);

    } else if(pipeline->continue_decode) {
        //NOOP
        if (data_hazard)
            data_hazard = 0;

        int decode_instruction = pipeline->decode_instruction;
        
        if (decode_instruction == -1) {
            cout << "NOOP! ";
            Instruction* noop = new Instruction();
            noop->isNoop = 1;
            execute_instructions[0] = noop;
            return 0;
        }
        
        string binary_instruction = bitset<32>(decode_instruction).to_string();
        cout << "Instruction is: " << binary_instruction << " ";
        Instruction *instruction = new Instruction();
        instruction->instruction_type = stoi(binary_instruction.substr(0, 3), nullptr, 2);

        // ALU instruction decoding
        if (instruction->instruction_type == 0)
            alu_decode(instruction, binary_instruction);
        else if (instruction->instruction_type == 1)
            load_store_decode(instruction, binary_instruction);
        else if (instruction->instruction_type == 4)
            branch_decode(instruction, binary_instruction);
        else
            cout << "Wrong instruction encountered!\n";

        if(data_hazard)
            return 0;
        // cout << "Instruction type is: " << instruction->instruction_type << " Opcode is: " << instruction->opcode << "\n";
        // cout << "Addressing mode is: " << instruction->addressing_mode << " Immediate bit is: " << instruction->immediate << "\n";

        execute_instructions[0] =  instruction;
        
    } else {
        cout << "Stalled!! ";
    }

}

/*
    Execute takes in an instruction object and does the execution
*/
int execute(){

    // This will be more complex. 
    // What should come into this? ALU operations?
    // PC + Indirect memory indexing etc. 
    // It performs all the operations related to memory
    cout << "Execute: " << std::endl;
    if(pipeline->execute_wait_time != 0) {
        cout << "Executing " << std::endl;
        pipeline->execute_wait_time--;
        pipeline->memory_access_instruction->isNoop = 1;
        if(pipeline->execute_wait_time == 0 && pipeline->continue_execute == 1) 
            restart_decode(pipeline);
    } else if(pipeline->continue_execute) {

        //NOOP
        
        // ALU
        Instruction *execute_instruction = pipeline->execute_instruction;
        if (execute_instruction->isNoop) {
            cout << "NOOP ";
            memory_access_instructions[0] = execute_instruction;
            return 0;
        }

        cout << "Executing instruction: " << execute_instruction->instruction_type << std::endl;
            if (execute_instruction->instruction_type == 0)
            {
                if (execute_instruction->opcode == 0)
                {
                    // Add
                    execute_instruction->result = execute_instruction->operands.at(0) + execute_instruction->operands.at(1);
                    cout << "Result is: " << execute_instruction->result << " ";
                } else if(execute_instruction->opcode == 13) {
                    // CMP instruction 
                    execute_instruction->result = execute_instruction->operands.at(0) - execute_instruction->operands.at(1);
                    cout << "Result is " << execute_instruction->result << " ";
                }
            } else if(execute_instruction->instruction_type == 4 && execute_instruction->opcode != 0) {
                // Branch instruction calculate the new program_counter, check condition and set PC 
                cout << "Received branch instruction type: " << execute_instruction->condition_code << " ";
                if(execute_instruction->condition_bits == 2) {
                    cout << "Entering less than! " << condition_register << " <- condition register! ";
                    // Check less than
                    string condition_string = bitset<16>(condition_register).to_string();
                    if(condition_string.at(0) == '1') {
                        // CMP was less than 
                        program_counter += execute_instruction->operands.at(0);
                        squash_instructions = 1;
                        cout << "Program counter now is: " << program_counter;
                    }
                } else if(execute_instruction->condition_bits == 0) {
                    // Branch if equal
                    if(condition_register == 0) {
                        cout << "BRANCHING!";
                        cout << "Check equality is correct! ";
                        program_counter += execute_instruction->operands.at(0);
                        squash_instructions = 1;
                        cout << "Program counter now is: " << program_counter;
                    }
                } else {}
            }
            else
            {
                // For now do nothing
            }
        
        if(pipeline->execute_wait_time != 0) {
            
            pipeline->execute_wait_time--;
        }
            
        
        if (pipeline->execute_wait_time != 0) {
            pipeline->continue_fetch = 0;
            pipeline->continue_decode = 0;
            
        } else if (pipeline->continue_decode == 0) {
            restart_decode(pipeline);
        }
        else {}
        memory_access_instructions[0] = execute_instruction;
    } else {
        cout << "Stalled!! ";
    }

    
}

/*
    Memory access stage.
*/
int memory_access() {
    cout << "Memory: " << std::endl;
    // Pretty straightforward because LDR and STR go here
    // Call cache-> search and store the result in the instruction object
    
    if(pipeline->memory_access_wait_time != 0) {
        cout << "Executing ";
        Instruction* noop = new Instruction();
        noop->isNoop = 1;
        write_back_instructions[0] = noop;
        pipeline->memory_access_wait_time--;
        cout << "Reducing the time: " << pipeline->memory_access_wait_time << " ";
        if(pipeline->memory_access_wait_time == 0) {
            cout << "Restarting pipeline ";
            write_back_instructions[0] = stored_memory_instruction;
            restart_memory_access(pipeline);
        }
            
    } else if(pipeline->continue_memory_access) {

        // Just putting write back for now
        // Next instruction should be stored for now
        Instruction *memory_access_instruction = pipeline->memory_access_instruction;
        if(memory_access_instruction->isNoop) {
            write_back_instructions[0] = memory_access_instruction;
            cout << "NOOP ";
            return 1;
        }
        if(memory_access_instruction->instruction_type == 1 && memory_access_instruction->opcode == 1 && memory_access_instruction->immediate == 0) {
            // This is a load instruction with a register. 
            // Call search on cache
            int result = 0;
            string address = bitset<16>(memory_access_instruction->memory_access_address).to_string();
            // cout << "Address is: " << address << "\n";

            // Convert it into tag and index field
            string tagStr = address.substr(0, 6);
            string indexStr = address.substr(6, 10);

            int tag = stoi(tagStr, nullptr, 2);
            int index = stoi(indexStr, nullptr, 2);

            // cout << "Tag in fetch is: " << tag << " Index is: " << index << "\n";
            int memory_address[3]{tag, index, 0};
            cout << "Going to search!\n";
            pipeline->memory_access_wait_time = pipeline->cache->search(memory_address, &result);
            memory_access_instruction->result = result;
        } else if(memory_access_instruction->instruction_type == 1 && memory_access_instruction->opcode == 2) {
            // Store instruction with data in register
            cout << "Storing to memory!" << std::endl;
            string address = bitset<16>(memory_access_instruction->write_back_memory).to_string();

            // Convert it into tag and index field
            string tagStr = address.substr(0, 6);
            string indexStr = address.substr(6, 10);

            int tag = stoi(tagStr, nullptr, 2);
            int index = stoi(indexStr, nullptr, 2);

            int memory_address[3]{tag, index, 0};
            cout << "Going to write ";
            cout << "Writing to tag " << tag << " " << index << " " << memory_access_instruction->result << " ";
            pipeline->memory_access_wait_time = pipeline->cache->write(memory_address, memory_access_instruction->result);
        } else {}
        
        
        
        if(pipeline->memory_access_wait_time != 0)
            pipeline->memory_access_wait_time--;
        
        // Stall the pipeline behind it
        if(pipeline->memory_access_wait_time != 0) {
            stored_memory_instruction = memory_access_instruction;
            pipeline->continue_fetch = 0;
            pipeline->continue_decode = 0;
            pipeline->continue_execute = 0;
            pipeline->continue_memory_access = 0;
            write_back_instructions[0] = noop;
        } else if(pipeline->continue_execute == 0) {
            restart_memory_access(pipeline);
        } else {
            write_back_instructions[0] = memory_access_instruction;
        }
        
    } else {
        cout << "Stalled!! ";
    }
    return 0;
}

/*
    Write back stage
*/
int write_back() {
    // Writes result back to a register based on ALU or load operations - this should be 1 cycle.
    // Store result back to the register value to be stored to
    cout << "Write Back: ";
    if(pipeline->write_back_wait_time != 0) {
        cout << "Executing ";
        pipeline->write_back_wait_time--;
        // This is obviously wrong because it shouldn't restart all stages. 
        if (pipeline->write_back_wait_time == 0) {
            restart_memory_access(pipeline);
        }
    
    } else if(pipeline->continue_write_back) {
        if(pipeline->write_back_instruction->isNoop) {
            cout << "NOOP ";
            return 0;
        }
        Instruction *write_back_instruction = pipeline->write_back_instruction;

        // Still needs to be done 
        if(write_back_instruction->instruction_type == 0 && write_back_instruction->opcode == 13) {
            // CMP instruction write to control register 
            condition_register = write_back_instruction->result;
            cout << "Condition register is: " << condition_register << "\n";
            cout << "Writing to condition register: " << std::bitset<16>(condition_register).to_string() << " ";

            if(condition_being_written) {
                condition_being_written = 0;
                cout << "Changing condition variable! ";
                if(data_hazard) {
                    restart_decode(pipeline);
                }
            }        

        } else if(write_back_instruction->instruction_type == 0 || write_back_instruction->instruction_type == 1) {

            if (register_bank.count(write_back_instruction->write_back_register) > 0)
                register_bank[write_back_instruction->write_back_register] = write_back_instruction->result;
            else
                register_bank.insert(pair<int, int>(write_back_instruction->write_back_register, write_back_instruction->result));
        }
        // Check the register bank now
        cout << "Value of register at: " << write_back_instruction->write_back_register << " is: " << register_bank.at(write_back_instruction->write_back_register) << "\n";
        

        //Set time taken to 3 cycles and make the pipe wait till then
        
        //pipeline->continue_memory_access = 0;
        //pipeline->continue_write_back = 0;
        // Stall all previous things
        if(pipeline->write_back_wait_time != 0)
            pipeline->write_back_wait_time--;
        
        if(pipeline->write_back_wait_time != 0) {
            pipeline->continue_fetch = 0;
            pipeline->continue_decode = 0;
            pipeline->continue_execute = 0;
            pipeline->continue_memory_access = 0;

        } else if(pipeline->continue_memory_access == 0) {
            restart_memory_access(pipeline);
        } else {}
        if(pipeline->single_instruction)
            pipeline->continue_fetch = 1;

        // if(write_back_instruction->instruction_type == 1 && write_back_instruction->opcode == 2)
        //     write_back_completed = true;
        cout << "Removing from pipeline " << write_back_instruction->write_back_register << "\n";
        register_in_flight.erase(write_back_instruction->write_back_register);
        cout << "Data hazard set? " << data_hazard << "\n";
        if(data_hazard == 1)
            restart_decode(pipeline);
    } else {
        cout << "Stalled\n";
    }
    return 0;
}

void run_pipeline(Cache* cache_array, int sizeCache, int cycles) {
    pipeline->continue_fetch = 1;
    pipeline->continue_decode = 1;
    pipeline->continue_execute = 1;
    pipeline->continue_memory_access = 1;
    pipeline->continue_write_back = 1;
    pipeline->cache = cache_array;
    pipeline->cache_size = sizeCache;
    pipeline->single_instruction = 0;
    noop->isNoop = 1;
    decode_instructions[0] = -1;
    int cyclesComp = 0;
    // decode_instructions.push_back(-1);

    //Instruction* noop = new Instruction();
    //noop->isNoop = 1; 

    //execute_instructions.push_back(noop);
    // execute_instructions.push_back(noop);

    //  memory_access_instructions.push_back(noop);
    // memory_access_instructions.push_back(noop);

    //  write_back_instructions.push_back(noop);
    // write_back_instructions.push_back(noop);

    register_bank.insert(pair<int, int>(5,0));
    while(program_counter < 8450 && cyclesComp < cycles) {

        // Increment the current cycle 
        total_cycles++;
        cout << "Cycle " << cycles;

        if (pipeline->continue_write_back && pipeline->write_back_wait_time == 0)
        {

            pipeline->write_back_instruction = write_back_instructions[0];
            Instruction *noop = new Instruction();
            noop->isNoop = 1;
            write_back_instructions[0] = noop;
        }

        write_back();

        if (pipeline->continue_memory_access && pipeline->memory_access_wait_time == 0)
        {

            pipeline->memory_access_instruction = memory_access_instructions[0];
            
            Instruction *noop = new Instruction();
            noop->isNoop = 1;
            memory_access_instructions[0] = noop;
        }

        memory_access();

        if (pipeline->continue_execute && pipeline->execute_wait_time == 0)
        {

            pipeline->execute_instruction = execute_instructions[0];
            

            Instruction *noop = new Instruction();
            noop->isNoop = 1;
            execute_instructions[0] = noop;
        }

        execute();

        if (pipeline->continue_decode && pipeline->decode_wait_time == 0 && !data_hazard)
        {
            pipeline->decode_instruction = decode_instructions[0];
            decode_instructions[0] = -1;
        }

        decode();

        
        if(program_counter < 8448)
            fetch(cache_array, sizeCache);

        // Here update the pipeline object with the instruction for the next cycle. 
        // We will keep a buffer for each 1 and use the buffer to decide the next object.

        // std::this_thread::sleep_for(0.05s);

        cout << "Cache values are: "; 
        cyclesComp++;
        
    }
    cout << "Cycles taken are: " << total_cycles << "\n";
    
    
    return;
    
}

/*int main() {
    cout << "This is the pipeline";
}*/