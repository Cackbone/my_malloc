#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <my_malloc.h>

void *START_MEM_PTR = NULL;

static void *get_free_space(size_t size)
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *header = NULL;

    while (it < top) {
        header = it;
        if (header->free && header->size >= size)
            return it;
        it += sizeof(mem_block_t) + header->size;
    }
    return NULL;
}

void split_free_space(void *ptr, int size)
{
    mem_block_t *free_space = NULL;
    mem_block_t *header = ptr;
    size_t remaining_space = header->size - size;
    mem_block_t new_block = {remaining_space, 1};

    if (remaining_space <= sizeof(mem_block_t))
        return;
    header->size = size;
    write(1, "1", 1);
    free_space = (mem_block_t *)(ptr + sizeof(mem_block_t) + header->size);
    write(1, "2", 1);
    *free_space = new_block;
    write(1, "3", 1);
}

void *malloc(size_t size)
{
    void *free = NULL;
    mem_block_t block;
    void *ptr = NULL;

    if (size == 0)
	return NULL;
    if (!START_MEM_PTR)
        START_MEM_PTR = sbrk(0);
    free = get_free_space(ALIGN(size));
    if (!free) {
        block.size = ALIGN(size);
        block.free = 0;
        ptr = sbrk(sizeof(mem_block_t) + block.size);
        *((mem_block_t *)ptr) = block;
	return (ptr + sizeof(mem_block_t));
    } else {
	write(1, "a", 1);
        split_free_space(free, ALIGN(size));
	write(1, "b", 1);
        block.size = ALIGN(size);
        block.free = 0;
        *((mem_block_t *)free) = block;
        return (free + sizeof(mem_block_t));
    }
}


static void show_alloc_mem()
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *header = NULL;
    
    printf("break : %p\n", START_MEM_PTR);
    while (it < top) {
	header = it;
	printf("%p - %p : %ld - %d\n",
	       it + sizeof(mem_block_t),
	       it + sizeof(mem_block_t) + header->size,
	       header->size,
	       header->free);
	it += sizeof(mem_block_t) + header->size;
    }
}


void free(void *ptr)
{
    if (!ptr) {
    	show_alloc_mem();
	return;
    }
    mem_block_t *block = ptr - sizeof(mem_block_t);

    block->free = 1;
}
