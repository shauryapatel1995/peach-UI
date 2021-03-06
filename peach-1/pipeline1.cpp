#include <iostream>
#include <bitset>
#include "mainMemory.h"
#include <unordered_map>
#include <vector>
#include "pipeline-helper-new.h"
#include <deque>
#include <set>

using namespace std;

void restart_fetch(Pipeline *pipeline)
{
    pipeline->continue_fetch = 1;
}

void restart_decode(Pipeline *pipeline)
{
    pipeline->continue_decode = 1;
    if (pipeline->decode_wait_time == 0 && !pipeline->single_instruction)
        restart_fetch(pipeline);
}

void restart_execute(Pipeline *pipeline)
{
    pipeline->continue_execute = 1;
    if (pipeline->execute_wait_time == 0)
        restart_decode(pipeline);
}

void restart_memory_access(Pipeline *pipeline)
{
    pipeline->continue_memory_access = 1;
    if (pipeline->memory_access_wait_time == 0)
        restart_execute(pipeline);
}

void convert_to_address(int *instruction, int decimal)
{
    int count = 15;
    int flag = 0;
    
   
    while (decimal != 0)
    {
        instruction[count] = decimal % 2;
        decimal /= 2;
        count--;
    }
    
    return;
}

void convert_to_instruction(int *instruction, int decimal) {
    int count = 31;
    int flag = 0;
    
    if(decimal < 0) {
        flag = 1;
        decimal *= -1;
    }   
        
    while(decimal != 0) {
        instruction[count] = decimal % 2;
        decimal /= 2;
        count--;
    }
    if(flag) {
        for(int i = 0; i <= 31; i++) {
            if(instruction[i] == 1)
                instruction[i] = 0;
            else 
                instruction[i] = 1;
        }
        count = 31;
        while(instruction[count] == 1){
            instruction[count] = 0;
            count--;
        }

        instruction[count] = 1;
    }
    
    return; 
}

void print_array(int* a){
    // cout << "Printing array\n";
    for(int i = 0; i <= 15; i++)
        cout << a[i];
    cout << "\n" << std::endl;
    return;
}

int convert_to_decimal(int* instruction, int start, int end) {
    // cout << "Converting to decimal!\n";
    // cout << start << " " << end;
    int number = 0; 
    int mul = 1; 

    for(int i = end; i >= start; i--) {
        number += instruction[i]*mul;
        mul *= 2;
    } 
    // cout << "Number returned is : " << number << std::endl;
    return number;
}

int convert_negative_to_decimal(int* instruction, int start, int end) {
    int number = 0; 
    int mul = 1;

    
    // 
    for(int i = end; i >= start; i--) {
        if(instruction[i] == 0) {
            number += 1*mul;
        } 
        mul *= 2;
    }
    number += 1;
    return -number;
}

// Implement the sort
// TODO: Create a Pipeline object that contains the stall and the instruction object for all stages.
// TODO: Handle a data hazard. 
// TODO: If a conditional branch is taken, mark instructions behind it as squashed (Conditional hazard)
// TODO: Making conditional and other registers a part of general register groups make it easier to check for data hazards. 

// Let's make this start at 0
// This currently stands for tag 8 and index 235.


bool data_hazard_check(int reg, Pipeline* pipeline) {
    if(pipeline->register_in_flight.count(reg) > 0) {
        // Stop decode and fetch

        // cout << "Stopping fetch and decode for reg: " << reg << " ";
        pipeline->continue_decode = 0;
        pipeline->continue_fetch = 0;
        pipeline->data_hazard = 1;
        // cout << "Data hazard set to " << pipeline->data_hazard;
        return true;
    } else {
        return false;
    }
}

