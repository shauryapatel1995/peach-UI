#include <bits/stdc++.h> 
#include <iostream> 
#include <queue>

using namespace std;



// defining enumeration of instructions type
// check if valid by seeing if the instruction type = 111
// instruction type = 111 is not assigned and means that it is invalid 
// 3 bits
enum instructions { ADD, //0
                    ADC, 
                    SUB,
                    MUL, //3
                    AND,
                    OR,
                    NOT, //6
                    ROR, 
                    ROL,
                    CMP, //9 
                    LSH, 
                    RSH, 
                    B, //12
                    BL, 
                    LDR, 
                    STR, //15
                    LDD, 
                    STD,
                    CALL, //18
                    RET,
                    RPT,
                    BE, //21
                    BNE,
                    BGE,
                    BLE, //24
                    BG,
                    MAX_INSTR};

// enumeration of registers 1-16 
// 4 bits 
enum registers {R0, //0
                R1, 
                R2,
                R3, //3
                R4,
                R5,
                R6, //6
                R7,
                R8,
                R9, //9 
                R10,
                R11,
                R12, //12
                R13,
                R14,
                R15,
                MAX_REG};

// TODO:not sure if I should create an assembler class

class Assembly 
{
    public: 
        // constructor 
        Assembly();
        ~Assembly();
        // variables
        queue<string> stack;
        string binInstrType = "empty";
        string binImAdMode = "empty";
        vector<string> binRegisters;
        string binValue = "empty";
        string binFinal = "empty";
        int instrMemAddress = 8427;
        string myFileOut = "instr_output.txt";

        // flags for the decode
        bool isAlu = 0;
        bool isAdd = 0;
        bool isCmp = 0;
        string immediateBit = "0";
        string addressingMode = "0";

        // methods 
        int readFile();
        int decodeStack();
        int strToEnum(string str);
        string decodeInstructionType(int enumInstr);
        void decodeRegisters(int enumReg);
        string decodeCond();
        // this makes sure all the instructions are the correct size 
        //by adding zeros for the extra space that is not used
        string addJunk(string myTestBin, string myBin, string myValue);

        // enum arrays
        string enumInstrString[26] = {"ADD", "ADC", "SUB", "MUL", "AND", "OR", "NOT", "ROR", "ROL", "CMP", "LSH", "RSH", "B", "BL", "LDR", "STR", "LDD", "STD", "CALL", "RET", "RPT", "BE", "BNE", "BGE", "BLE", "BG"};
        string enumRegString[16] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
};


