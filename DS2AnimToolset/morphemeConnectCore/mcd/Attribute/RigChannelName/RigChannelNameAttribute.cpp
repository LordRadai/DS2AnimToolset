#include "RigChannelNameAttribute.h"

namespace mcd
{
	RigChannelNameAttribute::RigChannelNameAttribute(db::Attribute* parent, const std::string& name)
		: Attribute(parent, "RigChannelNameAttribute", name),
		m_rigChannelName(std::make_unique<db::StringAttribute>(this, "Value", ""))
	{
	}

	void RigChannelNameAttribute::setRigChannelName(const std::string& rigChannelName)
	{
		removeAttribute(m_rigChannelName.get());

		m_rigChannelName->setValue(rigChannelName);

		if (!rigChannelName.empty())
			addAttribute(m_rigChannelName.get());
	}
}