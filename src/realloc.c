/*
** EPITECH PROJECT, 2019
** malloc
** File description:
** realloc
*/

#include <string.h>
#include <my_malloc.h>

void *realloc(void *ptr, size_t size)
{
    mem_block_t *block = ptr - sizeof(mem_block_t);
    void *p2 = NULL;

    if (!ptr)
        return malloc(size);
    if (block->size > size) {
        split_free_space(ptr  - sizeof(mem_block_t), size);
        return ptr;
    } else {
        p2 = malloc(size);
        memcpy(p2, ptr, block->size);
        free(ptr);
        return p2;
    }
}
