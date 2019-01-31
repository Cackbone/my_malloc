#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <my_malloc.h>

void *START_MEM_PTR = NULL;

void show_alloc_mem(void)
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *header = NULL;

    printf("break : %lX\n", (size_t)top);
    while (it < top) {
        header = it;
        if (!header->free)
            printf("%lX - %lX : %ld bytes\n", (size_t)(it + sizeof(mem_block_t)), (size_t)(it + sizeof(mem_block_t) + header->size), header->size);
        it += sizeof(mem_block_t) + header->size;
    }
}

static void *get_free_space(size_t size)
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *header = NULL;

    while (it < top) {
        header = it;
        if (header->free && header->size >= size)
            return (it);
        it += sizeof(mem_block_t) + header->size;
    }
    return (NULL);
}

void split_free_space(void *ptr, int size)
{
    void *free_space = NULL;
    mem_block_t *header = ptr;
    size_t remaining_space = header->size - size;
    mem_block_t new_block = {remaining_space - sizeof(mem_block_t), 1};

    if (remaining_space <= sizeof(mem_block_t))
        return;
    header->size = size;
    free_space = (ptr + sizeof(mem_block_t) + size);
    *((mem_block_t *)free_space) = new_block;
}

char alloc_space(size_t size)
{
    size_t pagesize = getpagesize();
    mem_block_t block;
    void *ptr = NULL;

    if (size % (pagesize - sizeof(mem_block_t)) == 0)
        block.size = (size / (pagesize - sizeof(mem_block_t))) * pagesize;
    else
        block.size = ((size / (pagesize - sizeof(mem_block_t))) + 1) * pagesize;
    block.free = 1;
    ptr = sbrk(block.size);
    block.size -= sizeof(mem_block_t);
    if (ptr == (void *)-1)
        return (0);
    *((mem_block_t *)ptr) = block;
    return (1);
}

void *malloc(size_t size)
{
    void *free = NULL;

    if (size == 0)
        return (NULL);
    if (!START_MEM_PTR)
        START_MEM_PTR = sbrk(0);
    free = get_free_space(ALIGN(size));
    if (free == NULL) {
        if (!alloc_space(size))
            return (NULL);
        return (malloc(size));
    } else {
        split_free_space(free, ALIGN(size));
        ((mem_block_t *)free)->free = 0;
        return (free + sizeof(mem_block_t));
    }
}
