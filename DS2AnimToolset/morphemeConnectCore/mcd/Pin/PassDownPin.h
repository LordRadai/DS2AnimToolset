#pragma once
#include "Pin.h"

namespace mcd
{
	class PassDownPin : public Pin
	{
	public:
		PassDownPin(db::Node* parent, const std::string& name);
	};
}