#include<stdio.h>
#include<math.h>

// simulator for RISC-V

// take input from binary file
// save it to memory

// initialise the register + memory

// run each instruction

// save the registers to binary file

int branch(int pc, int offset) {
	printf("branching...\n");
	pc--; // undo the increment (do not go to the next instruction)
	printf("pc: %d to pc %d \n", pc, pc + offset);
	pc = pc + offset; // change the pc by the offset

	return pc;
}

void dec2bin(int c) {
	int i = 0;
	for (i = 31; i >= 0; i--) {
		if ((c & (1 << i)) != 0) {
			printf("1");
		}
		else {
			printf("0");
		}
	}
}

// prints all the register values
void printReg(int reg[]) {
	for (int i = 0; i < 32; i++) {
		printf("%d ", reg[i]);
	}
	printf("\n");
}

// these functions convert from binary 2'compliment to decimal
int twoComp2Dec_20(int a) {
	return (0x80000 & a ? (int)(0x7FFFF & a) - 0x80000 : a);
}

int twoComp2Dec_7(int a) {
	return (0x40 & a ? (int)(0x3F & a) - 0x40 : a);
}

int twoComp2Dec_12(int a) {
	return (0x800 & a ? (int)(0x7FF & a) - 0x800 : a);
}

int main() {

	char file_path[] = "shift.bin";
	printf("Test file: %s \n", file_path);

	unsigned int memory[300000];
	for (int i = 0; i < 300000; i++) { // clear the memory
		memory[i] = 0;
	}
	unsigned int* memory_pointer = &memory[0]; // pointer to the memory array

	int reg[32];
	for (int i = 0; i < 32; i++) { // clear the memory
		reg[i] = 0;
	}

	FILE* ptr_myfile;

	ptr_myfile = fopen(file_path, "rb");
	if (!ptr_myfile) {
		printf("Unable to open file!");
		return 1;
	}

	/* Seek to the beginning of the file */
	fseek(ptr_myfile, 0, SEEK_SET);

	int word_counter = 0;

	while (fread(memory_pointer, sizeof(int), 1, ptr_myfile)) {
		dec2bin(*memory_pointer);
		printf("\n");
		word_counter++;
		memory_pointer++;
	}
	fclose(ptr_myfile);
	printf("Read %d words from the input file. \n", word_counter);

	// RUNNING THE SIMULATOR

	int pc = 0; // set pc to zero to start

	printf("Starting simulation... \n");
	while (pc < word_counter) {
		printf("Start of instruction. pc: %d.\n", pc);

		// decoding:
		unsigned int instr = memory[pc];
		unsigned int opcode = instr & 0x7f;
		int rd = (instr >> 7) & 0x1f;
		int rs1 = (instr >> 15) & 0x1f;
		int rs2 = (instr >> 20) & 0x1f;
		unsigned int funct3 = (instr >> 12) & 0x7;
		unsigned int funct7 = (instr >> 25);
		unsigned int imm_25_31 = (instr >> 25);
		unsigned int imm_20_31 = (instr >> 20);
		unsigned int imm_12_31 = (instr >> 12);
		unsigned int x = 0;
		int offset = 0;
		int address = 0;
		int word_address = 0;

		// signed versions of the immediate
		int imm_12_31_s = twoComp2Dec_20(imm_12_31);//signed immediate
		int imm_25_31_s = twoComp2Dec_7(imm_25_31);
		int imm_20_31_s = twoComp2Dec_12(imm_20_31);

		// increment the pc for the next instruction
		pc++;

		// x0 should be zero always
		reg[0] = 0;

		switch (opcode) {
		case 0x73: // e-call
			// exit:
			if (reg[10] == 10) {
				// exits program
				printf("Exiting through ecall...\n");
				return 0; // exit main function
			}
			break;

		case 0x17: // AUIPC 0010111
			printf("AUIPC\n");
            reg[rd] = (imm_12_31 << 12) + (pc-1); // rd = offset + pc
			break;

		case 0x6F: // JAL 1101111
			printf("JAL\n");

			// work out offset here
			unsigned int j_imm11 = (instr >> 20) & 0x1; // 1 bit
			unsigned int j_imm19_12 = (instr >> 12) & 0xFF; // 8 bits
			unsigned int j_imm10_1 = (instr >> 21) & 0x3FF; // 10 bits
			unsigned int j_imm20 = (instr >> 31) & 0x1; // 1 bit

			offset = 0b0 + (j_imm10_1 << 1) + (j_imm11 << 11) + (j_imm19_12 << 12) + (j_imm20 << 20);
			// is unsigned (needs to be signed for jumping back)
			offset = twoComp2Dec_20(offset); // converts to signed

			// divide by 4 because of bytes -> words
			offset = offset / 4;
			printf("Offset: %d \n", offset);

			// stores the pc+1 to rd
			reg[rd] = pc;

			pc = branch(pc, offset);

			break;

		case 0x67: // JALR 1101111
			printf("JALR\n");

			// stores the pc+1 to rd
			reg[rd] = pc;

			address = reg[rs1] + imm_20_31_s;
			address = address & 0b0; // set the least-significant bit of the result to zero

			pc = address; // jump

			break;

		case 0x23: //store 0100011
			printf("Store: \n");
            offset = rd + (funct7 << 5); // 12bits
            offset = twoComp2Dec_12(offset);
            address = reg[rs1] + offset; // byte address

            printf("stack pointer: %d   ", reg[rs1]);
            printf("offset: %d  ", offset);

            word_address = floor(address / 4); // byte to word address
            printf("word address: %d\n",word_address);

            int s_offset = 0;
            unsigned int word = 0;

			switch (funct3) {
			case 0b000: //SB
			    printf("byte\n");
				s_offset = address % 4; // byte offset

				unsigned int store_byte = reg[rs2] & 0xFF; // taking the byte from the register
				word = memory[word_address]; // load word from memory

				switch (s_offset) {
				case 0:
					// shift by 3 bytes
					store_byte = (store_byte << 24);
					word = word & 0x00FFFFFF; // clear the memory to be stored to
					break;

				case 1:
					// shift by 2 bytes
					store_byte = (store_byte << 16);
					word = word & 0xFF00FFFF; // clear the memory to be stored to
					break;

				case 2:
					// shift by 1 byte
					store_byte = (store_byte << 8);
					word = word & 0xFFFF00FF; // clear the memory to be stored to
					break;

				case 3:
					// shift by 0 bytes
					store_byte = (store_byte << 0);
					word = word & 0xFFFFFF00; // clear the memory to be stored to
					break;
				}

				memory[word_address] = word | store_byte; // placing the memory to be stored in the word and then to memory
				break;

			case 0b001://SH
			    printf("halfword\n");
				s_offset = address % 2; // halfword offset

				unsigned int store_halfword = reg[rs2] & 0xFFFF; // taking the halfword from the register
				word = memory[word_address]; // load word from memory

				switch (s_offset) {
				case 0:
					// shift by 2 bytes
					store_halfword = (store_halfword << 16);
					word = word & 0x0000FFFF; // clear the memory to be stored to
					break;

				case 1:
					// shift by 0 bytes
					store_halfword = (store_halfword << 0);
					word = word & 0xFFFF0000; // clear the memory to be stored to
					break;
				}

				memory[word_address] = word | store_halfword; // placing the memory to be stored in the word and then to memory
				break;

			case 0b010://SW
			    printf("word\n");
				memory[word_address] = reg[rs2]; // save to memory
				break;

            default:
                printf("error...\n");
			}
			break;

		case 0x3: // load 0000011
			printf("Load\n");
			offset = imm_20_31;
			offset = twoComp2Dec_12(offset);
			address = reg[rs1] + offset;
			printf("stack pointer: %d   ", reg[rs1]);
            printf("offset: %d   ", offset);

            word_address = floor(address / 4); // byte to word address
			printf("word address: %d\n",word_address);
			int l_offset = 0;

			switch (funct3) {
			case 0b000://LB
			    printf("byte\n");
				l_offset = address % 4;
				int load_byte = 0;
				switch (l_offset) {

				case 0:
					load_byte = memory[word_address] & 0x000000FF;
					//if the 8th bit is 1, sign extend.
					if ((load_byte & 0x80) == 0x80) {
						load_byte = (load_byte | 0xFFFFFF00); //sign extension
					}
                    reg[rd] = load_byte;
					break;

				case 1:
					load_byte = memory[word_address] & 0x0000FF00;
					//if the 16th bit is 1, sign extend.
					load_byte = load_byte >> 8;
					if ((load_byte & 0x80) == 0x80) {
						load_byte = (load_byte | 0xFFFFFF00); //sign extension
					}
					reg[rd] = load_byte;
					break;

				case 2:
					load_byte = memory[word_address] & 0x00FF0000;
					//if the 24th bit is 1, sign extend.
					load_byte = load_byte >> 16;
					if ((load_byte & 0x80) == 0x80) {
						load_byte = (load_byte | 0xFFFFFF00); //sign extension
					}
					reg[rd] = load_byte;
					break;

				case 3:
					load_byte = memory[word_address] & 0xFF000000;
					//if the 32th bit is 1, sign extend.
					load_byte = load_byte >> 24;
					if ((load_byte & 0x80) == 0x80) {
						load_byte = (load_byte | 0xFFFFFF00); //sign extension
					}
                    reg[rd] = load_byte;
                    break;
				}

			case 0b001://LH
			    printf("halfword\n");
				l_offset = address % 2;
				int load_half = 0;
				switch (l_offset) {

				case 0:
					load_half = memory[word_address] & 0x0000FFFF;
					//if the 16th bit is 1, sign extend.
					if ((load_half & 0x8000) == 0x8000) {
						load_half = load_half | 0xFFFF0000;
						reg[rd] = load_half;
					}
					else {
						reg[rd] = load_half;
					}

					break;

				case 1:
					load_half = memory[word_address] & 0xFFFF0000;
					//if the last 16 bits are to be uploaded, no sign extension required.
					reg[rd] = load_half;
					break;
				}
				break;

			case 0b010://LW
			    printf("word\n");
				reg[rd] = memory[word_address]; //load to rd
				break;

			case 0b100://LBU
			    //unsigned int load_byte_u;
				//load_byte_u = (memory[address] & 0x000000FF);
				//reg[rd] = load_byte_u;
				break;

			case 0b101://LHU
				//unsigned int load_halfword_u = (memory[address] & 0x0000FFFF);
				//reg[rd] = load_halfword_u;
				break;
			}
			break;

		case 0x13: //instructions with immediate 0010011
			switch (funct3) {
			case 0b000:
				//addi
				//immediate can be signed
				printf("addi \n");
				printf("imm = : %d \n", imm_20_31_s);

				reg[rd] = reg[rs1] + imm_20_31_s;
				break;
			case 0b001:
				//slli- shift left logical immediate
				printf("slli   ");
				printf("shamt: %d \n", rs2);
				reg[rd] = reg[rs1] << rs2; // rs2 = shamt
				break;
			case 0b010:
				//slti- set less than immediate
				if (reg[rs1] < imm_20_31_s) {
					reg[rd] = 1;
				}
				else {
					reg[rd] = 0;
				}
				break;
			case 0b011:
				//sltiu-set less than immediate unsigned
				//*****************
				if (reg[rs1] > imm_20_31) {
					reg[rd] = 1;
				}
				else {
					reg[rd] = 0;
				}
				break;
			case 0b100:
				//xori
				reg[rd] = reg[rs1] ^ imm_20_31_s;

			case 0b101://******************
				//srli and srai- shift right logical and arithmetic immediate
				printf("sr__ \n");
				//SHAMPT as rs2

				if (funct7 == 0b0000000) { // logical
					printf("srli  ");
					printf("shamt: %d \n", rs2);
					reg[rd] = reg[rs1] >> rs2; // rs2 = shamt
				}

				if (funct7 == 0b0100000) { //arithmetic
					printf("srai   ");
					printf("shamt: %d \n", rs2);
					x = reg[rs1] & 0x80000000;//accessing the first digit of the number

					if (x == 0x80000000) {
						for (int i = 0; i < rs2; i++) {
							reg[rd] = reg[rs1] >> 1; // shift by 1
							reg[rd] = reg[rd] | x; // adding ones to start
						}
					}
					else {
						reg[rd] = reg[rs1] >> reg[rs2];
					}
				}
				break;
			case 0b110:
				//ori
				reg[rd] = reg[rs1] | imm_20_31_s;

			case 0b111:
				//andi
				reg[rd] = reg[rs1] & imm_20_31_s;
			default:
				printf("ERROR funct3 not recognised... \n");
			}
			break;

		case 0x33: // 0110011
			switch (funct3) {
			case 0b000://
				//add
				printf("add \n");
				if (funct7 == 0b0000000) {
					//add
					reg[rd] = reg[rs1] + reg[rs2];
				}
				if (funct7 == 0b0100000) {
					//sub
					reg[rd] = reg[rs1] - reg[rs2];
				}
				break;
			case 0b001:
				//sll
				//shift left logical-unsigned
				reg[rd] = reg[rs1] << reg[rs2]; // FIX
				break;
			case 0b010:
				//slt-set less than. slt rd, rs1, rs2.
				//rd is 1 if rs1<rs2
				if (reg[rs1] < reg[rs2]) {
					reg[rd] = 1;
				}
				else {
					reg[rd] = 0;
				}
				break;
			case 0b011:
				//sltu-unsigned
				//*****************

				if (reg[rs1] < reg[rs2]) {
					reg[rd] = 1;
				}
				else {
					reg[rd] = 0;
				}
				break;
			case 0b100:
				//xor
				reg[rd] = reg[rs1] ^ reg[rs2];
				break;

			case 0b101:
				//srl and  sra

				if (funct7 == 0b0000000) {
					//srl-logical-unsigned
					reg[rd] = reg[rs1] >> reg[rs2];
				}
				//sra-should be signed
				if (funct7 == 0b0100000) {
					if (reg[rs1] > 0) {
						reg[rd] = reg[rs1] >> reg[rs2];
					}
					else {
						x = reg[rs1] & 0x80000000;
						reg[rd] = reg[rs1] >> reg[rs2];

						reg[rd] = reg[rd] | x;
					}
				}
				break;
			case 0b110:
				//or
				reg[rd] = reg[rs1] | reg[rs2];
				break;
			case 0b111:
				//and
				reg[rd] = reg[rs1] & reg[rs2];
				break;
			}
			break;

		case 0x37://lui	110111
			printf("lui inst \n");
			reg[rd] = imm_12_31 << 12;
			break;

		case 0x63: // branch 1100011
			printf("branch \n");
			// work out offset here
			unsigned int imm11 = (instr >> 7) & 0x1; // 1 bit
			unsigned int imm4_1 = (instr >> 8) & 0xF; // 4 bits
			unsigned int imm10_5 = (instr >> 25) & 0x3F; // 6 bits
			unsigned int imm12 = (instr >> 31) & 0x1; // 1 bit

			offset = 0b0 + (imm4_1 << 1) + (imm10_5 << 5) + (imm11 << 11) + (imm12 << 12);
			// is unsigned (needs to be signed for branching back)
			offset = twoComp2Dec_12(offset); // converts to signed

			// divide by 4 because of bytes -> words
			offset = offset / 4;

			printf("Offset: %d \n", offset);

			switch (funct3) {
			case 0b000: //BEQ
				printf("BEQ \n");
				if (reg[rs1] == reg[rs2]) {
					// take branch
					pc = branch(pc, offset);
				}
				break;
			case 0b001: // BNE
				printf("BNE \n");
				if (reg[rs1] != reg[rs2]) {
					// take branch
					pc = branch(pc, offset);
				}
				break;
			case 0b100: // BLT
				printf("BLT \n");
				if (reg[rs1] < reg[rs2]) {
					// take branch
					pc = branch(pc, offset);
				}
				break;
			case 0b101: // BGE
				printf("BGE \n");
				if (reg[rs1] >= reg[rs2]) {
					// take branch
					pc = branch(pc, offset);
				}
				break;
			case 0b110: // BLTU
				printf("BLTU \n");
				if ((unsigned)reg[rs1] < (unsigned)reg[rs2]) {
					// take branch
					pc = branch(pc, offset);
				}
				break;
			case 0b111: // BGEU
				printf("BGEU \n");
				if ((unsigned)reg[rs1] >= (unsigned)reg[rs2]) {
					// take branch
					pc = branch(pc, offset);
				}
				break;
			}
			break;

		default:
			printf("Opcode %d not yet implemented \n", opcode);
		}


		printReg(reg);
	}
	return 0;
}
