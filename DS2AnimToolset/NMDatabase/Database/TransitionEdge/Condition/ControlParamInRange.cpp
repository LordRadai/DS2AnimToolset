#include "ControlParamInRange.h"

namespace db
{
	ControlParamInRange::ControlParamInRange(Node* parent, std::string name, float lowerTest) : Condition(parent, name, "ControlParamInRange", 1)
	{
		addAttribute(new BoolAttribute(this, "NotInRange", false));
		addAttribute(new ControlParameterAttribute(this, "ControlParameter", nullptr));
		addAttribute(new FloatAttribute(this, "LowerTestValue", lowerTest));
		addAttribute(new FloatAttribute(this, "UpperTestValue", 0.0f));
	}

	bool ControlParamInRange::isValid() const
	{
		if (!Condition::isValid())
			return false;

		if (getNumAttributes() < 4)
			return false;

		if (getNotInRange() == nullptr || getControlParameter() == nullptr || getLowerTestvalue() == nullptr || getUpperTestValue() == nullptr)
			return false;

		return true;
	}
}
