#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Pin : public db::Node
	{
		std::unique_ptr<db::BoolAttribute> m_reference;

	public:
		Pin(db::Node* parent, const std::string& name, const std::string pinName)
			: db::Node(parent, name, pinName), 
			m_reference(std::make_unique<db::BoolAttribute>(this, "Reference", false)) {};

		virtual ~Pin() override {};

		void setReference(bool isReference);
	};
}