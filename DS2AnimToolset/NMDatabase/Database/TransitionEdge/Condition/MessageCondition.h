#pragma once
#include "Condition.h"

namespace db
{
	class MessageCondition : public Condition
	{
	public:
		MessageCondition(Node* parent, std::string name, Request* request, bool onNotSet);

		virtual ~MessageCondition() {};
		virtual bool isValid() const;

		RequestAttribute* getMessageAttribute() const { return dynamic_cast<RequestAttribute*>(findAttribute("Message")); }
		BoolAttribute* getOnNotSet() const { return dynamic_cast<BoolAttribute*>(findAttribute("OnNotSet")); }
	};
}