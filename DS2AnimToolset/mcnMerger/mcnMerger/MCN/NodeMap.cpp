#include "NodeMap.h"

namespace MCN
{
	NodeMap::NodeMap(ME::NodeExportXML* src, NodeCategory category, std::string name)
	{
		this->m_nodeXml = src;
		this->m_nodeID = src->getNodeID();
		this->m_nodeCategory = category;
		this->m_name = name;
	}
}