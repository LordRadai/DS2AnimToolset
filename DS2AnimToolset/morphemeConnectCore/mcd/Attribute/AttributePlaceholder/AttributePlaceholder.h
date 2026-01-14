#pragma once
#include "NMDatabase/Node/Node.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "../Attribute.h"

namespace mcd
{
	class AttributePlaceholder : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Attribute>> m_attributes;

	public:
		AttributePlaceholder(db::Attribute* parent, std::string name);

		virtual ~AttributePlaceholder() override {}

		mcd::Attribute* getMcdAttribute() const;
		void setMcdAttribute(mcd::Attribute* attribute);
	};
}
