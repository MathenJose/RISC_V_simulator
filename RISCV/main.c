#include<stdio.h>

// simulator for RISC-V

// take input from binary file
// save it to memory

// initialise the register + memory

// run each instruction

// save the registers to binary file

int branch(int pc, int offset){
    printf("branching...\n");
    pc--; // undo the increment (do not go to the next instruction)
    printf("pc: %d to pc %d \n", pc, pc+offset);
    pc = pc + offset; // change the pc by the offset

    return pc;
}

void dec2bin(int c){
   int i = 0;
   for(i = 31; i >= 0; i--){
     if((c & (1 << i)) != 0){
       printf("1");
     }else{
       printf("0");
     }
   }
}

// prints all the register values
void printReg(int reg[]){
    for(int i = 0; i < 32; i++){
        printf("%d ", reg[i]);
    }
    printf("\n");
}

// these functions convert from binary 2'compliment to decimal
int twoComp2Dec_20(int a){
    return (0x80000&a ? (int)(0x7FFFF&a)-0x80000 : a);
}

int twoComp2Dec_7(int a){
    return (0x40&a ? (int)(0x3F&a)-0x40 : a);
}

int twoComp2Dec_12(int a){
    return (0x800&a ? (int)(0x7FF&a)-0x800 : a);
}

