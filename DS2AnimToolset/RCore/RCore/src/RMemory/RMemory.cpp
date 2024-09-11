#include "RMemory.h"
#include <assert.h>

UINT64 RMemory::align(UINT64 value, UINT64 alignment)
{
	if ((alignment == 0) || (alignment & (alignment - 1)) != 0)
		throw("Alignment is not a power of 2");

	const ptrdiff_t align_ = (alignment - 1);
	return (value + align_) & ~align_;
}