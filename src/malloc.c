#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <my_malloc.h>

void *START_MEM_PTR = NULL;
size_t PAGE_SIZE = 0;
pthread_mutex_t MALLOC_MUTEX = PTHREAD_MUTEX_INITIALIZER;

static void *get_free_space(size_t size)
{
    void *it = START_MEM_PTR;
    void *top = my_sbrk(0);
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

void *reuse_space(void *ptr, size_t size)
{
    if (!ptr || size == 0)
        return (NULL);
    split_free_space(ptr, ALIGN(size));
    ((mem_block_t *)ptr)->free = 0;
    return (ptr + sizeof(mem_block_t));
}

void *alloc_space(size_t size)
{
    size_t pagesize = PAGE_SIZE;
    mem_block_t block;
    void *ptr = NULL;

    if (size % (pagesize - sizeof(mem_block_t)) == 0)
        block.size = (size / (pagesize - sizeof(mem_block_t))) * pagesize;
    else
        block.size = ((size / (pagesize - sizeof(mem_block_t))) + 1) * pagesize;
    block.free = 1;
    ptr = my_sbrk(block.size);
    block.size -= sizeof(mem_block_t);
    if (ptr == (void *)-1)
        return (reuse_space(NULL, 0));
    *((mem_block_t *)ptr) = block;
    return (reuse_space(ptr, size));
}

void *malloc(size_t size)
{
    void *free = NULL;

    pthread_mutex_lock(&MALLOC_MUTEX);
    if (PAGE_SIZE == 0)
        PAGE_SIZE = getpagesize();
    if (size == 0) {
        pthread_mutex_unlock(&MALLOC_MUTEX);
        return (NULL);
    }
    if (!START_MEM_PTR)
        START_MEM_PTR = my_sbrk(0);
    free = get_free_space(ALIGN(size));
    if (free == NULL) {
        pthread_mutex_unlock(&MALLOC_MUTEX);
        return (alloc_space(size));
    } else {
        pthread_mutex_unlock(&MALLOC_MUTEX);
        return (reuse_space(free, size));
    }
}
