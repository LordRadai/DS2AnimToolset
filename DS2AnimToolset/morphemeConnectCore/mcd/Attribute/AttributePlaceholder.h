#pragma once
#include "NMDatabase/Node/Node.h"
#include "db/TypedNodeContainer.inl"
#include "Attribute.h"

namespace mcd
{
	class AttributePlaceholder : public db::Node
	{
		db::TypedNodeContainer<mcd::Attribute> m_attributes;

	public:
		AttributePlaceholder(db::Attribute* parent, std::string name, std::string nodeName) :
			Node(parent, name, nodeName),
			m_attributes(this, "AttributeEntry") {}

		virtual ~AttributePlaceholder() override {}

		mcd::Attribute* getMcdAttribute() const;
		void setMcdAttribute(mcd::Attribute* attribute);
	};
}
