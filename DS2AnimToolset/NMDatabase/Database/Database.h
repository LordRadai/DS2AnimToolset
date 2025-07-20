#pragma once
#include "RCore.h"
#include "../Network/Network.h"
#include "../Node/Node.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class Database : public Node
	{
		std::string m_productVersion = "3.6.2";
		uint32_t m_dataVersion = 3506;
		uint32_t m_formatVersion = 5;
		NodeContainer m_networks;

	public:
		Database() : Node(nullptr, "MorphemeDB"), m_networks(this) {};
		~Database() {};

		std::string getProductVersion() const { return m_productVersion; }
		void setProductVersion(const std::string& version) { m_productVersion = version; }

		uint32_t getDataVersion() const { return m_dataVersion; }
		void setDataVersion(uint32_t version) { m_dataVersion = version; }

		uint32_t getFormatVersion() const { return m_formatVersion; }
		void setFormatVersion(uint32_t version) { m_formatVersion = version; }

		Network* getNetwork(int index) const;
		Network* addNetwork();

		void removeNetwork(int index);

		size_t getNumNetworks() const { return m_networks.getNumNodes(); }
	};
}