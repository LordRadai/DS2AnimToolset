#pragma once
#include <stdio.h>
#include <stdexcept>
#include <cstring>
#include <stdlib.h> //needed for MSVC

#include "../../../umem.h"

#ifndef CFR_ENDIAN
#define CFR_ENDIAN

namespace cfr
{
	//size_t getline2(char** lineptr, size_t* n, FILE* stream);

	//Read each element in, flips element byte order on true, VERY SLOW
	size_t ureade(void* dest, size_t size, size_t count, UMEM* src, bool flipEndian);

	uint16_t switchEndian(uint16_t i, bool bigEndian = true);
	int16_t  switchEndian( int16_t i, bool bigEndian = true);
	uint32_t switchEndian(uint32_t i, bool bigEndian = true);
	int32_t  switchEndian( int32_t i, bool bigEndian = true);
	uint64_t switchEndian(uint64_t i, bool bigEndian = true);
	int64_t  switchEndian( int64_t i, bool bigEndian = true);
};

#endif