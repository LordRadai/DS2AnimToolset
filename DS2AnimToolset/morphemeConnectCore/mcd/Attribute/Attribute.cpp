#include "Attribute.h"

namespace mcd
{
	bool Attribute::assignValue(Attribute* attr)
	{
		if ((this->size() != 1) || (attr->size() != 1))
			return false;

		db::Attribute* thisValueAttr = this->getAttribute(0);
		db::Attribute* otherValueAttr = attr->getAttribute(0);

		if (!thisValueAttr->isSingleValue() || !otherValueAttr->isSingleValue())
			return false;

		this->assign(otherValueAttr);
		return true;
	}
}
