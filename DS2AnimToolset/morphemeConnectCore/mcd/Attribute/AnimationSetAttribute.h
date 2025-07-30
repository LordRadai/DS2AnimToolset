#pragma once
#include "Attribute.h"
#include "AttributePlaceholder.h"
#include "NMDBExtensions/TypedNodeContainer.inl"

namespace mcd
{
	class AnimationSetAttribute : public Attribute
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::AttributePlaceholder>> m_attributePlaceholders;

	public:
		AnimationSetAttribute(db::Attribute* parent, const std::string& attribName, const std::string& animSetName, mcd::Attribute* attribute);

		virtual ~AnimationSetAttribute() override {};

		void setAttribute(const std::string& animSetName, mcd::Attribute* attribute);
	};
}
