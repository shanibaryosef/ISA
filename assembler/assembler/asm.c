#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ROW_LEN 500
#define MAX_LABEL_LEN 50
#define MEM_LEN (1 << 12)
#define OPCODE ((char const*[]){ "add", "sub", "mul", "and", "or", "xor", "sll", "sra", "srl", "beq",\
"bne", "blt", "bgt", "ble", "bge", "jal", "lw", "sw", "reti", "in", "out", "halt" })
#define REGS ((char const*[]){ "$zero", "$imm", "$v0", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",\
"$s0", "$s1", "$s2", "$gp", "$sp", "$ra"})

enum opcodes {
	add, sub, mul, and, or , xor, sll, sra, srl, beq, bne, blt, bgt, ble, bge, jal, \
	lw, sw, reti, in, out, halt
};

int read_asm(int* label_len, int* last_addr, int label_values[], char code[MEM_LEN][MAX_LABEL_LEN + 50],
	char final_mem[MEM_LEN][6], char label_names[MEM_LEN][MAX_LABEL_LEN + 1], char asm_file[]);
int find_and_check_register(char reg[], int dest_or_arg, int i);
int check_if_imm(int reg_num, int dest_or_arg, int opcode);
int parse_labels(int input_len, int label_len, int last_addr, int label_values[], \
	char label_names[MEM_LEN][MAX_LABEL_LEN + 1], char code[MEM_LEN][MAX_LABEL_LEN + 50], \
	char final_mem[MEM_LEN][6]);
void dump_mem(int last_addr, char final_mem[MEM_LEN][6], char mem_file[]);

int read_asm(int* label_len, int* last_addr, int label_values[], char code[MEM_LEN][MAX_LABEL_LEN + 50], \
	char final_mem[MEM_LEN][6], char label_names[MEM_LEN][MAX_LABEL_LEN + 1], char asm_file[]) {
	// The function gets the pointer to number of labels, the last address in memory
	// it gets the arrays of label anmes and values, first parsed code and the final memory
	// it returns the length of the command lines in the code
	FILE* asm_code;
	char code_line[MAX_ROW_LEN + 1], first_arg[MAX_ROW_LEN + 1], address[MAX_ROW_LEN], data[MAX_ROW_LEN];
	char rd[6], rs[6], rt[6], imm[MAX_ROW_LEN];
	int temp, using_imm;
	int i, j, file_len = 0, input_len = 0;
	asm_code = fopen(asm_file, "r");
	if (asm_code == NULL)
		exit(1);
	while (fgets(code_line, MAX_ROW_LEN, asm_code)) {
		first_arg[0] = '\0';
		sscanf(code_line, " %s ", first_arg);// reading first arg of code line to decide line type
		if ((first_arg[0] == '#') || (first_arg[0] == '\0')) // if it not a code ignore
		{
			continue;
		}
		else if (first_arg[0] == '.') { // if it has the type of data
			if (strcmp(first_arg, ".word") == 0) {
				sscanf(code_line, " %s %s %s ", first_arg, address, data);// check that the format fits
				temp = (int)strtol(address, NULL, 0); // write data to memory
				sprintf(final_mem[temp], "%05X", (int)strtol(data, NULL, 0));
				if ((temp + 1) > *last_addr) // update the last address in which there is data
					*last_addr = temp + 1;
			}
			else {
				printf("invalid data");
				exit(1);
			}
		}
		else if (first_arg[strlen(first_arg) - 1] == ':') {//if it is of type label
			first_arg[strlen(first_arg) - 1] = '\0';
			strcpy(label_names[*label_len], first_arg); // save the label value and name in the label array
			label_values[*label_len] = file_len;
			(*label_len)++;//update length
		}
		else { //if it is the type of real command
			i = 0;
			using_imm = 0;
			while (i < 22) {
				if (strcmp(first_arg, OPCODE[i]) == 0) // check if the word is a real opcode, if it is
				{
					//parse the whole line and check if immediate is used
					temp = sscanf(code_line, " %s %5[^ ,] , %5[^ ,] , %5[^ ,] , %s", first_arg, rd, rs, rt, imm);
					using_imm = find_and_check_register(rd, 0, i);
					using_imm = using_imm || find_and_check_register(rs, 1, i);
					using_imm = using_imm || find_and_check_register(rt, 1, i);
					if (using_imm == 1)
						//if immediate is used duplicate the jump
						file_len += 2;
					else
						file_len += 1;
					sprintf(code[input_len], " %s %s, %s, %s, %s", first_arg, rd, rs, rt, imm);
					// save the "clean" line for second scan
					input_len += 1;
					break;
				}
				i++;
			}
			if (i == 22) {
				printf("unknown opcode %s", first_arg);
				exit(1);
			}
		}
	}
	fclose(asm_code);
	return input_len;
}

int find_and_check_register(char reg[], int dest_or_arg, int opcode) {
	// The function gets register name, if it is rd or rs/rt, and opcode
	// The function checks if the reg name is part of the known ones
	// The function returns if immediate is really used in this opcode
	int reg_num = 0;
	reg_num = find_reg(reg);
	if (reg_num == 16) {
		printf("unknown register %s", reg);
		exit(1);
	}
	return check_if_imm(reg_num, dest_or_arg, opcode);
}

