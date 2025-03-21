#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ROW_LEN 500
#define MEM_SIZE (1<<12)
#define INPUT_ARG_NUM 14
#define OPCODE ((char const*[]){ "add", "sub", "mul", "and", "or", "xor", "sll", "sra", "srl", "beq",\
"bne", "blt", "bgt", "ble", "bge", "jal", "lw", "sw", "reti", "in", "out", "halt" })
#define REGS ((char const*[]){ "$zero", "$imm", "$v0", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",\
"$s0", "$s1", "$s2", "$gp", "$sp", "$ra"})
#define IO_REGS ((char const*[]){ "irq0enable", "irq1enable", "irq2enable", "irq0status", "irq1status",\
 "irq2status", "irqhandler", "irqreturn", "clks", "leds","display7seg", "timerenable", "timercurrent",\
"timermax", "diskcmd", "disksector", "diskbuffer", "diskstatus", "reserved", "reserved","monitoraddr",\
"monitordata","monitorcmd"})
#define REGS ((char const*[]){ "$zero", "$imm", "$v0", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",\
"$s0", "$s1", "$s2", "$gp", "$sp", "$ra"};
enum opcodes {
	add, sub, mul, and, or , xor, sll, sra, srl, beq, bne, blt, bgt, ble, bge, jal, \
	lw, sw, reti, in, out, halt
};
enum files_names {
	memin, diskin, irq2in, memout, regout , trace, hwregtrace, cycles, leds_file, display7seg_file, diskout,\
	monitor_txt, monitoryuv
};
enum IO_names {
	irq0enable, irq1enable, irq2enable, irq0status, irq1status, irq2status, irqhandler, irqreturn, clks,\
	leds, display7seg, timerenable, timercurrent, timermax, diskcmd, disksector, diskbuffer,
	diskstatus, reserved, reserved2, monitoraddr, monitordata, monitorcmd
};

void CheckArg(int argc);
void LoadFiles(char* Filenames[], FILE* files[]);
int ReadIRQ2Cycles(int** irq2_cycles, FILE* irq2in);
void Readdisk(int disk[], FILE* diskin);
void Readmem(int mem[], FILE* memin);
void CheckIRQ(int* PC, int irq, int IO_regs[], int* is_irq_handler);
void IO_update(int IO_regs[], int* cycle_for_disk_end, int* irq2_cycles, int irq2_len);
void LoadInstructionData(int mem[], int* opcode, int* rd, int* rs, int* rt, int* imm, int* PC, FILE* trace);
void ExecuteCommand(int Opcode, int rd, int rs, int rt, int imm, int* PC, int* is_irq_handler, int* is_halt, int* cycle_for_disk_end,
	int RegArray[],int IO_regs[], int mem[], int monitor[256][256], int disk[], FILE* trace, FILE* hwregtrace, FILE* leds_file_name, FILE* display);
void inout_inst(int in_or_out, int rd, int rs, int rt, int* cycle_for_disk_end, int RegArray[], int IO_regs[],\
	int monitor[256][256], int disk[], int mem[], FILE* hwregtrace, FILE* leds_file_name, FILE* display);
void finish_program(int mem[], int disk[], int Reg_Array[], int IO_regs[], int monitor[256][256],
	FILE* memout, FILE* regout, FILE* cycles, FILE* diskout, FILE* monitor_file, FILE* monitoryuv);
void print_to_last_val(int mem[], int len, FILE* file);

void CheckArg(int argc) {
	// The function gets the number of arguments that were used when calling the program, 
	// and makes sure it is the number expected. elsewise it halts the program
	if (argc != (INPUT_ARG_NUM)) {
		printf("Wrong number of inputs!!!!!!!\n%d\n\n\n",argc);
		exit(1);
	}

}

void LoadFiles(char* Filenames[], FILE* files[]) {
	// The function gets a list of files and file names and opens them one by one
	// The first 3 files are for read and the others are for write.
	// The filenames are the program arguments so it is off by 1 because the first argument is the program name
	int i = 0;
	for (i = 0; i < 3; i++) {
		files[i] = fopen(Filenames[i + 1], "r");
	}
	for (i = 3; i < (INPUT_ARG_NUM-1); i++) {
		files[i] = fopen(Filenames[i + 1], "w");
	}
}

