#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "paging.h"

int index = 0;    //number of page table rows
int visited_entries = -1;    //used allow access to all the necessary rows when string a text in multiple frames

void* pt_init() {

	PageTableEntry* table_pointer = malloc(PAGETABLE_ROWS * sizeof(PageTableEntry));   //sets the size of the table using a macro

	printf("Initialised page table with %i rows", PAGETABLE_ROWS);
	printf("\n");

	return (void *)table_pointer;    //casts back to void* so that it is compatable with the header file
}

void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable) {

	PageTableEntry* casted_table = (PageTableEntry*) table;     //casts back to a struct type so that the fields can be accessed

	//assigns a value to all the necessary fields to the row
	casted_table[index].page_number = page_number;    
	casted_table[index].frame_number = frame_number;
	casted_table[index].read_only = readonly;
	casted_table[index].executable = executable;
	index++;

	printf("Added mapping from page number: %i to frame number: %i", page_number, frame_number);
	printf("\n");
}

void unmap_page(void* table, uint16_t page_number) {

	PageTableEntry* casted_table = (PageTableEntry*) table;

	for(int counter1 = 0; counter1 < index; counter1++){
	
		if(page_number == casted_table[counter1].page_number){   //if this row is the one to be deleted
		
			for(int counter2 = counter1 ;counter2 < index; counter2++){    //gets the next row and replace it with the current one
				casted_table[counter2].page_number = casted_table[counter2 + 1].page_number;
				casted_table[counter2].frame_number = casted_table[counter2 + 1].frame_number;
				casted_table[counter2].read_only = casted_table[counter2 + 1].read_only;
				casted_table[counter2].executable = casted_table[counter2 + 1].executable;
			}

			index--;   //updates the size of the page table
			printf("Removed mapping for page number: %i", page_number);
			printf("\n");
			return;
		}
	}

	printf("Cannot remove mapping for page number: %i due to it being invalid", page_number);
	printf("\n");
}

uint16_t virtual_to_physical(void* table, uint16_t virtual_address) {

	PageTableEntry* casted_table = (PageTableEntry*) table;
	int table_index = virtual_address >> OFFSET_BITS;   //calculates the page number
	int frame_index = virtual_address & 127;   //calculates the page offset

	for(int counter = 0; counter < index; counter++){

		if((table_index == casted_table[counter].page_number) && (casted_table[counter].executable == true) && (visited_entries < counter)){   //checks to see if the row has the page number so that it can be mapped to a frame number
			visited_entries++;
			uint16_t address = (casted_table[counter].frame_number << 7) | frame_index;    //concatinates the frame number and page offset to form the physical address
			return address;
		}

		if((table_index == casted_table[counter].page_number) && (casted_table[counter].executable == false)){    //output message if the frame trying to be read is inexecutable
			printf("Virtual address: %i is inexecutable", virtual_address);
			printf("\n");
		}
	}
}

