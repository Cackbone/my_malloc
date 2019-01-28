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
        if (header->free && header->size <= size)
            return it;
        it += sizeof(mem_block_t) + header->size + header->offset;
    }
    return NULL;
}

char get_alignment(void *ptr)
{
    const size_t ptr_addr = (size_t)ptr;
    return ((ptr_addr + sizeof(mem_block_t)) >>
            (BLOCK_SIZE >> 1)) << (BLOCK_SIZE >> 1);
}

void split_free_space(void *ptr, int size)
{
    mem_block_t *free_space;
    mem_block_t *header = ptr;
    size_t remaining_space = header->size - size;
    mem_block_t new_block = {remaining_space,
                             1,
                             get_alignment(ptr + sizeof(mem_block_t) +
                                           header->offset + header->size)
    };

    if (remaining_space <= sizeof(mem_block_t))
        return;
    header->size = size;
    free_space = (mem_block_t *)(ptr + sizeof(mem_block_t) + header->offset + header->size);
    *free_space = new_block;
}

void *malloc(size_t size)
{
    void *free = NULL;
    mem_block_t block;
    void *ptr = NULL;

    if (!START_MEM_PTR)
        START_MEM_PTR = sbrk(0);
    free = get_free_space(size);
    if (!free) {
        block.size = size;
        block.free = 0;
        block.offset = get_alignment(sbrk(0));
        ptr = sbrk(sizeof(mem_block_t) + block.size + block.offset);
        *((mem_block_t *)ptr) = block;
        return (ptr + sizeof(mem_block_t) + block.offset);
    } else {
        split_free_space(free, size);
        block.size = size;
        block.free = 0;
        block.offset = get_alignment(free);
        *((mem_block_t *)free) = block;
        return (free + sizeof(mem_block_t) + block.offset);
    }
}

void free(__attribute__((unused))void *ptr) {}