/*
    Branch instruction decode function. 
    This must flush the pipeline
*/
void branch_decode(Instruction* instruction, int* binary_instruction, Pipeline* pipeline) {
    if(pipeline->condition_being_written) {
        // cout << "Conditional stop! ";
        pipeline->data_hazard = 1;
        pipeline->continue_decode = 0; 
        pipeline->continue_fetch = 0;
        return;
    }
    // cout << "Branch decode!\n";
    instruction->opcode = convert_to_decimal(binary_instruction, 3, 5);
    instruction->addressing_mode = convert_to_decimal(binary_instruction, 6, 6);
    instruction->immediate = convert_to_decimal(binary_instruction, 7, 7);
    int newPC =0;
    if (instruction->opcode == 0)
    {
        // cout << "Found unconditional branch";
        if(instruction->immediate == 1)
        {
            if (binary_instruction[16] == 1)
            {
                // negative number
                int result = convert_negative_to_decimal(binary_instruction , 16, 31);
                
                newPC = result;
                // cout << newPC << "New pc is ";
            }
            else
            {
                newPC = convert_to_decimal(binary_instruction, 8, 31);
            }
            pipeline->program_counter += newPC;
            // cout << "Program counter is: " << pipeline->program_counter;
        } 

    } else {
        instruction->addressing_mode = convert_to_decimal(binary_instruction, 10, 10);
        instruction->immediate = convert_to_decimal(binary_instruction, 11, 11);
        // BL
        // This instruction will be register direct. Which means PC + reg
        if(instruction->immediate == 0) {

            int reg = convert_to_decimal(binary_instruction, 12, 15);

            // Reg that contains where to go
            instruction->operands.push_back(pipeline->register_bank.at(reg));
            instruction->operands.push_back(pipeline->program_counter);
            instruction->condition_bits = convert_to_decimal(binary_instruction, 6, 9);
        } else {
            // cout << "Immediate jump value is: " << stoi(binary_instruction.substr(16, 16), nullptr, 2);
            if (binary_instruction[16] == 1) {
                // negative number
                int result = convert_negative_to_decimal(binary_instruction , 16, 31);
                newPC = result;
                instruction->operands.push_back(newPC);
                instruction->operands.push_back(pipeline->program_counter);
                instruction->condition_bits = convert_to_decimal(binary_instruction, 6, 9);
                
            }
            else
            {
                newPC = convert_to_decimal(binary_instruction, 16, 31);
                instruction->operands.push_back(newPC);
                instruction->operands.push_back(pipeline->program_counter);
                instruction->condition_bits = convert_to_decimal(binary_instruction, 6, 9);
            }
        }
    
    }
    
    return;
}

/*
    ALU decode, can lead to data hazards. 
*/
void alu_decode(Instruction *instruction, int* binary_instruction, Pipeline* pipeline) {
    
    instruction->opcode = convert_to_decimal(binary_instruction, 3, 6);
    instruction->addressing_mode = convert_to_decimal(binary_instruction, 7, 7);
    instruction->immediate = convert_to_decimal(binary_instruction, 8, 8);
    if (instruction->opcode == 2)
    {
        // cout << "\n\n -- THIS IS THE MUL OPCODE --- \n\n";
    }
    // cout << "ALU decode!\n";
    // opcode == 13 means CMP
    if(instruction->opcode == 13)
        pipeline->condition_being_written = 1;

       if (instruction->immediate != 1)
        {
            int reg1 = convert_to_decimal(binary_instruction, 9, 12);
            int reg2 = convert_to_decimal(binary_instruction, 13, 16);

            // Check for data hazards
            if (data_hazard_check(reg1, pipeline) || data_hazard_check(reg2, pipeline))
            {
                return;
            }
            int operand1 = pipeline->register_bank.at(reg1);
            instruction->operands.push_back(operand1);

            int operand2 = pipeline->register_bank.at(reg2);
            instruction->operands.push_back(operand2);
            
            if(instruction->opcode != 13) {
                int reg3 = convert_to_decimal(binary_instruction, 17, 20);
                instruction->write_back_register = reg3;
                pipeline->register_in_flight.insert(instruction->write_back_register);
            }
                
        
            // Let's keep the write back semantics like this for now
            // This will push the register to infligth values
            
            // cout << "Entering to inflight: " << instruction->write_back_register << " ";
        
        }
    else
    {
        // Immediate add or multiply 
        // TODO - Not checked if it is negative or positive. We can make a single method to extract data from immediate as it is always 16 bits. 
        int operand1 = convert_to_decimal(binary_instruction, 16, 31);
        instruction->operands.push_back(operand1);
        // cout << "Operand in immediate is: " << operand1 << " ";
        
        if (data_hazard_check(convert_to_decimal(binary_instruction, 9, 12), pipeline))
        {
            return;
        }
        int operand2 = pipeline->register_bank.at(convert_to_decimal(binary_instruction, 9, 12));
        
        instruction->operands.push_back(operand2);
        instruction->write_back_register = convert_to_decimal(binary_instruction, 9, 12);
        // cout << "Entering to inflight: " << instruction->write_back_register << " ";
        pipeline->register_in_flight.insert(instruction->write_back_register);
        // cout << "Found the write back register!" << instruction->write_back_register << " ";
    }
}

