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

		tinyxml2::XMLDeclaration* createXMLDeclaration(tinyxml2::XMLDocument* doc);
		tinyxml2::XMLElement* createRootElement(tinyxml2::XMLDocument* doc);
	public:
		Database() : Node(nullptr, "MorphemeDB", "MorphemeDB"), m_namespace("mcd"), m_networks(this, "Networks") {};
		
		virtual ~Database() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		const char* getProductVersion() const { return "3.6.2"; }
		int getDataVersion() const { return 3506; }
		int getFormatVersion() const { return 5; }

		void exportXML(const char* filepath);

		Network* getNetwork(int index) const;
		Network* addNetwork();

		void removeNetwork(int index);

		size_t getNumNetworks() const { return m_networks.getNumNodes(); }
	};
}