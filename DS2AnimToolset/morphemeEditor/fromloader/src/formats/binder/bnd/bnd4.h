#pragma once
#include "../../../util/include.h"
#include "../binder.h"

namespace cfr
{
	class BND4 : public Binder
	{
		private:
		class Header
		{
			public:
			int a;
			Header(UMEM* src);
		};

		class BND4_Entry
		{
			public:
			
			BND4_Entry();

			Entry* newEntry();
		};

		Header header;
		std::vector<BND4_Entry> internalEntries;
		
		public:
		//init from path
		BND4(const char* path);

		BND4(UMEM* src);

		private:
		~BND4();
	};
};