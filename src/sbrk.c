#include <my_malloc.h>

void *my_sbrk(intptr_t inc)
{
    static void *counter = NULL;

    if (!counter)
        counter = sbrk(0);
    if (inc == 0)
        return counter;
    counter += inc;
    return sbrk(inc);
}