int ReadIRQ2Cycles(int** irq2_cycles, FILE* irq2in) {
	// The function gets the pointer to the array that should hold the cycles in which the irq2 interrupt raises
	// The function gets the file that holds the irq2 inputs and saves them in the array.
	// The function returns the length of the array
	int temp = 4,i = 0, tmp2;
	char irq2_line[MAX_ROW_LEN];
	*irq2_cycles = (int*)malloc(temp * sizeof(int));
	while (fgets(irq2_line, MAX_ROW_LEN, irq2in)) {
		if (i >= temp) { // dynamic relocation if space is too small
			temp *= 2;
			*irq2_cycles = (int*)realloc(*irq2_cycles, temp * sizeof(int));
		}
		tmp2 = (int)strtol(irq2_line,NULL,10);
		if (tmp2 > 0) { //Assuming that the interrupt doesnwt happen in the beginning of the processor cycle,
			// 0 indicates a non-numeric value in the line so it should be ignored
			(*irq2_cycles)[i] = tmp2;
			i++;
		}
	}
	return i;
}
void Readdisk(int disk[], FILE* diskin) {
	// The function gets the disk array and the diskin file, 
	// and copies the numeric val of the disk content to the array
	char disk_line[7];
	int i=0;
	while (fgets(disk_line, 7, diskin)) {
		disk_line[6] = '\0';
		disk[i] = (int)strtol(disk_line,NULL,16);
		i++;
	}
}
void Readmem(int mem[], FILE* memin) {
	// The function gets the memory array and the memin file, 
	// and copies the numeric val of the memory content to the array
	char mem_line[7];
	int i = 0;
	while (fgets(mem_line, 7, memin)) {
		mem_line[6] = '\0';
		mem[i] = (int)strtol(mem_line, NULL, 16);
		i++;
	}
}
void CheckIRQ(int* PC, int irq, int IO_regs[], int* is_irq_handler) {
	// The function gets PC pointer, the irq indicator, IO regs array and irq handler indicator
	// The function decides if the next PC shold jump to the irqhandler (if irq and not in irqhandler already)
	// If the PC jumps, it updates the relevant IO-reg with the return address
	if (irq && !(*is_irq_handler)) {
		IO_regs[irqreturn] = *PC;
		*PC = IO_regs[irqhandler];
		*is_irq_handler = 1;
	}
}
void IO_update(int IO_regs[], int* cycle_for_disk_end, int* irq2_cycles, int irq2_len) {
	// The function gets the IO regs array, the cycle that disk interaction should end with, the irq2 interrrupt
	// cycles array and its len, and decides how to change the relevant IO regs according to number of cycles and
	// the cycles in which interrupts should happen
	int i=0;
	IO_regs[monitorcmd] = 0;
	if (IO_regs[timerenable]) { 
		if (IO_regs[timercurrent] == IO_regs[timermax]) {
			//if timer is enabled and reaches maximum, it is zeroed and raises irq0
			IO_regs[timercurrent] = 0;
			IO_regs[irq0status] = 1;
		}
		else {//if timer is enabled and doesn't reach maximum, it proceeds
			IO_regs[timercurrent] += 1;
		}
	}
	if (IO_regs[clks] == *cycle_for_disk_end) { 
		//when disk ends copying the cmd and ataus are 0 
		//and irq1 status is 1
		IO_regs[irq1status] = 1;
		IO_regs[diskcmd] = 0;
		IO_regs[diskstatus] = 0;
	}
	while (i < irq2_len) {
		if (irq2_cycles[i] == IO_regs[clks]) {
			IO_regs[irq2status] = 1;
			break;
		}
		i++;
	}

}