void store_data(void* table, char* store, char* buffer, uint16_t virtual_address, size_t length) {

	PageTableEntry* casted_table = (PageTableEntry*) table;
	int physical_address = virtual_to_physical(casted_table, virtual_address);
	int frame_index = virtual_address & 127;
	int frame_number = physical_address >> OFFSET_BITS;

	visited_entries = -1;

	for(int num = 0; num < index; num++){     
		if((physical_address == casted_table[num].frame_number) && (casted_table[num].read_only == true)){    //checks to see if the frame to be read is defined as read-only
			printf("Cannot write to the frame number: %i because it is read-only", frame_number);
			return;
		}
	}

	if(128 - frame_index >= length){  //checks to see if the text can fit it in one frame

		physical_address = virtual_to_physical(casted_table, virtual_address);
		frame_number = physical_address >> OFFSET_BITS;
		frame_index = virtual_address & 127;
		int starting_point = (frame_number * 128) + frame_index;    //sets the byte t start writing from

		printf("\"");
		for(int counter = 0; counter < length; counter++){
			strcpy(&store[starting_point + counter], &buffer[counter]);    //copies each char from the buffer to the store
			printf("%c", store[starting_point + counter]);
		}

		printf("\"");
		printf(" written to frame number: %i", frame_number);
		printf("\n");
	}
	else{  //if it can't fit in one frame

		int index = virtual_address & 127;    
		int num_of_frames = length / (128 - index);   //the number of frames required is calculated 
		int count = 0;

		printf("\"");
		for(int counter = 0; counter < num_of_frames; counter++){    //loop for each page table row with the relevant frame number 

			physical_address = virtual_to_physical(casted_table, virtual_address);
			frame_number = physical_address >> OFFSET_BITS;
			frame_index = virtual_address & 127;
			int starting_point = (frame_number * 128) + frame_index;    //sets the starting byte to write from
			int parition_length;   //defines the number of chars to read into the specified frame

			if(counter < num_of_frames - 1){
				parition_length = 128 + frame_index;  
			}
			else{
				parition_length = length - (counter  * 128); 
			}
			

			for(int counter2 = 0; counter2 < parition_length; counter2++){
				strcpy(&store[starting_point + counter2], &buffer[count]);    //copies the chars from the buffer to the store
				printf("%c", store[starting_point + counter2]);
				count++;
			}
		}

		printf("\"");
		printf(" has been written to the physical address space");
		printf("\n");
	}
}

void read_data(void* table, char* store, char* buffer, uint16_t virtual_address, size_t length) {    //this function is only the same as the store_data() one

	PageTableEntry* casted_table = (PageTableEntry*) table;
	int physical_address = virtual_to_physical(casted_table, virtual_address);
	int frame_index = virtual_address & 127;
	int frame_number = physical_address >> OFFSET_BITS;

	visited_entries = -1;

	if(128 - frame_index >= length){    //checks to see if the text is in one frame only

		physical_address = virtual_to_physical(casted_table, virtual_address);
		frame_number = physical_address >> OFFSET_BITS;
		frame_index = virtual_address & 127;
		int starting_point = (frame_number * 128) + frame_index;

		printf("\"");
		for(int counter = 0; counter < length; counter++){
			strcpy(&buffer[counter], &store[starting_point + counter]);   //copies the chars from the store to the buffer 
			printf("%c", store[starting_point + counter]);
		}

		printf("\"");
		printf(" read from frame number: %i", frame_number);
		printf("\n");
	}
	else{     //if it isn't in one frame

		int index = virtual_address & 127;
		int num_of_frames = length / (128 - index);
		int count = 0;

		printf("\"");
		for(int counter = 0; counter < num_of_frames; counter++){    //loop to get each frame 

			physical_address = virtual_to_physical(casted_table, virtual_address);
			frame_number = physical_address >> OFFSET_BITS;
			frame_index = virtual_address & 127;
			int starting_point = (frame_number * 128) + frame_index;
			int parition_length;

			if(counter < num_of_frames - 1){
				parition_length = 128 + frame_index;
			}
			else{
				parition_length = length - (counter  * 128); 
			}
			

			for(int counter2 = 0; counter2 < parition_length; counter2++){
				strcpy(&buffer[count], &store[starting_point + counter2]);     //copies it from teh specified frame number in the store to the buffer
				printf("%c", store[starting_point + counter2]);
				count++;
			}
		}

		printf("\"");
		printf(" has been reasd from the physical address space");
		printf("\n");
	}
}

void print_table(void* table) {

	PageTableEntry* casted_table = (PageTableEntry*) table;

	//sets the headers for the page table
	printf("Page table:");
	printf("\n");
	printf("Page number | Frame number | read-only | executable");
	printf("\n");

	for(int counter = 0; counter < index; counter++){    //prints out the page table rows

		printf("     %i      |      %i       |     %d     |     %d ", casted_table[counter].page_number, casted_table[counter].frame_number, casted_table[counter].read_only, casted_table[counter].executable);
		printf("\n");
	}
}