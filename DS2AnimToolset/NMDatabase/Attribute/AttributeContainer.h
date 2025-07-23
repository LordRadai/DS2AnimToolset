#pragma once
#include "CompositeAttribute.h"

namespace db
{
	class AttributeContainer : public CompositeAttribute
	{
	public:
		AttributeContainer(Attribute* parent, std::string name, std::string label, std::string type) : 
			CompositeAttribute(parent, name, label, type) {};

		virtual void clearArray() override;
	};
}
