# Custom Memory Allocator in C

A simple custom memory allocator written in C. The project implements basic versions of `malloc`, `free`, `calloc`, and `realloc`, using a linked list of memory blocks and different allocation strategies.

## Features

- Custom `_malloc`, `_free`, `_calloc`, and `_realloc` functions
- First-fit allocation
- Best-fit allocation
- Next-fit allocation
- Block splitting
- Adjacent free block merging
- Basic test program for comparing allocation strategies
