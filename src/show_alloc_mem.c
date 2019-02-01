#include <unistd.h>
#include <stdio.h>
#include <my_malloc.h>

void show_alloc_mem(void)
{
    void *it = START_MEM_PTR;
    void *top = my_sbrk(0);
    mem_block_t *header = NULL;

    printf("break : %lX\n", (size_t)top);
    while (it < top) {
        header = it;
        if (!header->free)
            printf("%lX - %lX : %ld bytes\n", (size_t)(it + sizeof(mem_block_t)), (size_t)(it + sizeof(mem_block_t) + header->size), header->size);
        it += sizeof(mem_block_t) + header->size;
    }
}
