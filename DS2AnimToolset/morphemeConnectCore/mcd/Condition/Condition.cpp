#include "Condition.h"

namespace mcd
{
	Condition::Condition(db::Node* parent, const std::string& name, const std::string& type, const int manifestVersion)
		: db::Node(parent, "Condition", name),
		m_attributes(std::make_unique<db::TypedNodeContainer<mcd::Attribute>>(this, "Attributes")),
		m_conditionType(std::make_unique<db::StringAttribute>(this, "ConditionType", type)),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion))
	{
		db::Node::addAttribute(m_attributes.get());
		db::Node::addAttribute(m_conditionType.get());
		db::Node::addAttribute(m_manifestVersion.get());
	}
}
