#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

map<string, int> label;     // <label name, label index>
map<int, string> memory;
string binaryform(int num);
int decimalform_unsigned(string binary);
int decimalform(string binaryString);
string hexform(int decimal);
//=======================================================================
//initialize registers
//=======================================================================
map<string, int> registers = { {"x0", 0},{"x1",0},{"x2",0},{"x3",0},
    {"x4",0},{"x5",0},{"x6",0}, {"x7", 0},{"x8", 0},{"x9", 0},{"x10", 0},
    {"x11", 0},{"x12", 0}, {"x13", 0},{"x14", 0},{"x15", 0},{"x16", 0},
    {"x17", 0},{"x18", 0},{"x19", 0},{"x20", 0},{"x21", 0},{"x22", 0},
    {"x23", 0},{"x24", 0}, {"x25", 0},{"x26", 0},{"x27", 0},{"x28", 0},
    {"x29", 0},{"x30", 0},{"x31", 0} };

//=======================================================================
//types of instructions
//=======================================================================

struct RFormat { string instr;    string rd;    string rs1;    string rs2; };
struct IFormat { string instr;    string rd;    string rs1;    int imm; };
struct SBFormat { string instr;    string rs1;   string rs2;  string label; };
struct UJFormat { string instr;    string rd;    int imm; string label; };

//=======================================================================
//types of execution functions
//=======================================================================

void execute(RFormat I);
void execute(IFormat I, int& pc);
void execute(SBFormat I, int& pc);
void execute(UJFormat I, int& pc);
void printreg();
void printmem();

//=======================================================================
// main program
//=======================================================================

