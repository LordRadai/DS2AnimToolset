#pragma once
#include "NMDatabase/Node/Node.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "../Attribute.h"
#include "../SingleValue/StringAttribute.h"

namespace mcd
{
	class RigChannelNameAttribute : public mcd::Attribute
	{
		std::unique_ptr<db::StringAttribute> m_rigChannelName;

	public:
		RigChannelNameAttribute(db::Attribute* parent, const std::string& name);

		std::string getRigChannelName() const { return m_rigChannelName->getValue(); }
		void setRigChannelName(const std::string& rigChannelName);
	};
}