int find_reg(char reg[]) {
	// The function gets a reg name and returns reg number, 16 if name is not in the known names
	int reg_num = 0;
	while (reg_num < 16) {
		if (strcmp(reg, REGS[reg_num]) == 0) {
			break;
		}
		reg_num++;
	}
	return reg_num;
}

int check_if_imm(int reg_num, int dest_or_arg, int opcode) {
	// The function gets reg_num, if it is rd or rs/rt, and opcode
	// it returns if the immediate is used by the register
	if (reg_num == 1) {
		if (dest_or_arg == 1) //argument register which is immediate uses immediate
			return 1;
		else { // dest register use immediate just in special cases
			if ((opcode >= beq && opcode <= bge) || (opcode == sw) || (opcode == out))
				return 1;
		}
	}
	return 0;
}

int parse_labels(int input_len, int label_len, int last_addr, int label_values[], char label_names[MEM_LEN][MAX_LABEL_LEN + 1], \
	char code[MEM_LEN][MAX_LABEL_LEN + 50], char final_mem[MEM_LEN][6]) {
	// The function gets the length of "clean" code, the last known address in which there is data in memory
	// arrays for labels values and names and its length, the code and the final memory array
	// The function returns the updated last known address in which there is data in memory
	int file_len = 0, i, j;
	char first_arg[MAX_ROW_LEN + 1], rd[6], rs[6], rt[6], imm[MAX_ROW_LEN];
	int using_imm, final_op, temp;
	for (i = 0; i < input_len; i++) {
		final_op = 0;
		using_imm = 0;
		sscanf(code[i], " %s %5[^ ,] , %5[^ ,] , %5[^ ,] , %s", first_arg, rd, rs, rt, imm); //reading the code line
		j = 0;
		while (j < 22) { //finding the opcode
			if (strcmp(first_arg, OPCODE[j]) == 0) {
				final_op = j << 12; // putting its value in the right place in line
				break;
			}
			j++;
		}
		temp = j;
		j = 0;
		j = find_reg(rd);
		final_op += j << 8;// putting rd value in the right place in line
		using_imm = check_if_imm(j, 0, temp);
		j = 0;
		j = find_reg(rs);
		final_op += j << 4;// putting rs value in the right place in line
		using_imm = using_imm || check_if_imm(j, 1, temp);
		j = 0;
		j = find_reg(rt);
		final_op += j;// putting rt value in the right place in line
		using_imm = using_imm || check_if_imm(j, 1, temp);
		sprintf(final_mem[file_len], "%05X", final_op & 0xfffff);
		file_len += 1;
		if (using_imm == 1) { // if immediate is used we need to add it to the memory too
			temp = (int)strtol(imm, NULL, 0);
			if (temp != 0) { // if the data is value it is just put in place
				sprintf(final_mem[file_len], "%05X", temp & 0xfffff);
			}
			else { // if the data is 0 it can be label or real 0
				j = 0;
				while (j < label_len) { //All labels are checked to see if it fits
					if (strcmp(imm, label_names[j]) == 0) {
						//if there is match the value is aqccording to the corresponding value in the value array
						sprintf(final_mem[file_len], "%05X", label_values[j] & 0xfffff);
						break;
					}
					j++;
				}
				if (j == label_len) // if a matching label was not found, it is a real zero
				{
					sprintf(final_mem[file_len], "%05X", temp & 0xfffff);
				}
			}
			file_len += 1;
		}
		if (file_len > last_addr) // updating last_addr
			last_addr = file_len;
	}
	return last_addr;
}

void dump_mem(int last_addr, char final_mem[MEM_LEN][6], char mem_file[]) {
	// The function gets the last address in which there is data, the memory and the file name of memin
	// The function dumps memory into file
	FILE* memin_file;
	int i;
	memin_file = fopen(mem_file, "w");
	if (memin_file == NULL)
		exit(1);
	for (i = 0; i < last_addr; i++) {
		fprintf(memin_file, "%s\n", final_mem[i]);
	}
	fclose(memin_file);
}
int main(int argc, char* argv[]) {
	// getting input
	int label_len = 0, input_len = 0, last_addr = 0;
	char label_names[MEM_LEN][MAX_LABEL_LEN + 1];
	int label_values[MEM_LEN], i;
	char code[MEM_LEN][MAX_LABEL_LEN + 50]; // each line can be long as label plus the arguments
	char final_mem[MEM_LEN][6];
	for (i = 0; i < MEM_LEN; i++)
		sprintf(final_mem[i], "%05X", 0); // fill memory with zeros
	input_len = read_asm(&label_len, &last_addr, label_values, code, final_mem, label_names, argv[1]); // first iteration to read code
	last_addr = parse_labels(input_len, label_len, last_addr, label_values, label_names, code, final_mem);// second iteration to solve labels
	dump_mem(last_addr, final_mem, argv[2]); //dump memory to file
	return 0;
}