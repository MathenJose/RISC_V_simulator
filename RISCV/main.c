#include<stdio.h>
// simulator for RISC-V

// take input from binary file
// save it to memory

// initialise the register + memory

// run each instruction

// save the registers to binary file
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

void printReg(int reg[]){
    for(int i = 0; i < 32; i++){
        printf("%d ", reg[i]);
    }
    printf("\n");
}

int main(){

    char file_path[] = "addpos.bin";

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

    // decoding:
        unsigned int instr = memory[pc];
        unsigned int opcode = instr & 0x7f;
        unsigned int rd = (instr >> 7) & 0x1f;
        unsigned int rs1 = (instr >> 15) & 0x1f;
        unsigned int rs2 = (instr >> 20) & 0x1f;
        unsigned int funct3 = (instr>> 12) & 0x7;
        unsigned int funct7 = (instr>> 23);
        unsigned int imm_25_31 = (instr>> 25);
        unsigned int imm_20_31 = (instr >> 20);
        unsigned int imm_12_31 = (instr >> 12);

        // signed versions of the immediate
        int imm_12_31_s =(instr >> 12);//signed imeediate
		int imm_25_31_s = (instr>> 25);
        int imm_20_31_s = (instr >> 20);

        pc ++;


        switch (opcode) {

			case 0x73: // e-call
				funct3=000;
				break;

			case 0x23: //load and store 0100011

				reg[rd] = reg[rs1] + imm_12_31_s;
				break;

			case 0x13: //instructions with immediate 0010011
				switch(funct3){
				 	case 000:
						//addi
				 		//System.out.println("addi");
				 		//System.out.println(rd);
				 		//System.out.println(rs1);
						//immediate can be signed

						reg[rd]=reg[rs1]+imm_20_31_s;
						break;
					case 001://***************
						//slli-shifr left logical immediate
						reg[rd]=reg[rs1] << imm_20_31_s;
						break;
					case 010:
						//slti-set less than immediate
						if(reg[rs1]<imm_20_31_s){
                            reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 011:
						//sltiu-set less than immediate unsigned
						//*****************
						if(reg[rs1]>imm_20_31){
						reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 100:
						//xori
						reg[rd]=reg[rs1]^imm_20_31_s;

					case 101://******************
						//srli and srai- shift right logical and arithmetic immediate
						//SHAMPT as rs2
						if(funct7==0b0000000){

						reg[rd]=reg[rs1]<<rs2;
						}
						   //arithmetic
						if(funct7==0b0100000){
						reg[rd]=reg[rs1]>>rs2;
						}
						break;
					case 110:
						//ori
						reg[rd]=reg[rs1]|imm_20_31_s;

					case 111:
						//andi
						reg[rd]=reg[rs1]&imm_20_31_s;
				}
				break;

			case 0x33: // 0110011
				switch(funct3){
					case 000://
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
					case 001:
						//sll
						//shift left logical-unsigned
						reg[rd]=reg[rs1]<<reg[rs2]; // FIX
						break;
					case 010:
						//slt-set less than. slt rd, rs1, rs2.
						//rd is 1 if rs1<rs2
						if(reg[rs1]<reg[rs2]){
						reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 011:
						//sltu-unsigned
						//*****************

						if(reg[rs1]<reg[rs2]){
						reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						break;
					case 100:
						//xor
						reg[rd]=reg[rs1]^reg[rs2];
						break;

					case 101://**************
						//srl and  sra
						//rs2 = getSigned(rs2);

						if(funct7 == 0b0000000){
						//srl-logical-unsigned
						reg[rd]=reg[rs1]>>reg[rs2];
						}
						//sra-should be signed
						if(funct7 == 0b0100000){

						reg[rd]=reg[rs1]>>reg[rs2];
						}
						break;
					case 110:
						//or
						reg[rd]=reg[rs1]|reg[rs2];
						break;
					case 111:
						//and
						reg[rd]=reg[rs1]&reg[rs2];
						break;
				}
				break;

			case 0x43: // 1000011
				switch(funct3){
				 	case 000:
						//SB
						break;
					case 001://***************
						//SH
						break;
					case 010:
						//SW
						break;
				}
				break;


			case 0x37://lui	110111
				printf("lui inst \n");
				reg[rd] = imm_12_31;
				break;

			default:
				printf("Opcode %d not yet implemented", opcode);
		}


        if(opcode==0x43){ // TO DO
            switch(funct3){
                case 000:
                    //SB
                    break;
                case 001://***************
                    //SH
                    break;
                case 010:
                    //SW
                    break;
            }
        }


    printReg(reg);
    }
    return 0;
}