int main(){

    char file_path[] = "branchmany.bin";
    printf("Test file: %s \n", file_path);

    unsigned int memory [1000];
    for(int i = 0; i < 1000; i++){ // clear the memory
        memory[i] = 0;
    }
    unsigned int * memory_pointer = &memory[0]; // pointer to the memory array

    int reg [32];
    for(int i = 0; i < 32; i++){ // clear the memory
        reg[i] = 0;
    }

    FILE *ptr_myfile;

    ptr_myfile=fopen(file_path,"rb");
    if (!ptr_myfile){
        printf("Unable to open file!");
        return 1;
    }

    /* Seek to the beginning of the file */
    fseek(ptr_myfile, 0, SEEK_SET);

    int word_counter = 0;

    while(fread(memory_pointer, sizeof(int), 1, ptr_myfile)){
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
    while(pc < word_counter){
        printf("Start of instruction. pc: %d.\n",pc);

    // decoding:
        unsigned int instr = memory[pc];
        unsigned int opcode = instr & 0x7f;
        int rd = (instr >> 7) & 0x1f;
        int rs1 = (instr >> 15) & 0x1f;
        int rs2 = (instr >> 20) & 0x1f;
        unsigned int funct3 = (instr>> 12) & 0x7;
        unsigned int funct7 = (instr>> 25);
        unsigned int imm_25_31 = (instr>> 25);
        unsigned int imm_20_31 = (instr >> 20);
        unsigned int imm_12_31 = (instr >> 12);
        unsigned int x = 0;

        // signed versions of the immediate
        int imm_12_31_s = twoComp2Dec_20(imm_12_31);//signed immediate
		int imm_25_31_s = twoComp2Dec_7(imm_25_31);
        int imm_20_31_s = twoComp2Dec_12(imm_20_31);

        pc ++;


        switch (opcode) {

			case 0x73: // e-call
				// exit:
				if (reg[10] == 10){
                    // exits program
                    printf("Exiting through ecall...");
                    return 0; // exit main function
				}
				break;

			case 0x23: //load and store 0100011

				// TODO
				break;

			case 0x13: //instructions with immediate 0010011
				switch(funct3){
				 	case 0b000:
						//addi
						//immediate can be signed
                        printf("addi \n");
                        printf("imm = : %d \n", imm_20_31_s);

						reg[rd]=reg[rs1]+imm_20_31_s;
						break;
					case 0b001://***************
						//slli- shift left logical immediate
						printf("slli \n");
						reg[rd]=reg[rs1] << rs2; // rs2 = shamt
						break;
					case 0b010:
						//slti- set less than immediate
						if(reg[rs1]<imm_20_31_s){
                            reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 0b011:
						//sltiu-set less than immediate unsigned
						//*****************
						if(reg[rs1]>imm_20_31){
						reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 0b100:
						//xori
						reg[rd]=reg[rs1]^imm_20_31_s;

					case 0b101://******************
						//srli and srai- shift right logical and arithmetic immediate
                        printf("sr__ \n");
						//SHAMPT as rs2
						if(funct7==0b0000000){ // logical
                        printf("srli \n");

						reg[rd]=reg[rs1]<<rs2; // rs2 = shamt

						dec2bin(reg[rs1]); // debug
                        printf("\n");
                        dec2bin(reg[rd]);
                        printf("\n");
						}

						if(funct7==0b0100000){ //arithmetic
                        printf("srai \n");
						//check srai (Mathen: this might require a loop for repeatedly shifting by 1)
						reg[rd]=reg[rs1]>>rs2; // rs2 = shamt
						x=reg[rs1]&0x80000000;
						reg[rd]=reg[rs1]>>reg[rs2];
						reg[rd]=reg[rd]|x;

						dec2bin(reg[rs1]);
                        printf("\n");
                        dec2bin(reg[rd]);
                        printf("\n");
						}
						break;
					case 0b110:
						//ori
						reg[rd]=reg[rs1]|imm_20_31_s;

					case 0b111:
						//andi
						reg[rd]=reg[rs1]&imm_20_31_s;
                    default:
                        printf("ERROR funct3 not recognised... \n");
				}
				break;

			case 0x33: // 0110011
				switch(funct3){
					case 0b000://
						//add
						printf("add \n");
						if(funct7==0b0000000){
							//add
							reg[rd]=reg[rs1]+reg[rs2];
						}
						if(funct7==0b0100000){
						//sub
							reg[rd]=reg[rs1]-reg[rs2];
						}
						break;
					case 0b001:
						//sll
						//shift left logical-unsigned
						reg[rd]=reg[rs1]<<reg[rs2]; // FIX
						break;
					case 0b010:
						//slt-set less than. slt rd, rs1, rs2.
						//rd is 1 if rs1<rs2
						if(reg[rs1]<reg[rs2]){
						reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 0b011:
						//sltu-unsigned
						//*****************

						if(reg[rs1]<reg[rs2]){
						reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 0b100:
						//xor
						reg[rd]=reg[rs1]^reg[rs2];
						break;

					case 0b101:
						//srl and  sra

						if(funct7 == 0b0000000){
						//srl-logical-unsigned
						reg[rd]=reg[rs1]>>reg[rs2];
						}
						//sra-should be signed
						if(funct7 == 0b0100000){
							if(reg[rs1]>0){
                                reg[rd]=reg[rs1]>>reg[rs2];
							}
							else{
								x=reg[rs1]&0x80000000;
								reg[rd]=reg[rs1]>>reg[rs2];

								reg[rd]=reg[rd]|x;
							}
						}
						break;
					case 0b110:
						//or
						reg[rd]=reg[rs1]|reg[rs2];
						break;
					case 0b111:
						//and
						reg[rd]=reg[rs1]&reg[rs2];
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

                int offset = 0b0 + (imm4_1 << 1) + (imm10_5 << 5) + (imm11 << 11) + (imm12 << 12);
                // is unsigned (needs to be signed for branching back)
                offset = twoComp2Dec_12(offset); // converts to signed

                // divide by 4 because of bytes -> words
                offset = offset/4;

                printf("Offset: %d \n", offset);

                switch(funct3){
				 	case 0b000: //BEQ
				 	    printf("BEQ \n");
				 	    if(reg[rs1] == reg[rs2]){
                            // take branch
                            pc = branch(pc, offset);
				 	    }
						break;
					case 0b001: // BNE
					    printf("BNE \n");
					    if(reg[rs1] != reg[rs2]){
                            // take branch
                            pc = branch(pc, offset);
				 	    }
						break;
					case 0b100: // BLT
					    printf("BLT \n");
					    if(reg[rs1] < reg[rs2]){
                            // take branch
                            pc = branch(pc, offset);
				 	    }
						break;
                    case 0b101: // BGE
                        printf("BGE \n");
                        if(reg[rs1] >= reg[rs2]){
                            // take branch
                            pc = branch(pc, offset);
				 	    }
						break;
                    case 0b110: // BLTU
                        printf("BLTU \n");
                        if( (unsigned) reg[rs1] < (unsigned) reg[rs2]){
                            // take branch
                            pc = branch(pc, offset);
				 	    }
						break;
                    case 0b111: // BGEU
                        printf("BGEU \n");
						break;
						if( (unsigned) reg[rs1] >= (unsigned) reg[rs2]){
                            // take branch
                            pc = branch(pc, offset);
				 	    }
				}
				break;

			default:
				printf("Opcode %d not yet implemented \n", opcode);
		}


    printReg(reg);
    }
    return 0;
}
