#include <Windows.h>

namespace RMemory
{
	//Aligns value to a power of 2 alignment
	UINT64 align(UINT64 value, UINT64 alignment);

	inline UINT extractBits(int value, int shiftValue, int mask)
	{
		return (UINT)(value >> shiftValue) & mask;
	}

	template <typename T> inline void fixPtr(T*& ptr, void* base)
	{
		if (ptr != nullptr)
			ptr = reinterpret_cast<T*>((char*)ptr + (ptrdiff_t)base);
	}

	inline ptrdiff_t getOffset(void* ptr, void* base)
	{
		if (ptr != nullptr)
			return (char*)ptr - (char*)base;
	}
}