#include "MessageCondition.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	MessageCondition::MessageCondition(Node* parent, std::string name, Request* request, bool onNotSet) : Condition(parent, name, "MessageCondition", 2)
	{
		addAttribute(new RequestAttribute(this, "Message", request));
		addAttribute(new BoolAttribute(this, "OnNotSet", onNotSet));
	}

	bool MessageCondition::isValid() const
	{
		if (!Condition::isValid())
			return false;

		if (getNumAttributes() < 2)
			return false;

		if (getMessageAttribute() == nullptr)
			return false;

		if (getOnNotSet() == nullptr)
			return false;

		return true;
	}
}