void LoadInstructionData(int mem[], int* opcode, int* rd, int* rs, int* rt, int* imm, int* PC, FILE* trace) {
	// The function gets the memory, the pointer for assembly line arguments, PC, and trace file
	// The function reads and parse the code line from memory and decides if there was a use in immediate
	// and the PC should proceed by 2.
	// The instruction and PC are printed to the trace file
	int instruction = 0;
	instruction = mem[*PC];
	fprintf(trace, "%03X %05X ", *PC, instruction);
	*opcode = (instruction >> 12) & 0xff;
	*rd = (instruction >> 8) & 0xf;
	*rs = (instruction >> 4) & 0xf;
	*rt = instruction & 0xf;
	*PC += 1;
	*imm = 0; 
	if (((*rd == 1) && ((*opcode >= beq && *opcode <= bge) || (*opcode == sw) || (*opcode == out)))||(*rs==1)||\
		(*rt==1)) {
		// The uses of immediate in the function are in rs and rt, and in specific cases in rd
		// The Pc is proceeded by another 1 if there is an immediate use
		*imm = mem[*PC] + (mem[*PC] >> 19) * 0xfff00000; // sign extension
		*PC += 1;
	}
}

void ExecuteCommand(int Opcode, int rd, int rs, int rt, int imm, int* PC, int* is_irq_handler, int* is_halt,
	int* cycle_for_disk_end, int RegArray[], int IO_regs[],int mem[], int monitor[256][256], int disk[],\
	FILE* trace, FILE* hwregtrace, FILE* leds_file_name, FILE* display) {
	// The function gets the code arguments, the PC, the irq handler, disk end and halt indicators, the reg array
	// and the IO reg array, the memory, monitor and disk, and some files
	// The function executes the command and updates the relevant files,arrays and indicators
	int i = 0,temp=0;
	RegArray[0] = 0;
	RegArray[1] = imm;
	// First the reg array status is printed in trace
	for (i = 0; i < 15; i++) {
		fprintf(trace, "%08X ", RegArray[i]);
	}
	fprintf(trace, "%08X\n", RegArray[15]);
	switch (Opcode) { // The opcode detrmines which functionality to do
		case add:
			RegArray[rd] = (RegArray[rs] + RegArray[rt])&0xffffffff;
			break;
		case sub:
			RegArray[rd] = (RegArray[rs] - RegArray[rt]) & 0xffffffff;
			break;
		case mul:
			RegArray[rd] = (RegArray[rs] * RegArray[rt]) & 0xffffffff;
			break;
		case and:
			RegArray[rd] = (RegArray[rs] & RegArray[rt]) & 0xffffffff;
			break;
		case or:
			RegArray[rd] = (RegArray[rs] | RegArray[rt]) & 0xffffffff;
			break;
		case xor:
			RegArray[rd] = (RegArray[rs] ^ RegArray[rt]) & 0xffffffff;
			break;
		case sll:
			RegArray[rd] = (RegArray[rs] << RegArray[rt]) & 0xffffffff;
			break;
		case sra: // the default shift right for int is arithmtic
			RegArray[rd] = (RegArray[rs] >> RegArray[rt]) & 0xffffffff;
			break;
		case srl: // the sign extension is replaced by zeros in logical right shift
			RegArray[rd] = (RegArray[rs] >> RegArray[rt]) & (0xffffffff >> (RegArray[rt]));
			break;
		case beq: 
			if (RegArray[rs] == RegArray[rt])
				*PC = RegArray[rd] & 0xfff; // PC size is 12 bits
			break;
		case bne:
			if (RegArray[rs] != RegArray[rt])
				*PC = RegArray[rd] & 0xfff;// PC size is 12 bits
			break;
		case blt:
			if (RegArray[rs] < RegArray[rt])
				*PC = RegArray[rd] & 0xfff;// PC size is 12 bits
			break;
		case bgt:
			if (RegArray[rs] > RegArray[rt])
				*PC = RegArray[rd] & 0xfff;// PC size is 12 bits
			break;
		case ble:
			if (RegArray[rs] <= RegArray[rt])
				*PC = RegArray[rd] & 0xfff;// PC size is 12 bits
			break;
		case bge:
			if (RegArray[rs] >= RegArray[rt])
				*PC = RegArray[rd] & 0xfff;// PC size is 12 bits
			break;
		case jal:
			RegArray[rd] = *PC;
			*PC = RegArray[rs]&0xfff;
			break;
		case lw:
			temp = mem[(RegArray[rs] + RegArray[rt]) & 0xfff]; // We keep just the 12 lsbs to read 
			//from 12-bit memory size
			RegArray[rd] = temp | 0xfff00000*(temp >> 19); // the input is 20 bit, 
			//so the 20th bit is taken and it is replicated for sign extension
			break;
		case sw:
			mem[(RegArray[rs] + RegArray[rt]) & 0xfff] = RegArray[rd] & 0xfffff; // 20 lsbs are saved
			break;
		case reti:
			// reti indicates the end of irqhandler
			*is_irq_handler = 0;
			*PC = IO_regs[irqreturn] & 0xfff;
			break;
		case in:
			// in and out instructions are handled specially
			inout_inst(0, rd, rs, rt, cycle_for_disk_end, RegArray, IO_regs, \
				monitor, disk, mem, hwregtrace, leds_file_name, display);
			break;
		case out:
			// in and out instructions are handled specially
			inout_inst(1, rd, rs, rt, cycle_for_disk_end, RegArray, IO_regs, \
				monitor, disk, mem, hwregtrace, leds_file_name, display);
			break;
		case halt:
			// halt just updates the halt indicator
			*is_halt = 1;
			break;
		default:
			printf("Invalid case\n");



	}
}

