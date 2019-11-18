#include<stdio.h>
// simulator for RISC-V 

// take input from binary file
// save it to memory

// initialise the register + memory

// run each instruction

// save the registers to binary file


int main(){

    unsigned int memory [1000];
    for(int i = 0; i < 1000; i++){
        memory[i] = 0;
    }
    unsigned int * memory_pointer = &memory[0]; // pointer to the memory array
    
    unsigned int registers [32]; 
    for(int i = 0; i < 32; i++){
        registers[i] = 0;
    }

    FILE *ptr_myfile;

    ptr_myfile=fopen("addpos.bin","rb");
    if (!ptr_myfile){
        printf("Unable to open file!");
        return 1;
    }

    /* Seek to the beginning of the file */
   fseek(ptr_myfile, 0, SEEK_SET);

    // change memory pointer in the while loop
    while(fread(memory_pointer, sizeof(int), 1, ptr_myfile)){
        memory_pointer++; // go to next memory location
        printf_s("%x \n", *memory_pointer);
    }
    
    fclose(ptr_myfile);

	return 0;
}
