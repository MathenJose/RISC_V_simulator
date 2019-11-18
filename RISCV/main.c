#include<stdio.h>
// simulator for RISC-V

// take input from binary file
// save it to memory

// initialise the register + memory

// run each instruction

// save the registers to binary file
void dec2bin(int c)
{
   int i = 0;
   for(i = 31; i >= 0; i--){
     if((c & (1 << i)) != 0){
       printf("1");
     }else{
       printf("0");
     }
   }
}

int main(){

    char file_path[] = "addpos.bin";

    unsigned int memory [1000];
    for(int i = 0; i < 1000; i++){ // clear the memory
        memory[i] = 0;
    }
    unsigned int * memory_pointer = &memory[0]; // pointer to the memory array

    unsigned int reg [32];
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
        unsigned int rd = (instr >> 7) & 0x1f;// 7bit shift to right and clears everything except for first 5 digits
        unsigned int rs1 = (instr >> 15) & 0x1f;
		unsigned int rs2 = (instr >> 20) & 0x1f;
		unsigned int funct3 = (instr>> 12) & 0x7;
		unsigned int funct7 = (instr>> 23);
		unsigned int imm_25_31 = (instr>> 25);
		unsigned int imm_20_31 = (instr >> 20);
		unsigned int imm_12_31 =(instr >> 12);

        pc ++;
    }

	return 0;
}
