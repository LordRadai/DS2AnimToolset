#include "NodeDef.h"

namespace db
{
	bool NodeDef::isValid() const
	{
		if (!Node::isValid())
			return false;

		if (m_nodeType.empty())
			return false;

		return true;
	}

	tinyxml2::XMLElement* NodeDef::serialize(tinyxml2::XMLElement* parent)
	{
		return Node::serialize(parent);
	}
}
