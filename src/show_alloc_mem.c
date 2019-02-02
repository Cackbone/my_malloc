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
        if (!header->free) {
            printf("%lX", (size_t)(it + sizeof(mem_block_t)));
            printf(" - %lX", (size_t)(it + sizeof(mem_block_t) + header->size));
            printf(" : %ld bytes\n", header->size);
        }
        it += sizeof(mem_block_t) + header->size;
    }
}
