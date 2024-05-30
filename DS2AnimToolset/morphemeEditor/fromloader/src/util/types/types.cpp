#include "types.h"

namespace cfr
{
	FILE_FORMAT getFormat(UMEM* src)
	{
		useek(src,0,SEEK_SET);
		char magicBytes[20];
		int32_t bhd5_buckets = 0;
		uread(magicBytes,20,1,src);
		useek(src,0,SEEK_SET);

		if(memcmp(magicBytes,"BDF3",4) == 0)
			return FROM_BDF3;
		else if(memcmp(magicBytes,"BDF4",4) == 0)
			return FROM_BDF4;
		else if(memcmp(magicBytes,"BHD5",4) == 0)
			if(magicBytes[5] == 0 && bhd5_buckets == 0x18)
				return FROM_BHD5_0;
			else if(magicBytes[6] == 1 && bhd5_buckets != 0x18)
				return FROM_BHD5_1;
			else
				throw std::runtime_error("Could not determine BHD5 version...\n");
		else if(memcmp(magicBytes,"BHF3",4) == 0)
			return FROM_BHF3;
		else if(memcmp(magicBytes,"BHF4",4) == 0)
			return FROM_BHF4;
		else if(memcmp(magicBytes,"BND3",4) == 0)
			return FROM_BND3;
		else if(memcmp(magicBytes,"BND4",4) == 0)
			return FROM_BND4;
		else if(false)
			return FROM_BTL;
		else if(memcmp(magicBytes,"DCX",3) == 0)
			return FROM_DCX;
		else if(false)
			return FROM_DRB;
		else if(false)
			return FROM_ENTRYFILELIST;
		else if(false)
			return FROM_ESD;
		else if(false)
			return FROM_EMEVD;
		else if(false)
			return FROM_FFX;
		else if(memcmp(magicBytes,"FLVER0",6) == 0) //this will never trigger
			return FROM_FLVER0;
		else if(memcmp(magicBytes,"FLVER\0",6) == 0)
			return FROM_FLVER2;
		else if(false)
			return FROM_FSB;
		else if(false)
			return FROM_FEV;
		else if(false)
			return FROM_ITL;
		else if(false)
			return FROM_GFX;
		else if(false)
			return FROM_GPARAM;
		else if(false)
			return FROM_LUA;
		else if(memcmp(magicBytes,"MSB ",4) == 0)
			return FROM_MSB;
		else if(false)
			return FROM_MTD;
		else if(false)
			return FROM_NGP;
		else if(false)
			return FROM_NVA;
		else if(false)
			return FROM_NVM;
		else if(false)
			return FROM_PARAM;
		else if(false)
			return FROM_SL2;
		else if(memcmp(magicBytes,"TAE ",4) == 0)
			return FROM_TAE;
		else if(memcmp(magicBytes,"TPF\0",4) == 0)
			return FROM_TPF;
		else if(false)
			return HAVOK_HKS;
		else if(
			(magicBytes[4] == 'T') && 
			(magicBytes[5] == 'A') && 
			(magicBytes[6] == 'G') &&
			(magicBytes[7] == '0')
		)
		{
			return HAVOK_HKX;
		}
		else if(
			(magicBytes[0] == 1) && 
			(magicBytes[1] == 0) && 
			(magicBytes[2] == 0) &&
			(magicBytes[3] == 0) &&
			(magicBytes[4] == 0) &&
			(magicBytes[5] == 0) &&
			(magicBytes[6] != 0) &&
			(magicBytes[7] == 0)
		)
		{
			return HAVOK_HKXPWV;
		}
		else if(false)
			return MS_DDS;
		else
		{
			printf("Found type: ");

			for(int i = 0; i < 20; i++)
				printf("%c",magicBytes[i]);

			printf("\n");
			return UNKOWN_FORMAT;
		}
	};
};