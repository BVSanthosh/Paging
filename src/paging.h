#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// You may find defining the following macros useful. Edit the values below to match your report
#define PAGETABLE_ROWS	512
#define PAGETABLE_SIZE	704
#define FRAME_BITS 	9
#define OFFSET_BITS 	7
#define OFFSET_MASK 	0

//Struct for page table entries
typedef struct PageTableEntry {
	uint16_t page_number;
	uint16_t frame_number;
	bool read_only;
	bool executable;
} PageTableEntry;

/**
 * Initialise the page table. This should allocate the memory for the page table and return the pointer.
 * It is entirely up to you how you organise the data at this memory address!
 */
void* pt_init();

/**
 * Convert virtual memory address to a physical address
 *
 * This function will use the supplied page table to convert a virtual address (16-bit integer)
 * to a physical address (another 16-bit integer). The latter can be used to index straight into an
 * in-memory store that simulates our physical memory
 *
 * @param	table		Pointer to the page table returned by pt_init()
 * @param	virtual_address	Virtual address to be converted to a physical address
 *
 * @return			The physical address corresponding to the virtual address
 */
uint16_t virtual_to_physical(void* table, uint16_t virtual_address);

/**
 * Add a page to frame mapping to the page table
 *
 * This function will add an entry to the page table which maps a logical (virtual) page to a 
 * physical frame. In our implementation, physical memory is modelled by an in-memory store that 
 * we can index into. This operation does not touch the store, only updates the page table 
 * at the supplied memory address.
 *
 * @param	table		Pointer to the page table returned by pt_init()
 * @param	page_number	Page number 
 * @param	frame_number	The corresponding frame number to be entered into the table
 * @param	readonly	Read-only bit that determines whether this frame can be written to
 * @param	executable	Executable bit that determines whether this fram can contain code
 */
void map_page_to_frame(void* table, uint16_t page_number, uint16_t frame_number, bool readonly, bool executable);

/**
 * Remove a page to frame mapping to the page table
 *
 * This function will remove an entry from the page table which maps a logical (virtual) page to a 
 * physical frame. 
 *
 * @param	table		Pointer to the page table returned by pt_init()
 * @param	page_number	Page number whose mapping we want to remove
 */
void unmap_page(void* table, uint16_t page_number);

/**
 * Stores a chunk of data into our store
 *
 * This function will take data of a given length and copy it into the store. It will do this 
 * by using the page table to make sure that the virtual addresses are correctly translated to
 * physical addresses (which are used to index into our store).
 *
 * @param	table		Pointer to the page table returned by pt_init()
 * @param	store		Pointer to the in-memory store which simulates physical memory
 * @param	buffer		Pointer to the start of the data we want to store
 * @param	virtual_address	The 16-bit virtual address describing where to store the data
 * @param	length		The number of bytes to copy
 */
void store_data(void *table, char* store, char* buffer, uint16_t virtual_address, size_t length);

/**
 * Reads a chunk of data from our store
 *
 * This function is the opposite of store_data. It will read a chunk of data from a supplied
 * virtual memory address. It will make use of the page table to mape virtual pages to physical
 * frames in order to correctly index into the in-memory store.
 *
 * @param	table		Pointer to the page table returned by pt_init()
 * @param	store		Pointer to the in-memory store which simulates physical memory
 * @param	buffer		Pointer to a pre-allocated buffer our function will fill
 * @param	virtual_address	The 16-bit virtual address describing where to read the data
 * @param	length		The number of bytes to copy
 */
void read_data(void *table, char *store, char* buffer, uint16_t virtual_address, size_t length);

/**
 * Print the contents of the table
 *
 * This function is useful for debugging. Here you can write code to print all the valid 
 * mappings from pages to frames in your page table to make sure that you are storing the
 * table correctly. Hint: make use of the valid bit to only print the entries that have a valid
 * mapping defined.
 *
 * @param	table		Pointer to the page table returned by pt_init()
 */
void print_table(void* table);

#endif
