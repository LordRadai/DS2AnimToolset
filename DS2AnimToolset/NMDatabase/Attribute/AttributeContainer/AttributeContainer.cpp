#include "AttributeContainer.h"

namespace db
{
	void AttributeContainer::clearArray()
	{
		while (!empty())
		{
			removeAttribute(back());
		}
	}
}