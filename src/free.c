/*
** EPITECH PROJECT, 2019
** malloc
** File description:
** free
*/

#include <unistd.h>
#include <my_malloc.h>

mem_block_t *merge_blocks()
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *block = NULL;
    mem_block_t *next = NULL;
    mem_block_t *last = NULL;

    while ((size_t)it < (size_t)top) {
        block = it;
        next = it + block->size + sizeof(mem_block_t);
        if (block->free && (size_t)next < (size_t)top && next->free) {
            block->size += sizeof(mem_block_t) + next->size;
        } else {
            last = it;
            it += block->size + sizeof(mem_block_t);
        }
    }
    return (last);
}

void free(void *ptr)
{
    mem_block_t *block = ptr - sizeof(mem_block_t);
    mem_block_t *last = NULL;
    size_t pagesize = getpagesize();

    if (!ptr)
        return;
    block->free = 1;
    last = merge_blocks();
    if (last->free && last->size >= 2 * pagesize) {
        sbrk(-((last->size / pagesize - 1) * pagesize));
        last->size -= (last->size / pagesize - 1) * pagesize;
    }
}
