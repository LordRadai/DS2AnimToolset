#include "fileio.h"
#define _CRT_SECURE_NO_DEPRECATE //shuts up microsoft's prattle

namespace cfr
{
#ifdef _WIN32
	FILE* v_fopen(const char* filename, const char* modes)
#else
	FILE* v_fopen(const char* __restrict__ __filename, const char* __restrict__ __modes)
#endif
	{
		errno = 0;

		FILE* ptr = fopen(
#ifdef _WIN32
			filename,modes
#else
			__filename,__modes
#endif
		);

		if(ptr == NULL || errno != 0)
		{
			perror("v_fopen() error: ");
			exit(1);
		}

		return ptr;
	};

};