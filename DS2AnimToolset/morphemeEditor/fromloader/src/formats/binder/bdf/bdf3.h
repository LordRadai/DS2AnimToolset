#pragma once
#include "../binder.h"

#ifndef CFR_BDF3
#define CFR_BDF3

namespace cfr
{
	class BDF3 : public Binder
	{
		private:
		class Header
		{
			int a;
		};

		class BDF3_Entry
		{
			int a;
		};

		Header header;
		std::vector<BDF3_Entry> internalEntries;

		public:
		//Init from path
		BDF3(const char* path);

		BDF3(UMEM* src);

		private:
		~BDF3();
	};
};

#endif