#pragma once
#include "Node/Node.h"

namespace mcd
{
	class Pin : public db::Node
	{
		Pin(db::Node* parent, std::string identifier, std::string label) :
			db::Node(parent, identifier, label)
		{};

	public:

		virtual ~Pin() override {};
	};
}
