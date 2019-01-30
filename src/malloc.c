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

void show_alloc_mem()
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
    putstr("diff : ");
    print_nbr((size_t)top - (size_t)START_MEM_PTR);
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
	if (header->size == 0) {
	    putstr("\e[0;31mHeader free: ");
	    print_nbr((size_t)&(header->free));
	    putstr("\e[0m\n");
	    stop = 1;
	}
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
        if (header->free && header->size >= size)
            return it;
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

    if (remaining_space <= sizeof(mem_block_t))
        return;
    header->size = size;
    free_space = (ptr + sizeof(mem_block_t) + size);
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
    if (!START_MEM_PTR)
        START_MEM_PTR = sbrk(0);
    free = get_free_space(ALIGN(size));
    if (free == NULL) {
	if (size % (pagesize - sizeof(mem_block_t)) == 0)
	    block.size = (size / (pagesize - sizeof(mem_block_t))) * pagesize;
	else
	    block.size = ((size / (pagesize - sizeof(mem_block_t))) + 1) * pagesize;
        block.free = 1;
        ptr = sbrk(block.size);
	block.size -= sizeof(mem_block_t);
	if (ptr == (void *)-1)
	    return NULL;
        *((mem_block_t *)ptr) = block;
        return malloc(size);
    } else {
        split_free_space(free, ALIGN(size));
	((mem_block_t *)free)->free = 0;
        return free + sizeof(mem_block_t);
    }
}

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
    return last;
}

void free(void *ptr)
{
    mem_block_t *block = ptr - sizeof(mem_block_t);
    mem_block_t *last = NULL;
    size_t pagesize = getpagesize();

    if (!ptr) {
	//show_alloc_mem();
	return;
    }

    block->free = 1;
    last = merge_blocks();

    if (last->free && last->size >= 2 * pagesize) {
	sbrk(-((last->size / pagesize - 1) * pagesize));
	last->size -= (last->size / pagesize - 1) * pagesize;
    }
}

void *calloc(size_t nmemb, size_t size)
{
    void *ptr = malloc(nmemb * size);

    memset(ptr, 0, nmemb * size);
    return ptr;
}

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
