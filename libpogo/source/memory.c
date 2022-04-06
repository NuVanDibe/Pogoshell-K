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
#include "io.h"

typedef struct _MemHead {
	struct _MemHead *next;
	uint32 size;
	uint32 data[0];
} MemHead;

uint32 *mem_base = NULL;
MemHead *first_block;
int mem_size = 63*1024;
static uint32 context = GLOBAL;

/* Exported by linkscript as start of available memory (after any WRAM variables) */
extern uint32 __eheap_start;
extern uint32 __iheap_start;
extern uint32 __iheap_end;

void memory_init(uint32 *ptr, int size)
{
	mem_base = ptr;
	mem_size = size;
	first_block = NULL;
	context = GLOBAL;
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

/*
void print_memory(void)
{
	MemHead *next_block, *block = first_block;

	while (block)
	{
		next_block = block->next;
		fprintf(stderr, "%p (%d: %d)\n", block, (block->size & 0x00ffffff) * 4 + sizeof(MemHead), (int) next_block - (int) block - (block->size & 0x00ffffff) * 4 - sizeof(MemHead));
		block = next_block;
	}
}
*/

void memory_set_context(int new_context)
{
	context = (new_context & 0xff)<<24;
}

void memory_free_context(int free_context)
{
	MemHead *last_block = NULL;
	MemHead *block = first_block;

	while(block)
	{
		if(((block->size)>>24) == free_context)
		{
			if (!last_block) {
				first_block = NULL;
				return;
			}
			last_block->next = block = block->next;
		} else {
			last_block = block;
			block = block->next;
		}
	}

}

/* Alloc 32bit words */
void *memory_alloc(int alloc_size)
{
	int free_after;
	MemHead *newblock, *block = first_block;

	if(!mem_base)
	{
		//mem_base = &__eheap_start;
		//mem_size = (0x02040000 - (int)mem_base) / 4;
		memory_init(&__iheap_start, ((int) &__iheap_end - (int) &__iheap_start)>>2);
	}
	
	if(!first_block)
	{
		if(alloc_size > mem_size)
			return NULL;
		block = first_block = (MemHead *)mem_base;
		block->next = 0;
		block->size = alloc_size | context;
		//fprintf(stderr, "%p(%p) = malloc(%d+%d)\n", block, block->data, (block->size & 0x00FFFFFF) * 4, sizeof(MemHead));
		return block->data;
	}

	while(block)
	{

		if(block->next)
			free_after = (uint32 *)block->next - (uint32*)&block->data[block->size & 0x00FFFFFF];
		else
			free_after = mem_size - (uint32)(&block->data[block->size & 0x00FFFFFF] - mem_base);

		if(free_after >= (alloc_size + 2))
		{
			newblock = (MemHead *)&block->data[block->size & 0x00FFFFFF];
			newblock->next = block->next;
			newblock->size = alloc_size | context;
			block->next = newblock;
			//fprintf(stderr, "%p(%p) = malloc(%d+%d)\n", newblock, newblock->data, (newblock->size & 0x00FFFFFF) * 4, sizeof(MemHead));
			//fprintf(stderr, "%p\n", newblock->data);
			return newblock->data;
		}

		block = block->next;
	}

	dprint("Out of memory!\n");
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
			//fprintf(stderr, "free(%p) //%d\n", block, block->size * 4 + sizeof(MemHead));
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
				free_after = (uint32 *)block->next - (uint32*)&block->data[block->size & 0x00FFFFFF];
			else
				free_after = mem_size - (uint32)(&block->data[block->size & 0x00FFFFFF] - mem_base);
			if((alloc_size < (block->size & 0x00FFFFFF)) ||
			   (alloc_size - (block->size & 0X00FFFFFF) < free_after))
			{
				block->size = alloc_size | context;
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
			free_after = (uint32 *)block->next - (uint32*)&block->data[block->size & 0x00FFFFFF];
		else
			free_after = mem_size - (uint32)(&block->data[block->size & 0x00FFFFFF] - mem_base);

		if (free_after)
			fprintf(stderr, "%p %d ", &block->data[block->size & 0x00FFFFFF], free_after);
		if(free_after > largest_block)
			largest_block = free_after;
		free_total += free_after;
		block = block->next;
	}
	fprintf(stderr, "\n");
	return free_total;
}

#ifndef MALLOC_DEBUG

/* C-library versions of allocation functions */

void *malloc(int x)
{
	void *p = memory_alloc((x+3)>>2);
	return p;
};

void *realloc(void *p, int l)
{
	return memory_realloc(p, (l+3)>>2);
}

void free(void *x)
{ 
	memory_free(x); 
}

#endif
