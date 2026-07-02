#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "CMA.h"

static Block *heapStart = NULL;
static Block *searchStart = NULL;
static Block *top = NULL;
static SearchMode searchMode = FirstFit;

// rounds up size n to the next multiple of a pointer size on the system
static inline size_t align(size_t n)
{
    return (n + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
}

/* changed from
static Block* getBlock(void *data) {
    return (Block *)((char *)data - sizeof(Block) + sizeof(data));
}*/
static Block *getBlock(void *data)
{
    return (Block *)((char *)data - sizeof(Block));
}

static bool canMerge(Block *block)
{
    if (!top)
        return false;
    Block *next = block->next;
    return next != NULL && next <= top && next->free;
}

static void merge(Block *block)
{
    Block *next = block->next;

    if (!next || next->free == NOT_FREE)
        return;

    block->size += next->size + sizeof(Block);
    block->next = next->next;

    if (block->next)
        block->next->prev = block;
}

void _free(void *data)
{
    Block *block = getBlock(data);
    block->free = FREE;
    if (canMerge(block))
        merge(block);
}

static Block *firstFit(size_t size)
{
    for (Block *current = heapStart; current; current = current->next)
        if (current->free && current->size >= size)
            return current;

    return NO_FREE_BLOCK;
}

static Block *nextFit(size_t size)
{
    // added
    if (heapStart == NULL)
    {
        return NO_FREE_BLOCK;
    }

    if (searchStart == NULL)
        searchStart = heapStart;

    Block *current = searchStart;
    do
    {
        if (current->free && current->size >= size)
        {
            searchStart = current->next;
            return current;
        }
        current = current->next ? current->next : heapStart;
    } while (current != searchStart);

    return NO_FREE_BLOCK;
}

static Block *bestFit(size_t size)
{
    Block *best = NULL;

    for (Block *current = heapStart; current; current = current->next)
    {

        if (current->free && current->size >= size)
        {
            if (current->size == size)
            {
                return current;
            }
            else if (!best || current->size < best->size)
            {
                best = current;
            }
        }
    }

    return best;
}

static bool canSplit(Block *block, size_t size)
{
    return block->size >= size + sizeof(Block);
}

static void split(Block *block, size_t size)
{

    size_t originalSize = block->size;
    block->size = size;
    block->data = (char *)block + sizeof(Block);

    Block *newBlock = (Block *)((char *)block + sizeof(Block) + size);
    newBlock->size = originalSize - size - sizeof(Block);
    newBlock->free = true;
    newBlock->data = (char *)newBlock + sizeof(Block);

    newBlock->next = block->next;
    if (newBlock->next)
        newBlock->next->prev = newBlock;

    block->next = newBlock;
    newBlock->prev = block;
}

void selectAlgorithm(SearchMode algorithm)
{
    searchMode = algorithm;
}

static Block *findBlock(size_t size)
{
    Block *block;

    switch (searchMode)
    {
    case BestFit:
        block = bestFit(size);
        break;
    case FirstFit:
        block = firstFit(size);
        break;
    case NextFit:
        block = nextFit(size);
        break;
    }

    if (block && canSplit(block, size))
        split(block, size);

    return block;
}

static Block *requestFromOS(size_t size)
{
    size = align(size + sizeof(Block));
    Block *block = (Block *)sbrk(size);

    if (block == (void *)-1)
        return NO_FREE_BLOCK;

    block->size = size - sizeof(Block);
    block->free = FREE;
    block->next = NULL;
    block->prev = top;
    block->data = (void *)((char *)block + sizeof(Block));

    if (top)
        top->next = block;

    top = block;

    if (!heapStart)
        heapStart = block;

    return block;
}

void *_malloc(size_t size)
{
    if (size <= 0)
        return NULL;

    size = align(size);
    Block *block = findBlock(size);

    if (block != NULL)
    {
        block->free = false;
        return block->data;
    }

    block = requestFromOS(size + sizeof(Block));

    if (block != NULL)
    {
        block->size = size;
        block->free = false;
        return block->data;
    }

    return NULL;
}

void *_calloc(size_t n, size_t size)
{
    void *ptr = _malloc(n * size);
    if (ptr)
        memset(ptr, 0, n * size);

    return ptr;
}

// should split after realloc in case newsize <= oldblock.size??
void *_realloc(void *data, size_t newSize)
{
    if (data == NULL)
        return _malloc(newSize);

    Block *oldBlock = getBlock(data);
    if (newSize <= oldBlock->size)
        return data;

    void *newBlock = _malloc(newSize);
    if (newBlock)
    {
        memcpy(newBlock, data, oldBlock->size);
    }
    _free(data);
    return newBlock;
}