int main()
{
    //================================
    //input & storing of file contents
    //================================

    string filename, memoryfilename;
    cout << "Enter program file name: " << endl;
    cin >> filename;

    ifstream inputfile;
    inputfile.open(filename);

    if (!inputfile.is_open())
    {
        cout << "Error: Unable to open program file " << endl;
        return 0;
    }

    string line;
    vector<string> inputcode;

    while (getline(inputfile, line))
    {
        inputcode.push_back(line);
    }
    cout << "Enter memory file name: " << endl;
    cin >> memoryfilename;
    ifstream memoryfile;
    memoryfile.open(memoryfilename);

    if (!memoryfile.is_open())
    {
        cout << "Error: Unable to open memory file " << endl;
        return 0;
    }

    string memoryline;
    vector<string> memorycode;

    while (getline(memoryfile, memoryline))
    {
        memorycode.push_back(memoryline);
    }
    //================================

    //==========================
    // P a r s i n g    d a t a
    //==========================
    //~~~~~~~
    // (1)  Memory file
    //~~~~~~~

    string address;
    string value;
    string byte_0;
    string byte_1;
    string byte_2;
    string byte_3;
    string word;
    cout << memorycode.size();
    for (int i = 0; i < memorycode.size() * 4 + 1; i++)
    {
        memory[i] = "00000000";
    }
    for (int i = 0; i < memorycode.size(); i++)
    {

        long comma_index = memorycode[i].find(", ");
        long space_index = memorycode[i].find(" ");
        if (comma_index != string::npos && space_index != string::npos) //if it is a address
        {
            address = memorycode[i].substr(0, comma_index);
            value = memorycode[i].substr(comma_index + 1, -1);
            word = binaryform(stoi(value));
            cout << word << endl;
            cout << endl;


            byte_0 = word.substr(24, 8);
            byte_1 = word.substr(16, 8);
            byte_2 = word.substr(8, 8);
            byte_3 = word.substr(0, 8);

            memory[stoi(address) + 3] = byte_3;
            memory[stoi(address) + 2] = byte_2;
            memory[stoi(address) + 1] = byte_1;
            memory[stoi(address)] = byte_0;
        }
    }
    printmem();

    //~~~~~~~
    // (2)  Program file
    //~~~~~~~
    //=================
    //Separating labels
    //=================

    string labels;
    string rest;
    string labelname;
    for (int i = 0; i < inputcode.size(); i++)
    {
        long colon_index = inputcode[i].find(":");
        if (colon_index != string::npos) //if it is a label
        {
            labelname = inputcode[i].substr(0, colon_index);
            label[labelname] = i;

            rest = inputcode[i].substr(colon_index + 1, -1);
            inputcode[i] = rest;

        }

    }

    //===============
    //Program counter
    //===============

    int PC = 0;
    cout << "Enter PC initial state :" << endl;
    cin >> PC;
    PC = PC / 4;            //as user will input it in multiple of 4 in assembly

    //=====================
    //Dividing Instructions
    //=====================
    vector<string> adjustedcode;

    while ((PC >= 0) && (PC < inputcode.size()))
    {
        registers["x0"] = 0;
        cout << "PC: " << PC * 4 << "\n";
        printreg();
        string name = "";


        //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘
        //LABEL OPERATION
        //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘

        //Adjusting data according to instruction/registers/immediates etc.
        //================================================================
        for (auto character : inputcode[PC])
        {
            if (character == ',' || character == ' ' || character == '(')
            {
                if (name != "")
                    adjustedcode.push_back(name);

                name = "";
            }
            else
            {
                if (character == ')') continue; //skippity
                name = name + character;
            }
        }
        adjustedcode.push_back(name);
        string function = adjustedcode[0];

        //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*
        //R-FORMAT OPERATION
        //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*

        if (function == "add" || function == "sub" || function == "sll" || function == "slt" || function == "sltu" || function == "xor" || function == "srl" || function == "sra" || function == "or" || function == "and")
        {
            RFormat instruction;
            instruction.instr = adjustedcode[0];
            instruction.rd = adjustedcode[1];
            instruction.rs1 = adjustedcode[2];
            instruction.rs2 = adjustedcode[3];
            execute(instruction);
            PC++;

        }

        //   else
       //
       //    //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*
       //    //I-FORMAT OPERATION
       //    //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*
        else if (function == "addi" || function == "slti" || function == "sltiu" || function == "xori" || function == "ori" || function == "andi" || function == "slli" || function == "srli")
        {
            IFormat instruction;
            instruction.instr = adjustedcode[0];
            instruction.rd = adjustedcode[1];

            instruction.rs1 = adjustedcode[2];
            instruction.imm = stoi(adjustedcode[3]);
            execute(instruction, PC);
        }
        else if (function == "jalr" || function == "lw" || function == "lb" || function == "sw" || function == "sb" || function == "lh" || function == "sh" || function == "lbu" || function == "lhu")
        {
            IFormat instruction;
            instruction.instr = adjustedcode[0];
            instruction.rd = adjustedcode[1];

            instruction.rs1 = adjustedcode[3];
            instruction.imm = stoi(adjustedcode[2]);
            execute(instruction, PC);
        }

        else

            //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*✧
            //SB-FORMAT OPERATION
            //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*✧

            if (function == "beq" || function == "bne" || function == "blt" || function == "bge" || function == "bltu" || function == "bgeu")
            {
                SBFormat instruction;
                instruction.instr = adjustedcode[0];
                instruction.rs1 = adjustedcode[1];
                instruction.rs2 = adjustedcode[2];
                instruction.label = adjustedcode[3];
                execute(instruction, PC);

            }
            else
                //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*✧
                //UJ-FORMAT OPERATION
                //✧∘* ✧･ﾟ✧∘* ✧･ﾟ✧∘✧∘*✧

                if (function == "jal" || function == "lui" || function == "auipc" || function == "fence" || function == "ecall" || function == "ebreak")
                {
                    UJFormat instruction;
                    instruction.instr = adjustedcode[0];
                    instruction.rd = adjustedcode[1];
                    if (function == "jal") instruction.label = adjustedcode[2];
                    else
                        instruction.imm = stoi(adjustedcode[2]);
                    execute(instruction, PC);
                }
                else
                    cout << "Unsupported Instruction" << endl;
        //cout << function;
        adjustedcode.clear();
    }
    cout << endl;
    cout << "________________________________________________________________________" << endl;
    cout << endl;
    cout << "PC :" << PC * 4 << endl;
    cout << endl;
    registers["x0"] = 0;
    printmem();
    printreg();




}

//=======================================================================
//implementation of execution functions
//=======================================================================



void execute(RFormat I) // Execute RISC-V instruction
{

    if (I.instr == "add") {
        registers[I.rd] = registers[I.rs1] + registers[I.rs2];
    }
    else if (I.instr == "sub") {
        registers[I.rd] = registers[I.rs1] - registers[I.rs2];
    }
    else if (I.instr == "sll") {
        registers[I.rd] = registers[I.rs1] << registers[I.rs2];
    }
    else if (I.instr == "slt") {
        if (registers[I.rs1] < registers[I.rs2])
            registers[I.rd] = 1;
        else
            registers[I.rd] = 0;
    }
    else if (I.instr == "sltu") {
        if (abs(registers[I.rs1]) < abs(registers[I.rs2]))
            registers[I.rd] = 1;
        else
            registers[I.rd] = 0;
    }
    else if (I.instr == "xor") {
        registers[I.rd] = registers[I.rs1] ^ registers[I.rs2];
    }
    else if (I.instr == "srl") {
        registers[I.rd] = registers[I.rs1] >> registers[I.rs2];
    }
    else if (I.instr == "sra") {
        registers[I.rd] = registers[I.rs1] >> registers[I.rs2];
    }
    else if (I.instr == "or") {
        registers[I.rd] = registers[I.rs1] | registers[I.rs2];
    }
    else if (I.instr == "and") {
        registers[I.rd] = registers[I.rs1] & registers[I.rs2];
    }
    else {
        cout << "Error: Unsupported instruction" << endl;
        exit(0);  // Exits the program
    }
}
void printreg() {
    for (auto it = registers.cbegin(); it != registers.cend(); ++it) {
        int second = it->second;
        cout << it->first << left << ": " << "| decimal form: " << second << "|  binary form: " << binaryform(second) << " | hexadecimal form: " << hexform(it->second) << "|" << "\n";
    }
}
void printmem() {
    for (auto it = memory.cbegin(); it != memory.cend(); ++it) {
        string second = it->second;
        if (it->second == "") { second = "00000000"; }
        cout << it->first << ": " << second << "\n";
    }
}

