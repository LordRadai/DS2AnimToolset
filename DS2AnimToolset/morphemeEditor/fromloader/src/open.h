#ifndef CFR_OPEN_FILE
#define CFR_OPEN_FILE

#pragma once
#include "formats/binder/bnd/bnd3.h"
#include "formats/dcx/dcx.h"
#include "formats/flver/flver2/flver2.h"
#include "util/include.h"

namespace cfr
{
	//Open file from disk. Null if fails.
	File* OpenFile(const char* path);

	//Open file from memory. Null if fails.
	File* OpenFile(UMEM* src);
};

#endif