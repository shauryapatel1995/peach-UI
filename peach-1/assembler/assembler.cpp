#include <iostream> 
#include <regex>
#include <string.h> 
#include "assembler.h"
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

// constructor
Assembly::Assembly()
{
    //TODO    
}

Assembly::~Assembly()
{
    //TODO 
}

int Assembly::strToEnum(string strToConvert)
{
    for (int ii=0; ii<MAX_INSTR; ii++)
    {
        if(strToConvert.compare(enumInstrString[ii]) == 0)
        {
            return ii;
        }
    }
    for (int jj=0; jj<MAX_REG; jj++)
    {
        if(strToConvert.compare(enumRegString[jj]) == 0)
        {
            return jj;
        }
    }
    return -1;
}

string Assembly::decodeInstructionType(int enumInstr)
{
    // instruction type
    switch (static_cast<instructions>(enumInstr))
    {
    case ADD:
        cout << "Current instruction type is ADD\n";
        binInstrType = "0000000";
        // THESE FLAGS ARE USED TO SET THE IMMEDIATE STRING AND ADDRESSING MODE 
        isAlu = 1;
        isAdd = 1;
        return binInstrType;
        break;

    case ADC:
        cout << "Current instruction type is ADC\n";
        break;

    case SUB:
        cout << "Current instruction type is SUB\n";
        isAlu =1;
        binInstrType = "0000001";
        return binInstrType;
        break;

    case MUL:
        cout << "Current instruction type is MUL\n";
        isAlu = 1;
        binInstrType = "0000010";
        return binInstrType;
        break;
//
    case AND:
        cout << "Current instruction type is AND\n";
        isAlu = 1;
        binInstrType = "0000010";
        return binInstrType;
        break;
    
    case OR:
        cout << "Current instruction type is OR\n";
        isAlu = 1;
        binInstrType = "0000011";
        return binInstrType;
        break;

    case NOT:
        cout << "Current instruction type is NOT\n";
        isAlu = 1;
        binInstrType = "0000101";
        return binInstrType;
        break;

    case ROR:
        cout << "Current instruction type is ROR\n";
        isAlu = 1;
        binInstrType = "0001010";
        return binInstrType;
        break;
    
    case ROL:
        cout << "Current instruction type is ROL\n";
        isAlu = 1;
        binInstrType = "0001100";
        return binInstrType;
        break;

    case CMP:
        cout << "Current instruction type is CMP\n";
        isAlu = 1;
        isCmp = 1;
        binInstrType = "0001101";
        return binInstrType;
        break;

    case LSH:
        cout << "Current instruction type is LSH\n";
        isAlu = 1;
        binInstrType = "0001000";
        return binInstrType;
        break;

    case RSH:
        cout << "Current instruction type is RSH\n";
        isAlu = 1;
        binInstrType = "0001001";
        return binInstrType;
        break;
    // unconditional branch 
    case B:
        cout << "Current instruction type is unconditional B\n";
        binInstrType = "100000";
        return binInstrType;
        break;
    // conditonal branch less than 
    case BL:
        cout << "Current instruction type is conditional BL\n";
        binInstrType = "1001100010";
        return binInstrType;
        break;

    case LDR:
        cout << "Current instruction type is LDR\n";
        binInstrType = "001000";
        return binInstrType;
        break;

    case STR:
        cout << "Current instruction type is STR\n";
        binInstrType = "001010";
        return binInstrType;
        break;

    case LDD:
        cout << "Current instruction type is LDD\n";
        binInstrType = "001001";
        return binInstrType;
        break;

    case STD:
        cout << "Current instruction type is STD\n";
        binInstrType = "001011";
        return binInstrType;
        break;

    case CALL:
        cout << "Current instruction type is CALL\n";
        binInstrType = "0100";
        return binInstrType;
        break;
    // return 
    case RET:
        cout << "Current instruction type is RET\n";
        binInstrType = "0101";
        return binInstrType;
        break;

    case RPT:
        cout << "Current instruction type is RPT\n";
        binInstrType = "100111";
        return binInstrType;
        break;

    // branch conditional equal 
    case BE:
        cout << "Current instruction type is BE\n";
        binInstrType = "1000010000";
        return binInstrType;
        break;
    // branch conditional not equal 
    case BNE:
        cout << "Current instruction type is RPT\n";
        binInstrType = "1000100001";
        return binInstrType;
        break;
    // branch conditional greater than or equal 
    case BGE:
        cout << "Current instruction type is RPT\n";
        binInstrType = "1000110101";
        return binInstrType;
        break;
    // branch conditional less than or equal 
    case BLE:
        cout << "Current instruction type is RPT\n";
        binInstrType = "1001000011";
        return binInstrType;
        break;
    // branching greater 
    case BG:
        cout << "Current instruction type is BG\n";
        binInstrType = "1001010100";
        return binInstrType;
        break;

    default: 
        cout << "ERROR SHOULD NOT GET THIS FOR INSTURCTION ENUM. \n";
    }
}


