#pragma once
#include "../binder.h"

namespace cfr
{
	class BDF4 : public Binder
	{
		private:
		class Header
		{

		};

		class BDF4_Entry
		{

		};

		Header header;
		std::vector<BDF4_Entry> internalEntries;

		public:
		//Init from path
		BDF4(const char* path);

		BDF4(UMEM* src);

		private:
		~BDF4();
	};
};