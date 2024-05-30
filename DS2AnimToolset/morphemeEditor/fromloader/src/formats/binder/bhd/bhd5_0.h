#pragma once
#include "../binder.h"

namespace cfr
{
	class BHD5_0 : public Binder
	{
		private:
		class Header
		{

		};

		class BHD5_0_Entry
		{

		};

		Header header;
		std::vector<BHD5_0_Entry> internalEntries;

		public:
		//Init from path
		BHD5_0(const char* path);

		BHD5_0(UMEM* src);

		private:
		~BHD5_0();
	};
};