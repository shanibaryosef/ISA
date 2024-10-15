#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 500
#define MAX_NUM_OF_LABLES 100
#define MAX_LABLE_LENGTH 50

//Parsing the line
typedef struct {
    char* instruction;  // The mnemonic (e.g., "add", "sub")
    char* operand1;     // The first operand (e.g., "$t1")
    char* operand2;     // The second operand (e.g., "$t2")
    char* operand3;     // The third operand (e.g., "$t3")
    char* imm;          // the imm
    int fomat;          // R-Format or I-Format
} ParsedInstruction;

ParsedInstruction parseInstruction(char* line) {
    ParsedInstruction result;

    // Parse the mnemonic (instruction)
    result.instruction = strtok(line, " ,\t\n");

    // Parse the operands
    result.operand1 = strtok(NULL, " ,\t\n");
    result.operand2 = strtok(NULL, " ,\t\n");
    result.operand3 = strtok(NULL, " ,\t\n");
    result.imm = strtok(NULL, " ,\t\n");

    return result;
}

int checkFormat(int opcode) {
    //The function gets an opcode and return 1 if it is a R-Foamat, 0 if it is a I-Format.
    int R = (opcode == 0) | (opcode == 1) | (opcode == 2) | (opcode == 3) | (opcode == 4) | (opcode == 5) | (opcode == 6) | (opcode == 7) | (opcode == 8);
    if (R == 1)
    {
        return 1;
    }
    return 0;
}

int getOpcode(const char* mnemonic) { 
    //the function gets the mnemonic (assembly name for an operation) of the operation and return its' opcode. If there is no operation that matches the mnemonic: it returns -1.
    if (strcmp(mnemonic, "add") == 0) {
        return 0;  
    }
    else if (strcmp(mnemonic, "sub") == 0) {
        return 1;  // Example opcode for "sub"
    }
    else if (strcmp(mnemonic, "mul") == 0) {
        return 2;
    }
    else if (strcmp(mnemonic, "and") == 0) {
        return 3;
    }
    else if (strcmp(mnemonic, "or") == 0) {
        return 4;
    }
    else if (strcmp(mnemonic, "xor") == 0) {
        return 5;
    }
    else if (strcmp(mnemonic, "sll") == 0) {
        return 6;
    }
    else if (strcmp(mnemonic, "sra") == 0) {
        return 7;
    }
    else if (strcmp(mnemonic, "srl") == 0) {
        return 8;
    }
    else if (strcmp(mnemonic, "beq") == 0) {
        return 9;
    }
    else if (strcmp(mnemonic, "bne") == 0) {
        return 10;
    }
    else if (strcmp(mnemonic, "blt") == 0) {
        return 11;
    }
    else if (strcmp(mnemonic, "bgt") == 0) {
        return 12;
    }
    else if (strcmp(mnemonic, "ble") == 0) {
        return 13;
    }
    else if (strcmp(mnemonic, "bge") == 0) {
        return 14;
    }
    else if (strcmp(mnemonic, "jal") == 0) {
        return 15;
    }
    else if (strcmp(mnemonic, "lw") == 0) {
        return 16;
    }
    else if (strcmp(mnemonic, "sw") == 0) {
        return 17;
    }
    else if (strcmp(mnemonic, "reti") == 0) {
        return 18;
    }
    else if (strcmp(mnemonic, "in") == 0) {
        return 19;
    }
    else if (strcmp(mnemonic, "out") == 0) {
        return 20;
    }
    else if (strcmp(mnemonic, "halt") == 0) {
        return 21;
    }
    return -1;  // Invalid mnemonic
}