void inout_inst(int in_or_out, int rd, int rs, int rt, int* cycle_for_disk_end, int RegArray[], int IO_regs[],\
	int monitor[256][256], int disk[], int mem[], FILE* hwregtrace, FILE* leds_file_name, FILE* display) {
	// The function gets the in or out indicator, the code line arguments, the cycle for disk end pointer,
	// the reg array, IO regs array and monitor array, the disk and memory arrays, and leds, hwregtrace and 
	//display files
	// The function updated the IO-regs according to the code line, and updates the desired indicators, arrays
	// and files
	int temp;
	if (in_or_out == 0) { //in command
		RegArray[rd] = IO_regs[RegArray[rs] + RegArray[rt]];
		fprintf(hwregtrace, "%d READ %s %08X\n", IO_regs[8], IO_REGS[RegArray[rs] + RegArray[rt]], \
			IO_regs[RegArray[rs] + RegArray[rt]]);
	}
	else { // out command
		if ((RegArray[rs] + RegArray[rt]) == leds) { //updating leds
			temp = IO_regs[RegArray[rs] + RegArray[rt]] ^ RegArray[rd]; // check if there was a change
			if (temp != 0) // if there was a change update in file
				fprintf(leds_file_name, "%d %08X\n", IO_regs[8], RegArray[rd]);
		}
		else if ((RegArray[rs] + RegArray[rt]) == display7seg) {  //updating display7seg
			temp = IO_regs[RegArray[rs] + RegArray[rt]] ^ RegArray[rd];// check if there was a change
			if (temp != 0)// if there was a change update in file
				fprintf(display, "%d %08X\n", IO_regs[8], RegArray[rd]);
		}
		else if ((RegArray[rs] + RegArray[rt]) == diskcmd) { //reading or writing from disk
			if (RegArray[rd] != 0) // if a valid command, update that the copy will be ended in 1024 cycles
				IO_regs[diskstatus] = 1;
				*cycle_for_disk_end = IO_regs[clks] + 1024;
			if (RegArray[rd] == 1) { // if read from disk, the buffer in memory will be filled with 128 words
				// from disk sector
				for (int i = 0; i < 128; i++)
					mem[IO_regs[diskbuffer] + i] = disk[IO_regs[disksector] * 128 + i];
			}
			else if (RegArray[rd] == 2) { // if write from disk, the disk sector will be filled with 128 words
				// from buffer in memory
				for (int i = 0; i < 128; i++)
					 disk[IO_regs[disksector] * 128 + i] = mem[IO_regs[diskbuffer] + i];
			}
		}
		else if ((RegArray[rs] + RegArray[rt]) == monitorcmd) {
			if (RegArray[rd] == 1) // if writing to the monitor
				monitor[(IO_regs[monitoraddr]>>8)&0xff][IO_regs[monitoraddr]&0xff] = IO_regs[monitordata]; 
			// the address is parsed to 2-D address, each byte for different axis
		}
		IO_regs[RegArray[rs] + RegArray[rt]] = RegArray[rd];
		fprintf(hwregtrace, "%d WRITE %s %08X\n", IO_regs[8], IO_REGS[RegArray[rs] + RegArray[rt]], \
			RegArray[rd]);
	}
	

}

