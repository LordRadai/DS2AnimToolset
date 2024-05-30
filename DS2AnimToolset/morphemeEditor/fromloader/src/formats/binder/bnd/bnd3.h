#ifndef CFR_BND3
#define CFR_BND3

#pragma once
#include "../binder.h"
#include "../../../util/stringio/stringio.h"
//#include
#include "../../dcx/dcx.h"

namespace cfr
{
	class BND3 : public Binder
	{
		protected:
		//fill whole struct
		class Header
		{
			public:
			char magic[4] = {};
			char version[8] = {};
			
			uint8_t rawFormat = 0;
			uint8_t bigEndian = 0;
			uint8_t bitBigEndian = 0;
			uint8_t unk0F = 0;

			int32_t fileCount = 0;
			int32_t fileHeadersEnd = 0;
			int32_t unk18 = 0;
			int32_t unk1C = 0; //assert 0

			Header();
			Header(UMEM* src);
		};

		class BND3_Entry
		{
			public:
			char rawFlags = '\0';
			char unk01 = '\0';
			char unk02 = '\0';
			char unk03 = '\0';

			int32_t compressedSize = 0;
			uint32_t dataOffset = 0;

			//circumstationally fill these in
			int32_t id = 0;
			int32_t nameOffset = 0;
			int32_t uncompressedSize = 0;

			BND3_Entry();
			BND3_Entry(Header* header, UMEM* src, long startPos);

			//Entry* newEntry(UMEM* src, int startPos);
		};

		//public:
		Header* header = nullptr;
		std::vector<BND3_Entry*> internalEntries = {};

		public:		
		//init from path
		BND3(const char* path);

		BND3(UMEM* src);

		private:
		void BND3init(BND3* b, UMEM* src);
		//~BND3();
	};
};

#endif