/*
    Decode for Load and store instructions
    Can also lead to data hazards
*/
void load_store_decode(Instruction *instruction, int* binary_instruction, Pipeline* pipeline)
{
    // cout << "Encountered load or store\n";
    instruction->opcode = convert_to_decimal(binary_instruction, 3, 5);
    instruction->addressing_mode = convert_to_decimal(binary_instruction, 6, 6);
    instruction->immediate = convert_to_decimal(binary_instruction, 7, 7);
    
    if (instruction->opcode == 1) {
        // Load instruction with immediate
        if (instruction->immediate == 1) {
            
            instruction->write_back_register = convert_to_decimal(binary_instruction, 8, 11);
            // cout << "Entering to inflight: " << instruction->write_back_register << " ";
            pipeline->register_in_flight.insert(instruction->write_back_register);
            
            // Immediate value to be stored to register
            if (binary_instruction[16] == 1) {
                // negative number
               instruction->result = convert_negative_to_decimal(binary_instruction , 16, 31);
            } else {
                instruction->result = convert_to_decimal(binary_instruction, 16, 31);
            }
            
            
           //  cout << "Result at load store decode is: " << instruction->result << "\n";
        } else {
            
            instruction->write_back_register = convert_to_decimal(binary_instruction, 8, 11);
            // cout << "Entering to inflight: " << instruction->write_back_register << " ";
            pipeline->register_in_flight.insert(instruction->write_back_register);
            
            // Location to access memory bank
            int memory_access_address = convert_to_decimal(binary_instruction, 12, 15);
            if(data_hazard_check(memory_access_address, pipeline))
                return; 

            instruction->memory_access_address = pipeline->register_bank.at(memory_access_address);
        }
    } else if(instruction->opcode == 2) {
        // cout << "This is a STR instruction " << std::endl;
        int write_back_reg = convert_to_decimal(binary_instruction, 8, 11);
        
        // cout << "Reg 1 is : " << write_back_reg << std::endl;
        // Location to access memory bank
        int memory_access_address = convert_to_decimal(binary_instruction, 12, 15);
        // cout << "Write back is: " << write_back_reg << std::endl;
        if (data_hazard_check(write_back_reg, pipeline) || data_hazard_check(memory_access_address, pipeline))
             return;
        // cout << "Write back is: " << write_back_reg << std::endl;
        instruction->result = pipeline->register_bank.at(write_back_reg);
        // cout << "Result is: " << instruction->result << std::endl;
        instruction->write_back_memory = pipeline->register_bank.at(memory_access_address);
        // cout << "Write back memory is: " << instruction->write_back_memory << std::endl;
    }
}

