/*
** EPITECH PROJECT, 2019
** malloc
** File description:
** realloc
*/

#include <string.h>
#include <pthread.h>
#include <my_malloc.h>

pthread_mutex_t REALLOC_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void *realloc(void *ptr, size_t size)
{
    mem_block_t *block = ptr - sizeof(mem_block_t);
    void *p2 = NULL;
    void *res = NULL;

    pthread_mutex_lock(&REALLOC_MUTEX);
    if (!ptr) {
        res = malloc(size);
        pthread_mutex_unlock(&REALLOC_MUTEX);
        return res;
    }
    if (block->size > size) {
        split_free_space(ptr  - sizeof(mem_block_t), size);
        pthread_mutex_unlock(&REALLOC_MUTEX);
        return ptr;
    }
    p2 = malloc(size);
    memcpy(p2, ptr, block->size);
    free(ptr);
    pthread_mutex_unlock(&REALLOC_MUTEX);
    return p2;
}
