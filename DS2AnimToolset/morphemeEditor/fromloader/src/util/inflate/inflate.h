#pragma once
#include "../file/file.h"
#include <zlib.h>
#include <assert.h>

#ifndef CFR_ZLIB_INF
#define CFR_ZLIB_INF

//TODO: is this needed?
#ifdef _WIN32
#include "io.h"
#endif

#ifndef CHUNK
#	define CHUNK 16384
#endif

namespace cfr
{
	int inf(UMEM *source, UMEM *dest);
};

#endif