/*  
    Reads the instruction from main memory based on the PC. 
*/
int fetch(Cache* cache, int cache_size, Pipeline* pipeline) {
    // cout << "Fetch: ";
    pipeline->write_back_completed = false;
    if(pipeline->fetch_wait_time > 0) {
        // cout << "Executing instruction " << pipeline->fetch_wait_time;
        pipeline->fetch_wait_time--; 

        // Means NOOP
        if(pipeline->fetch_wait_time == 0) {
            // Push the stored value in this
            
            if(!pipeline->squash_instructions) {

                pipeline->decode_instructions[0] = pipeline->stored_fetch_result;
                // cout << "Putting to decode! " << pipeline->stored_fetch_result << " ";
                pipeline->program_counter++;
            } else {
                // cout << "\nSQUASH COMPLETED!\n";
                pipeline->squash_instructions = 0;
            }
        } else {
            if (pipeline->continue_decode) {
                pipeline->decode_instructions[0] = -1;
            }
                
        }
        
    } else if(pipeline->continue_fetch) {
        // cout << "PROGRAM COUNTER IS: " << pipeline->program_counter << " ";
        // Convert program counter to address array
        int* address = new int[16]{0};
        convert_to_address(address, pipeline->program_counter);
        // cout << "Address is: " << address << "\n";
        
        int tag = convert_to_decimal(address, 0, 5);
        int index = convert_to_decimal(address, 6, 15);


        // cout << "Tag in fetch is: " << tag << " Index is: " << index << "\n";
        int next_instruction[3]{tag, index, 0};
        int result = 0;
        if(pipeline->enable_cache)
            pipeline->fetch_wait_time = pipeline->instruction_cache->search(next_instruction, &result);
        else 
            pipeline->fetch_wait_time = pipeline->memory->search(next_instruction, &result);

        // cout << "Pipeline fetch wait time is: " << pipeline->fetch_wait_time;
        // cout << "Result of fetch stage is: " << result << " ";
        
        pipeline->fetch_wait_time--;
        
        if(pipeline->fetch_wait_time != 0) {
            // Store the result for later use somehow. 
            pipeline->stored_fetch_result = result;

            // TODO: Do I need to check this?
            if(pipeline->continue_decode)
                pipeline->decode_instructions[0] = -1;
        } else {
            pipeline->decode_instructions[0] = result;
            pipeline->program_counter++;
        }
        
        delete address;  
    } else {
        // cout << "Stalled!! ";
    }
    
}

/*
    Decode the current instruction string and convert it to instruction object. 
*/
int decode(Pipeline* pipeline) {
    
    // TODO - For now we will just decode the instructions and get the register values in operands. 
    // TODO - We will only use the 3 register arch for now for ALU. 
    // cout << "Decode: " << std::endl;

    

    if(pipeline->decode_wait_time != 0) {
        // cout << "Executing "; 
        
        pipeline->decode_wait_time--; 
        
        // Pass noop to the next one. 
        
        if(pipeline->decode_wait_time == 0 && pipeline->continue_decode == 1)
            restart_fetch(pipeline);

    } else if(pipeline->continue_decode) {
        //NOOP
        if (pipeline->data_hazard)
            pipeline->data_hazard = 0;

        int decode_instruction = pipeline->decode_instruction;
        

        if (decode_instruction == -1 || pipeline->squash_instructions) {
            // cout << "NOOP! ";
            return 0;
        }
        
        Instruction *instruction = new Instruction();
        int* binary_instruction = new int[32]{0};
        convert_to_instruction(binary_instruction, decode_instruction);
        
        instruction->instruction_type = convert_to_decimal(binary_instruction, 0, 2);

        // ALU instruction decoding
        if (instruction->instruction_type == 0)
            alu_decode(instruction, binary_instruction, pipeline);
        else if (instruction->instruction_type == 1)
            load_store_decode(instruction, binary_instruction, pipeline);
        else if (instruction->instruction_type == 4)
            branch_decode(instruction, binary_instruction, pipeline);
        else {
            // cout << "Wrong instruction encountered!\n";
        }   
            

        if(pipeline->data_hazard)
            return 0;
        // cout << "Instruction type is: " << instruction->instruction_type << " Opcode is: " << instruction->opcode << "\n";
        // cout << "Addressing mode is: " << instruction->addressing_mode << " Immediate bit is: " << instruction->immediate << "\n";
      
        pipeline->execute_instructions[0] =  instruction;

        delete binary_instruction;
    } else {
        // cout << "Stalled!! ";
    }

}

