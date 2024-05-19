#include "paging.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void main() {

	/*Tests are written in sepereate methods where each one tests a particular functionality and convners several cases*/

	initialise_table();
	add_mappings_to_table();
	delete_mappings_from_table();
	invalid_delete_mappings_from_table();
	map_virtual_address_to_physical_address();
	invalid_map_virtual_address_to_physical_address();
	test1_read_write_data_in_single_frame();
	test2_read_write_data_in_single_frame();
	test3_read_write_data_in_single_frame();
	test1_read_write_data_in_multiple_frame();
	test2_read_write_data_in_multiple_frame();
	test3_read_write_data_in_multiple_frame();
	write_to_read_only();
	read_from_inexecutable();

}

//Tests whether the table can be initialised correctly with the specified frame bits
void initialise_table(){

	// Initialise our mapping table
	void *table = pt_init();

	free(table);
}

//Tests whether rows can be added to the page table
void add_mappings_to_table(){

	void *table = pt_init();

	//Adds some basic mappings to the table
	map_page_to_frame(table, 4, 0, false, true);
	map_page_to_frame(table, 10, 12, false, true);
	map_page_to_frame(table, 6, 5, false, true);
	map_page_to_frame(table, 7, 8, false, true);
	map_page_to_frame(table, 1, 2, false, true);

	//Prints the table
	print_table(table);

	free(table);
}

//Tests whether a specified row can be deleted from the table
void delete_mappings_from_table(){

	void *table = pt_init();

	map_page_to_frame(table, 4, 6, false, true);
	map_page_to_frame(table, 10, 11, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 15, 1, false, true);
	map_page_to_frame(table, 0, 8, false, true);

	print_table(table);

	//remves mapping from page table
	unmap_page(table, 2);

	print_table(table);

	free(table);
}

//Tests whether a row won't be deleted when an invalid page number is provided
void invalid_delete_mappings_from_table(){

	void *table = pt_init();

	map_page_to_frame(table, 4, 6, false, true);
	map_page_to_frame(table, 10, 11, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 15, 1, false, true);
	map_page_to_frame(table, 0, 8, false, true);

	print_table(table);

	unmap_page(table, 11);

	free(table);
}

//Tests whether a specified virtual addres can be converted to a physical address
void map_virtual_address_to_physical_address(){

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 11, 8, false, true);
	map_page_to_frame(table, 3, 10, false, true);
	map_page_to_frame(table, 15, 4, false, true);
	map_page_to_frame(table, 20, 11, false, true);

	print_table(table);

	//Provides a virtual address to map from
	uint16_t virtual_address = 386;
	uint16_t physical_address = virtual_to_physical(table, virtual_address);
	int page_number = virtual_address >> OFFSET_BITS;
	int frame_number = physical_address >> OFFSET_BITS;
	int frame_index = virtual_address & 127;

	printf("Virtual address: %i maps to the physical address: %i", virtual_address, physical_address);
	printf("\n");
	printf("The virtual address has the page number: %i and page offset: %i", page_number, frame_index);
	printf("\n");
	printf("The physical address has the frame number: %i and has page offset: %i", frame_number, frame_index);

	free(table);
}

//Tests whether no physical address is returned when an invalid virtual address is provided
void invalid_map_virtual_address_to_physical_address(){

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 11, 8, false, true);
	map_page_to_frame(table, 3, 10, false, true);
	map_page_to_frame(table, 15, 4, false, true);
	map_page_to_frame(table, 20, 11, false, true);

	print_table(table);

	//Provides an invalid virtual address to map from
	uint16_t page_num = 31;
	uint16_t frame_num = virtual_to_physical(table, page_num);

	free(table);
}

//Tests whether the given text can be stored and ready from a single frame
void test1_read_write_data_in_single_frame(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 0, 6, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 4, 8, false, true);
	map_page_to_frame(table, 6, 10, false, true);
	print_table(table);

	//set up to write to and read from store
	char text[]  = "It was the best of times, it was the worst of times";
	size_t length = strlen(text);
	char *buffer = malloc(length+1);

	store_data(table, store, text, 129, length);
	read_data(table, store, buffer, 129, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}

//Tests whether the given text can be stored and ready from a single frame
void test2_read_write_data_in_single_frame(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 0, 6, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 4, 8, false, true);
	map_page_to_frame(table, 6, 10, false, true);
	print_table(table);

	char text[]  = "That which does not kill us makes us stronger.";
	size_t length = strlen(text);
	char *buffer = malloc(length+1);

	store_data(table, store, text, 516, length);
	read_data(table, store, buffer, 516, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}

