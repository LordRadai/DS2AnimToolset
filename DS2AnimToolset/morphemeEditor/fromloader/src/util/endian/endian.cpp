#include "endian.h"

#if defined(__GNUC__) || defined(__clang__)
	#define cfr_bswap16 __builtin_bswap16
	#define cfr_bswap32 __builtin_bswap32
	#define cfr_bswap64 __builtin_bswap64
#elif defined(_MSC_VER)
	#define cfr_bswap16 _byteswap_ushort
	#define cfr_bswap32 _byteswap_ulong
	#define cfr_bswap64 _byteswap_uint64
#else
			#error "Undefined byteswap!" 
#endif

namespace cfr
{
	size_t ureade(void* dest, size_t size, size_t count, UMEM* src, bool flipEndian)
	{
		size_t c = 0;

		for(int o = 0; o < count; o++)
		{
			void* tmp = malloc(size);
			c += uread(tmp,size,1,src);

			if(flipEndian)
			{
				switch(size)
				{
					case(2):
					{
						uint16_t i = cfr_bswap16(*(uint16_t*)tmp);
						memcpy((char*)dest+(o*size),&i,sizeof(uint16_t));
					}
					case(4):
					{
						uint32_t i = cfr_bswap32(*(uint32_t*)tmp);
						memcpy((char*)dest+(o*size),&i,sizeof(uint32_t));
					}
					case(8):
					{
						uint64_t i = cfr_bswap64(*(uint64_t*)tmp);
						memcpy((char*)dest+(o*size),&i,sizeof(uint64_t));
					}
					default: memcpy((char*)dest+(o*size),tmp,size);
				}
			}
			else
			{
				memcpy((char*)dest+(o*size),tmp,size);
			}

			free(tmp);
		}
		
		return c;
	};

	uint16_t switchEndian(uint16_t i, bool bigEndian)
	{
		if(bigEndian)
			return cfr_bswap16(i);
		else
			return i;
	};

	int16_t switchEndian(int16_t i, bool bigEndian)
	{
		if(bigEndian)
			return cfr_bswap16(i);
		else
			return i;
	};

	uint32_t switchEndian(uint32_t i, bool bigEndian)
	{
		if(bigEndian)
			return cfr_bswap32(i);
		else
			return i;
	};

	int32_t switchEndian(int32_t i, bool bigEndian)
	{
		if(bigEndian)
			return cfr_bswap32(i);
		else
			return i;
	};

	uint64_t switchEndian(uint64_t i, bool bigEndian)
	{
		if(bigEndian)
			return cfr_bswap64(i);
		else
			return i;
	};

	int64_t switchEndian(int64_t i, bool bigEndian)
	{
		if(bigEndian)
			return cfr_bswap64(i);
		else
			return i;
	};

	/*void switchEndian(void* target, size_t size)
	{
		switch(size)
		{
			case(16):
			{
				uint16_t i = __builtin_bswap16(*(uint16_t*)target);
				memcpy(target,&i,sizeof(uint16_t));
			}
			case(32):
			{
				uint32_t i = __builtin_bswap32(*(uint32_t*)target);
				memcpy(target,&i,sizeof(uint32_t));
			}
			case(64):
			{
				uint64_t i = __builtin_bswap64(*(uint64_t*)target);
				memcpy(target,&i,sizeof(uint64_t));
			}
			case(128):
			{
				__uint128_t i = __builtin_bswap128(*(__uint128_t*)target);
				memcpy(target,&i,sizeof(__uint128_t));
			}
			default: throw std::runtime_error("Invalid bytecount!\n");
		}
	};*/

/*	size_t getline2(char** lineptr, size_t* n, FILE* stream)
	{
#ifdef _GNU
		return getline(lineptr,n,stream);
#else
		int i = 0;

		while(feof(stream))
		{

		}
#endif
	};*/
};