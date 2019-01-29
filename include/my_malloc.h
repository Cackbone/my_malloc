#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdlib.h>

#if defined(__x86_64__)
#define BLOCK_SIZE 8
#endif
#if defined(__i386__)
#define BLOCK_SIZE 4
#endif

// Get the number of block needed to reserve x bytes in memory
#define ALIGN(x) ((x + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1))

extern void *START_MEM_PTR;

typedef struct mem_block_s
{
    size_t size;
    char free;
} mem_block_t;

void *malloc(size_t);
void free(void *);

#endif
