#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class MorphemeDB : public db::Database
	{
	public:
		MorphemeDB() : db::Database("MorphemeDB", "MorphemeDB") {};
	};
}