void execute(IFormat I, int& pc)
{
    cout << I.instr;
    if (I.instr == "jalr") {
        registers[I.rd] = pc * 4 + 4;
        pc = (registers[I.rs1] + (I.imm)) / 4;
    }
    else if (I.instr == "lb") {
        string finalword;
        finalword = binaryform(decimalform(memory[I.imm + registers[I.rs1]]) & 0b11111111).substr(24, 8);
        registers[I.rd] = decimalform(finalword);
        // 8 btis of memory into register destination, memory is 8 bits
        pc++;

    }
    else if (I.instr == "lh") {
        string finalword;
        finalword = binaryform(decimalform(memory[I.imm + registers[I.rs1] + 1]) & 0b11111111).substr(24, 8) + binaryform(decimalform(memory[I.imm + registers[I.rs1]]) & 0b11111111).substr(24, 8);
        registers[I.rd] = decimalform(finalword);


        pc++;
    }
    else if (I.instr == "lw") {
        string finalword;
        finalword = binaryform(decimalform(memory[I.imm + registers[I.rs1] + 3]) & 0b11111111).substr(24, 8) + binaryform(decimalform(memory[I.imm + registers[I.rs1] + 2]) & 0b11111111).substr(24, 8) + binaryform(decimalform(memory[I.imm + registers[I.rs1] + 1]) & 0b11111111).substr(24, 8) + binaryform(decimalform(memory[I.imm + registers[I.rs1]]) & 0b11111111).substr(24, 8);
        registers[I.rd] = decimalform(finalword);

        pc++;

    }
    else if (I.instr == "lbu") {
        string finalword;
        finalword = binaryform(decimalform(memory[I.imm + registers[I.rs1]]) & 0b11111111).substr(24, 8);
        registers[I.rd] = decimalform_unsigned(finalword);
        pc++;

    }
    else if (I.instr == "lhu") {
        string finalword;

        finalword = binaryform(decimalform(memory[I.imm + registers[I.rs1] + 1]) & 0b11111111).substr(24, 8) + binaryform(decimalform(memory[I.imm + registers[I.rs1]]) & 0b11111111).substr(24, 8);
        registers[I.rd] = decimalform_unsigned(finalword);

        pc++;

    }
    else if (I.instr == "sb") {
        memory[I.imm + registers[I.rs1]] = binaryform(registers[I.rd] & 0b00000000000000000000000011111111).substr(24, 8); // we store only the first byte into memory
        pc++;

    }
    else if (I.instr == "sh") {
        memory[I.imm + registers[I.rs1]] = binaryform(registers[I.rd] & 0b00000000000000000000000011111111).substr(24, 8); // we store the first byte into memory
        memory[I.imm + registers[I.rs1] + 1] = binaryform(registers[I.rd] & 0b00000000000000001111111100000000).substr(16, 8); // we store the second byte into memory
        pc++;

    }
    else if (I.instr == "sw") {
        memory[I.imm + registers[I.rs1]] = binaryform(registers[I.rd] & 0b00000000000000000000000011111111).substr(24, 8); // we store the first byte into memory
        memory[I.imm + registers[I.rs1] + 1] = binaryform(registers[I.rd] & 0b00000000000000001111111100000000).substr(16, 8); // we store the second byte into memory
        memory[I.imm + registers[I.rs1] + 2] = binaryform(registers[I.rd] & 0b00000000111111110000000000000000).substr(8, 8); // we store the third byte into memory
        memory[I.imm + registers[I.rs1] + 3] = binaryform(registers[I.rd] & 0b11111111000000000000000000000000).substr(0, 8); // we store the last byte into memory
        pc++;

    }
    else

        //     int int_rd = stoi(I.rd);
        //     int int_rs1 = stoi
        if (I.instr == "addi") {
            registers[I.rd] = registers[I.rs1] + I.imm;
            pc++;
        }
        else if (I.instr == "slti") {
            if (registers[I.rs1] < I.imm)
                registers[I.rd] = 1;
            else
                registers[I.rd] = 0;
            pc++;
        }
        else if (I.instr == "sltiu") {
            unsigned int temp1 = registers[I.rs1];
            unsigned int temp2 = I.imm;
            if (temp1 < temp2)
                registers[I.rd] = 1;
            else
                registers[I.rd] = 0;
            pc++;
        }
        else if (I.instr == "xori") {
            registers[I.rd] = registers[I.rs1] ^ I.imm;
        }
        else if (I.instr == "ori") {
            registers[I.rd] = registers[I.rs1] | I.imm;
        }
        else if (I.instr == "andi") {
            registers[I.rd] = registers[I.rs1] & I.imm;
        }
        else if (I.instr == "slli") {
            registers[I.rd] = registers[I.rs1] << I.imm;
        }
        else if (I.instr == "srli") {
            unsigned int temp = registers[I.rs1];
            registers[I.rd] = temp >> I.imm;
        }
        else if (I.instr == "srai") {
            registers[I.rd] = registers[I.rs1] >> I.imm;
        }
        else {
            cout << "Error: Unsupported instruction: " << I.instr << endl;
            exit(0);  // Exits the program
        }
}
void execute(SBFormat I, int& pc)
{
    if (I.instr == "beq") {
        if (registers[I.rs1] == registers[I.rs2])
            pc = label[I.label];
        else
            pc = pc + 1;
    }
    else if (I.instr == "bne") {
        if (registers[I.rs1] != registers[I.rs2])
            pc = label[I.label];
        else
            pc = pc + 1;
    }
    else if (I.instr == "blt") {
        if (registers[I.rs1] < registers[I.rs2])
            pc = label[I.label];
        else
            pc = pc + 1;
    }
    else if (I.instr == "bge") {
        if (registers[I.rs1] >= registers[I.rs2])
            pc = label[I.label];
        else
            pc = pc + 1;
    }
    else if (I.instr == "bltu") {
        unsigned int temp1 = registers[I.rs1];
        unsigned int temp2 = registers[I.rs2];
        if (temp1 < temp2)
            pc = label[I.label];
        else
            pc = pc + 1;

    }
    else if (I.instr == "bgeu") {
        unsigned int temp1 = registers[I.rs1];
        unsigned int temp2 = registers[I.rs2];
        if (temp1 >= temp2)
            pc = label[I.label];
        else
            pc = pc + 1;

    }
    else {
        cout << "Error: Unsupported instruction" << endl;
        exit(0);  // Exits the program
    }
}
// jal rd, label
void execute(UJFormat I, int& pc)
{
    if (I.instr == "lui") {

        registers[I.rd] = I.imm << 12;
        pc++;
    }
    else if (I.instr == "auipc")
    {
        registers[I.rd] = (I.imm << 12) + (pc * 4);
        pc++;
    }
    else if (I.instr == "jal")
    {
        registers[I.rd] = pc * 4 + 4;

        if (label[I.label]) //not sure
        {
            pc = label[I.label];
        }
        else
            cout << "Error, label not found" << endl;
    }
    else if (I.instr == "fence") {
        exit(0);
    }
    else if (I.instr == "ecall") {
        exit(0);
    }
    else if (I.instr == "ebreak") {
        exit(0);
    }
    else {
        cout << "Error: Unsupported instruction" << endl;
        exit(0);  // Exits the program
    }
}


