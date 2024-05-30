#pragma once

#include "cmem.h"
#include <string.h> //memcpy
#include <stdbool.h> //bools
#include <stdlib.h> //free
#include <stdio.h>

#ifndef UMEM__
#define UMEM__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UMEM
{
	bool isFile;
	FILE* file;
	MEM* mem;
} UMEM;

//Open arbitrary data for RW operations, not thread safe :^)
UMEM* uopenMem(char* src, size_t size);

//Open file, just like fopen(). Does not load to memory.
UMEM* uopenFile(const char* path, const char* mode);

//Load the current file into memory and closes the handle.
size_t ufileToMem(UMEM*);

//Read data like with fread()
size_t uread(void* dest, size_t size, size_t n, UMEM* source);

//Write data like with fwrite()
size_t uwrite(char* source, size_t size, size_t count, UMEM* dest);

//Set position like with fseek()
void useek(UMEM* umem, long offset, int whence);

//Returns current position in data
long utell(UMEM* umem);

//Close the data or something
void uclose(UMEM* umem);

//If end of data has been reached
int ueod(UMEM* umem);

//If an error has occurred
int uerror(UMEM* umem);

//Return pointer to position in data
char* utellptr(UMEM* umem);

//Returns the next character
char ugetc(UMEM* umem);

#ifdef __cplusplus
}
#endif

#endif
