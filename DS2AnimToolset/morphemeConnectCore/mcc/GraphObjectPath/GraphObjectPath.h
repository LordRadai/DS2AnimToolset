#pragma once
#include "mcd/Network/Network.h"

namespace mcc
{
	class GraphObjectPath
	{
		mcd::Graph* m_graph;
		mcd::Network* m_network;

	public:
		GraphObjectPath(db::Node* node, mcd::Network* network);
		GraphObjectPath(std::string path, mcd::Network* network);
	};
}
