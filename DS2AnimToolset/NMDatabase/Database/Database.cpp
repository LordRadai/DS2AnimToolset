#include "Database.h"

namespace db
{
	Node* Database::find(std::string name) const
	{
		for (size_t i = 0; i < getNumNodes(); i++)
		{
			Node* node = getNode(i);

			if (node->getName() == name)
				return node;
		}

		return nullptr;
	}
}