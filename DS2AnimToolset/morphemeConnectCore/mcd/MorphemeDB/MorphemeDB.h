#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Network/Network.h"

namespace mcc
{
	class MorphemeDocument; // Forward declaration to allow friend access in MorphemeDB
}

namespace mcd
{
	class MorphemeDB : public db::Database
	{
		friend class mcc::MorphemeDocument; // Allow MorphemeDocument to access private members if needed

		db::NodeContainer m_networks;
	public:
		MorphemeDB();

		~MorphemeDB() override {};

		void createNetwork(const std::string& name);
		Network* getNetwork() const;
	};
}
