#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdlib.h>
#include <unistd.h>

#if defined(__x86_64__)
#define BLOCK_SIZE 8
#endif
#if defined(__i386__)
#define BLOCK_SIZE 4
#endif

// Get the number of block needed to reserve x bytes in memory
#define ALIGN(x) ((x + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1))

extern void *START_MEM_PTR;
extern size_t PAGE_SIZE;
extern pthread_mutex_t MALLOC_MUTEX;

typedef struct mem_block_s
{
    size_t size;
    char free;
} mem_block_t;

void *malloc(size_t);
void free(void *);
void split_free_space(void *, int);
void show_alloc_mem(void);
void *my_sbrk(intptr_t);


#endif