void finish_program(int mem[], int disk[], int Reg_Array[], int IO_regs[], int monitor[256][256],\
	FILE* memout, FILE* regout, FILE* cycles, FILE* diskout, FILE* monitor_file, FILE* monitoryuv) {
	// The function gets all the arrays of the program and some files
	// THe function dumps the arrays into files, and stops in last value that is different from 0
	int i=0,j=0,last_val_i=0, last_val_j=0;
	fprintf(cycles, "%d\n", IO_regs[8]);
	for(i=2;i<16;i++) // printing the reg array that is not zero or immediate
		fprintf(regout, "%08X\n", Reg_Array[i]);
	print_to_last_val(mem, 1 << 12, memout); // printing memory
	print_to_last_val(disk, 1 << 14, diskout); // printing disk
	for (i = 0; i < 256; i++) { // finding last monitor value that is not zero and printing the monitor yuv
		for (j = 0; j < 256; j++) {
			if (monitor[i][j] != 0) {
				last_val_i = i;
				last_val_j = j;
			}
			fprintf(monitoryuv, "%c", monitor[i][j]);
		}
	}
	for (i = 0; i < last_val_i; i++) { // printing the lines before the last value
		for (j = 0; j < 256; j++) {
			fprintf(monitor_file, "%02X\n", monitor[i][j]);
		}
	}
	for (j = 0; j <= last_val_j; j++) { // printing the line of the last value
		fprintf(monitor_file, "%02X\n", monitor[last_val_i][j]);
	}
}


void print_to_last_val(int mem[], int len, FILE* file) {
	// The function gets array and its len, with a file to dump the array to
	// The function prints the array to the file, until the last non-zero value
	int last_val = 0,i;
	for (i = 0; i < len; i++) { // finding last value
		if (mem[i] != 0)
			last_val = i;
	}
	for (i = 0; i <= last_val; i++) {  // printing to last value
		fprintf(file, "%05X\n", mem[i]);
	}
}

int main(int argc, char* argv[]) {
	FILE* files[INPUT_ARG_NUM-1];
	int memory[MEM_SIZE] = { 0 };
	int PC = 0, rd, rs, rt, imm, opcode, next_disk_cycle = -1, cycle_for_disk_end=-1, is_irq_handler=0\
		,irq=0, is_halt=0, irq2_cycles_len=0, instruction = 0, i = 0;
	int monitor[256][256] = { 0 };
	int* irq2_cycles=NULL;
	int Reg_Array[16] = { 0 }, IO_regs[23] = { 0 }, disk[16384] = { 0 };
	CheckArg(argc); // starting with basuc check and loading files and memories
	LoadFiles(argv, files);
	irq2_cycles_len = ReadIRQ2Cycles(&irq2_cycles, files[irq2in]);
	Readmem(memory, files[memin]);
	Readdisk(disk, files[diskin]);
	while (!is_halt) { // if not halted
		CheckIRQ(&PC, ((IO_regs[0] && IO_regs[3]) || (IO_regs[1] && IO_regs[4]) || (IO_regs[2] && IO_regs[5])),\
			IO_regs, &is_irq_handler); // update PC according to IRQ
		LoadInstructionData(memory, &opcode, &rd, &rs, &rt, &imm, &PC, files[5]); // Load instruction data
		ExecuteCommand(opcode, rd, rs, rt, imm, &PC, &is_irq_handler, &is_halt, &cycle_for_disk_end,
			Reg_Array, IO_regs, memory, monitor, disk, files[trace], files[hwregtrace], files[leds_file],\
			files[display7seg_file]); // execute command
		IO_update(IO_regs, &cycle_for_disk_end, irq2_cycles, irq2_cycles_len); // update the IOs
		IO_regs[clks] += 1; // proceed the clk
	}
	finish_program(memory, disk, Reg_Array, IO_regs, monitor, files[memout],\
		files[regout],files[cycles],files[diskout],files[monitor_txt],files[monitoryuv]); // dump files
	for (i = 0; i < INPUT_ARG_NUM-1; i++)
		fclose(files[i]); //close all the files
	free(irq2_cycles); // clear the dynamin memory
	return 0;
}

