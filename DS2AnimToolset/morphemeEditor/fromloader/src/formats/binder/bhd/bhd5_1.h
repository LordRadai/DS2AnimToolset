#pragma once
#include "../binder.h"

namespace cfr
{
	class BHD5_1 : public Binder
	{
		private:
		class Header
		{

		};

		class BHD5_1_Entry
		{

		};

		Header header;
		std::vector<BHD5_1_Entry> internalEntries;

		public:
		//Init from path
		BHD5_1(const char* path);

		BHD5_1(UMEM* src);

		private:
		~BHD5_1();
	};
};