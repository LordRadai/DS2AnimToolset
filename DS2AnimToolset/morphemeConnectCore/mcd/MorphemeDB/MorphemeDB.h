#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcc
{
	class MorphemeDocument; // Forward declaration to allow friend access in MorphemeDB
}

namespace mcd
{
	class MorphemeDB : public db::Database
	{
		friend class mcc::MorphemeDocument; // Allow MorphemeDocument to access private members if needed
	public:
		MorphemeDB() : db::Database("MorphemeDB", "MorphemeDB") {};
	};
}