/*
    Execute takes in an instruction object and does the execution
*/
int execute(Pipeline* pipeline){

    // This will be more complex. 
    // What should come into this? ALU operations?
    // PC + Indirect memory indexing etc. 
    // It performs all the operations related to memory
    // cout << "Execute: " << std::endl;
    if(pipeline->execute_wait_time != 0) {
        // cout << "Executing " << std::endl;
        pipeline->execute_wait_time--;
        pipeline->memory_access_instruction->isNoop = 1;
        if(pipeline->execute_wait_time == 0 && pipeline->continue_execute == 1) 
            restart_decode(pipeline);
    } else if(pipeline->continue_execute) {

        //NOOP
        
        // ALU
        Instruction *execute_instruction = pipeline->execute_instruction;
        if (execute_instruction->isNoop || pipeline->squash_instructions)
        {
            // cout << "NOOP ";
            execute_instruction->isNoop = 1;
            // pipeline->memory_access_instructions[0] = execute_instruction;
            pipeline->memory_access_instructions[0] = pipeline->noop;
            
            return 0;
        }


        // cout << "Executing instruction: " << execute_instruction->instruction_type << std::endl;
            if (execute_instruction->instruction_type == 0)
            {
                if (execute_instruction->opcode == 0)
                {
                    // Add
                    execute_instruction->result = execute_instruction->operands.at(0) + execute_instruction->operands.at(1);
                    // cout << "Result is: " << execute_instruction->result << " ";
                } else if(execute_instruction->opcode == 13) {
                    // CMP instruction 
                    execute_instruction->result = execute_instruction->operands.at(0) - execute_instruction->operands.at(1);
                    // cout << "Result is " << execute_instruction->result << " ";
                } else if (execute_instruction->opcode == 2) {
                    // MUL instruction
                    execute_instruction->result = execute_instruction->operands.at(0) * execute_instruction->operands.at(1);
                    // cout << "\nTHIS IS MULTIPLICATION!!\n\n";
                    // cout << "Result is: " << execute_instruction->result << " ";
                }
            } else if(execute_instruction->instruction_type == 4 && execute_instruction->opcode != 0) {
                // Branch instruction calculate the new program_counter, check condition and set PC 
                // cout << "Received branch instruction type: " << execute_instruction->condition_code << " ";
                if(execute_instruction->condition_bits == 2) {
                    // cout << "Entering less than! " << pipeline->condition_register << " <- condition register! ";
                    // Check less than
                    string condition_string = bitset<16>(pipeline->condition_register).to_string();
                    if(condition_string.at(0) == '1') {
                        // CMP was less than 
                        execute_instruction->result = execute_instruction->operands.at(1) + execute_instruction->operands.at(0);
                        // std::cout << "\nSQUASHING PIPELINE INSTRUCTIONS!\n";
                        pipeline->squash_instructions = 1;
                        // cout << "Program counter now is: " << pipeline->program_counter;
                    }
                } else if(execute_instruction->condition_bits == 0) {
                    // Branch if equal
                    if(pipeline->condition_register == 0) {
                        // cout << "BRANCHING!";
                        // cout << "Check equality is correct! ";
                        execute_instruction->result = execute_instruction->operands.at(1) + execute_instruction->operands.at(0);
                        pipeline->squash_instructions = 1;
                        // cout << "Program counter now is: " << pipeline->program_counter;
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
        
        pipeline->memory_access_instructions[0] = execute_instruction;
    } else {
        // cout << "Stalled!! ";
    }

    
}

/*
    Memory access stage.
*/
int memory_access(Pipeline* pipeline) {
    // cout << "Memory: " << std::endl;
    // Pretty straightforward because LDR and STR go here
    // Call cache-> search and store the result in the instruction object
    
    if(pipeline->memory_access_wait_time != 0) {
        // cout << "Executing ";
        // Instruction* noop = new Instruction();
        // noop->isNoop = 1;
       
        pipeline->write_back_instructions[0] = pipeline->noop;
        pipeline->memory_access_wait_time--;
        // cout << "Reducing the time: " << pipeline->memory_access_wait_time << " ";
        if(pipeline->memory_access_wait_time == 0) {
            // cout << "Restarting pipeline ";
            pipeline->write_back_instructions[0] = pipeline->stored_memory_instruction;
            restart_memory_access(pipeline);
        }
            
    } else if(pipeline->continue_memory_access) {

        // Just putting write back for now
        // Next instruction should be stored for now
        Instruction *memory_access_instruction = pipeline->memory_access_instruction;
        if(memory_access_instruction->isNoop) {
           
            
            //pipeline->write_back_instructions[0] = memory_access_instruction;
            pipeline->write_back_instructions[0] = pipeline->noop;
            // cout << "NOOP ";
            return 1;
        }
        if(memory_access_instruction->instruction_type == 1 && memory_access_instruction->opcode == 1 && memory_access_instruction->immediate == 0) {
            // This is a load instruction with a register. 
            // Call search on cache
            int result = 0;
            int *address = new int[16]{0};
            convert_to_address(address, memory_access_instruction->memory_access_address);
            // cout << "Address is: " << address << "\n";
            

            int tag = convert_to_decimal(address, 0, 5);
            int index = convert_to_decimal(address, 6, 15);

            // cout << "Tag in fetch is: " << tag << " Index is: " << index << "\n";
            int memory_address[3]{tag, index, 0};
            // cout << "Going to search!\n";
            if(pipeline->enable_cache)
                pipeline->memory_access_wait_time = pipeline->cache->search(memory_address, &result);
            else   
                pipeline->memory_access_wait_time = pipeline->memory->search(memory_address, &result);
                
            memory_access_instruction->result = result;
            delete address;
        } else if(memory_access_instruction->instruction_type == 1 && memory_access_instruction->opcode == 2) {
            // Store instruction with data in register
            // cout << "Storing to memory!" << std::endl;
            int *address = new int[16]{0};
            convert_to_address(address, memory_access_instruction->write_back_memory);
            // cout << "Address is: " << address << "\n";
            int tag = convert_to_decimal(address, 0, 5);
            int index = convert_to_decimal(address, 6, 15);

            int memory_address[3]{tag, index, 0};
            // cout << "Going to write ";
            // cout << "Writing to tag " << tag << " " << index << " " << memory_access_instruction->result << " ";
            pipeline->memory_access_wait_time = pipeline->cache->write(memory_address, memory_access_instruction->result);
            delete address;
        } else {}
        
        
        
        if(pipeline->memory_access_wait_time != 0)
            pipeline->memory_access_wait_time--;
        
        // Stall the pipeline behind it
        if(pipeline->memory_access_wait_time != 0) {
            pipeline->stored_memory_instruction = memory_access_instruction;
            pipeline->continue_fetch = 0;
            pipeline->continue_decode = 0;
            pipeline->continue_execute = 0;
            pipeline->continue_memory_access = 0;
            
            // Instruction* noop = new Instruction();
            // noop->isNoop = 1;
            pipeline->write_back_instructions[0] = pipeline->noop;
        } else if(pipeline->continue_execute == 0) {
            restart_memory_access(pipeline);
        } else {
           
            pipeline->write_back_instructions[0] = memory_access_instruction;
        }
        
    } else {
        // cout << "Stalled!! ";
    }
    return 0;
}

/*
    Write back stage
*/
int write_back(Pipeline* pipeline) {
    // Writes result back to a register based on ALU or load operations - this should be 1 cycle.
    // Store result back to the register value to be stored to
    // cout << "Write Back: ";
    if(pipeline->write_back_wait_time != 0) {
        // cout << "Executing ";
        pipeline->write_back_wait_time--;
        // This is obviously wrong because it shouldn't restart all stages. 
        if (pipeline->write_back_wait_time == 0) {
            restart_memory_access(pipeline);
        }
    
    } else if(pipeline->continue_write_back) {
        if(pipeline->write_back_instruction->isNoop) {
            // cout << "NOOP ";
           return 0;
        }
        Instruction *write_back_instruction = pipeline->write_back_instruction;

        // Still needs to be done 
        if(write_back_instruction->instruction_type == 0 && write_back_instruction->opcode == 13) {
            // CMP instruction write to control register 
            pipeline->condition_register = write_back_instruction->result;
            // cout << "Condition register is: " << pipeline->condition_register << "\n";
            // cout << "Writing to condition register: " << std::bitset<16>(pipeline->condition_register).to_string() << " ";

            if(pipeline->condition_being_written) {
                pipeline->condition_being_written = 0;
                // cout << "Changing condition variable! ";
                if(pipeline->data_hazard) {
                    restart_decode(pipeline);
                }
            }        

        } else if(write_back_instruction->instruction_type == 0 || write_back_instruction->instruction_type == 1) {

            if (pipeline->register_bank.count(write_back_instruction->write_back_register) > 0)
                pipeline->register_bank[write_back_instruction->write_back_register] = write_back_instruction->result;
            else
                pipeline->register_bank.insert(pair<int, int>(write_back_instruction->write_back_register, write_back_instruction->result));
        } else if(write_back_instruction->instruction_type == 4 && pipeline->squash_instructions == 1) {
            pipeline->program_counter = write_back_instruction->result;
            // cout << "Program counter now is: " << pipeline->program_counter;
            if(pipeline->fetch_wait_time == 0) {
                // cout << "\nSQUASH COMPLETED!\n";
                pipeline->squash_instructions = 0;
            }
                
        }
        // Check the register bank now
        // cout << "Value of register at: " << write_back_instruction->write_back_register << " is: " << pipeline->register_bank.at(write_back_instruction->write_back_register) << "\n";
        

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
        if(pipeline->single_instruction && pipeline->continue_decode == 1)
            pipeline->continue_fetch = 1;

        // if(write_back_instruction->instruction_type == 1 && write_back_instruction->opcode == 2)
        //     write_back_completed = true;
        // cout << "Removing from pipeline " << write_back_instruction->write_back_register << "\n";
        pipeline->register_in_flight.erase(write_back_instruction->write_back_register);
        // cout << "Data hazard set? " << pipeline->data_hazard << "\n";

        // When should I do this? What is memory is working then we can't restart it. 
        if(pipeline->data_hazard == 1 && pipeline->continue_memory_access == 1 && pipeline->continue_execute == 1)
            restart_decode(pipeline);
        
        delete pipeline->write_back_instruction;
        
    } else {
        // cout << "Stalled\n";
    }
    return 0;
}

int run_pipeline(Cache* cache_array, int sizeCache, int cycleCount, Pipeline* pipeline) {
    // pipeline->continue_fetch = 1;
    // pipeline->continue_decode = 1;
    // pipeline->continue_execute = 1;
    // pipeline->continue_memory_access = 1;
    // pipeline->continue_write_back = 1;
    // pipeline->cache = cache_array;
    // pipeline->cache_size = sizeCache;
    // pipeline->single_instruction = 0;
    // pipeline->noop->isNoop = 1;
    // pipeline->decode_instructions[0] = -1;
    int cycles = 0;
    // decode_instructions.push_back(-1);

    //Instruction* noop = new Instruction();
    //noop->isNoop = 1; 

    //execute_instructions.push_back(noop);
    // execute_instructions.push_back(noop);

    //  memory_access_instructions.push_back(noop);
    // memory_access_instructions.push_back(noop);

    //  write_back_instructions.push_back(noop);
    // write_back_instructions.push_back(noop);
    
    
    pipeline->register_bank.insert(pair<int, int>(5,0));
    while(cycleCount > cycles) {
        // std::cout << "Program counter is: " << pipeline->program_counter;
        // Increment the current cycle 
        pipeline->total_cycles++;
        // cout << "Cycle ";

        if (pipeline->continue_write_back && pipeline->write_back_wait_time == 0)
        {

            pipeline->write_back_instruction = pipeline->write_back_instructions[0];
            
            if(pipeline->write_back_instruction == nullptr) {
                // Create new noop
                // Instruction* noop = new Instruction(); 
                // noop->isNoop = 1;
                pipeline->write_back_instruction = pipeline->noop;
            }

             if(pipeline->write_back_instruction->isNoop && pipeline->memory_access_stopped) {
                 // cout << "STOPPING WRITE BACK!\n";
                 pipeline->write_back_stopped = 1;
                 break; 
             }
           
            pipeline->write_back_instructions[0] = nullptr;
        }
        
        write_back(pipeline);
        
            

        if (pipeline->continue_memory_access && pipeline->memory_access_wait_time == 0)
        {

            pipeline->memory_access_instruction = pipeline->memory_access_instructions[0];

            if(pipeline->memory_access_instruction == nullptr) {
                 // Create new noop
                // Instruction* noop = new Instruction(); 
                // noop->isNoop = 1;
                pipeline->memory_access_instruction = pipeline->noop;
            }

             if(pipeline->memory_access_instruction->isNoop && pipeline->execute_stopped) {
                 pipeline->memory_access_stopped = 1; 
                 // cout << "\nSTOPPING EXECUTE\n";
             }
            
             pipeline->memory_access_instructions[0] = nullptr;
        }

        memory_access(pipeline);
        

        if (pipeline->continue_execute && pipeline->execute_wait_time == 0)
        {

            pipeline->execute_instruction = pipeline->execute_instructions[0];

            if(pipeline->execute_instruction == nullptr) {
                 // Create new noop
                // Instruction* noop = new Instruction(); 
                // noop->isNoop = 1;
                pipeline->execute_instruction = pipeline->noop;
            }

            if(pipeline->execute_instruction->isNoop && pipeline->decode_stopped) {
                pipeline->execute_stopped = 1; 
                // cout << "\nSTOPPING EXECUTE\n";
            }
                
            pipeline->execute_instructions[0] = nullptr;
        }

        execute(pipeline);
        

        if (pipeline->continue_decode && pipeline->decode_wait_time == 0 && !pipeline->data_hazard)
        {
            pipeline->decode_instruction = pipeline->decode_instructions[0];
            if(pipeline->decode_instruction == -1 && pipeline->fetch_stopped) {
                pipeline->decode_stopped = 1;
                // cout << "\nSTOPPING DECODE\n";
            }
                
            // std::cout << "Decode instruction is: " << pipeline->decode_instruction;
            pipeline->decode_instructions[0] = -1;
        }

        decode(pipeline);

        
        if((pipeline->program_counter < pipeline->last_instruction && !pipeline->squash_instructions) || pipeline->fetch_wait_time > 0) {
            fetch(cache_array, sizeCache, pipeline);
           
            if(pipeline->single_instruction) {
                pipeline->continue_fetch = 0;
                // cout << "\nStopping fetch because of single instruction!\n";
            }   
        } else {
            // cout << "\nSTOPPED FETCH!\n";
            if(pipeline->program_counter >= pipeline->last_instruction) {
                pipeline->fetch_stopped = 1;
                // cout << "\nSTOPPING FETCH\n";
            }
                

        }
        // Here update the pipeline object with the instruction for the next cycle. 
        // We will keep a buffer for each 1 and use the buffer to decide the next object.

        // std::this_thread::sleep_for(0.05s);
        
        cycles++;   
    }
    // cout << "Cycles taken are: " << pipeline->total_cycles << "\n";
    pipeline->total_cycles = 0;
    
    return cycles;
    
}

/*int main() {
    cout << "This is the pipeline";
}*/
