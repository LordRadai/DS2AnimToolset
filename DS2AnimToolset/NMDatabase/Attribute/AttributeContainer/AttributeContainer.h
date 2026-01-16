#pragma once
#include "../CompositeAttribute/CompositeAttribute.h"

namespace db
{
	class AttributeContainer : public CompositeAttribute
	{
	public:
		AttributeContainer(CompositeAttribute* parent, std::string name, std::string type) :
			CompositeAttribute(parent, name, type) {};

		virtual ~AttributeContainer() override {};
		virtual void clearArray() override;
	};
}