void Assembly::decodeRegisters(int enumReg)
{
    // instruction type
    switch (static_cast<registers>(enumReg))
    {
        case R0:
            cout << "This is R0 case statement!\n";
            binRegisters.push_back("0000");
            break;
        case R1:
            cout << "This is R1 case statement!\n";
            binRegisters.push_back("0001");
            break;
        case R2:
            cout << "This is R2 case statement!\n";
            binRegisters.push_back("0010");
            break;
        case R3:
            cout << "This is R3 case statement!\n";
            binRegisters.push_back("0011");
            break;
        case R4:
            cout << "This is R4 case statement!\n";
            binRegisters.push_back("0100");
            break;
        case R5:
            cout << "This is R5 case statement!\n";
            binRegisters.push_back("0101");
            break;
        case R6:
            cout << "This is R6 case statement!\n";
            binRegisters.push_back("0110");
            break;
        case R7:
            cout << "This is R7 case statement!\n";
            binRegisters.push_back("0111");
            break;
        case R8:
            cout << "This is R8 case statement!\n";
            binRegisters.push_back("1000");
            break;
        case R9:
            cout << "This is R9 case statement!\n";
            binRegisters.push_back("1001");
            break;
        case R10:
            cout << "This is R10 case statement!\n";
            binRegisters.push_back("1010");
            break;
        case R11:
            cout << "This is R11 case statement!\n";
            binRegisters.push_back("1011");
            break;
        case R12:
            cout << "This is R12 case statement!\n";
            binRegisters.push_back("1100");
            break;
        case R13:
            cout << "This is R13 case statement!\n";
            binRegisters.push_back("1101");
            break;
        case R14:
            cout << "This is R14 case statement!\n";
            binRegisters.push_back("1110");
            break;
        case R15:
            cout << "This is R15 case statement!\n";
            binRegisters.push_back("1111");
            break;
    }
}

string Assembly::decodeCond()
{
    //
}

string Assembly::addJunk(string myTestBin, string myBin, string myValue)
{
    int numZeros = 32 - myTestBin.size();
    string strZeros(numZeros, '0');
    string myFinalBin;
    if (myValue.compare("empty") == 0)
    {
        myFinalBin = myBin + strZeros;
    }
    else
    {
        myFinalBin = myBin + strZeros + myValue;
    }
    return myFinalBin;
}

