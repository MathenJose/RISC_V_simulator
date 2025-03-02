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

    unsigned int memory [1000];
    for(int i = 0; i < 1000; i++){ // clear the memory
        memory[i] = 0;
    }
    unsigned int * memory_pointer = &memory[0]; // pointer to the memory array

    unsigned int registers [32];
    for(int i = 0; i < 32; i++){ // clear the memory
        registers[i] = 0;
    }

    FILE *ptr_myfile;

    ptr_myfile=fopen("shift.bin","rb");
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
    }
    fclose(ptr_myfile);
    printf("Read %i words from the input file. \n", word_counter);

    // RUNNING THE SIMULATOR

    int pc = 0; // set pc to zero to start
    while(true){
        printf("Starting simulation... \n");


        pc ++
    }

	return 0;
}
