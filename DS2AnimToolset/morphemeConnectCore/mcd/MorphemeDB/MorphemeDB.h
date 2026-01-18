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

		/*
		* \brief Get the number of flow edges connected to a given pin.
		* \param pin The pin to check.
		* \return The number of connected flow edges.
		*/
		int getConnectedFlowEdgeCount(const mcd::Pin* pin);
	};
}
