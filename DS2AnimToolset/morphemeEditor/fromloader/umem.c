#include "umem.h"

UMEM* uopenMem(char* src, size_t size)
{
	UMEM* umem = (UMEM*)malloc(sizeof(UMEM));
	umem->mem = (MEM*)malloc(sizeof(MEM));

	umem->mem->src = src;
	umem->mem->size = size;
	umem->mem->position = 0;
	umem->mem->eod = false;
	umem->mem->err = false;

	umem->isFile = false;

	return umem;
};

UMEM* uopenFile(const char* path, const char* mode)
{
	UMEM* umem = (UMEM*)malloc(sizeof(UMEM));

	umem->file = fopen(path,mode);

	umem->isFile = true;

	return umem;
};

size_t uread(void* dest, size_t size, size_t count, UMEM* source)
{
	if(source->isFile)
		return fread(dest,size,count,source->file);
	else
		return mread(dest,size,count,source->mem );
};

size_t uwrite(char* source, size_t size, size_t count, UMEM* dest)
{
	if(dest->isFile)
		return fwrite(source,size,count,dest->file);
	else
		return mwrite(source,size,count,dest->mem );
};

void useek(UMEM* umem, long offset, int whence)
{
	if(umem->isFile)
		fseek(umem->file,offset,whence);
	else
		mseek(umem->mem ,offset,whence);
};

long utell(UMEM* umem)
{
	if(umem->isFile)
		return ftell(umem->file);
	else
		return mtell(umem->mem );
};

void uclose(UMEM* umem)
{
	umem->isFile ? fclose(umem->file) : mclose(umem->mem);
};

char* utellptr(UMEM* umem)
{
	if(umem->isFile)
		return NULL;
	else
		return &umem->mem->src[umem->mem->position];
};

int ueod(UMEM* umem)
{
	if(umem->isFile)
		return feof(umem->file);
	else
		return meod(umem->mem );
};

int uerror(UMEM* umem)
{
	if(umem->isFile)
		return ferror(umem->file);
	else
		return merror(umem->mem );
};

char ugetc(UMEM* umem)
{
	char c;
	uread(&c,sizeof(char),1,umem);
	return c;
};
