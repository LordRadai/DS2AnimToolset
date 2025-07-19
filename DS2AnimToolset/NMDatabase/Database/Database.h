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
	};
}