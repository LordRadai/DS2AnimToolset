#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Pin : public db::Node
	{
	public:
		Pin(db::Node* parent, const std::string& name, const std::string pinName)
			: db::Node(parent, name, pinName) {};

		virtual ~Pin() override {};
	};
}