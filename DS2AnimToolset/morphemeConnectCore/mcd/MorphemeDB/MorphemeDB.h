#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Network/Network.h"
#include "NMDBExtensions/TypedNodeContainer.inl"

namespace mcc
{
	class MorphemeDocument;
}

namespace mcd
{
	class MorphemeDB : public db::Database
	{
		friend class mcc::MorphemeDocument;

		std::unique_ptr<db::TypedNodeContainer<mcd::Network>> m_networks;
	public:
		MorphemeDB();

		~MorphemeDB() override {};

		void createNetwork(const std::string& name);
		Network* getNetwork() const;
	};
}
