#pragma once
#include "RCore.h"
#include "Network/Network.h"
#include "Node/Node.h"
#include "NodeContainer/NodeContainer.h"

namespace db
{
	class Database : public Node
	{
		std::string m_namespace;
		NodeContainer m_networks;

	public:
		Database() : Node(nullptr, "MorphemeDB", "MorphemeDB"), m_namespace("mcd"), m_networks(this, "Networks") {};
		
		virtual ~Database() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		Network* getNetwork(int index) const;
		Network* addNetwork();

		void removeNetwork(int index);

		size_t getNumNetworks() const { return m_networks.getNumNodes(); }
	};
}