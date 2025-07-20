#include "Node.h"

namespace db
{
	std::string Node::getQualifiedName() const
	{
		if (!m_parent)
			return m_name;

		return m_parent->getQualifiedName() + "." + m_name;
	}
}