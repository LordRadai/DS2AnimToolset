#include "NodeMap.h"

namespace MCN
{
	NodeMap::NodeMap(ME::NodeExportXML* src, int btID, NodeCategory category, int parentBT, std::string name)
	{
		this->m_nodeXml = src;

		if (btID == -1)
			this->m_nodeID = src->getNodeID();
		
		this->m_blendTreeID = btID;

		if (parentBT == -1)
			this->m_parentSMNodeID = src->getDownstreamParentID();

		this->m_nodeCategory = category;
		this->m_name = name;
		this->m_parentBTNodeID = parentBT;
	}
}