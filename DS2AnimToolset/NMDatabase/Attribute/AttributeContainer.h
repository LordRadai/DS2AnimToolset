#pragma once
#include "CompositeAttribute.h"

namespace db
{
	class AttributeContainer : public CompositeAttribute
	{
	public:
		AttributeContainer(Attribute* parent, std::string name, std::string type) : 
			CompositeAttribute(parent, name, type) {};

		virtual ~AttributeContainer() override {};
		virtual void clearArray() override;
	};
}