int Assembly::decodeStack()
{
    while(!stack.empty())
    {
        vector<string> current_instruction; 
        for (int ii=0; ii<5; ii++)
        {
            current_instruction.push_back(stack.front());
            stack.pop();
        }

        // convert string to int 
        int enumInstr = strToEnum(current_instruction[0]);

        //convert int to enum 
        //cout << "THIS IS ENUM CASTING " << static_cast<instructions>(enumInstr) << endl;
        
        decodeInstructionType(enumInstr);

        //convert the rest of the instruction
        string valStr = "empty";
        regex numRegexObject("^-?[0-9]+");
        regex registerRegexObject("^R.+");
        long value = 0;

        for (int jj =1; jj<current_instruction.size() ; jj++)
        {
            if(current_instruction[jj].compare(valStr) != 0)
            {
                // testing that no empties got in 
                //cout << "Make sure this does not equal empty: " << current_instruction[jj] << endl;

                // decoding the registers and adding them to the bin registers vector             
                if (regex_match(current_instruction[jj], registerRegexObject))
                {
                    //cout << "THIS SHOULD BE THE REGISTERSS : " << current_instruction[jj] << endl;
                    int enumRegister = strToEnum(current_instruction[jj]); 
                    //cout << "THIS IS THE STR CONVERSION OF REGISTERS : " << enumRegister << endl;
                    //convert int to enum 
                    //cout << "THIS IS ENUM CASTING " << static_cast<registers>(enumRegister) << endl;
                    decodeRegisters(enumRegister);

                }
                // check to see if there is an immediate value that needs to be added 
                if(regex_match(current_instruction[jj], numRegexObject))
                {
                    //cout << "THIS SHOULD BE A NUMBA : " << current_instruction[jj] << endl;
                    // should figure out how many bits to set aside for the value
                    cout << "THIS IS THE INT VALUE OF THA NUMBA : " << current_instruction[jj] << endl;
                    value = stoi(current_instruction[jj]);
                    // check if it is negative 
                   // if (value < 0) 
                   // {
                   //     // have to do 2 complement 
                   //     cout << "\n\n-------------------------------- HAVE A NEGATIVE NUMBER -------------------------\n\n";
                   //     int tempVal = value + 1;
                   //     cout << "TEMP VAL: \n";
                   //     cout << tempVal << endl;
                   //     string tempBin = bitset<16>(tempVal).to_string();
                   //     cout << "TEMP BIN: \n";
                   //     cout << tempBin << endl;
                   //     binValue = "";
                   //     cout << binValue << endl;
                   //     for (int nn = 0; nn < tempBin.size(); nn++)
                   //     {
                   //         if (tempBin[nn] == '1')
                   //         {
                   //             cout << "\n" << tempBin[nn] << "\n";
                   //             binValue += '0';
                   //         } else {
                   //             binValue += '1';
                   //         }
                   //         cout << binValue << endl;
                   //     }
                   //     cout << "BIN: \n";
                   //     cout << binValue << endl;
                   //     immediateBit = "1";
                   // }
                   // else {
                        //cout << "THIS IS THE INT VALUE OF THA NUMBA : " << value << endl;
                        binValue = bitset<16>(value).to_string();
                        //cout << "THIS IS THE BITS OF THAT NUMBA : " << binValue << endl;
                        immediateBit = "1";
                   // }
                }
            }
        }
        // figure out the immediate bit and the addressing mode 
        if (isAdd == 1 && isAlu == 1)
        {   
            if(immediateBit.compare("1") == 0)
            {
                binImAdMode = "01";
            }
            
            else if(immediateBit.compare("0") == 0)
            {
                binImAdMode = "00";
            }
            
        }
        else if (isCmp == 1 && isAlu == 1)
        {
            if(immediateBit.compare("1") == 0)
            {
                cout << " THIS SHOULD NOT HAPPENNNNN - no immediate bit for compare\n";
            }
            else if(immediateBit.compare("0") == 0)
            {
                binImAdMode = "10";
            }
        }

        else if (isAlu == 1 && isAdd == 0 && isCmp == 0)
        {
            if(immediateBit.compare("1") == 0)
            {
                binImAdMode = "11";
            }
            else if(immediateBit.compare("0") == 0)
            {
                binImAdMode = "00";
            }
        }
        else if (isAlu == 0)
        {
            if(immediateBit.compare("1") == 0)
            {
                binImAdMode = "01";
            }
            else if(immediateBit.compare("0") == 0)
            {
                binImAdMode = "10";
            }
        }
        // PUTTING THE BINARY STRING TOGETHER!! 
        binFinal = binInstrType + binImAdMode;
        cout << "This is just the instr + address + im : " << binFinal << endl;
        cout << "This is the register vector size : " << binRegisters.size() << endl;
        cout << "This is the bin register vector: " << endl;
        for(int pp=0; pp<binRegisters.size(); pp++)
        {
            cout << binRegisters[pp] << endl;
        }

        for (int kk =0; kk < binRegisters.size(); kk++)
        {
            binFinal += binRegisters[kk];
            cout << "This is adding the register : " << binFinal << endl;
        }
        
        string testBin = binFinal;
        if (binValue.compare("empty") != 0)
        {
            testBin = binFinal + binValue;
        }

        string myBin = addJunk(testBin, binFinal, binValue);

        myBin = bitset<16>(instrMemAddress).to_string() + myBin;
        // TODO: have to find a way to write these instructions to memory 
        cout << "This is the final binary string : " << myBin << endl;

        ofstream myOutput;
        myOutput.open(myFileOut, fstream::app);
        myOutput << myBin + "\n";
        myOutput.close();

        // RESET ALL THE VARIABLES BEFORE THE NEXT INSTRUCTION
        binValue = "empty";
        binRegisters.clear();
        immediateBit = "0";
        isAlu = 0;
        isAdd = 0;
        isCmp = 0;
        instrMemAddress += 1;
        cout << "**********************************TIME TO DECODE THE NEXT INSTRUCTION!!!!\n";
        // TODO: CONDITIONAL INSTRUCTIONS ARE BROKE - do not have the COND decode section  

    }
    return 0;
}

