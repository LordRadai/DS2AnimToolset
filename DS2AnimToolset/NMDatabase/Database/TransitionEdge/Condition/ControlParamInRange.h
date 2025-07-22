#pragma once
#include "Condition.h"

namespace db
{
	class ControlParamInRange : public Condition
	{
	public:
		ControlParamInRange(Node* parent, std::string name, float lowerTest);

		virtual ~ControlParamInRange() {};
		virtual bool isValid() const;

		BoolAttribute* getNotInRange() const { return dynamic_cast<BoolAttribute*>(findAttribute("NotInRange")); }
		ControlParameterAttribute* getControlParameter() const { return dynamic_cast<ControlParameterAttribute*>(findAttribute("ControlParameter")); }
		FloatAttribute* getLowerTestvalue() const { return dynamic_cast<FloatAttribute*>(findAttribute("LowerTestValue")); }
		FloatAttribute* getUpperTestValue() const { return dynamic_cast<FloatAttribute*>(findAttribute("UpperTestValue")); }
	};
}