string binaryform(int num)
{
    string binary = "";
    for (int i = 31; i >= 0; i--)
    {
        int bit = num >> i;
        if (bit & 1)
            binary.push_back('1');
        else
            binary.push_back('0');
    }
    return binary;
}

//
int decimalform_unsigned(string binary) {
    int decimal = 0;
    int power = 1;

    for (int i = binary.length() - 1; i >= 0; --i) {
        if (binary[i] == '1') {
            decimal += power;
        }
        power *= 2;
    }
    return decimal;
}

int decimalform(string binary) {
    int decimal = 0;
    int power = 0;
    bool isNegative = false;

    if (binary[0] == '1') {
        isNegative = true;
        // Invert all bits
        for (int i = 0; i < binary.length(); i++) {
            if (binary[i] == '0') {
                binary[i] = '1';
            }
            else {
                binary[i] = '0';
            }
        }
        // Add 1 to get the 2's complement
        for (int i = binary.length() - 1; i >= 0; i--) {
            if (binary[i] == '0') {
                binary[i] = '1';
                break;
            }
            else {
                binary[i] = '0';
            }
        }
    }

    for (int i = binary.length() - 1; i >= 0; i--) {
        if (binary[i] == '1') {
            decimal += pow(2, power);
        }
        power++;
    }

    if (isNegative) {
        decimal = -decimal;
    }

    return decimal;
}

string hexform(int decimal)
{
    stringstream ss;
    ss << hex << decimal;
    string res(ss.str());
    return res;
}