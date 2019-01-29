#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <my_malloc.h>

void *START_MEM_PTR = NULL;

void putstr(char *str)
{
    write(1, str, strlen(str));
}

void print_nbr(size_t nb)
{
    char c = 0;
    
    if (nb > 9) {
	print_nbr(nb / 10);
	print_nbr(nb % 10);
    } else {
	c = nb + 48;
	write(1, &c, 1);
    }
}

void putstr_err(char *str)
{
    write(2, str, strlen(str));
}

void print_nbr_err(size_t nb)
{
    char c = 0;
    
    if (nb > 9) {
	print_nbr(nb / 10);
	print_nbr(nb % 10);
    } else {
	c = nb + 48;
	write(2, &c, 1);
    }
}

static void show_alloc_mem()
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *header = NULL;
    char stop = 0;
    
    putstr("break : ");
    print_nbr((size_t)START_MEM_PTR);
    putstr("\n");
    putstr("end : ");
    print_nbr((size_t)top);
    putstr("\n");
    while (it < top) {
	header = it;
        print_nbr((size_t)(it));
	write(1, " -- ", 4);
        print_nbr((size_t)(it + sizeof(mem_block_t) + header->size));
	write(1, " -- ", 4);
	print_nbr(header->size);
	write(1, " -- ", 4);
	print_nbr(header->free);
	write(1, "\n", 1);
	if (stop)
	    exit(1);
	/* if (header->size == 0) */
	/*     stop = 1; */
	it += sizeof(mem_block_t) + header->size;
    }
}

void print_split_debug(size_t hsize, size_t size, size_t rs)
{
    write(1, "DEBUG\n", 6);
    print_nbr(hsize);
    write(1, "\n", 1);
    print_nbr(size);
    write(1, "\n", 1);
    print_nbr(rs);
    write(1, "\n", 1);
}

static void *get_free_space(size_t size)
{
    void *it = START_MEM_PTR;
    void *top = sbrk(0);
    mem_block_t *header = NULL;

    while (it < top) {
        header = it;
	/* putstr("size: "); */
	/* print_nbr(header->size); */
	/* putstr("\tit: "); */
	/* print_nbr((size_t)it); */
	/* putstr("\n"); */
        if (header->free && header->size >= size)
            return it;
	if (header->size == 0) {
	    //putstr("\n\n\n\n");
	    show_alloc_mem();
	    //exit(1);
	}
        it += sizeof(mem_block_t) + header->size;
    }
    return NULL;
}

void split_free_space(void *ptr, int size)
{
    void *free_space = NULL;
    mem_block_t *header = ptr;
    size_t remaining_space = header->size - size;
    mem_block_t new_block = {remaining_space - sizeof(mem_block_t), 1};

    putstr("Remaining space: ");
    print_nbr(remaining_space);
    putstr("\n");
    if (remaining_space <= sizeof(mem_block_t))
        return;
    putstr("[split] Create new block of size ");
    print_nbr(remaining_space - sizeof(mem_block_t));
    putstr("\n");
    print_split_debug(header->size, size, remaining_space - sizeof(mem_block_t));
    header->size = size;
    putstr("[split] Resize block to ");
    print_nbr(size);
    putstr("\n");
    free_space = (ptr + sizeof(mem_block_t) + size);
    putstr("[Create block]: ");
    print_nbr((size_t)((mem_block_t *)free_space));
    putstr("\n");
    *((mem_block_t *)free_space) = new_block;
}

void *malloc(size_t size)
{
    void *free = NULL;
    mem_block_t block;
    void *ptr = NULL;
    size_t pagesize = getpagesize();

    if (size == 0)
	return NULL;
    putstr("malloc(");
    print_nbr(size);
    putstr(");\n");
    if (!START_MEM_PTR)
        START_MEM_PTR = sbrk(0);
    free = get_free_space(ALIGN(size));
    if (!free) {
	putstr("No space available\n");
	if (size % pagesize == 0)
	    block.size = ((size / pagesize)) * pagesize;
	else
	    block.size = ((size / pagesize) + 1) * pagesize;
        block.free = 1;
        ptr = sbrk(sizeof(mem_block_t) + block.size);
	if (ptr == (void *)-1)
	    return NULL;
        *((mem_block_t *)ptr) = block;
	putstr("[Create block]: ");
	print_nbr((size_t)((mem_block_t *)ptr));
	putstr("\n");
	return (malloc(size));
    } else {
        split_free_space(free, ALIGN(size));
	((mem_block_t *)free)->free = 0;
	putstr("[malloc] Create new block of size ");
	print_nbr(ALIGN(size));
	putstr("\n");
        return (free + sizeof(mem_block_t));
    }
}

void free(void *ptr)
{
    if (!ptr) {
	return;
    }
    mem_block_t *block = ptr - sizeof(mem_block_t);

    block->free = 1;
}