int getRegister(const char* reg) {
    //the function gets a register and return its' opcode. If there is no regiter that matches: it returns -1.

    if (strcmp(reg, "$zero") == 0)
    {
        return 0;
    }
    if (strcmp(reg, "$imm") == 0) 
    {
        return 1;
    }
    if (strcmp(reg, "$v0") == 0)
    {
        return 2;
    }
    if (strcmp(reg, "$a0") == 0)
    {
        return 3;
    }
    if (strcmp(reg, "$a1") == 0)
    {
        return 4;
    }
    if (strcmp(reg, "$a2") == 0)
    {
        return 5;
    }
    if (strcmp(reg, "$a3") == 0)
    {
        return 6;
    }
    if (strcmp(reg, "$t0") == 0)
    {
        return 7;
    }
    if (strcmp(reg, "$t1") == 0)
    {
        return 8;
    }
    if (strcmp(reg, "$t2") == 0)
    {
        return 9;
    }
    if (strcmp(reg, "$s0") == 0)
    {
        return 10;
    }
    if (strcmp(reg, "$s1") == 0)
    {
        return 11;
    }
    if (strcmp(reg, "$s2") == 0)
    {
        return 12;
    }
    if (strcmp(reg, "$gp") == 0)
    {
        return 13;
    }
    if (strcmp(reg, "$sp") == 0)
    {
        return 14;
    }
    if (strcmp(reg, "$ra") == 0)
    {
        return 15;
    }
    return -1;
}

int getImmediate(const char* imm_str) {
    return atoi(imm_str);  // Convert a string number to integer (for immediate values)
}

void writeToOutputFile(const char* filename, int machine_code) {
    FILE* file = fopen(filename, "a");  // Open in append mode
    if (!file) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%04x\n", machine_code);  // Write in hexadecimal format
    fclose(file);
}

void generateRType(int opcode, int rd, int rs, int rt) {
    // The R-type format is: opcode[19:12]rd[11:8]rs[7:4]rt[3:0]
    int machineCode = (opcode << 12) | (rd << 8) | (rs << 4) | rt;
    printf("Machine Code: %05x\n", machineCode); // padding with zeros if necessary. 5 Hex digits in total (the adress is 20 bits long).

    // TODO: Write the machine code to the output file !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void generateIType(int opcode, int rd, int rs, int rt, int imm) {
    // The R-type format is: opcode[39:32]rd[31:28]rs[27:24]rt[23:20]imm[19:0]
    int machineCode = (opcode << 32) | (rd << 28) | (rs << 24) | (rt << 20) | (imm & 0xFF);
    printf("Machine Code: %010x\n", machineCode);
    // TODO: Write the machine code to the output file !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

// Handle Lables
typedef struct {
    char label[MAX_LABLE_LENGTH];
    int address;
} Label;

Label labelTable[MAX_NUM_OF_LABLES];  // Assuming a max of 100 labels
int labelCount = 0;

void addLabel(const char* label, int address) {
    strcpy(labelTable[labelCount].label, label);
    labelTable[labelCount].address = address;
    labelCount++;
}

int getLabelAddress(const char* label) {
    // The function gets a lable and checks for it in the "lable table" array. If the lable was saved in the array, the function returns its' adress, if it isn't there the function returns -1. 
    
    int i - 0;
    for (i = 0; i < labelCount; i++)
    {
        if (strcmp(label, labelTable[i].label) == 0)
        {
            return labelTable[i].address;
        }
    }
    return -1;  // Label not found
}

void readAssemblyFile(const char* filename) {
    ParsedInstruction parsedLine;
    int opcode = 0;
    int rd = 0;
    int rs = 0;
    int rt = 0;
    int imm = 0;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {

        printf("Line: %s", line);  // Print the line for now, later you'll parse it
        parsedLine = parseInstruction(line);

        // Turn the line to binary
        opcode = getOpcode(parsedLine.instruction);
        rd = getRegister(parsedLine.operand1);
        rs = getRegister(parsedLine.operand2);
        rt = getRegister(parsedLine.operand3);
        imm = getImmediate(parsedLine.imm);

        //Entering the line into the output-file acoordding to its' format ( R or I).
        parsedLine.format = checkFormat(opcode);
        if (parsedLine.fomat == 1)
        {
            generateRType(opcode, rd, rs, rt); // The instruction is R-Format
        }
        else if (parsedLine.fomat == 0) { // The instruction is I-Format
            generateIType(opcode, rd, rs, rt, imm);
        }
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <assembly_file>\n", argv[0]);
        return 1;
    }

    readAssemblyFile(argv[1]);
    return 0;
}