// this reads in the instructions file, then separates it by spaces, and saves it to a vector
int Assembly::readFile() 
{
    // a vector of the instructions for the code to read in
    // TODO: this should be a part of the instructions class
    string line;
    string word;
    // streaming the file in 
    ifstream myFile;
    myFile.open("test_matrix_instr.txt"); // previously tested on"parse_me" 
    while(getline(myFile, line))
    {
        int ii = 0;
        istringstream ss(line);
        while (ss >> word)
        {
            //check values of word
            cout << word << "\n";
            // TODO: strip words of commas
            //cout << word.end() << endl;
            /*if (word.end() == ",")
            {
                word.erase(remove(word.begin(), word.end(), ','), word.end());
            }*/
            stack.push(word);
            cout << "This words was pushed to the stack: " << word << "\n";
            ii ++;
        }
        cout << " This should be the end of the line... \n";
        while(ii < 5)
        {
            stack.push("empty");
            cout << "putting 'empty' into the vector\n"; 
            ii ++;
        }
    }
    // clean the file that will be written to
    ofstream myOutput;
    myOutput.open(myFileOut, ofstream::out | ofstream::trunc);
    myOutput.close();
    return 0;
}

// TODO: write a way to write these instructions to memory

int main()
{
    //TODO: NEED TO ADD MEMORY START PC 8427
    Assembly assembly;
    assembly.readFile();
    assembly.decodeStack();
    string str_a = "ADD R3 1";

    // Here b is an object of regex (regular expression)
    regex obj_a("(ADD)(.*)"); // Geeks followed by any character

    // regex_match function matches string a against regex b
    if ( regex_match(str_a, obj_a) )
        cout << "String 'a' matches regular expression 'b' \n";

    // regex_match function for matching a range in string
    // against regex b
    if ( regex_match(str_a.begin(), str_a.end(), obj_a) )
        cout << "String 'a' matches with regular expression "
                "'b' in the range from 0 to string end\n";


    string target_str = "ADD R5, R7, R7";
    regex r("A[A-Z]+");

    smatch m;

    regex_search(target_str, m, r);

    // for each loop
    for (auto x : m)
    {
	    cout << x << "\n";
    }

    return 0;
}