//Tests whether the given text can be stored and ready from a single frame
void test3_read_write_data_in_single_frame(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 0, 6, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 4, 8, false, true);
	map_page_to_frame(table, 11, 10, false, true);
	print_table(table);

	char text[]  = "101. TYPEWRITER is the longest word that you can write using the letters only on one row of the keyboard of your computer.";
	size_t length = strlen(text);
	char *buffer = malloc(length+1);

	store_data(table, store, text, 11, length);
	read_data(table, store, buffer, 11, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}

//Tests whether the given test can be stored and ready from multiple frames
void test1_read_write_data_in_multiple_frame(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 1, 3, false, true);
	map_page_to_frame(table, 1, 4, false, true);
	map_page_to_frame(table, 1, 5, false, true);
	map_page_to_frame(table, 1, 6, false, true);
	print_table(table);


	char text2[] = "It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of light, it was the season of darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing before us, we were all going direct to heaven, we were all going direct the other way–in short, the period was so far like the present period, that some of its noisiest authorities insisted on its being received, for good or for evil, in the superlative degree of comparison only.";
	size_t length = strlen(text2);
	char *buffer = malloc(length+1);

	store_data(table, store, text2, 129, length);
	read_data(table, store, buffer, 129, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}

//Tests whether the given test can be stored and ready from multiple frames
void test2_read_write_data_in_multiple_frame(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 1, 3, false, true);
	map_page_to_frame(table, 1, 4, false, true);
	map_page_to_frame(table, 1, 5, false, true);
	map_page_to_frame(table, 1, 6, false, true);
	print_table(table);


	char text2[] = "It was hard to say what had depressed him more: the studied footwork of the couples on the dance floor, or the heartrending petty bourgeois piteousness of cucumber sandwiches passed around by accounting majors whose overly colorful bow ties had been expressly chosen to keep them from looking like waiters";
	size_t length = strlen(text2);
	char *buffer = malloc(length+1);

	store_data(table, store, text2, 141, length);
	read_data(table, store, buffer, 141, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}

//Tests whether the given test can be stored and ready from multiple frames
void test3_read_write_data_in_multiple_frame(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 4, 2, false, true);
	map_page_to_frame(table, 4, 3, false, true);
	map_page_to_frame(table, 4, 4, false, true);
	map_page_to_frame(table, 4, 5, false, true);
	map_page_to_frame(table, 4, 6, false, true);
	map_page_to_frame(table, 4, 7, false, true);
	map_page_to_frame(table, 4, 8, false, true);
	map_page_to_frame(table, 4, 9, false, true);
	map_page_to_frame(table, 4, 10, false, true);
	print_table(table);


	char text2[] = "Then they went into José Arcadio Buendía’s room, shook him as hard as they could, shouted in his ear, put a mirror in front of his nostrils, but they could not awaken him. A short time later, when the carpenter was taking measurements for the coffin, through the window they saw a light rain of tiny yellow flowers falling. They fell on the town all through the night in a silent storm, and they covered the roofs and blocked the doors and smothered the animals who slept outdoors. So many flowers fell from the sky that in the morning the streets were carpeted with a compact cushion and they had to clear them away with shovels and rakes so that the funeral procession could pass by. I wanted so badly to lie down next to her on the couch, to wrap my arms around her and sleep. Not fuck, like in those movies. Not even have sex. Just sleep together, in the most innocent sense of the phrase. But I lacked the courage and she had a boyfriend and I was gawky and she was gorgeous and I was hopelessly boring and she was endlessly fascinating. So I walked back to my room and collapsed on the bottom bunk, thinking that if people were rain, I was drizzle and she was a hurricane.";
	size_t length = strlen(text2);
	char *buffer = malloc(length+1);

	store_data(table, store, text2, 518, length);
	read_data(table, store, buffer, 518, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}

//Tests whether the program rejects a read-only frame number to be ready from 
void write_to_read_only(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, true, true);
	map_page_to_frame(table, 0, 6, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 4, 8, false, true);
	map_page_to_frame(table, 6, 10, false, true);
	print_table(table);

	char text[]  = "It was the best of times, it was the worst of times";
	size_t length = strlen(text);
	char *buffer = malloc(length+1);

	store_data(table, store, text, 129, length);
	read_data(table, store, buffer, 129, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);

}

//Tests whether the program rejects a frame number being used when it is inexecutable
read_from_inexecutable(){

	char *store = malloc(1<<15);

	void *table = pt_init();

	map_page_to_frame(table, 1, 2, false, true);
	map_page_to_frame(table, 0, 6, false, true);
	map_page_to_frame(table, 2, 4, false, true);
	map_page_to_frame(table, 4, 8, false, false);
	map_page_to_frame(table, 6, 10, false, true);
	print_table(table);

	char text[]  = "That which does not kill us makes us stronger.";
	size_t length = strlen(text);
	char *buffer = malloc(length+1);

	store_data(table, store, text, 516, length);
	read_data(table, store, buffer, 516, length);
	//printf("%s\n", buffer);

	free(buffer);
	free(store);
	free(table);
}