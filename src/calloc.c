/*
** EPITECH PROJECT, 2019
** malloc
** File description:
** caloc
*/

#include <string.h>
#include <my_malloc.h>

void *calloc(size_t nmemb, size_t size)
{
    void *ptr = malloc(nmemb * size);

    memset(ptr, 0, nmemb * size);
    return (ptr);
}
