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
		* \return The number of flow edges referencing this pin.
		*/
		int getConnectedFlowEdgeCount(const mcd::Pin* pin);

		/*
		* \brief Get the connected flow edge at a given index for a pin.
		* \param pin The pin to check.
		* \param index The index of the connected flow edge.
		* \return The flow edge connecting the pin.
		*/
		mcd::FlowEdge* getConnectedFlowEdge(const mcd::Pin* pin, int index);
	};
}
