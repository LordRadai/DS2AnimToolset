#include "PointerArrayAttribute.h"

namespace db
{
	PointerAttribute* PointerArrayAttribute::add(Attribute* to)
	{
		PointerAttribute* attr = insert(to, static_cast<int>(m_attributes.size()));
		return attr->asPointer();
	}

	PointerAttribute* PointerArrayAttribute::insert(Attribute* to, int idx)
	{
		PointerAttribute* attr = new PointerAttribute(this, "elem", to);
		insertAttribute(idx, attr);
		return attr->asPointer();
	}

	Attribute* PointerArrayAttribute::find(const std::string& name) const
	{
		for (uint32_t i = 0; i < size(); ++i)
		{
			PointerAttribute* ptrAttr = getAttribute(i)->asPointer();

			if (ptrAttr)
			{
				Attribute* pointedAttr = ptrAttr->getValue();

				if (pointedAttr && pointedAttr->getName() == name)
					return pointedAttr;
			}
		}

		return nullptr;
	}

	void PointerArrayAttribute::setElement(Attribute* to, int index)
	{
		PointerAttribute* ptrAttr = getAttribute(index)->asPointer();

		if (ptrAttr != nullptr)
			ptrAttr->setValue(to);
	}

	void PointerArrayAttribute::setValue(std::vector<Attribute*> values)
	{
		clearArray();

		resize(static_cast<uint32_t>(values.size()));

		for (size_t i = 0; i < values.size(); ++i)
		{
			PointerAttribute* ptrAttr = getAttribute(static_cast<uint32_t>(i))->asPointer();
			if (ptrAttr != nullptr)
				ptrAttr->setValue(values[i]);
		}
	}
}