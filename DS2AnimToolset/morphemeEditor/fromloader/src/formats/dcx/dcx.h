#pragma once
#include <zlib.h>
#include "../../util/endian/endian.h"
#include "../../util/inflate/inflate.h"
#include "../../../umem.h"

#ifndef CFR_DCX
#define CFR_DCX

namespace cfr
{
	//Returns a UMEM handle for a decompressed DCX file.
	UMEM* openDCX(const char* path);

	//Returns a UMEM handle for a decompressed DCX file.
	UMEM* openDCX(UMEM* src);
};

#endif