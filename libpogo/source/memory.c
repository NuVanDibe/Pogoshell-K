/*
 * Core memory handling functions (memory.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Functions for allocating and freeing memory in WRAM
 *
 **/

#include "core.h"

typedef struct _MemHead {
	struct _MemHead *next;
	uint32 size;
	uint32 data[0];
} MemHead;

uint32 *mem_base = NULL;
MemHead *first_block;
int mem_size = 63*1024;

/* Exported by linkscript as start of available memory (after any WRAM variables) */
extern uint32 __eheap_start;
extern uint32 __iheap_start;

void memory_init(uint32 *ptr, int size)
{
	mem_base = ptr;
	mem_size = size;
	first_block = NULL;
}

extern void dprint(const char *sz);
/*
void mem_check(void)
{
	MemHead *last_block = NULL;
	MemHead *block = first_block;

	while(block)
	{
		if((int)block > 0x03004000 || (int)block < 0x03000000)
		{
			char tmp[20];
			sprintf(tmp, "last %p block %p\n", last_block, block);
			dprint(tmp);
			while(1);
		}

		last_block = block;
		block = block->next;
	}
}
*/

int free_after;

/* Alloc 32bit words */
void *memory_alloc(int alloc_size)
{
	MemHead *newblock, *block = first_block;

	if(!mem_base)
	{
		//mem_base = &__eheap_start;
		//mem_size = (0x02040000 - (int)mem_base) / 4;
		memory_init(&__iheap_start, (0x03007E00 - (int) &__iheap_start) / 4);
	}
	
	if(!first_block)
	{
		if(alloc_size > mem_size)
			return NULL;
		block = first_block = (MemHead *)mem_base;
		block->next = 0;
		block->size = alloc_size;
		return block->data;
	}

	while(block)
	{

		if(block->next)
			free_after = (uint32 *)block->next - (uint32*)&block->data[block->size];
		else
			free_after = mem_size - (uint32)(&block->data[block->size] - mem_base);

		if(free_after >= (alloc_size + 3))
		{
			newblock = (MemHead *)&block->data[block->size];
			newblock->next = block->next;
			newblock->size = alloc_size;
			block->next = newblock;
			return newblock->data;
		}

		block = block->next;
	}

	//dprint("Out of memory!\n");
	return NULL;
/*
	block->next = (MemHead *)&block->data[block->size];
	newblock = block->next;
	newblock->next = NULL;
	newblock->size = alloc_size;
	return newblock->data;
*/
}

void memory_free(void *mem)
{
	MemHead *last_block = NULL;
	MemHead *block = first_block;

	while(block)
	{
		/*char tmp[20];
		sprintf(tmp, "block %p\n", block);
		dprint(tmp);
		if((int)block > 0x03008000 || (int)block < 0x03000000)
		{
			while(1);
		}*/
		if(mem == block->data)
		{
			if(!last_block)
				first_block = NULL;
			else
				last_block->next = block->next;
			return;
		}
		last_block = block;
		block = block->next;
	}
}

void *memory_realloc(void *mem, int alloc_size)
{
	int free_after;
	void *newmem;
	MemHead *block = first_block;

	while(block) {
		if(mem == block->data) {
			/* If we decrease size or if theres enough free space after, just change the size and
			   return the same pointer */
			if(block->next)
				free_after = (uint32 *)block->next - (uint32*)&block->data[block->size];
			else
				free_after = mem_size - (uint32)(&block->data[block->size] - mem_base);
			if((alloc_size < block->size) ||
			   (alloc_size - block->size < free_after))
			{
				block->size = alloc_size;
				return mem;
			}

			/* Try allocating a new block */
			newmem = memory_alloc(alloc_size);
			if(newmem) {
				memory_free(mem);
				return newmem;
			}
			//dprint("Out of memory!\n");
			/* No memory for block, fail */
			return NULL;

		}
		block = block->next;
	}
	return NULL;
}

/* Return available memory (also calculates largest block) */
int memory_avail(void)
{
	int free_after;
	int free_total = 0;
	int largest_block = 0;
	MemHead *block = first_block;

	if(!block)
		return mem_size;

	while(block)
	{
		if(block->next)
			free_after = (uint32 *)block->next - (uint32*)&block->data[block->size];
		else
			free_after = mem_size - (uint32)(&block->data[block->size] - mem_base);

		if(free_after > largest_block)
			largest_block = free_after;
		free_total += free_after;
		block = block->next;
	}
	return free_total;
}

#ifndef MALLOC_DEBUG

/* C-library versions of allocation functions */

void *malloc(int x)
{
	void *p = memory_alloc((x+3)/4);
	return p;
};

void *realloc(void *p, int l)
{
	return memory_realloc(p, (l+3)/4);
}

void free(void *x)
{ 
	memory_free(x); 
}

#endif
