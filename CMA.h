#ifndef CMA_H
#define CMA_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct Block
{
    size_t size;
    bool free;
    struct Block *prev;
    struct Block *next;
    void *data;
} Block;

typedef enum SearchMode
{
    BestFit,
    FirstFit,
    NextFit,
} SearchMode;

#define FREE true
#define NOT_FREE false
#define NO_FREE_BLOCK NULL

void selectAlgorithm(SearchMode algorithm);

void *_malloc(size_t size);
void *_calloc(size_t n, size_t size);
void *_realloc(void *data, size_t newSize);
void _free(void *data);

#endif /* CMA_H */
