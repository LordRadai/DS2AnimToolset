#pragma once
#include "RCore.h"
#include <string>
#include <vector>
#include "../Network/Network.h"

namespace db
{
	class Database
	{
		std::string m_productVersion = "3.6.2";
		uint32_t m_dataVersion = 3506;
		uint32_t m_formatVersion = 5;
		std::vector<Network*> m_networks;

	public:
		Database() {};
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

		size_t getNumNetworks() const { return m_networks.size(); }
	};
}